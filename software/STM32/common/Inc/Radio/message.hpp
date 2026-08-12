/*
 * message.hpp
 *
 *  Created on: Jul 23, 2026
 *      Author: Nikolai Philipenko
 *
 *  NP1 Drone Over-The-Air (OTA) message definitions
 */

#pragma once

// --------------------------------------------
// HEARTBEAT MESSAGE
// --------------------------------------------
const int MSG_ID_HEARTBEAT		= 0;

// --------------------------------------------
// LOSS-OF-LINK MESSAGE
// --------------------------------------------
const int MSG_ID_LOSS_OF_LINK	= 1;
struct loss_of_link_msg_t
{
	bool loss_of_link;
};

// --------------------------------------------
// ARM / DISARM MESSAGE
// --------------------------------------------
const int MSG_ID_CMD_ARM_DISARM	= 5;
struct arm_disarm_msg_t
{
	bool armed;
};

// --------------------------------------------
// ANGLE MESSAGE
// --------------------------------------------
const int MSG_ID_CMD_ANGLE		= 6;
struct angle_msg_t
{
	uint16_t angle;
};

// --------------------------------------------
// THROTTLE MESSAGE
// --------------------------------------------
const int MSG_ID_CMD_THROTTLE	= 7;
struct throttle_msg_t
{
	uint16_t throttle;
};


