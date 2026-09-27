/*
 * garaunteed_delivery_radio.hpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "Drivers/CC2500/cc2500.hpp"
#include "Logger/logger.hpp"


/*
 * Class to support garaunteed delivery RADIO communication for the NP1 drone.
 */
class GaraunteedDeliveryRadio {
public:
	GaraunteedDeliveryRadio(CC2500& transmitter, Logger& logger) : transmitter(transmitter), logger(logger) {}

	[[nodiscard]] bool init();

	void transmit(const cc2500_packet_t& packet);

private:
	CC2500& transmitter;
	Logger& logger;

	// Configuration
	const uint32_t MAX_RETRY_COUNT 		= 1000;
};
