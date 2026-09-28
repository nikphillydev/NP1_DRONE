/*
 * radio_thread.cpp
 *
 *  Created on: May 28, 2025
 *      Author: Nikolai Philipenko
 *
 *  Packets are received from the CC2500 transceiver, decoded, and sent to the control system.
 *
 *  The heartbeat packet is monitored for loss-of-link with the ground station.
 */
#include "main.h"
#include "spi.h"
#include "gpio.h"

#include <cstring>
#include <cstdio>

#include "Threads/radio_thread.hpp"
#include "Drivers/CC2500/cc2500.hpp"
#include "Logger/usb_serial_port.hpp"
#include "Logger/logger.hpp"
#include "Radio/radio_message.hpp"
#include "Radio/radio_link.hpp"
#include "constants.hpp"

/*
 * GLOBAL
 */
static USBSerialPort serial_port(usbMutexHandle);
static Logger logger(serial_port);
static CC2500 receiver(&hspi1, spi1MutexHandle, CC2500_CS_GPIO_Port, CC2500_CS_Pin, logger);

/*
 * TIMING
 */
const int radio_rx_timeout_ms = (1.0 / constants::REQUIRED_GCS_HEARTBEAT_HZ) * 1000;		// ms

/*
 *
 * THREADS
 *
 */
void radio_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.info("--- RADIO THREAD STARTING ---");
	osDelay(10);

	/*
	 * INITIALIZATION
	 */
	if (!receiver.init())
	{
		logger.error("RADIO THREAD: Failed to init modem");
		// Delete this thread
		vTaskDelete( NULL );
	}

	// Enter receive mode
	while(!receiver.enter_rx_mode())
	{
		logger.error("CC2500 failed to enter RX mode");
	}

	// GCS heartbeat (receive)
	const unsigned gcs_heartbeat_tick_delta = osKernelGetTickFreq() / constants::REQUIRED_GCS_HEARTBEAT_HZ * constants::HEARTBEAT_RX_TOLERANCE_MULTIPLIER;
	unsigned gcs_last_heartbeat_tick = osKernelGetTickCount();

	// Radio Input
	RadioInput radio_input{};
	radio_input.loss_of_link = true;
	osMessageQueuePut(radioQueueHandle, &radio_input, 0, 0);

	/*
	 * RECEIVE MESSAGES FROM GCS
	 */
	while (1)
	{
		osStatus_t sem_status = osSemaphoreAcquire(radioRxSemaphoreHandle, radio_rx_timeout_ms);

		// Check GCS heartbeat
		if (osKernelGetTickCount() - gcs_last_heartbeat_tick > gcs_heartbeat_tick_delta)
		{
			radio_input.loss_of_link = true;

			// Send to control system
			osMessageQueuePut(radioQueueHandle, &radio_input, 0, 0);
		}

		if (sem_status == osOK)
		{
			// Packet ready to be received

			cc2500_packet_t packet;
			cc2500_packet_status_t status;

			if (receiver.receive_packet(packet, status) && status.crc_ok)
			{
//				logger.log("RADIO RX ID: {}, RSSI: {}, LQI: {}, CRC: {}",
//						packet.id, status.rssi, status.lqi, status.crc_ok ? "OK" : "ERROR");

				switch (static_cast<RadioMsgID>(packet.id))
				{
					case RadioMsgID::Heartbeat:
					{
						gcs_last_heartbeat_tick = osKernelGetTickCount();
						radio_input.loss_of_link = false;
						break;
					}
					case RadioMsgID::ArmDisarm:
					{
						ArmDisarmMsg msg;
						RadioLink::arm_disarm_msg_decode(packet, msg);
						radio_input.armed = msg.armed;

						logger.info("RADIO THREAD: Received armed: {}", radio_input.armed);
						break;
					}
					case RadioMsgID::Angle:
					{
						break;
					}
					case RadioMsgID::Throttle:
					{
						ThrottleMsg msg;
						RadioLink::throttle_msg_decode(packet, msg);
						radio_input.throttle = msg.throttle;

						logger.info("RADIO THREAD: Received new throttle {}", radio_input.throttle);
						break;
					}
				}

				// Send to control system
				osMessageQueuePut(radioQueueHandle, &radio_input, 0, 0);
			}
			else
			{
				logger.error("RADIO THREAD: Failed to receive message");
			}
		}
	}
}

