/*
 * gdelivery_radio.cpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#include "Radio/GaraunteedDelivery/gdelivery_radio.hpp"


bool GaraunteedDeliveryRadio::init()
{
	if (!transmitter.init()) return false;

	if (!transmitter.enter_tx_mode()) return false;

	return true;
}

void GaraunteedDeliveryRadio::transmit(const cc2500_packet_t& packet)
{
	bool transmit_ok = transmitter.transmit_packet(packet);

	uint32_t retry_count = 0;
	while (!transmit_ok && retry_count < MAX_RETRY_COUNT)
	{
		logger.error("GaraunteedDeliveryRadio: RADIO transmit failure, retrying...");

		transmit_ok = transmitter.transmit_packet(packet);
		retry_count++;
	}

	if (!transmit_ok)
	{
		logger.error("GaraunteedDeliveryRadio: Failed to transmit packet.");
	}
}
