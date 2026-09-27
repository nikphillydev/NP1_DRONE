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
	cc2500_packet_t heartbeat_msg_pack();

	cc2500_packet_t loss_of_link_msg_pack(const LossOfLinkMsg& tx_msg);
	void loss_of_link_msg_decode(const cc2500_packet_t& packet, LossOfLinkMsg& rx_msg);

	cc2500_packet_t arm_disarm_msg_pack(const ArmDisarmMsg& tx_msg);
	void arm_disarm_msg_decode(const cc2500_packet_t& packet, ArmDisarmMsg& rx_msg);

	cc2500_packet_t angle_msg_pack(const AngleMsg& tx_msg);
	void angle_msg_decode(const cc2500_packet_t& packet, AngleMsg& rx_msg);

	cc2500_packet_t throttle_msg_pack(const ThrottleMsg& tx_msg);
	void throttle_msg_decode(const cc2500_packet_t& packet, ThrottleMsg& rx_msg);
};






