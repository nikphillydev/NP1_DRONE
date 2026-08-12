/*
 * radio_link.cpp
 *
 *  Created on: Jul 21, 2026
 *      Author: Nikolai Philipenko
 */

#include "Radio/radio_link.hpp"


cc2500_packet_t NP1RadioLink::heartbeat_msg_pack()
{
	cc2500_packet_t packet{};
	packet.id = MSG_ID_HEARTBEAT;

	return packet;
}

cc2500_packet_t NP1RadioLink::loss_of_link_msg_pack(const loss_of_link_msg_t& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = MSG_ID_LOSS_OF_LINK;
	packet.payload[0] = static_cast<uint8_t>(tx_msg.loss_of_link);

	return packet;
}

bool NP1RadioLink::loss_of_link_msg_decode(const cc2500_packet_t& packet, loss_of_link_msg_t& rx_msg)
{
	if (packet.id != MSG_ID_LOSS_OF_LINK)
	{
		return false;
	}

	rx_msg.loss_of_link =  static_cast<bool>(packet.payload[0]);
	return true;
}

cc2500_packet_t NP1RadioLink::arm_disarm_msg_pack(const arm_disarm_msg_t& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = MSG_ID_CMD_ARM_DISARM;
	packet.payload[0] = static_cast<uint8_t>(tx_msg.armed);

	return packet;
}

bool NP1RadioLink::arm_disarm_msg_decode(const cc2500_packet_t& packet, arm_disarm_msg_t& rx_msg)
{
	if (packet.id != MSG_ID_CMD_ARM_DISARM)
	{
		return false;
	}

	rx_msg.armed =  static_cast<bool>(packet.payload[0]);
	return true;
}

cc2500_packet_t NP1RadioLink::angle_msg_pack(const angle_msg_t& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = MSG_ID_CMD_ANGLE;
	packet.payload[0] = static_cast<uint8_t>((tx_msg.angle >> 8) & 0xFF);
	packet.payload[1] = static_cast<uint8_t>(tx_msg.angle & 0xFF);

	return packet;
}

bool NP1RadioLink::angle_msg_decode(const cc2500_packet_t& packet, angle_msg_t& rx_msg)
{
	if (packet.id != MSG_ID_CMD_ANGLE)
	{
		return false;
	}

	rx_msg.angle = static_cast<uint16_t>(packet.payload[0] << 8) | static_cast<uint16_t>(packet.payload[1]);
	return true;
}

cc2500_packet_t NP1RadioLink::throttle_msg_pack(const throttle_msg_t& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = MSG_ID_CMD_THROTTLE;
	packet.payload[0] = static_cast<uint8_t>((tx_msg.throttle >> 8) & 0xFF);
	packet.payload[1] = static_cast<uint8_t>(tx_msg.throttle & 0xFF);

	return packet;
}

bool NP1RadioLink::throttle_msg_decode(const cc2500_packet_t& packet, throttle_msg_t& rx_msg)
{
	if (packet.id != MSG_ID_CMD_THROTTLE)
	{
		return false;
	}

	rx_msg.throttle = static_cast<uint16_t>(packet.payload[0] << 8) | static_cast<uint16_t>(packet.payload[1]);
	return true;
}

