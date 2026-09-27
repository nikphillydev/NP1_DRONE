/*
 * can_message.hpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include <cstdint>


enum class CanbusMsgID : uint8_t
{
	Disarm 		= 0,
	Arm 		= 1,
	Speed 		= 2,
	Heartbeat 	= 3,
};
