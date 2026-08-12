/*
 * radio_link.hpp
 *
 *  Created on: Jun 29, 2026
 *      Author: Nikolai Philipenko
 * 
 * 	NP1 Drone Over-The-Air (OTA) message encode / decode functions for transmission.
 * 	Pseudo-modelled after MAVLINK 2.0
 */
#pragma once

#include "Drivers/CC2500/cc2500_types.h"
#include "Radio/message.hpp"

namespace NP1RadioLink
{
	cc2500_packet_t heartbeat_msg_pack();

	cc2500_packet_t loss_of_link_msg_pack(const loss_of_link_msg_t& tx_msg);
	bool loss_of_link_msg_decode(const cc2500_packet_t& packet, loss_of_link_msg_t& rx_msg);

	cc2500_packet_t arm_disarm_msg_pack(const arm_disarm_msg_t& tx_msg);
	bool arm_disarm_msg_decode(const cc2500_packet_t& packet, arm_disarm_msg_t& rx_msg);

	cc2500_packet_t angle_msg_pack(const angle_msg_t& tx_msg);
	bool angle_msg_decode(const cc2500_packet_t& packet, angle_msg_t& rx_msg);

	cc2500_packet_t throttle_msg_pack(const throttle_msg_t& tx_msg);
	bool throttle_msg_decode(const cc2500_packet_t& packet, throttle_msg_t& rx_msg);
};






