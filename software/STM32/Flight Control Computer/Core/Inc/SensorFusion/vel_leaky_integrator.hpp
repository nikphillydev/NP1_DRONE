/*
 * vel_leaky_integrator.hpp
 *
 *  Created on: Aug 4, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "motion_fx.h"

#include <array>

#include "Drivers/PMW3901/pmw3901.hpp"
#include "Drivers/BMI088/bmi088.hpp"

/*
 * First-order settling time (2%) of leaky integrator from integrated XY velocities
 * (from accelerations) to absolute camera XY velocities.
 */
#define VEL_SETTLING_TIME			0.6f	// seconds


/*
 * Class to run a leaky integrator that integrates accelerations into velocites but
 * slowly leaks the running summation toward velocities from an optical flow camera.
 */
class VelocityLeakyIntegrator
{
public:
	VelocityLeakyIntegrator(PMW3901& optical_flow, BMI088& imu);

	/*
	 * Run the leaky integrator. Must be called frequently.
	 */
	std::array<float, 2> update(const MFX_output_t& state, float altitude, float deltatime_s);

private:
	// Sensors
	PMW3901& optical_flow;
	BMI088& imu;

	// Integrator state

	bool initialized = false;
	MFX_output_t prev_state;

	std::array<float, 2> velocity{};		// Device frame XY velocites
};
