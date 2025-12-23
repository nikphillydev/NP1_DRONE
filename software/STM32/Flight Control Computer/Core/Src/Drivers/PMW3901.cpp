/*
 * PMW3901.cpp
 *
 *  Created on: Mar 17, 2025
 *      Author: Nikolai Philipenko
 *
 *  Based on PMW3901MB-TXQT: Optical Motion Tracking Chip
 */

#include "Drivers/PMW3901.hpp"
#include "Drivers/usb.hpp"
#include "Utility/lock_guard.hpp"
#include <cstring>
#include <cstdio>


PMW3901::PMW3901(SPI_HandleTypeDef* spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef* cs_port, uint16_t cs_pin, osMutexId_t& data_mutex)
	: spi_handle(spi_handle),
	  spi_mutex(spi_mutex),
	  cs_port(cs_port),
	  cs_pin(cs_pin),
	  data_mutex(data_mutex),
	  delta_x_filter(4),
	  delta_y_filter(4) {}

bool PMW3901::init()
{
	osDelay(100);			// Testing found this delay required on start-up
	bool status = false;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];
	memset(tx_data, 0, sizeof(tx_data));
	memset(rx_data, 0, sizeof(rx_data));

	// Perform power up reset
	tx_data[0] = 0x5A;
	status = write_register(REG_POWER_UP_RESET, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Check PMW3901 product ID and inverse product ID
	rx_data[0] = 0;
	rx_data[1] = 0;

	status = read_register(REG_PRODUCT_ID, rx_data, 1);
	if (!status) return status;
	osDelay(10);

	status = read_register(REG_INVERSE_PRODUCT_ID, rx_data+1, 1);
	if (!status) return status;
	osDelay(10);

	if (rx_data[0] == 0x49 && rx_data[1] == 0xB6)
	{
		USB_Log("Found PMW3901 optical flow sensor, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find PMW3901 optical flow sensor. Initialization failed.", ERR);
		return false;
	}
	osDelay(10);

	// Reading the motion registers one time
	status = read_register(REG_MOTION, rx_data, 1);
	if (!status) return status;
	osDelay(10);
	status = read_register(REG_DELTA_X_L, rx_data, 1);
	if (!status) return status;
	osDelay(10);
	status = read_register(REG_DELTA_X_H, rx_data, 1);
	if (!status) return status;
	osDelay(10);
	status = read_register(REG_DELTA_Y_L, rx_data, 1);
	if (!status) return status;
	osDelay(10);
	status = read_register(REG_DELTA_Y_H, rx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Initialize device registers on startup
	status = initialize_registers();
	if (!status) return status;

	// LED startup sequence
	status = set_led(false);
	if (!status) return status;
	osDelay(50);
	status = set_led(true);
	if (!status) return status;
	osDelay(50);
	status = set_led(false);
	if (!status) return status;
	osDelay(50);
	status = set_led(true);
	if (!status) return status;

	USB_Log("PMW3901 optical flow sensor initialized OK.", CRITICAL);
	osDelay(100);

	return status;
}

bool PMW3901::update()
{
	bool status = false;
	uint8_t rx_data[5];
	memset(rx_data, 0, sizeof(rx_data));

	// Read motion data
	status = read_register(REG_MOTION, rx_data, 1);
	if (!status) return status;
	status = read_register(REG_DELTA_X_H, rx_data+1, 1);
	if (!status) return status;
	status = read_register(REG_DELTA_X_L, rx_data+2, 1);
	if (!status) return status;
	status = read_register(REG_DELTA_Y_H, rx_data+3, 1);
	if (!status) return status;
	status = read_register(REG_DELTA_Y_L, rx_data+4, 1);
	if (!status) return status;

	int16_t x = (rx_data[1] << 8) | rx_data[2];
	int16_t y = (rx_data[3] << 8) | rx_data[4];

	np::lock_guard lock(data_mutex);
	delta_raw[0] = delta_x_filter.update(static_cast<float>(x));
	delta_raw[1] = delta_y_filter.update(static_cast<float>(y));
	return status;
}

std::array<int16_t, 2> PMW3901::get_delta_raw()
{
	np::lock_guard lock(data_mutex);
	return delta_raw;
}

std::array<float, 2> PMW3901::get_delta_m(const float& height_m, const float& pitch_change, const float& roll_change)
{
	std::array<float, 2> delta_m {};

	float expected_X_change = pitch_change * expected_cf;
	float expected_Y_change = roll_change * expected_cf;

	np::lock_guard lock(data_mutex);
	delta_m[0] = (delta_raw[0] + expected_X_change) * height_m * conversion;
	delta_m[1] = (delta_raw[1] - expected_Y_change) * height_m * conversion;

	return delta_m;
}

void PMW3901::log_data_to_gcs()
{
	char string[128];
	{
		np::lock_guard lock(data_mutex);
		snprintf(string, sizeof(string), "PMW3901 %d %d", delta_raw[0], delta_raw[1]);
	}
	USB_Log(string, SENSOR);
}

bool PMW3901::initialize_registers()
{
	// Set optimum performance (see datasheet - no further information given)

	bool status = false;
	uint8_t tx_data[4];
	memset(tx_data, 0, 4);

    tx_data[0] = 0x00;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xAD;
    status = write_register(0x61, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x03;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x00;
    status = write_register(0x40, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x05;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xB3;
    status = write_register(0x41, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xF1;
    status = write_register(0x43, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x14;
    status = write_register(0x45, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x32;
    status = write_register(0x5B, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x34;
    status = write_register(0x5F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x08;
    status = write_register(0x7B, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x06;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x1B;
    status = write_register(0x44, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xBF;
    status = write_register(0x40, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x3F;
    status = write_register(0x4E, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x08;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x20;
    status = write_register(0x65, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x18;
    status = write_register(0x6A, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x09;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xAF;
    status = write_register(0x4F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x40;
    status = write_register(0x5F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x80;
    status = write_register(0x48, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x80;
    status = write_register(0x49, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x77;
    status = write_register(0x57, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x78;
    status = write_register(0x60, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x78;
    status = write_register(0x61, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x08;
    status = write_register(0x62, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x50;
    status = write_register(0x63, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x0A;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x60;
    status = write_register(0x45, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x00;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x11;
    status = write_register(0x4D, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x80;
    status = write_register(0x55, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x1F;
    status = write_register(0x74, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x1F;
    status = write_register(0x75, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x78;
    status = write_register(0x4A, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x78;
    status = write_register(0x4B, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x08;
    status = write_register(0x44, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x50;
    status = write_register(0x45, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xFF;
    status = write_register(0x64, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x1F;
    status = write_register(0x65, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x14;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x60;
    status = write_register(0x65, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x08;
    status = write_register(0x66, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x78;
    status = write_register(0x63, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x15;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x58;
    status = write_register(0x48, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x07;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x0D;
    status = write_register(0x41, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x14;
    status = write_register(0x43, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x0E;
    status = write_register(0x4B, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x0F;
    status = write_register(0x45, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x42;
    status = write_register(0x44, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x80;
    status = write_register(0x4C, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x10;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x02;
    status = write_register(0x5B, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x07;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x41;
    status = write_register(0x40, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x00;
    status = write_register(0x70, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    osDelay(100);

    tx_data[0] = 0x44;
    status = write_register(0x32, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x07;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x40;
    status = write_register(0x40, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x06;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xF0;
    status = write_register(0x62, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x00;
    status = write_register(0x63, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x0D;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xC0;
    status = write_register(0x48, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xD5;
    status = write_register(0x6F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x00;
    status = write_register(0x7F, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xA0;
    status = write_register(0x5B, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0xA8;
    status = write_register(0x4E, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x50;
    status = write_register(0x5A, tx_data, 1);
    if (!status) return status;
    osDelay(10);

    tx_data[0] = 0x80;
    status = write_register(0x40, tx_data, 1);
    if (!status) return status;
    osDelay(10);

	return status;
}

bool PMW3901::set_led(bool on_flag)
{
	// Turn the onboard led ON or OFF

	bool status = false;
	uint8_t tx_data[4];
	memset(tx_data, 0, sizeof(tx_data));

	osDelay(200);

	tx_data[0] = 0x14;
	status = write_register(0x7F, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = on_flag ? 0x1C : 0x00;
	status = write_register(0x6F, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x00;
	status = write_register(0x7F, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	return status;
}

/*
 *
 *  Low-level register read / write
 *
 */
bool PMW3901::read_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	uint8_t rx_buffer[num_bytes];
	memset(tx_buffer, 0, sizeof(tx_buffer));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	tx_buffer[0] = reg_addr | PMW3901_READ;

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_TransmitReceive(spi_handle, tx_buffer, rx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}

	if (status)
	{
		for (int i = 1; i < num_bytes; i++)
		{
			rx_data[i - 1] = rx_buffer[i];
		}
	}
	else
	{
		USB_Log("PMW3901 register read failed.", ERR);
	}

	return status;
}

bool PMW3901::write_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	memset(tx_buffer, 0, sizeof(tx_buffer));

	tx_buffer[0] = reg_addr | PMW3901_WRITE;

	for (int i = 1; i < num_bytes; i++)
	{
		tx_buffer[i] = tx_data[i - 1];
	}

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_Transmit(spi_handle, tx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}

	if (!status)
	{
		USB_Log("PMW3901 register write failed.", ERR);
	}

	return status;
}

