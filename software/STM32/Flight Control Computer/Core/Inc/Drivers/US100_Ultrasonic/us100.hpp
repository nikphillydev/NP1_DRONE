/*
 * us100.hpp
 *
 *  Created on: Mar 8, 2025
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"

#include <cstdint>

#include "Utility/MovingAverageFilter.hpp"
#include "Drivers/usb.hpp"

#define RANGE_MAX_DISTANCE_M		(float)4.5


class US100
{
public:
	US100(UART_HandleTypeDef* uart_handle, osMutexId_t& uart_mutex, osMutexId_t& data_mutex, USB_Logger& logger);

	// DMA interface
	void start_distance_transfer();
	void finish_distance_transfer();

	// Logging
	void log_data_to_gcs();

	// Getters
	float get_distance();

private:
	// UART communication
	UART_HandleTypeDef* uart_handle;
	osMutexId_t& uart_mutex;
	uint8_t rx_buffer_DMA[2];	

	// Ultrasonic data
	osMutexId_t& data_mutex;
	float distance = 0;			// meters

	// Filter for distance data
	const int MOVING_AVG_FLT_COUNT = 4;
	MovingAverageFilter<float> distance_filter;

	// Logger
	USB_Logger& logger;
};
