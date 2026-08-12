/*
 * sensor.cpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 *
 *      Custom sensor fusion algorithm for the NP1 Drone. Running at 250 Hz using NED coordinate system.
 *
 *      HOW TO TUNE SENSOR FUSION ALGORITHM:
 *
 *      	SENSORS:
 *
 *      	- IMU:
 *      		(param) ACCEL_CUTOFF_FREQ_HZ: Accelerometer LPF cutoff frequency
 *      		(param) GYRO_CUTOFF_FREQ_HZ: Gyroscope LPF cutoff frequency
 *			- MAGNETOMETER
 *				(param) CUTOFF_FREQ_HZ: Magnetometer LPF cutoff frequency
 *			- BAROMETER
 *				(param) MOVING_AVG_FLT_COUNT: Moving average filter on altitude
 *      	- OPTICAL FLOW:
 *      		(param) MOVING_AVG_FLT_COUNT: Moving average filter on xy flow rates
 *      	- ULTRASONIC DISTANCE:
 *      		(param) MOVING_AVG_FLT_COUNT: Moving average filter on distance
 *
 *      	SENSOR FUSION THREAD ALGORITHM:
 *
 *      	(param) FUSION_FREQ_HZ: Frequency to run sensor fusion algorithm.
 *      	(param) SHOULD_CALIBRATE_MAG: Boolean on whether or not magnetometer calibration should take place on startup.
 *
 *      	- ALTITUDE COMPLEMENTARY FILTER:
 *      		(param) ALTITUDE_ALPHA: Degree of trust placed on barometer altitude vs. ultrasonic sensor for altitude.
 *      	- VELOCITY LEAKY INTEGRATOR:
 *      		(param) First-order settling time (2%) of leaky integrator from integrated XY velocities (from accelerations)
 *      				to absolute camera XY velocities.
  */

#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "usart.h"

#include <cstdio>
#include <cmath>

#include "Threads/sensor_fusion_thread.hpp"

#include "SensorFusion/orient_ekf.hpp"
#include "SensorFusion/alt_comp_filter.hpp"
#include "SensorFusion/vel_leaky_integrator.hpp"

#include "Utility/lock_guard.hpp"

#include "Drivers/usb.hpp"
#include "Drivers/BMI088/bmi088.hpp"
#include "Drivers/BMP388/bmp388.hpp"
#include "Drivers/LIS3MDL/lis3mdl.hpp"
#include "Drivers/US100_Ultrasonic/us100.hpp"
#include "Drivers/PMW3901/pmw3901.hpp"

#define TIMER_PERIOD				1700 				// cycles per count, must match in tim.c
#define TIMER_FREQUENCY				170000000			// cycles per second, MCU timers clock
#define COUNTS_TO_SECONDS			(float)TIMER_PERIOD / TIMER_FREQUENCY

#define FUSION_FREQ_HZ				250					// Frequency to run sensor fusion thread

#define SHOULD_CALIBRATE_MAG		false

/*
 * Sensors
 */
static USB_Logger logger(usbMutexHandle);
static BMI088 imu(&hspi1, spi1MutexHandle, ACCEL_CS_GPIO_Port, GYRO_CS_GPIO_Port, ACCEL_CS_Pin,
		GYRO_CS_Pin, accelDataMutexHandle, gyroDataMutexHandle, logger);
static LIS3MDL magnetometer(&hi2c2, i2c2MutexHandle, magDataMutexHandle, logger);
static BMP388 barometer(&hi2c2, i2c2MutexHandle, baroDataMutexHandle, logger);
static US100 range_finder(&huart2, uart2MutexHandle, ultrasonicDataMutexHandle, logger);
static PMW3901 optical_flow(&hspi1, spi1MutexHandle, FLOW_CS_GPIO_Port, FLOW_CS_Pin, flowDataMutexHandle, logger);

/*
 * Timing
 */
extern volatile unsigned long ulHighFrequencyTimerCounts;
const uint32_t fusion_period_ms = (1.0 / FUSION_FREQ_HZ) * 1000;		// ms

/*
 * Drone state
 */
drone_state_t drone_state;

/*
 *
 * THREADS
 *
 */
void sensor_fusion_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.log("--- SENSOR FUSION THREAD STARTING ---", CRITICAL);
	osDelay(10);

	/*
	 * INITIALIZATION
	 */
	bool imu_init = imu.init();
	bool mag_init = magnetometer.init();
	bool baro_init = barometer.init();
	bool flow_init = optical_flow.init();

	if (!imu_init)  logger.log("SENSOR FUSION THREAD: IMU initialization failed.", ERR);
	if (!mag_init)  logger.log("SENSOR FUSION THREAD: Magnetometer initialization failed.", ERR);
	if (!baro_init) logger.log("SENSOR FUSION THREAD: Barometer initialization failed.", ERR);
	if (!flow_init) logger.log("SENSOR FUSION THREAD: Optical flow sensor initialization failed.", ERR);

	OrientationEKF ekf(imu, magnetometer, logger);
	bool ekf_init = ekf.init();

	AltitudeComplementaryFilter alt_comp_filter(range_finder, barometer);
	VelocityLeakyIntegrator vel_leaky_integrator(optical_flow, imu);

	// Determine sensor fusion feasibility
	bool ekf_okay = imu_init && mag_init && ekf_init;
	bool comp_filter_okay = baro_init && ekf_okay;
	bool leaky_integrator_okay = flow_init && imu_init && comp_filter_okay;

	if (!ekf_okay)
	{
		logger.log("SENSOR FUSION THREAD: Orientation EKF not feasible. Killing thread.", ERR);
		osDelay(10);

		// Delete this thread
		vTaskDelete( NULL );
	}

	if (!comp_filter_okay) logger.log("SENSOR FUSION THREAD: Altitude complementary filter not feasible.", CRITICAL);
	if (!leaky_integrator_okay) logger.log("SENSOR FUSION THREAD: Velocity leaky integrator not feasible.", CRITICAL);


	// Calibrate magnetometer if necessary
	if (SHOULD_CALIBRATE_MAG)
	{
		ekf.calibrate_magnetometer();
	}

	// Start sensor fusion logging thread
//	osThreadFlagsSet(fusionLoggingTaskHandle, 0x00000001U);

	uint32_t wakeup_time = osKernelGetTickCount();
	unsigned long ulLastHighFrequencyTimerCounts = ulHighFrequencyTimerCounts;

	while (1)
	{
		/*
		 * Run Sensor Fusion algorithm
		 */
		wakeup_time += fusion_period_ms;
		osDelayUntil(wakeup_time);

		// Compute delta time since last iteration
		unsigned long delta_counts = ulHighFrequencyTimerCounts - ulLastHighFrequencyTimerCounts;
		ulLastHighFrequencyTimerCounts = ulHighFrequencyTimerCounts;

		float dT = delta_counts * COUNTS_TO_SECONDS;
		if (dT <= 0.0f || dT > 0.1f) dT = 1.0f / FUSION_FREQ_HZ;

		// ORIENTATION KALMAN FILTER

		MFX_output_t data_out = ekf.update(dT);

		// ALTITUDE COMPLIMENTARY FILTER

		float altitude = 0;
		if (comp_filter_okay)
			altitude = alt_comp_filter.update(data_out);

		// VELOCITY LEAKY INTEGRATOR

		std::array<float, 2> xy_velocity{};

		if (leaky_integrator_okay)
			xy_velocity = vel_leaky_integrator.update(data_out, altitude, dT);

		// UPDATE DRONE STATE

		{
			np::lock_guard lock(stateMutexHandle);
			drone_state.rotation[0] = data_out.rotation[2];			// Roll
			drone_state.rotation[1] = data_out.rotation[1];			// Pitch
			drone_state.rotation[2] = data_out.rotation[0];			// Yaw
			drone_state.quaternion[0] = data_out.quaternion[0];		// qx
			drone_state.quaternion[1] = data_out.quaternion[1];		// qy
			drone_state.quaternion[2] = data_out.quaternion[2];		// qz
			drone_state.quaternion[3] = data_out.quaternion[3];		// qw
			drone_state.velocity[0] = xy_velocity[0];				// X velocity
			drone_state.velocity[1] = xy_velocity[1];				// Y velocity
			drone_state.altitude = altitude;
		}

		// PUBLISH DRONE STATE

		osMessageQueuePut(stateQueueHandle, &drone_state, 0, 0);
	}
}

void fusion_logging_thread()
{
	osThreadFlagsWait(0x00000001U, osFlagsWaitAll, osWaitForever);

	logger.log("--- SENSOR FUSION LOGGING THREAD STARTING ---", CRITICAL);
	osDelay(10);

	char state_log[256];
	while (1)
	{
		{
			np::lock_guard lock(stateMutexHandle);
			snprintf(state_log, sizeof(state_log),
					"%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f",
					drone_state.rotation[0], drone_state.rotation[1], drone_state.rotation[2],
					drone_state.quaternion[0], drone_state.quaternion[1], drone_state.quaternion[2], drone_state.quaternion[3],
					drone_state.velocity[0], drone_state.velocity[1],
					drone_state.altitude);
		}
		logger.log(state_log, STATE);		// Log drone state data
		osDelay(50);
	}
}


/*
 *
 * IRQ FUNCTIONS
 *
 */
void service_BMI088_Accel(){ imu.service_irq_accelerometer(); }
void service_BMI088_Gyro() { imu.service_irq_gyroscope(); }
void service_BMP388() { barometer.service_irq(); }
void service_LIS3MDL() {
	magnetometer.service_irq();
	imu.service_irq_temperature();	// Magnetometer low output data rate, so include IMU temperature read here
}
void poll_US100_Ultrasonic(uint8_t start_transfer)
{
	if (start_transfer) {
		range_finder.start_distance_transfer();
	} else {
		range_finder.finish_distance_transfer();
	}
}
void poll_PMW3901(float polling_period_s) { optical_flow.poll(polling_period_s); }


