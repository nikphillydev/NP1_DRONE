/*
 * CC2500.hpp
 *
 *  Created on: May 25, 2025
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "spi.h"
#include "cmsis_os.h"

#include "Drivers/CC2500/cc2500_regs.hpp"
#include "Drivers/CC2500/cc2500_types.hpp"
#include "Logger/logger.hpp"


class CC2500 {
public:
	CC2500(SPI_HandleTypeDef *spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef *cs_port, uint16_t cs_pin, Logger& logger);

	[[nodiscard]] bool init();

	[[nodiscard]] bool enter_rx_mode();
	[[nodiscard]] bool enter_tx_mode();

	[[nodiscard]] bool transmit_packet(const CC2500_Packet &packet);
	[[nodiscard]] bool receive_packet(CC2500_Packet &packet, CC2500_PacketStatus &packet_status);

private:
	// Low-level register read / write
	[[nodiscard]] bool command_strobe(uint8_t strobe, CC2500_StatusUpdate status_update);
	[[nodiscard]] bool write_register(uint8_t reg, uint8_t *tx_data, uint16_t data_len);
	[[nodiscard]] bool read_register(uint8_t reg, uint8_t *rx_data, uint16_t data_len);

	[[nodiscard]] bool flush_rx_fifo();
	[[nodiscard]] bool flush_tx_fifo();

	// SPI communication
	SPI_HandleTypeDef *spi_handle;
	osMutexId_t& spi_mutex;
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;

	// CC2500 status
	CC2500_Status chip_status;

	// Logger
	Logger& logger;
};
