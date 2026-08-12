/*
 * radio.cpp
 *
 *  Created on: May 28, 2025
 *      Author: Nikolai Philipenko
 *
 *  Packets are received from the CC2500 transceiver.
 *
 *  The heartbeat packet is monitored and signals a good GCS comms link to the control system.
 *  When the GCS heartbeat is lost, then a loss-of-link is signalled to the control system.
 *
 *  All other packets are forwarded to the control system uninterrupted.
 */
#include "main.h"
#include "spi.h"
#include "gpio.h"

#include <cstring>
#include <cstdio>

#include "Threads/radio_thread.hpp"
#include "Drivers/CC2500/cc2500.hpp"
#include "Drivers/usb.hpp"
#include "Radio/message.hpp"
#include "Radio/radio_link.hpp"
#include "constants.hpp"

const int radio_rx_timeout_ms = (1.0 / REQUIRED_GCS_HEARTBEAT_HZ) * 1000;		// ms

/*
 * Sensors
 */
static USB_Logger logger{usbMutexHandle};
static CC2500 transceiver(&hspi1, spi1MutexHandle, CC2500_CS_GPIO_Port, CC2500_CS_Pin, logger);

/*
 *
 * THREADS
 *
 */
void radio_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.log("--- RADIO THREAD STARTING ---", CRITICAL);
	osDelay(10);

	/*
	 * INITIALIZATION
	 */
	bool modem_init = transceiver.init();
	if (!modem_init)
	{
		logger.log("RADIO THREAD: Failed to init modem", ERR);
		osDelay(10);

		// Delete this thread
		vTaskDelete( NULL );
	}

	// Enter receive mode
	while(!transceiver.enter_rx_mode())
	{
		logger.log("CC2500 failed to enter RX mode", ERR);
	}

	const unsigned heartbeat_tick_delta = osKernelGetTickFreq() / REQUIRED_GCS_HEARTBEAT_HZ * HEARTBEAT_RX_TOLERANCE_MULTIPLIER;
	unsigned last_heartbeat_tick = osKernelGetTickCount();

	bool loss_of_link_flag = false;

	/*
	 * MAIN LOOP
	 */
	while (1)
	{
		osStatus_t sem_status = osSemaphoreAcquire(radioRxSemaphoreHandle, radio_rx_timeout_ms);

		// Check GCS heartbeat
		// Runs whether packet received or timeout occured
		if (osKernelGetTickCount() - last_heartbeat_tick > heartbeat_tick_delta)
		{
			if (!loss_of_link_flag)
			{
				// Notify control system of loss of link

				loss_of_link_msg_t msg{};
				msg.loss_of_link = true;
				cc2500_packet_t lol_packet = NP1RadioLink::loss_of_link_msg_pack(msg);
				osMessageQueuePut(radioQueueHandle, &lol_packet, 0, 0);

				logger.log("RADIO THREAD: LOSS-OF-LINK", CRITICAL);
			}
			loss_of_link_flag = true;
		}

		if (sem_status == osOK)
		{
			// Packet ready to be received

			cc2500_packet_t packet;
			cc2500_packet_status_t status;

			if (transceiver.receive_packet(packet, status) && status.crc_ok)
			{
//				char output[128];
//				snprintf(output, sizeof(output), "RADIO RX ID: %d, RSSI: %.2f, LQI: %d, CRC: %s",
//						packet.id, status.rssi, status.lqi, status.crc_ok ? "OK" : "ERROR");
//				logger.log(output, CRITICAL);

				if (packet.id == MSG_ID_HEARTBEAT)
				{
					// Received GCS heartbeat, good communication link

					last_heartbeat_tick = osKernelGetTickCount();

					if (loss_of_link_flag)
					{
						// Notify control system if previously had loss of link

						loss_of_link_msg_t msg{};
						msg.loss_of_link = false;
						cc2500_packet_t lol_packet = NP1RadioLink::loss_of_link_msg_pack(msg);
						osMessageQueuePut(radioQueueHandle, &lol_packet, 0, 0);

						logger.log("RADIO THREAD: LINK RESTORED", CRITICAL);
					}
					loss_of_link_flag = false;
				}
				else
				{
					// Forward all other packets to control system

					osMessageQueuePut(radioQueueHandle, &packet, 0, 0);
				}
			}
			else
			{
				logger.log("CC2500 failed to receive message", ERR);
			}
		}
	}
}

