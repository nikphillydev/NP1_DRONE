/*
 * orient_ekf.hpp
 *
 *  Created on: Aug 4, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "motion_fx.h"

#include "Drivers/BMI088/bmi088.hpp"
#include "Drivers/LIS3MDL/lis3mdl.hpp"
#include "Drivers/usb.hpp"

#define MFX_STATE_SIZE		2432


/*
 * Class to run an Extended Kalman Filter (EKF) to fuse accelerometer, gyroscope,
 * and magnetometer data into an orientation in NED coordinate system.
 */
class OrientationEKF
{
public:
	OrientationEKF(BMI088& imu, LIS3MDL& magnetometer, USB_Logger& logger);

	[[nodiscard]] bool init();

	bool calibrate_magnetometer();

	/*
	 * Run the EKF. Must be called frequently.
	 */
	MFX_output_t update(float deltatime_s);

private:
	// MotionFX
	uint8_t mfxstate[MFX_STATE_SIZE]{};

	// Sensors
	BMI088& imu;
	LIS3MDL& magnetometer;

	// Magnetometer calibration
	std::array<float, 3> hard_iron_offsets{};

	// Logger
	USB_Logger& logger;
};
