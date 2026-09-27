/*
 * alt_comp_filter.hpp
 *
 *  Created on: Aug 4, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "motion_fx.h"

#include "Drivers/US100_Ultrasonic/us100.hpp"
#include "Drivers/BMP388/bmp388.hpp"

#define ABS_MAX_ROLL_PITCH			80		// [0, 90]

/*
 * COMPLEMENTARY FILTER ALPHA
 *
 * Degree of trust placed on barometer altitude measurement vs. ultrasonic distance sensor. Between [0-1]
 */
#define ALTITUDE_ALPHA				0.005f


/*
 * Class to run a simple complementary filter to fuse barometer altitude and ultrasonic distance measurements.
 */
class AltitudeComplementaryFilter
{
public:
	AltitudeComplementaryFilter(US100& range_finder, BMP388& barometer);

	/*
	 * Run the complementary filter. Must be called frequently.
	 */
	float update(const MFX_output_t& state);

private:
	// Sensors
	US100& range_finder;
	BMP388& barometer;
};
