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

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cassert>
#include <cstdio>
#include <vector>
#include <iostream>
#include <memory>  // for auto_ptr
#include <fftw3.h>
#include <armadillo>

namespace avrs
{

// typedefs
typedef fftwf_complex complex_t;  // for spectral data
typedef float sample_t;  // for time data
typedef std::vector<sample_t> data_t;
typedef arma::frowvec3 point3_t;
typedef arma::fmat::fixed<3,3> matrix33_t;

typedef struct BinauralData
{
	data_t left;
	data_t right;

	BinauralData(unsigned long size = 0)
	{
		left.resize(size);
		right.resize(size);
	}

	unsigned long size()
	{
		return left.size();
	}
} binauraldata_t;

typedef struct Position
{
	float x;	///< X position (in cm)
	float y;	///< Y position (in cm)
	float z;	///< Z position (in cm)

	Position()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Position operator+(const Position &val) const
	{
		Position res;
		res.x = x + val.x;
		res.y = y + val.y;
		res.z = z + val.z;

	    return res;
	}

	Position operator-(const Position &val) const
	{
		Position res;
		res.x = x - val.x;
		res.y = y - val.y;
		res.z = z - val.z;

	    return res;
	}

	point3_t to_point3() const
	{
		point3_t p;
		p << x << y << z << arma::endr;

		return p;
	}

} position_t;

typedef struct OrientationAngles
{
	float az;	///< Azimuth angle (in degrees)
	float el;	///< Elevation angle (in degrees)
	float ro;	///< Roll angle (in degrees)

	OrientationAngles()
	{
		az = 0.0f;
		el = 0.0f;
		ro = 0.0f;
	}

	OrientationAngles(float az_val, float el_val, float ro_val = 0)
	{
		az = az_val;
		el = el_val;
		ro = ro_val;
		correct_angles(*this);
	}

	OrientationAngles operator+(const OrientationAngles &val) const
	{
		OrientationAngles res;

		res.az = az + val.az;
		res.el = el + val.el;
		res.ro = ro + val.ro;
		correct_angles(res);

	    return res;
	}

	OrientationAngles operator-(const OrientationAngles &val) const
	{
		OrientationAngles res;

		res.az = az - val.az;
		res.el = el - val.el;
		res.ro = ro - val.ro;
		correct_angles(res);

	    return res;
	}

	void correct_angles(OrientationAngles &ori) const
	{
		// azimuth (-180, +180]
		if (ori.az > 180)
			ori.az = ori.az - 360;
		else if (ori.az  < -180)
			ori.az = ori.az + 360;

		// elevation [-90, +90]
		if (ori.el > 90)
			ori.el = 180 - ori.el;
		else if (ori.el < -90)
			ori.el = -180 - ori.el;
	}
} orientationangles_t;


// Definitions for all the system

/// Coordinates for linear algebra
static const short int X = 0;
static const short int Y = 1;
static const short int Z = 2;
/// Precision required for calculus
static const float PRECISION = 1E-6f;
/// Sample rate
static const int SAMPLE_RATE = 44100;
/// Number of output channels
static const short int N_CHANNELS = 2;
/// Samples of system processing buffer
static const short int BUFFER_SAMPLES = 512;
/// Samples per VS
static const short int VS_SAMPLES = 256;

// Preprocessor macros

/// Write a verbose message to stderr.
#define VERBOSE(format, ...) \
	fprintf(stderr, "VERBOSE [%s:%d]: " format "\n", \
							__FILE__, __LINE__, ## __VA_ARGS__)

/// Write a warning message to stderr.
#define WARNING(format, ...) \
	fprintf(stderr, "WARNING [%s:%d]: " format "\n", \
							__FILE__, __LINE__, ## __VA_ARGS__)

/// Write an error message to stderr.
#define ERROR(format, ...) \
	fprintf(stderr, "ERROR [%s:%d]: " format "\n", \
							__FILE__, __LINE__, ## __VA_ARGS__)

/// Write debug message to stderr
#ifndef NDEBUG
#define DPRINT(format, ...)	\
	fprintf(stderr, "DEBUG [%s:%d]: " format "\n", \
								__FILE__, __LINE__, ## __VA_ARGS__)
#else
#define DPRINT(format, ...)  // nothing
#endif

#define APPLY_DIRECTIVITY_FILTERING
#define APPLY_SURFACE_FILTERING
#define APPLY_AIR_FILTERING
#define APPLY_HRTF_FILTERING
#define APPLY_FDN_REVERBERATION

//#define FDN_SCALING_DISTANCE

}  // namespace

#endif // COMMON_HPP_
