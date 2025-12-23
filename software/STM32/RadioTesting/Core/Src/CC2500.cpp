/*
 * CC2500.cpp
 *
 *  Created on: May 25, 2025
 *      Author: nikolai
 */

#include "CC2500.hpp"

#include <cstring>

CC2500::CC2500(SPI_HandleTypeDef *spi_handle, GPIO_TypeDef *cs_port, uint16_t cs_pin)
	: spi_handle(spi_handle),
	  cs_port(cs_port),
	  cs_pin(cs_pin) {}

bool CC2500::init()
{
	bool status = false;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];
	memset(tx_data, 0, sizeof(tx_data));
	memset(rx_data, 0, sizeof(rx_data));

	// Reset device
	status = command_strobe(CMD_SRES);
	if (!status) return status;

	tx_data[0] = 0x69;
	status = write_register(REG_ADDR, tx_data, 1);
	if (!status) return status;

	status = read_register(REG_ADDR, rx_data, 1);
	if (!status) return status;

	return status;
}

/*
 *
 *  Low-level register read / write
 *
 */
bool CC2500::command_strobe(uint8_t strobe)
{
	bool status = false;

	// For command strobes, the burst bit must be 0
	strobe &= ~CC2500_BURST;

	{
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_Transmit(spi_handle, &strobe, 1, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}

	if (!status)
	{
		// TODO log error
	}

	return status;
}

bool CC2500::write_register(uint8_t reg, uint8_t *tx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	memset(tx_buffer, 0, sizeof(tx_buffer));

	tx_buffer[0] = reg | CC2500_WRITE | CC2500_BURST;
//	if (data_len > 1) tx_buffer[0] |= CC2500_BURST;

	for (int i = 1; i < num_bytes; i++)
	{
		tx_buffer[i] = tx_data[i - 1];
	}

	{
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_Transmit(spi_handle, tx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}

	if (!status)
	{
		// TODO log error
	}

	return status;
}

bool CC2500::read_register(uint8_t reg, uint8_t *rx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	uint8_t rx_buffer[num_bytes];
	memset(tx_buffer, 0, sizeof(tx_buffer));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	tx_buffer[0] = reg | CC2500_READ | CC2500_BURST;
//	if (data_len > 1) tx_buffer[0] |= CC2500_BURST;

	{
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
		// TODO log error
	}

	return status;
}

