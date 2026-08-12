/*
 * lis3mdl.hpp
 *
 *  Created on: Dec 17, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"

#include <cstdint>
#include <array>
#include <memory>

#include "Utility/IIRFilter.hpp"
#include "Drivers/usb.hpp"

/* I2C ADDRESS */
#define LIS3MDL_ADDRESS				0x1C


struct LIS3MDL_CalibrationData
{
	/* HARD IRON OFFSETS (gauss) */
	const float x_offset = -0.2884;
	const float y_offset = -0.2835;
	const float z_offset = -0.1110;
};

class LIS3MDL
{
public:
	LIS3MDL(I2C_HandleTypeDef* i2c_handle, osMutexId_t& i2c_mutex, osMutexId_t& mag_data_mutex, USB_Logger& logger);

	// Initialization after startup
	[[nodiscard]] bool init();

	// IRQ service and logging
	bool service_irq();
	void log_data_to_gcs();

	// Getters
	std::array<float, 3> get_axis_intensities();

private:
	// Low-level register read / write
	[[nodiscard]] bool read_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
	[[nodiscard]] bool write_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

	// I2C communication
	I2C_HandleTypeDef* i2c_handle;
	osMutexId_t& i2c_mutex;

    // Calibration
    LIS3MDL_CalibrationData calib_data;

	// Conversion constant
	const float LSB_per_gauss = 6842.0; 		// Based on +-4G range

	// Magnetometer data
	osMutexId_t& mag_data_mutex;
	std::array<float, 3> axis_intensities {};	// Current magnetic intensities [x, y, z] (gauss)

	// IIR Filters
	const float CUTOFF_FREQ_HZ = 10.0f;
	std::array<std::unique_ptr<IIRFilter>, 3> filters {};

	// Logger
	USB_Logger& logger;
};


