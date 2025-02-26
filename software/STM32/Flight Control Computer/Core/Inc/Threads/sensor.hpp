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
* THREADS
*/
void sensor_fusion_thread();

/*
* FUNCTIONS
*/
void service_BMI088_Accel();
void service_BMI088_Gyro();
void service_BMP388();
void service_LIS3MDL();


#ifdef __cplusplus
}
#endif
