/*
 * PMW3901.hpp
 *
 *  Created on: Mar 17, 2025
 *      Author: Nikolai Philipenko
 *
 *  Please see calculations in
 *  	https://ardupilot.org/copter/docs/common-mouse-based-optical-flow-sensor-adns3080.html
 *  for optical flow sensor readings to translational distance measurements.
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include "Utility/MovingAverageFilter.hpp"
#include <array>
#include <cmath>

/* PMW3901 REGISTERS */
#define REG_PRODUCT_ID                  0x00
#define REG_REVISION_ID                 0x01
#define REG_MOTION                      0x02
#define REG_DELTA_X_L                   0x03
#define REG_DELTA_X_H                   0x04
#define REG_DELTA_Y_L                   0x05
#define REG_DELTA_Y_H                   0x06
#define REG_SQUAL                       0x07        
#define REG_RAW_DATA_SUM                0x08
#define REG_MAXIMUM_RAW_DATA            0x09
#define REG_MINIMUM_RAW_DATA            0x0A
#define REG_SHUTTER_LOWER               0x0B
#define REG_SHUTTER_UPPER               0x0C
#define REG_OBSERVATION                 0x15
#define REG_MOTION_BURST                0x16
#define REG_POWER_UP_RESET              0x3A
#define REG_SHUTDOWN                    0x3B
#define REG_RAW_DATA_GRAB               0x58
#define REG_RAW_DATA_GRAB_STATUS        0x59
#define REG_INVERSE_PRODUCT_ID          0x5F

/* ACCESS TYPES */
#define PMW3901_READ					0x00
#define PMW3901_WRITE					0x80

/* CONSTANTS AND SCALERS */
#define FOV_DEG							42
#define RES_PIX							30
#define MOTION_SCALER					8		// Determined experimentally
#define CORRECTION_SCALER				29		// Determined experimentally


class PMW3901
{
public:
	PMW3901(SPI_HandleTypeDef* spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef* cs_port, uint16_t cs_pin, osMutexId_t& data_mutex);

	// Initialization after startup
	[[nodiscard]] bool init();

	// Polling interface
	bool update();

	// Logging
	void log_data_to_gcs();

	// Getters
	std::array<int16_t, 2> get_delta_raw();
	std::array<float, 2> get_delta_m(const float& height_m, const float& pitch_change, const float& roll_change);

private:
	// Low-level register read / write
	[[nodiscard]] bool read_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
	[[nodiscard]] bool write_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

	// On init helpers
	[[nodiscard]] bool initialize_registers();
	[[nodiscard]] bool set_led(bool on);

	// SPI communication
	SPI_HandleTypeDef* spi_handle;
	osMutexId_t& spi_mutex;
	GPIO_TypeDef* cs_port;
	uint16_t cs_pin;

	// Conversion constants
	const float conversion = 2 * tanf(FOV_DEG / 2) / (RES_PIX * MOTION_SCALER);
	const float expected_cf = RES_PIX * CORRECTION_SCALER / FOV_DEG;

	// Optical flow data
	std::array<int16_t, 2> delta_raw {};		// camera delta x, y

	osMutexId_t& data_mutex;

	// Filter for delta x,y data
	MovingAverageFilter<float> delta_x_filter;
	MovingAverageFilter<float> delta_y_filter;
};
