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

#include "Drivers/usb.hpp"


/*
 * Class to support CANBUS operations for the NP1 drone.
 */
class CANController {
public:
	CANController(FDCAN_HandleTypeDef *hfdcan, USB_Logger &logger) : hfdcan(hfdcan), logger(logger) {};

	void send_heartbeat();
	void send_arm();
	void send_disarm();
	void send_speed(uint16_t speed);

private:
	CANBUS_STATE get_canbus_state();

	FDCAN_HandleTypeDef *hfdcan;
	USB_Logger &logger;
};
