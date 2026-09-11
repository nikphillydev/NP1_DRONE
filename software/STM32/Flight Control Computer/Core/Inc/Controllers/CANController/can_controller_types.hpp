/*
 * can_controller_types.hpp
 *
 *  Created on: Aug 31, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

/*
 *
 * CANController Types
 *
 */
typedef enum {
	ACTIVE_ERROR,
	NORMAL
} CANBUS_STATE;

typedef enum {
	DISARM = 0,
	ARM = 1,
	SPEED = 2,
	HEARTBEAT = 3,
} CANBUS_MSG;

const uint32_t CANBUS_DELAY_MS_TX_FIFO_FULL = 1;
