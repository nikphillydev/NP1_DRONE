/*
 * ekf.cpp
 *
 *  Created on: Aug 4, 2026
 *      Author: Nikolai Philipenko
 */

#include "cmsis_os.h"

#include <array>

#include "SensorFusion/orient_ekf.hpp"

#define MS2_TO_G					1 / 9.80665
#define G_TO_MS2					9.80665
#define RADIANS_TO_DEGREES			180.0 / M_PI
#define GAUSS_TO_uTESLA				100.0


OrientationEKF::OrientationEKF(BMI088& imu, LIS3MDL& magnetometer, USB_Logger& logger)
	: imu(imu),
	  magnetometer(magnetometer),
	  logger(logger) {}

bool OrientationEKF::init()
{
	const float GBIAS_ACC_TH_SC 	= 2 * 0.000765;
	const float GBIAS_GYRO_TH_SC 	= 2 * 0.002;
	const float GBIAS_MAG_TH_SC 	= 2 * 0.001500;

	logger.log("EKF: Starting initialization.", CRITICAL);

	// Check MotionFX state size
	if (sizeof(mfxstate) < MotionFX_GetStateSize())
	{
		logger.log("EKF: MotionFX algorithm state not enough memory", ERR);
		return false;
	}
	MotionFX_initialize((MFXState_t *)mfxstate);

	// Turn library knobs...
	MFX_knobs_t iKnobs;
	MotionFX_getKnobs(mfxstate, &iKnobs);

	iKnobs.acc_orientation[0] = 'w';			// positive orientation to the
	iKnobs.acc_orientation[1] = 's';			// 	   board's reference frame
	iKnobs.acc_orientation[2] = 'u';
	iKnobs.gyro_orientation[0] = 'w';
	iKnobs.gyro_orientation[1] = 's';
	iKnobs.gyro_orientation[2] = 'u';
	iKnobs.mag_orientation[0] = 'n';
	iKnobs.mag_orientation[1] = 'w';
	iKnobs.mag_orientation[2] = 'u';

	iKnobs.gbias_acc_th_sc = GBIAS_ACC_TH_SC;
	iKnobs.gbias_gyro_th_sc = GBIAS_GYRO_TH_SC;
	iKnobs.gbias_mag_th_sc = GBIAS_MAG_TH_SC;

	iKnobs.output_type = MFX_ENGINE_OUTPUT_NED;	// NED output orientation
	iKnobs.LMode = 1;							// static learning
	iKnobs.modx = 1;							// decimation of MotionFX_update call frequency

	MotionFX_setKnobs(mfxstate, &iKnobs);

	// Enable 9-axis sensor fusion
	MotionFX_enable_6X(mfxstate, MFX_ENGINE_DISABLE);
	MotionFX_enable_9X(mfxstate, MFX_ENGINE_ENABLE);

	logger.log("EKF: Initialized OK.", CRITICAL);
	return true;
}

bool OrientationEKF::calibrate_magnetometer()
{
	bool magnetometer_calibrated = false;
	MFX_MagCal_input_t mag_data_in;
	MFX_MagCal_output_t mag_data_out;

	logger.log("EKF: Slowly rotate the device in a figure 8 pattern in space to calibrate the magnetometer...", INFO);

	const uint32_t calibration_period_ms = 25;
	uint32_t wakeup_time = osKernelGetTickCount();

	uint32_t count = 0;
	const uint32_t max_count = 1 * 60 * 1000 / calibration_period_ms;		// 1 minute timeout

	MotionFX_MagCal_init(calibration_period_ms, 1);

	while(!magnetometer_calibrated && count < max_count)
	{
		wakeup_time += calibration_period_ms;
		osDelayUntil(wakeup_time);

		// Apply timestamp to data
		mag_data_in.time_stamp = osKernelGetTickCount();	// in ms

		// Get magnetometer data
		std::array<float, 3> mag_intensity = magnetometer.get_axis_intensities();
		mag_data_in.mag[0] = mag_intensity[0] * GAUSS_TO_uTESLA / 50.0f;		// in uT/50
		mag_data_in.mag[1] = mag_intensity[1] * GAUSS_TO_uTESLA / 50.0f;
		mag_data_in.mag[2] = mag_intensity[2] * GAUSS_TO_uTESLA / 50.0f;

		// Run calibration
		MotionFX_MagCal_run(&mag_data_in);
		MotionFX_MagCal_getParams(&mag_data_out);

		if (mag_data_out.cal_quality == MFX_MAGCALGOOD)
		{
			magnetometer_calibrated = true;
		}

		count++;
	}

	MotionFX_MagCal_init(calibration_period_ms, 0);

	if (!magnetometer_calibrated)
	{
		logger.log("EKF: Magnetometer calibration failed. Timeout.", ERR);
		return false;
	}

	logger.log("EKF: Magnetometer calibration complete.", INFO);

	hard_iron_offsets[0] = mag_data_out.hi_bias[0];			// in uT/50
	hard_iron_offsets[1] = mag_data_out.hi_bias[1];
	hard_iron_offsets[2] = mag_data_out.hi_bias[2];

	return true;
}

MFX_output_t OrientationEKF::update(float deltatime_s)
{
	MFX_input_t data_in;
	MFX_output_t data_out;

	// Get sensor data
	std::array<float, 3> linear_acceleration = imu.get_linear_accelerations();
	std::array<float, 3> angular_velocity = imu.get_angular_velocities();
	std::array<float, 3> mag_intensity = magnetometer.get_axis_intensities();

	// Format data
	data_in.acc[0] = linear_acceleration[0] * MS2_TO_G;				// in g
	data_in.acc[1] = linear_acceleration[1] * MS2_TO_G;
	data_in.acc[2] = linear_acceleration[2] * MS2_TO_G;
	data_in.gyro[0] = angular_velocity[0] * RADIANS_TO_DEGREES;		// in dps
	data_in.gyro[1] = angular_velocity[1] * RADIANS_TO_DEGREES;
	data_in.gyro[2] = angular_velocity[2] * RADIANS_TO_DEGREES;
	data_in.mag[0] = mag_intensity[0] * GAUSS_TO_uTESLA / 50.0f - hard_iron_offsets[0];		// in uT/50
	data_in.mag[1] = mag_intensity[1] * GAUSS_TO_uTESLA / 50.0f - hard_iron_offsets[1];
	data_in.mag[2] = mag_intensity[2] * GAUSS_TO_uTESLA / 50.0f - hard_iron_offsets[2];

	// Kalman filter predict and update orientation
	MotionFX_propagate(mfxstate, &data_out, &data_in, &deltatime_s);
	MotionFX_update(mfxstate, &data_out, &data_in, &deltatime_s, NULL);

	return data_out;
}

