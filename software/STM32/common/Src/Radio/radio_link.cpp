/*
 * radio_link.cpp
 *
 *  Created on: Jul 21, 2026
 *      Author: Nikolai Philipenko
 */

#include "Radio/radio_link.hpp"


CC2500_Packet RadioLink::heartbeat_msg_pack()
{
	CC2500_Packet packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::Heartbeat);

	return packet;
}

CC2500_Packet RadioLink::arm_disarm_msg_pack(const ArmDisarmMsg& tx_msg)
{
	CC2500_Packet packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::ArmDisarm);
	packet.payload[0] = static_cast<uint8_t>(tx_msg.armed);

	return packet;
}

void RadioLink::arm_disarm_msg_decode(const CC2500_Packet& packet, ArmDisarmMsg& rx_msg)
{
	rx_msg.armed =  static_cast<bool>(packet.payload[0]);
}

CC2500_Packet RadioLink::angle_msg_pack(const AngleMsg& tx_msg)
{
	CC2500_Packet packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::Angle);
	packet.payload[0] = static_cast<uint8_t>((tx_msg.angle >> 8) & 0xFF);
	packet.payload[1] = static_cast<uint8_t>(tx_msg.angle & 0xFF);

	return packet;
}

void RadioLink::angle_msg_decode(const CC2500_Packet& packet, AngleMsg& rx_msg)
{
	rx_msg.angle = static_cast<uint16_t>(packet.payload[0] << 8) | static_cast<uint16_t>(packet.payload[1]);
}

CC2500_Packet RadioLink::throttle_msg_pack(const ThrottleMsg& tx_msg)
{
	CC2500_Packet packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::Throttle);
	packet.payload[0] = static_cast<uint8_t>((tx_msg.throttle >> 8) & 0xFF);
	packet.payload[1] = static_cast<uint8_t>(tx_msg.throttle & 0xFF);

	return packet;
}

void RadioLink::throttle_msg_decode(const CC2500_Packet& packet, ThrottleMsg& rx_msg)
{
	rx_msg.throttle = static_cast<uint16_t>(packet.payload[0] << 8) | static_cast<uint16_t>(packet.payload[1]);
}

