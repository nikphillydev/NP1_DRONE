/*
 * pmw3901.hpp
 *
 *  Created on: Mar 17, 2025
 *      Author: Nikolai Philipenko
 * 
 *  Flow rate calculation based off PMW3901 driver from the PX4-Autopilot.
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"

#include <array>
#include <cmath>

#include "Utility/MovingAverageFilter.hpp"
#include "Drivers/usb.hpp"


class PMW3901
{
public:
	PMW3901(SPI_HandleTypeDef* spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef* cs_port, uint16_t cs_pin,
			osMutexId_t& data_mutex, USB_Logger& logger);

	// Initialization after startup
	[[nodiscard]] bool init();

	// Polling interface
	bool poll(float deltatime_s);

	// Logging
	void log_data_to_gcs();

	// Getters
	std::array<float, 2> get_flow_rate();

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

	// Optical flow data
	osMutexId_t& data_mutex;
	std::array<float, 2> flow_rate {};		// camera [x,y] flow rates [rad/s]

	// Filter for delta x,y data
	const int MOVING_AVG_FLT_COUNT = 2;
	MovingAverageFilter<float> rate_x_filter;
	MovingAverageFilter<float> rate_y_filter;

	// Logger
	USB_Logger& logger;
};
