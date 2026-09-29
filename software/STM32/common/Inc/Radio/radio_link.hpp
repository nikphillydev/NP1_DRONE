/*
 * radio_link.hpp
 *
 *  Created on: Jun 29, 2026
 *      Author: Nikolai Philipenko
 * 
 * 	NP1 Drone Over-The-Air (OTA) message encode / decode functions for transmission / reception.
 * 	Pseudo-modelled after MAVLINK 2.0
 */
#pragma once

#include "Drivers/CC2500/cc2500_types.hpp"
#include "Radio/radio_message.hpp"

namespace RadioLink
{
	CC2500_Packet heartbeat_msg_pack();

	CC2500_Packet arm_disarm_msg_pack(const ArmDisarmMsg& tx_msg);
	void arm_disarm_msg_decode(const CC2500_Packet& packet, ArmDisarmMsg& rx_msg);

	CC2500_Packet angle_msg_pack(const AngleMsg& tx_msg);
	void angle_msg_decode(const CC2500_Packet& packet, AngleMsg& rx_msg);

	CC2500_Packet throttle_msg_pack(const ThrottleMsg& tx_msg);
	void throttle_msg_decode(const CC2500_Packet& packet, ThrottleMsg& rx_msg);
};






