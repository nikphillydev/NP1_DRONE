/*
 * gdelivery_can.hpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "fdcan.h"
#include "Logger/logger.hpp"
#include "Canbus/GaraunteedDelivery/gdelivery_can_types.hpp"


/*
 * Class to support garaunteed delivery CANBUS communication for the NP1 drone.
 */
class GaraunteedDeliveryCAN {
public:
	GaraunteedDeliveryCAN(FDCAN_HandleTypeDef *hfdcan, Logger &logger) : hfdcan(hfdcan), logger(logger) {};

	void transmit_heartbeat();
	void transmit_arm();
	void transmit_disarm();
	void transmit_speed(uint16_t speed);

private:
	FDCAN_HandleTypeDef *hfdcan;
	Logger &logger;

	// Configuration
	const uint32_t CAN_TX_FIFO_FULL_DELAY_MS 	= 1;
	const uint32_t MAX_RETRY_COUNT 				= 1000;

	CanbusState get_canbus_state();
	void transmit(const FDCAN_TxHeaderTypeDef* header, const uint8_t* tx_data);
};


