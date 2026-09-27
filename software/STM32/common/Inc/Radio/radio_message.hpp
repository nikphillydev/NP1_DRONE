/*
 * radio_message.hpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 *
 *  NP1 Drone Over-The-Air (OTA) message definitions
 */
#pragma once

#include <cstdint>


enum class RadioMsgID : uint8_t
{
	Heartbeat 	= 0,
	LossOfLink 	= 1,
	ArmDisarm 	= 5,
	Angle 		= 6,
	Throttle 	= 7,
};

// --------------------------------------------
// LOSS-OF-LINK MESSAGE
// --------------------------------------------
struct LossOfLinkMsg
{
	bool loss_of_link;
};

// --------------------------------------------
// ARM / DISARM MESSAGE
// --------------------------------------------
struct ArmDisarmMsg
{
	bool armed;
};

// --------------------------------------------
// ANGLE MESSAGE
// --------------------------------------------
struct AngleMsg
{
	uint16_t angle;
};

// --------------------------------------------
// THROTTLE MESSAGE
// --------------------------------------------
struct ThrottleMsg
{
	uint16_t throttle;
};
