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
#include "Drivers/CC2500/cc2500_types.h"
#include "Drivers/usb.hpp"


class CC2500 {
public:
	CC2500(SPI_HandleTypeDef *spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef *cs_port, uint16_t cs_pin, USB_Logger& logger);

	[[nodiscard]] bool init();

	[[nodiscard]] bool enter_rx_mode();
	[[nodiscard]] bool enter_tx_mode();

	[[nodiscard]] bool transmit_packet(const cc2500_packet_t &packet);
	[[nodiscard]] bool receive_packet(cc2500_packet_t &packet, cc2500_packet_status_t &packet_status);

private:
	[[nodiscard]] bool flush_rx_fifo();
	[[nodiscard]] bool flush_tx_fifo();

	// Low-level register read / write
	[[nodiscard]] bool command_strobe(uint8_t strobe, CC2500_STATUS_UPDATE status_update);
	[[nodiscard]] bool write_register(uint8_t reg, uint8_t *tx_data, uint16_t data_len);
	[[nodiscard]] bool read_register(uint8_t reg, uint8_t *rx_data, uint16_t data_len);

	// SPI communication
	SPI_HandleTypeDef *spi_handle;
	osMutexId_t& spi_mutex;
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;

	// CC2500 status
	cc2500_status_t chip_status;

	// Logger
	USB_Logger& logger;
};
