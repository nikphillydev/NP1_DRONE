/*
 * CC2500.cpp
 *
 *  Created on: May 25, 2025
 *      Author: nikolai
 */

#include "CC2500.hpp"
#include "Drivers/usb.hpp"
#include <cstring>

CC2500::CC2500(SPI_HandleTypeDef *spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef *cs_port, uint16_t cs_pin, osMutexId_t& status_mutex, uint8_t packet_length, uint8_t address)
	: spi_handle(spi_handle),
	  spi_mutex(spi_mutex),
	  cs_port(cs_port),
	  cs_pin(cs_pin),
	  status_mutex(status_mutex),
	  packet_length(packet_length),
	  address(address) {}

bool CC2500::init()
{
	/**
	 * Register values were determined with the TI SmartRF program
	 */

	bool status = false;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];
	memset(tx_data, 0, sizeof(tx_data));
	memset(rx_data, 0, sizeof(rx_data));

	// Reset device
	status = command_strobe(CMD_SRES, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	// Wait till CC2500 chip ready
	while (chip_ready == false)
	{
		status = command_strobe(CMD_SNOP, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
		if (!status) return status;
		osDelay(100);
	}

	// Check CC2500 chip ID
	rx_data[0] = 0;
	status = read_register(REG_PARTNUM, rx_data, 1);
	if (status && rx_data[0] == 0x80)
	{
		USB_Log("Found CC2500 RF transceiver, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find CC2500 RF transceiver. Initialization failed.", ERR);
		return false;
	}
	osDelay(10);

	// GDO0 as interrupt: asserts on sync, deasserts on end of packet
	tx_data[0] = 0x06;
	status = write_register(REG_IOCFG0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Sync word: 0xBEEF
	tx_data[0] = 0xBE;
	status = write_register(REG_SYNC1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Sync word: 0xBEEF
	tx_data[0] = 0xEF;
	status = write_register(REG_SYNC0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Fixed packet length: (packet length + 1) for the address at byte 0
	tx_data[0] = packet_length + 1;
	status = write_register(REG_PKTLEN, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Packet control: CRC autoflush, append status bytes, strict address check
	tx_data[0] = 0x0D;
	status = write_register(REG_PKTCTRL1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Packet control: data whitening, CRC enabled, fixed packet length mode
	tx_data[0] = 0x44;
	status = write_register(REG_PKTCTRL0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// RX / TX Address
	tx_data[0] = address;
	status = write_register(REG_ADDR, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Channel: 127
	tx_data[0] = 0x7F;
	status = write_register(REG_CHANNR, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x0C;
	status = write_register(REG_FSCTRL1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x5C;
	status = write_register(REG_FREQ2, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0xF6;
	status = write_register(REG_FREQ1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x27;
	status = write_register(REG_FREQ0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x0E;
	status = write_register(REG_MDMCFG4, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x3B;
	status = write_register(REG_MDMCFG3, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Modem configuration: MSK, 30/32 sync bits detected
	tx_data[0] = 0x73;
	status = write_register(REG_MDMCFG2, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Modem configuration: FEC, 8 byte minimum preamble
	tx_data[0] = 0xC2;
	status = write_register(REG_MDMCFG1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x00;
	status = write_register(REG_DEVIATN, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Radio state machine: stay in TX mode after sending a packet, stay in RX mode after receiving
	tx_data[0] = 0x0E;
	status = write_register(REG_MCSM1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x18;
	status = write_register(REG_MCSM0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x1D;
	status = write_register(REG_FOCCFG, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x1C;
	status = write_register(REG_BSCFG, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0xC7;
	status = write_register(REG_AGCCTRL2, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0xB0;
	status = write_register(REG_AGCCTRL0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0xB6;
	status = write_register(REG_FREND1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0xEA;
	status = write_register(REG_FSCAL3, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x00;
	status = write_register(REG_FSCAL1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	tx_data[0] = 0x19;
	status = write_register(REG_FSCAL0, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Output power: +1dBm (the maximum possible)
	tx_data[0] = 0xFF;
	status = write_register(REG_PATABLE, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	USB_Log("CC2500 RF transceiver initialized OK.", CRITICAL);
	osDelay(100);

	return status;
}

bool CC2500::enter_rx_mode()
{
	bool status = false;

	USB_Log("CC2500 entering RECEIVE mode", CRITICAL);

	// Command RX mode
	status = command_strobe(CMD_SRX, CC2500_STATUS_UPDATE::RX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	while (state != CC2500_STATE::RX)
	{
		// Command RX mode
		status = command_strobe(CMD_SRX, CC2500_STATUS_UPDATE::RX_FIFO_BYTES);
		if (!status) return status;
		osDelay(10);
	}

	return status;
}

bool CC2500::enter_tx_mode()
{
	bool status = false;

	USB_Log("CC2500 entering TRANSMIT mode", CRITICAL);

	// Command TX mode
	status = command_strobe(CMD_STX, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	while (state != CC2500_STATE::TX)
	{
		// Command TX mode
		status = command_strobe(CMD_STX, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
		if (!status) return status;
		osDelay(10);
	}

	return status;
}

bool CC2500::transmit_packet(uint8_t tx_data[])
{
	bool status = false;

	// Wait for empty TX FIFO
	uint8_t num_bytes = 0xFF;
	status = read_register(REG_TXBYTES, &num_bytes, 1);
	if (!status) return status;
	while (num_bytes != 0)
	{
		osDelay(10);
		status = read_register(REG_TXBYTES, &num_bytes, 1);
		if (!status) return status;
	}

	// Send the address
	uint8_t tx_address = address;
	status = write_register(REG_TX_FIFO, &tx_address, 1);
	if (!status) return status;

	// Send the payload
	for (int i = 0; i < packet_length; i++)
	{
		status = write_register(REG_TX_FIFO, &tx_data[i], 1);
		if (!status) return status;
	}

	// Check for TX FIFO underflow

	// Update CC2500 state
	status = command_strobe(CMD_SNOP, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;

	if (state == CC2500_STATE::TXFIFO_UNDERFLOW)
	{
		status = flush_tx_fifo();
		if (!status) return status;
		status = enter_tx_mode();
		if (!status) return status;
	}

	return true;
}

uint8_t CC2500::receive_packet(uint8_t rx_data[])
{
	bool status = false;

	// Read number of bytes in RX FIFO
	uint8_t byte_count = 0;
	status = read_register(REG_RXBYTES, &byte_count, 1);
	if (!status) return status;

	// Check for RX FIFO overflow
	if (byte_count >> 7)
	{
		status = flush_rx_fifo();
		if (!status) return status;
		status = enter_rx_mode();
		if (!status) return status;
		return 0;		// No received bytes
	}

	// Extract received packet
	for (int i = 0; i < byte_count; i++)
	{
		status = read_register(REG_RX_FIFO, &rx_data[i], 1);
		if (!status) return status;
	}

	// Re-enter RX mode
	status = enter_rx_mode();
	if (!status) return status;

	return byte_count;
}

bool CC2500::flush_rx_fifo()
{
	bool status = false;

	USB_Log("CC2500 flushing RX FIFO", CRITICAL);

	// Flush the receive FIFO
	status = command_strobe(CMD_SFRX, CC2500_STATUS_UPDATE::RX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	return status;
}

bool CC2500::flush_tx_fifo()
{
	bool status = false;

	USB_Log("CC2500 flushing TX FIFO", CRITICAL);

	// Flush the transmit FIFO
	status = command_strobe(CMD_SFTX, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	return status;
}

/*
 *
 *  Low-level register read / write
 *
 */
bool CC2500::command_strobe(uint8_t strobe, CC2500_STATUS_UPDATE status_update)
{
	bool status = false;

	// Check command strobe validity
	if (strobe < 0x30 || strobe > 0x3D)
	{
		USB_Log("CC2500 command strobe register does not exist.", ERR);
		return status;
	}

	// Received status byte from CC2500
	uint8_t status_byte = 0;

	switch (status_update) {
		case CC2500_STATUS_UPDATE::NONE:
			{
				np::lock_guard lock(spi_mutex);
				HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
				status = (HAL_SPI_Transmit(spi_handle, &strobe, 1, HAL_MAX_DELAY) == HAL_OK);
				HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
			}
			break;
		case CC2500_STATUS_UPDATE::RX_FIFO_BYTES:
			{
				strobe |= CC2500_READ;
				{
					np::lock_guard lock(spi_mutex);
					HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
					status = (HAL_SPI_TransmitReceive(spi_handle, &strobe, &status_byte, 1, HAL_MAX_DELAY) == HAL_OK);
					HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
				}
			}
			break;
		case CC2500_STATUS_UPDATE::TX_FIFO_BYTES:
			{
				strobe |= CC2500_WRITE;
				{
					np::lock_guard lock(spi_mutex);
					HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
					status = (HAL_SPI_TransmitReceive(spi_handle, &strobe, &status_byte, 1, HAL_MAX_DELAY) == HAL_OK);
					HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
				}
			}
			break;
		default:
			USB_Log("CC2500 invalid command strobe state update.", ERR);
			break;
	}

	if (!status)
	{
		USB_Log("CC2500 command strobe write failed.", ERR);
	}
	else if (status_update == CC2500_STATUS_UPDATE::RX_FIFO_BYTES || status_update == CC2500_STATUS_UPDATE::TX_FIFO_BYTES)
	{
		np::lock_guard lock(status_mutex);
		chip_ready = ((status_byte & CC2500_CHIP_RDY_BITMASK) >> CC2500_CHIP_RDY_SHIFT) == 0;
		state = static_cast<CC2500_STATE>((status_byte & CC2500_STATE_BITMASK) >> CC2500_STATE_SHIFT);
		if (status_update == CC2500_STATUS_UPDATE::RX_FIFO_BYTES) {
			rx_fifo_bytes_available = (status_byte & CC2500_FIFO_BYTES_BITMASK) >> CC2500_FIFO_BYTES_SHIFT;
		}
		else if (status_update == CC2500_STATUS_UPDATE::TX_FIFO_BYTES) {
			tx_fifo_bytes_free = (status_byte & CC2500_FIFO_BYTES_BITMASK) >> CC2500_FIFO_BYTES_SHIFT;
		}
	}

	return status;
}

bool CC2500::write_register(uint8_t reg, uint8_t *tx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	memset(tx_buffer, 0, sizeof(tx_buffer));

	// Check if status register
	if (reg >= 0x30 && reg <= 0x3D)
	{
		USB_Log("CC2500 status registers can only be read.", ERR);
		return status;
	}

	tx_buffer[0] = reg | CC2500_WRITE;

	if (data_len > 1)
		tx_buffer[0] |= CC2500_BURST;

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
		USB_Log("CC2500 register write failed.", ERR);
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

	tx_buffer[0] = reg | CC2500_READ;

	// Check if register is a status register
	bool is_status_reg = false;
	if (reg >= 0x30 && reg <= 0x3D)
		is_status_reg = true;

	if (data_len > 1 && is_status_reg)
	{
		USB_Log("CC2500 status registers can only be read one at a time.", ERR);
		return status;
	}
	else if (data_len > 1 || is_status_reg)
	{
		// For status registers, the burst bit must be 1
		tx_buffer[0] |= CC2500_BURST;
	}

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
		USB_Log("CC2500 register read failed.", ERR);
	}

	return status;
}

