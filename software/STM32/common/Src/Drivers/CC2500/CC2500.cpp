/*
 * CC2500.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Nikolai Philipenko
 *
 *  Optimum register values were determined with the TI SmartRF program.
 */

#include <cstring>
#include <cstdio>

#include "Drivers/CC2500/cc2500.hpp"
#include "Utility/lock_guard.hpp"

#define CC2500_TX_PACKET_LENGTH			PACKET_LENGTH + 1		// packet + address byte


CC2500::CC2500(SPI_HandleTypeDef *spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef *cs_port, uint16_t cs_pin, Logger& logger)
	: spi_handle(spi_handle),
	  spi_mutex(spi_mutex),
	  cs_port(cs_port),
	  cs_pin(cs_pin),
	  logger(logger) {}

bool CC2500::init()
{
	bool status = false;

	// Temporary buffers
	uint8_t tx_data[4]{};
	uint8_t rx_data[4]{};

	// Reset device
	status = command_strobe(CMD_SRES, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	// Wait till CC2500 chip ready
	while (chip_status.chip_ready == false)
	{
		logger.warn("Waiting for CC2500 to start-up...");
		status = command_strobe(CMD_SNOP, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
		if (!status) return status;
		osDelay(100);
	}
	logger.info("CC2500 start-up OK.");

	// Check CC2500 chip ID
	rx_data[0] = 0;
	status = read_register(REG_PARTNUM, rx_data, 1);
	if (status && rx_data[0] == 0x80)
	{
		logger.info("Found CC2500 RF transceiver, starting initialization.");
	}
	else
	{
		logger.error("Failed to find CC2500 RF transceiver. Initialization failed.");
		return false;
	}
	osDelay(10);

	// GDO2 as interrupt: asserts on sync, deasserts on end of packet
	tx_data[0] = 0x06;
	status = write_register(REG_IOCFG2, tx_data, 1);
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

	// Packet length validity check
	if (CC2500_TX_PACKET_LENGTH > CC2500_FIFO_SIZE) {
		logger.error("CC2500 invalid packet length {}. Packet length must be less than {}", PACKET_LENGTH, CC2500_FIFO_SIZE);
		return false;
	}

	// Fixed packet length: (packet length + 1) for the address at byte 0
	tx_data[0] = CC2500_TX_PACKET_LENGTH;
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
	tx_data[0] = ADDRESS;
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

	// Modem configuration: Forward Error Correction (FEC), 8 byte minimum preamble
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

	// Update internal driver state
	status = command_strobe(CMD_SNOP, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	logger.info("CC2500 RF transceiver initialized OK.");
	osDelay(100);

	return status;
}

bool CC2500::enter_rx_mode()
{
	logger.info("CC2500 entering RECEIVE mode");

	if (chip_status.state == CC2500_STATE::RX)
	{
		return true;
	}

	bool status = false;

	// Command RX mode
	status = command_strobe(CMD_SRX, CC2500_STATUS_UPDATE::RX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	while (chip_status.state != CC2500_STATE::RX)
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
	logger.info("CC2500 entering TRANSMIT mode");

	if (chip_status.state == CC2500_STATE::TX)
	{
		return true;
	}

	bool status = false;

	// Command TX mode
	status = command_strobe(CMD_STX, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	while (chip_status.state != CC2500_STATE::TX)
	{
		// Command TX mode
		status = command_strobe(CMD_STX, CC2500_STATUS_UPDATE::TX_FIFO_BYTES);
		if (!status) return status;
		osDelay(10);
	}

	return status;
}

bool CC2500::transmit_packet(const cc2500_packet_t &packet)
{
	bool status = false;

	uint8_t reg_txbytes = 0;
	bool txfifo_underflow = false;
	uint8_t num_txbytes = 0;

	// Initial check for TX FIFO underflow
	status = read_register(REG_TXBYTES, &reg_txbytes, 1);
	if (!status) return status;

	txfifo_underflow = (reg_txbytes & CC2500_TXBYTES_UNDERFLOW_BITMASK) >> CC2500_TXBYTES_UNDERFLOW_SHIFT;
	while (txfifo_underflow)
	{
		logger.warn("CC2500 TX FIFO underflow...");

		// Recover from TX FIFO underflow
		status = flush_tx_fifo();
		if (!status) return status;
		status = enter_tx_mode();
		if (!status) return status;

		status = read_register(REG_TXBYTES, &reg_txbytes, 1);
		if (!status) return status;

		txfifo_underflow = (reg_txbytes & CC2500_TXBYTES_UNDERFLOW_BITMASK) >> CC2500_TXBYTES_UNDERFLOW_SHIFT;
	}

	// Wait for space in TX FIFO
	num_txbytes = (reg_txbytes & CC2500_TXBYTES_NUM_TXBYTES_BITMASK) >> CC2500_TXBYTES_NUM_TXBYTES_SHIFT;
	uint8_t remaining_txbytes = CC2500_FIFO_SIZE - num_txbytes;

	while (remaining_txbytes < CC2500_TX_PACKET_LENGTH)		// need space for packet + address
	{
		logger.warn("CC2500 TX FIFO waiting for space...");
		osDelay(1);

		status = read_register(REG_TXBYTES, &reg_txbytes, 1);
		if (!status) return status;

		num_txbytes = (reg_txbytes & CC2500_TXBYTES_NUM_TXBYTES_BITMASK) >> CC2500_TXBYTES_NUM_TXBYTES_SHIFT;
		remaining_txbytes = CC2500_FIFO_SIZE - num_txbytes;
	}

	// Send the CC2500 address
	uint8_t tx_address = ADDRESS;
	status = write_register(REG_TX_FIFO, &tx_address, 1);
	if (!status) return status;

	// Encode and send the packet
	uint8_t tx_data[PACKET_LENGTH];
	tx_data[0] = packet.id;
	memcpy(&tx_data[1], packet.payload, PAYLOAD_LENGTH);

	status = write_register(REG_TX_FIFO, tx_data, PACKET_LENGTH);
	if (!status) return status;

	// Final check for TX FIFO underflow
	status = read_register(REG_TXBYTES, &reg_txbytes, 1);
	if (!status) return status;

	txfifo_underflow = (reg_txbytes & CC2500_TXBYTES_UNDERFLOW_BITMASK) >> CC2500_TXBYTES_UNDERFLOW_SHIFT;
	if (txfifo_underflow)
	{
		logger.error("CC2500 TX FIFO underflow. Transmit packet failed.");

		// Message failed to send
		status = flush_tx_fifo();
		if (!status) return status;
		status = enter_tx_mode();
		if (!status) return status;

		return false;
	}

	return true;
}

bool CC2500::receive_packet(cc2500_packet_t &packet, cc2500_packet_status_t &packet_status)
{
	bool status = false;

	uint8_t reg_rxbytes = 0;
	bool rxfifo_overflow = false;
	uint8_t num_rxbytes = 0;

	// Initial check for RX FIFO overflow
	status = read_register(REG_RXBYTES, &reg_rxbytes, 1);
	if (!status) return status;

	rxfifo_overflow = (reg_rxbytes & CC2500_RXBYTES_OVERFLOW_BITMASK) >> CC2500_RXBYTES_OVERFLOW_SHIFT;
	if (rxfifo_overflow)
	{
		logger.error("CC2500 RX FIFO overflow. Receive packet failed.");

		// Recover from RX FIFO overflow
		status = flush_rx_fifo();
		if (!status) return status;
		status = enter_rx_mode();
		if (!status) return status;

		return false;
	}

	// Extract received packet
	num_rxbytes = (reg_rxbytes & CC2500_RXBYTES_NUM_RXBYTES_BITMASK) >> CC2500_RXBYTES_NUM_RXBYTES_SHIFT;

	if (num_rxbytes >= CC2500_TX_PACKET_LENGTH + 2) 		// packet + address + status
	{
		// Extract CC2500 address byte
		uint8_t address_byte = 0;
		status = read_register(REG_RX_FIFO, &address_byte, 1);
		if (!status) return status;

		if (address_byte != ADDRESS) {
			logger.error("CC2500 incorrect RX packet address. Receive packet failed.");
			return false;
		}

		// Extract and decode the packet
		uint8_t rx_data[PACKET_LENGTH];
		status = read_register(REG_RX_FIFO, rx_data, PACKET_LENGTH);
		if (!status) return status;

		packet.id = rx_data[0];
		memcpy(packet.payload, &rx_data[1], PAYLOAD_LENGTH);

		// Extract received packet status bytes
		uint8_t packet_status_bytes[2];
		status = read_register(REG_RX_FIFO, packet_status_bytes, 2);
		if (!status) return status;

		uint8_t raw_rssi = packet_status_bytes[0];
		if (raw_rssi >= 128)
		{
			packet_status.rssi = (raw_rssi - 256.0) / 2.0 - 72.0;
		}
		else
		{
			packet_status.rssi = raw_rssi / 2.0 - 72.0;
		}
		packet_status.lqi = (packet_status_bytes[1] & CC2500_RX_PACKET_LQI_BITMASK) >> CC2500_RX_PACKET_LQI_SHIFT;
		packet_status.crc_ok = (packet_status_bytes[1] & CC2500_RX_PACKET_CRC_OK_BITMASK) >> CC2500_RX_PACKET_CRC_OK_SHIFT;

		return true;
	}

	logger.error("CC2500 RX FIFO not enough bytes for full packet. Receive packet failed.");
	return false;
}

bool CC2500::flush_rx_fifo()
{
	bool status = false;

	logger.warn("CC2500 flushing RX FIFO");

	// Flush the receive FIFO
	status = command_strobe(CMD_SFRX, CC2500_STATUS_UPDATE::RX_FIFO_BYTES);
	if (!status) return status;
	osDelay(10);

	return status;
}

bool CC2500::flush_tx_fifo()
{
	bool status = false;

	logger.warn("CC2500 flushing TX FIFO");

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
	if (strobe < CMD_SRES || strobe > CMD_SNOP)
	{
		logger.error("CC2500 command strobe does not exist.");
		return false;
	}

	switch (status_update) {
		case CC2500_STATUS_UPDATE::RX_FIFO_BYTES:
			strobe |= CC2500_READ;
			break;
		case CC2500_STATUS_UPDATE::TX_FIFO_BYTES:
			strobe |= CC2500_WRITE;
			break;
	}

	// Received status byte from CC2500
	uint8_t status_byte = 0;

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_TransmitReceive(spi_handle, &strobe, &status_byte, 1, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}

	if (status)
	{
		chip_status.chip_ready = ((status_byte & CC2500_STATUS_CHIP_RDY_BITMASK) >> CC2500_STATUS_CHIP_RDY_SHIFT) == 0;
		chip_status.state = static_cast<CC2500_STATE>((status_byte & CC2500_STATUS_STATE_BITMASK) >> CC2500_STATUS_STATE_SHIFT);
		chip_status.fifo_bytes_available = (status_byte & CC2500_STATUS_FIFO_BYTES_BITMASK) >> CC2500_STATUS_FIFO_BYTES_SHIFT;

//		logger.info("CC2500 Status: Chip Ready: {}, State: {}, Bytes: {}",
//				chip_status.chip_ready, chip_status.state, chip_status.fifo_bytes_available);
	}
	else
	{
		logger.error("CC2500 command strobe write failed.");
	}

	return status;
}

bool CC2500::write_register(uint8_t reg, uint8_t *tx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes]{};

	// Check if status register
	if (REG_PARTNUM <= reg && reg <= REG_RCCTRL0_STATUS)
	{
		logger.error("CC2500 status registers can only be read.");
		return false;
	}

	tx_buffer[0] = reg | CC2500_WRITE;

	if (data_len > 1)
	{
		// For burst writes, the burst bit must be 1
		tx_buffer[0] |= CC2500_BURST;
	}

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
		logger.error("CC2500 register write failed.");
	}

	return status;
}

bool CC2500::read_register(uint8_t reg, uint8_t *rx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes]{};
	uint8_t rx_buffer[num_bytes]{};

	tx_buffer[0] = reg | CC2500_READ;

	// Check if register is a status register
	bool is_status_reg = false;
	if (REG_PARTNUM <= reg && reg <= REG_RCCTRL0_STATUS)
	{
		is_status_reg = true;
	}

	if (data_len > 1 && is_status_reg)
	{
		logger.error("CC2500 status registers can only be read one at a time.");
		return false;
	}
	else if (data_len > 1 || is_status_reg)
	{
		// For status registers and burst reads, the burst bit must be 1
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
		logger.error("CC2500 register read failed.");
	}

	return status;
}

