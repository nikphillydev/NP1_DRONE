/*
 * radio_link.cpp
 *
 *  Created on: Jul 21, 2026
 *      Author: Nikolai Philipenko
 */

#include "Radio/radio_link.hpp"


cc2500_packet_t RadioLink::heartbeat_msg_pack()
{
	cc2500_packet_t packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::Heartbeat);

	return packet;
}

cc2500_packet_t RadioLink::loss_of_link_msg_pack(const LossOfLinkMsg& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::LossOfLink);
	packet.payload[0] = static_cast<uint8_t>(tx_msg.loss_of_link);

	return packet;
}

void RadioLink::loss_of_link_msg_decode(const cc2500_packet_t& packet, LossOfLinkMsg& rx_msg)
{
	rx_msg.loss_of_link =  static_cast<bool>(packet.payload[0]);
}

cc2500_packet_t RadioLink::arm_disarm_msg_pack(const ArmDisarmMsg& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::ArmDisarm);
	packet.payload[0] = static_cast<uint8_t>(tx_msg.armed);

	return packet;
}

void RadioLink::arm_disarm_msg_decode(const cc2500_packet_t& packet, ArmDisarmMsg& rx_msg)
{
	rx_msg.armed =  static_cast<bool>(packet.payload[0]);
}

cc2500_packet_t RadioLink::angle_msg_pack(const AngleMsg& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::Angle);
	packet.payload[0] = static_cast<uint8_t>((tx_msg.angle >> 8) & 0xFF);
	packet.payload[1] = static_cast<uint8_t>(tx_msg.angle & 0xFF);

	return packet;
}

void RadioLink::angle_msg_decode(const cc2500_packet_t& packet, AngleMsg& rx_msg)
{
	rx_msg.angle = static_cast<uint16_t>(packet.payload[0] << 8) | static_cast<uint16_t>(packet.payload[1]);
}

cc2500_packet_t RadioLink::throttle_msg_pack(const ThrottleMsg& tx_msg)
{
	cc2500_packet_t packet{};
	packet.id = static_cast<uint8_t>(RadioMsgID::Throttle);
	packet.payload[0] = static_cast<uint8_t>((tx_msg.throttle >> 8) & 0xFF);
	packet.payload[1] = static_cast<uint8_t>(tx_msg.throttle & 0xFF);

	return packet;
}

void RadioLink::throttle_msg_decode(const cc2500_packet_t& packet, ThrottleMsg& rx_msg)
{
	rx_msg.throttle = static_cast<uint16_t>(packet.payload[0] << 8) | static_cast<uint16_t>(packet.payload[1]);
}

