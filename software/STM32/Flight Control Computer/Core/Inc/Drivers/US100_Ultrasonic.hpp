/*
 * US100_Ultrasonic.hpp
 *
 *  Created on: Mar 8, 2025
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include "Utility/MovingAverageFilter.hpp"
#include <cstdint>

#define MAX_DISTANCE_M			(float)4.5


class US100_Ultrasonic
{
public:
	US100_Ultrasonic(UART_HandleTypeDef* uart_handle, osMutexId_t& uart_mutex, osMutexId_t& data_mutex);

	// DMA interface
	void start_distance_transfer();
	void finish_distance_transfer();

	// Logging
	void log_data_to_gcs();

	// Getters
	float get_distance();
	int	get_temperature();

private:
	// UART communication
	UART_HandleTypeDef* uart_handle;
	osMutexId_t& uart_mutex;
	uint8_t rx_buffer_DMA[4];

	// Ultrasonic data
	float distance;			// meters
	int temperature;		// deg C

	osMutexId_t& data_mutex;

	// Filter for distance data
	MovingAverageFilter<float> distance_filter;
};
