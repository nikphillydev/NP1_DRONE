/*
 * us100.cpp
 *
 *  Created on: Mar 8, 2025
 *      Author: Nikolai Philipenko
 */

#include <cstdio>

#include "Drivers/US100_Ultrasonic/us100.hpp"
#include "Utility/lock_guard.hpp"
#include "Drivers/usb.hpp"


US100::US100(UART_HandleTypeDef* uart_handle, osMutexId_t& uart_mutex, osMutexId_t& data_mutex, USB_Logger& logger)
	: uart_handle(uart_handle),
	  uart_mutex(uart_mutex),
	  data_mutex(data_mutex),
	  distance_filter(MOVING_AVG_FLT_COUNT),
	  logger(logger) {}

void US100::start_distance_transfer()
{
	uint8_t tx_buffer[1] = { 0x55 };	// Request distance
	np::lock_guard lock(uart_mutex);
	HAL_UART_Receive_DMA(uart_handle, rx_buffer_DMA, sizeof(rx_buffer_DMA));
	HAL_UART_Transmit_DMA(uart_handle, tx_buffer, 1);
}

void US100::finish_distance_transfer()
{
	uint16_t distance_mm = (rx_buffer_DMA[0] << 8) | rx_buffer_DMA[1];

	np::lock_guard lock(data_mutex);
	distance = distance_filter.update(static_cast<float>(distance_mm) / 1000.0f);
	if (distance > RANGE_MAX_DISTANCE_M)
	{
		distance = RANGE_MAX_DISTANCE_M;
	}
	else if (distance < 0)
	{
		distance = 0;
	}
}

void US100::log_data_to_gcs()
{
	char string[128];
	{
		np::lock_guard lock(data_mutex);
		snprintf(string, sizeof(string), "US100 %.2f", distance);
	}
	logger.log(string, SENSOR);
}

float US100::get_distance()
{
	np::lock_guard lock(data_mutex);
	return distance;
}

