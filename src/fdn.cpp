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

#include <cmath>
#include <stk/Stk.h>
#include <boost/make_shared.hpp>

#include "fdn.hpp"

namespace avrs
{

Fdn::Fdn(unsigned int N, double gain_A, const double *b, const double *c,
		double d, const long *m, double RTatDC, double RTatPI,
		std::string b_coeff_file, std::string a_coeff_file) :
	_N(N), _gA(gain_A), _d(d), _t60_0(RTatDC), _t60_pi(RTatPI)
{
	_m.resize(_N);
	_delayline.resize(_N);

	_lp_filter.resize(_N);
	_filter.resize(_N);

	_b.reshape(_N, 1);
	_c.reshape(_N, 1);

	for (unsigned int i = 0; i < _N; i++)
	{
		_m[i] = m[i]; // m delays
		_b[i] = b[i]; // b gains
		_c[i] = c[i]; // c gains
	}

	_s.reshape(_N, 1);
	_s_delayed.reshape(_N, 1);
	_s_filtered.reshape(_N, 1);

	_b_coeff.load(b_coeff_file, raw_ascii);
	_a_coeff.load(a_coeff_file, raw_ascii);

	_init();
}

Fdn::~Fdn()
{
	;
}

Fdn::ptr_t Fdn::create(unsigned int N, double gain_A, const double *b,
		const double *c, double d, const long *m, double RTatDC, double RTatPI,
		std::string b_coeff_file, std::string a_coeff_file)
{
	ptr_t p_tmp(new Fdn(N, gain_A, b, c, d, m, RTatDC, RTatPI,  b_coeff_file,  a_coeff_file));
	return p_tmp;
}

void Fdn::_init()
{
	// Setup filters
	StkFloat T;
	StkFloat alpha;
	StkFloat beta;

	T = 1.0 / (Stk::sampleRate());

	if (_t60_0 == 0.0)
		alpha = 1.0; // to prevent dividing by 0
	else
		alpha = _t60_pi / _t60_0;

	// Tone control filter
	_tc = boost::make_shared<OneZero>();
	beta = (1 - alpha) / (1 + alpha);
	_tc->setZero(beta);
	_tc->setGain(1/(1-beta));

	double tmp_val = (log(10) / 4) * (1 - (1 / (alpha * alpha)));
	std::vector<StkFloat> g(_N);
	std::vector<StkFloat> a(_N);

	// Absorption filters
	for (unsigned int i = 0; i < _N; i++)
	{
		// Delay line
		_delayline[i] = boost::make_shared<Delay>(_m[i], _m[i]);

		// Low-pass filters
		float exponent = -(3.0 * _m[i] * T) / _t60_0;
		g[i] = pow(10, exponent);
		a[i] = log10(g[i]) * tmp_val;
		_lp_filter[i] = boost::make_shared<OnePole>();
		_lp_filter[i]->setB0(g[i] * (1 - a[i]));
		_lp_filter[i]->setA1(a[i]);

		// IIR filters
		std::vector<StkFloat> b;

		// row by row
		for (mat::row_iterator ri = _b_coeff.begin_row(i); ri != _b_coeff.end_row(i); ri++)
		{
			b.push_back(*ri);
		}

		std::vector<StkFloat> a;

		// row by row
		for (mat::row_iterator ri = _a_coeff.begin_row(i); ri != _a_coeff.end_row(i); ri++)
		{
			a.push_back(*ri);
		}

		_filter[i] = boost::make_shared<Iir>();
		_filter[i]->setCoefficients(b, a);
	}

	// Feedback matrix (A) calculation
	colvec u = ones<colvec>(_N);
	mat I = eye<mat>(_N, _N);
	_A = _gA * (I - (2.0 / _N) * u * trans(u));
	clear();

	// stabilize the FDN
	// eliminate the first part of the output
	long n_ticks = _m[_N - 1] * 2;
	_stabilize(n_ticks);
}

void Fdn::clear(void)
{
	for (unsigned int i = 0; i < _N; i++)
	{
		_delayline[i]->clear();
		_lp_filter[i]->clear();
		_filter[i]->clear();

		_s[i] = 0.0;
		_s_delayed[i] = 0.0;
		_s_filtered[i] = 0.0;
	}

	_tc->clear();
}

StkFloat Fdn::tick(StkFloat sample)
{
	// for each line
	for (unsigned int i = 0; i < _N; i++)
	{
		_s_delayed[i] = _delayline[i]->tick(_s[i]);
//		_s_filtered[i] = _lp_filter[i]->tick(_s_delayed[i]);
		_s_filtered[i] = _filter[i]->tick(_s_delayed[i]);
	}

	_s = (sample * _b) + (_A * _s_filtered);

	return (_tc->tick(dot(_c, _s_filtered)) + (_d * sample));
	//return (dot(_c, _s_filtered)) + (_d * sample);
}

void Fdn::_stabilize(long n_ticks)
{
	// ticks without attenuation
	// for initialization purposes

	for (long i = 0; i < n_ticks; i++)
	{
		// for each line
		for (unsigned int j = 0; j < _N; j++)
		{
			_s_delayed[j] = _delayline[j]->tick(_s[j]);
			_s_filtered[j] = _s_delayed[j];
		}

		_s = (1.0 * _b) + (_A * _s_filtered);
	}
}

}  // namespace avrs
