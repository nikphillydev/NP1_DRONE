/*
 * can_controller.hpp
 *
 *  Created on: Aug 30, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "main.h"
#include "fdcan.h"
#include "can_controller_types.hpp"

#include "Drivers/Logger/logger.hpp"


class CANController {
	/*
	 * Class to support CANBUS operations for the NP1 drone.
	 */
public:
	CANController(FDCAN_HandleTypeDef *hfdcan, Logger &logger) : hfdcan(hfdcan), logger(logger) {};

	bool send_heartbeat();
	bool send_arm();
	bool send_disarm();
	bool send_speed(uint16_t speed);

private:
	FDCAN_HandleTypeDef *hfdcan;
	Logger &logger;

	CANBUS_STATE get_canbus_state();
};
