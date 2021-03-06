/*
 * Copyright (C) 2009-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

#include <sys/mman.h>
#include <sys/io.h>
#include <pthread.h>
#include <csignal>
#include <cstring>
#include <rtai_lxrt.h>
#include <rtai_mbx.h>
#include <rtai_fifos.h>
#include <stk/Stk.h>
#include <stk/FileWvOut.h>

#include "utils/rttools.hpp"
#include "utils/math.hpp"
#include "utils/timerrtai.hpp"
#include "common.hpp"
#include "avrsexception.hpp"
#include "configuration.hpp"
#include "system.hpp"

namespace avrs
{

System::System(std::string config_filename, bool show_config)
{
	_config_manager.load_configuration(config_filename);
	_config_sim = _config_manager.get_configuration();

	if (show_config)
		_config_manager.show_configuration();

	_init();
}

System::~System()
{
	;
}

// Singleton (static ptr_t)
System::ptr_t System::get_instance(std::string config_filename, bool show_config)
{
	static System::ptr_t p_instance(new System(config_filename, show_config));  // instance
	return p_instance;
}

void System::_init()
{
	_in = InputWaveLoop::create(_config_sim->anechoic_file);
	assert(_in.get() != NULL);
	_out = Player::create(avrs::math::dB2linear(_config_sim->master_gain_db));
	assert(_out.get() != NULL);

	_conv_l = Convolver::create(BUFFER_SAMPLES);
	_conv_r = Convolver::create(BUFFER_SAMPLES);

	uint read_interval_ms = 10;  // ms (100 Hz)

#ifdef WIIMOTE_TRACKER
	std::cout << "Starting Wiimote tracker\n";
	_tracker = TrackerWiimote::create(
			"tracker_points",
			_config_sim->tracker_params,
			read_interval_ms);
#else
	std::cout << "Starting simulated tracker\n";

//	_tracker = TrackerSim::create(
//			TrackerSim::from_file,
//			read_interval_ms,
//			_config_sim->tracker_sim_file);

	_tracker = TrackerSim::create(
			TrackerSim::constant,
			read_interval_ms);
#endif

	assert(_tracker.get() != NULL);

	_ve = VirtualEnvironment::create(_config_sim, _tracker);
	assert(_ve.get() != NULL);
}

// SRT main task
bool System::run()
{
	signal(SIGINT, end_system);
	signal(SIGTERM, end_system);

	RT_TASK *wait_task;

	rt_allow_nonroot_hrt(); // allow hard real-time from non-root users

	if (rt_is_hard_timer_running())
	{
		// Skip hard real time timer setting...
		_sampling_interval = nano2count(TICK_TIME);
	}
	else
	{
		// Starting real time timer...
		rt_set_oneshot_mode();
		start_rt_timer(0);
		_sampling_interval = nano2count(TICK_TIME);
	}

	mlockall(MCL_CURRENT | MCL_FUTURE);

	// create WAIT task
	wait_task = rt_task_init_schmod(nam2num("TSKWAI"), 2, 0, 0, SCHED_FIFO, 0xFF);

	if (!wait_task)
	{
		ERROR("Cannot init WAIT task");
		return false;
	}

	_is_running = true;
	pthread_create(&_thread_id, NULL, System::_rt_wrapper, this); // run simulation (in another thread)

	printf(">> ");

	while (!g_end_system)
	{
		switch (getchar())
		{
			case 'Q':
			case 'q':
				g_end_system = 1;
				break;

			case 'C':
			case 'c':
				_ve->calibrate_tracker();
				printf("Calibrating done\n");
				printf(">> ");
				break;

			case '\n':
				printf(">> ");
				break;
		}
	}

	pthread_join(_thread_id, NULL); // wait until run thread finish...
	rt_task_delete(wait_task);
	printf("Quit\n");

	return true;
}

void *System::_rt_wrapper(void *arg)
{
	return reinterpret_cast<System *> (arg)->_rt_thread(NULL);
}

// HRT Task
void *System::_rt_thread(void *arg)
{
	RT_TASK *sys_task;
	RTIME start_time;
	int *retval = NULL;
	// time measurements
//	TimerRtai t_loop, t_render, t_conv;
	// other variables
	int val;
	unsigned int i;

	rt_allow_nonroot_hrt();
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// create SYS task
	sys_task = rt_task_init_schmod(nam2num("TSKSYS"), 0, 0, 0, SCHED_FIFO, 0xFF);

	if (!sys_task)
	{
		DPRINT("Cannot init SYS task\n");
		*retval = 1;
		pthread_exit(retval);
	}

	// create the output RT-FIFO
	if (rtf_create(RTF_OUT_NUM, RTF_OUT_BLOCK * sizeof(sample_t) * 4) < 0) // capacity x 4
	{
		ERROR("Cannot init rt-fifo");
		*retval = 1;
		pthread_exit(retval);
	}

	rtf_reset(RTF_OUT_NUM); // clear it out

	_input.resize(BUFFER_SAMPLES);
	int n_bytes = RTF_OUT_BLOCK * sizeof(sample_t);  // both ears
	sample_t *output_player = (sample_t *) malloc(RTF_OUT_BLOCK * sizeof(sample_t));  // sending by RT-FIFO
	sample_t *output_l = (sample_t *) malloc(BUFFER_SAMPLES * sizeof(sample_t));
	sample_t *output_r = (sample_t *) malloc(BUFFER_SAMPLES * sizeof(sample_t));

	for (i = 0; i < BUFFER_SAMPLES; i++)
		output_l[i] = output_r[i] = 0.0f;

	_out->start(); // start the output
	_ve->start_simulation();

	start_time = rt_get_time() + 50 * _sampling_interval;
	rt_task_make_periodic(sys_task, start_time, _sampling_interval); // make the task periodic
	rt_make_hard_real_time(); // execute the task in hard real-time

	while (!g_end_system)
	{
//		t_loop.start();

		// get input (anechoic signal)
		for (i = 0; i < BUFFER_SAMPLES; i++)
			_input[i] = _in->tick();

		// update the position
		if (!_ve->update_listener_orientation())
			end_system(-1);

		// renderize BIR
//		t_render.start();
		_ve->renderize();
//		t_render.stop();

		// get the new BIR
		_bir = _ve->get_BIR();

		// update the BIR in the real-time convolver
//		t_conv.start();

		if (_ve->is_new_BIR())
		{
			_conv_l->set_filter_t(_bir.left);
			_conv_r->set_filter_t(_bir.right);
		}

		// convolve with anechoic signal
		output_l = _conv_l->convolve_signal(_input.data());
		output_r = _conv_r->convolve_signal(_input.data());

//		t_conv.stop();

		// preparing output for RT-FIFO
		memcpy(output_player, output_l, BUFFER_SAMPLES * sizeof(sample_t));
		memcpy(&output_player[BUFFER_SAMPLES], output_r, BUFFER_SAMPLES * sizeof(sample_t));

		// send to output player
		val = rtf_put(RTF_OUT_NUM, output_player, n_bytes); // both ears [left right]

		if (val != n_bytes)
		{
			ERROR("Error sending through RT-FIFO, %d bytes instead of %d bytes", val, n_bytes * 2);
		}

//		t_loop.stop();

//		if (_ve->is_new_BIR())
//		{
//		DPRINT("Render: %6.3f - RT Convolution: %6.3f - Loop: %6.3f - Tick: %6.3f ms",
//				t_render.elapsed_time(millisecond),
//				t_conv.elapsed_time(millisecond),
//				t_loop.elapsed_time(millisecond),
//				TICK_TIME / 1.0e+6f);
//		}

		rt_task_wait_period();
	}

	free(output_player);
	_out->stop(); // stop the output
	_ve->stop_simulation();
	rt_make_soft_real_time();
	rt_task_delete(sys_task);
	rtf_destroy(RTF_OUT_NUM);

	return 0;
}

}  // namespace avrs
