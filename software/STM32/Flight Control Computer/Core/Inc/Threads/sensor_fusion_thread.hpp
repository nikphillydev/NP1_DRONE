/*
 * sensor_fusion_thread.hpp
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
typedef struct						// NED coordinate system
{
	float rotation[3];				// Roll, Pitch, Yaw 					(deg)
	float quaternion[4];			// Quaternion rotation 					[x,y,z,w]
	float velocity[2];				// Linear velocity in BODY frame (m/s) 	[x,y]
	float altitude;					// Altitude (m)		 					[z]
} drone_state_t;

/*
 *
 * THREADS
 *
 */
void sensor_fusion_thread();		// Runs sensor fusion algorithm
void fusion_logging_thread();		// Logs sensor and vehicle state data to GCS

/*
 *
 * IRQ FUNCTIONS
 *
 */
void service_BMI088_Accel();
void service_BMI088_Gyro();
void service_BMP388();
void service_LIS3MDL();
void poll_US100_Ultrasonic(uint8_t start_transfer);
void poll_PMW3901(float polling_period_s);

#ifdef __cplusplus
}
#endif
