/*
 * Copyright (C) 2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#ifndef TIMERRTAI_HPP_
#define TIMERRTAI_HPP_

#include <rtai_mbx.h>

namespace avrs
{

class TimerRtai
{
public:
	void start();
	void stop();

	double get_elapsed_ns();
	double get_elapsed_us();
	double get_elapsed_ms();
	double get_elapsed_s();

	void print_elapsed_time();

private:
	RTIME _t0;
	RTIME _t1;
	double _diff;
};

}  // namespace avrs

#endif /* TIMERRTAI_HPP_ */
