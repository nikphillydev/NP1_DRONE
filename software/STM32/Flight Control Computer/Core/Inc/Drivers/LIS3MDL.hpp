/*
 * LIS3MDL.hpp
 *
 *  Created on: Dec 17, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "Utility/IIRFilter.hpp"
#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include <cstdint>
#include <array>
#include <memory>


/* LIS3MDL REGISTERS */
#define REG_OFFSET_X_REG_L_M		0x05
#define REG_OFFSET_X_REG_H_M		0x06
#define REG_OFFSET_Y_REG_L_M		0x07
#define REG_OFFSET_Y_REG_H_M		0x08
#define REG_OFFSET_Z_REG_L_M		0x09
#define REG_OFFSET_Z_REG_H_M		0x0A
#define REG_WHO_AM_I				0x0F
#define REG_CTRL_REG1				0x20
#define REG_CTRL_REG2				0x21
#define REG_CTRL_REG3				0x22
#define REG_CTRL_REG4				0x23
#define REG_CTRL_REG5				0x24
#define REG_STATUS_REG				0x27
#define REG_OUT_X_L					0x28
#define REG_OUT_X_H					0x29
#define REG_OUT_Y_L					0x2A
#define REG_OUT_Y_H					0x2B
#define REG_OUT_Z_L					0x2C
#define REG_OUT_Z_H					0x2D
#define REG_TEMP_OUT_L				0x2E
#define REG_TEMP_OUT_H				0x2F
#define REG_INT_CFG					0x30
#define REG_INT_SRC					0x31
#define REG_INT_THS_L				0x32
#define REG_INT_THS_H				0x33

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
	LIS3MDL(I2C_HandleTypeDef* i2c_handle, osMutexId_t& i2c_mutex, osMutexId_t& mag_data_mutex);

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
	const float LSB_per_gauss = 6842.0; 		/* Based on +-4G range */

	// Magnetometer data mutex
	osMutexId_t& mag_data_mutex;

	// Magnetometer data
	std::array<float, 3> axis_intensities {};	// Current magnetic intensities [x, y, z] (gauss)

	// IIR Filters
	std::array<std::unique_ptr<IIRFilter>, 3> filters {};
};


















