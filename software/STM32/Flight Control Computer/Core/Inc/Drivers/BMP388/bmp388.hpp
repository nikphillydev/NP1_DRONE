/*
 * bmp388.hpp
 *
 *  Created on: Dec 16, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"

#include "Utility/MovingAverageFilter.hpp"
#include "Drivers/usb.hpp"

/* I2C ADDRESS */
#define BMP388_ADDRESS			0x76


struct BMP388_CalibrationData
{
	float par_t1;
	float par_t2;
	float par_t3;
	float par_p1;
	float par_p2;
	float par_p3;
	float par_p4;
	float par_p5;
	float par_p6;
	float par_p7;
	float par_p8;
	float par_p9;
	float par_p10;
	float par_p11;
};

class BMP388
{
public:
	BMP388(I2C_HandleTypeDef* i2c_handle, osMutexId_t& i2c_mutex, osMutexId_t& baro_data_mutex, USB_Logger& logger);

	// Initialization after startup
	[[nodiscard]] bool init();

	// IRQ service and logging
	bool service_irq();
	void log_data_to_gcs();

	// Getters
	float get_pressure();
	float get_altitude();
	float get_temperature();

private:
	// Low-level register read / write
	[[nodiscard]] bool read_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
	[[nodiscard]] bool write_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

	// On init helpers
	[[nodiscard]] bool read_calibration_nvm();
	[[nodiscard]] bool compute_startup_pressure();

	// I2C communication
	I2C_HandleTypeDef* i2c_handle;
	osMutexId_t& i2c_mutex;

	// Calibration
	BMP388_CalibrationData calib_data;

	// Barometer data
	osMutexId_t& baro_data_mutex;
	float pressure;				// Current atmospheric pressure (Pa)
	float startup_pressure;		//								(Pa)
	float altitude; 			// Based on startup_pressure 	(meters)
	float temperature;			// Current temperature 			(degC)

	// Filter for altitude data
	const int MOVING_AVG_FLT_COUNT = 4;
	MovingAverageFilter<float> alt_filter;

	// Logger
	USB_Logger& logger;
};

