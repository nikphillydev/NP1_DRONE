/*
 * sensor.hpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/*
 * This #ifdef clause is needed because if a Cpp file defines a function declaration / prototype,
 * than that declaration cannot be used in a C file unless extern "C" is used.
 */

/*
 *
 * TYPES
 *
 */
struct state
{
	float roll;
	float pitch;
	float yaw;
};

/*
 *
 * THREADS
 *
 */
void sensor_fusion_thread();		// Implements sensor fusion algorithm
void logging_thread();				// Logs sensor and state data to GCS

/*
 *
 * FUNCTIONS
 *
 */
void initialize_sensor_fusion();
void service_BMI088_Accel();
void service_BMI088_Gyro();
void service_BMP388();
void service_LIS3MDL();


#ifdef __cplusplus
}
#endif
