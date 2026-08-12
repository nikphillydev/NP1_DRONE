/*
 * alt_comp_filter.cpp
 *
 *  Created on: Aug 4, 2026
 *      Author: Nikolai Philipenko
 */

#include <array>
#include <cmath>

#include "SensorFusion/alt_comp_filter.hpp"


AltitudeComplementaryFilter::AltitudeComplementaryFilter(US100& range_finder, BMP388& barometer)
	: range_finder(range_finder),
	  barometer(barometer) {}

float AltitudeComplementaryFilter::update(const MFX_output_t& state)
{
	bool roll_valid = -ABS_MAX_ROLL_PITCH < state.rotation[2] && state.rotation[2] < ABS_MAX_ROLL_PITCH;
	bool pitch_valid = -ABS_MAX_ROLL_PITCH < state.rotation[1] && state.rotation[1] < ABS_MAX_ROLL_PITCH;

	// Get range finder distance
	float rf_distance_BODY = range_finder.get_distance();
	bool rf_valid = 0 < rf_distance_BODY && rf_distance_BODY < RANGE_MAX_DISTANCE_M;

	// Get barometer altitude
	float barometer_altitude = barometer.get_altitude();

	float altitude = 0;

	if (roll_valid && pitch_valid && rf_valid)
	{
		// Extract and normalize orientation quaternion
		float qx = state.quaternion[0], qy = state.quaternion[1], qz = state.quaternion[2], qw = state.quaternion[3];
		float n = 1.0f / std::sqrt(qx*qx + qy*qy + qz*qz + qw*qw);
		qx *= n; qy *= n; qz *= n; qw *= n;

		// Rotation matrix from BODY frame to WORLD frame
		std::array<std::array<float, 3>, 3> R {};
		// R[0][0] = 1.0f - 2.0f * (qy*qy + qz*qz);			// Row 1
		// R[0][1] = 2.0f * (qx*qy - qw*qz);
		// R[0][2] = 2.0f * (qx*qz + qw*qy);
		// R[1][0] = 2.0f * (qx*qy + qw*qz);				// Row 2
		// R[1][1] = 1.0f - 2.0f * (qx*qx + qz*qz);
		// R[1][2] = 2.0f * (qy*qz - qw*qx);
		// R[2][0] = 2.0f * (qx*qz - qw*qy);				// Row 3
		// R[2][1] = 2.0f * (qy*qz + qw*qx);
		R[2][2] = 1.0f - 2.0f * (qx*qx + qy*qy);

		float rf_distance_WORLD = R[2][2] * rf_distance_BODY;

		// Fuse measurements via complimentary filter
		altitude = ALTITUDE_ALPHA * barometer_altitude + (1.0f - ALTITUDE_ALPHA) * rf_distance_WORLD;
	}
	else
	{
		// Range finder value invalid, ignore
		altitude = barometer_altitude;
	}

	return altitude;
}
