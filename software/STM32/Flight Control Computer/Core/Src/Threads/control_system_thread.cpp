/*
 * control_system_thread.cpp
 *
 *  Created on: Aug 24, 2026
 *      Author: Nikolai Philipenko
 *
 *  Control system for the NP1 Drone.
 */
#include "main.h"
#include "fdcan.h"
#include "fcc_topics.hpp"
#include "constants.hpp"

#include "Threads/control_system_thread.hpp"

#include "Radio/radio_message.hpp"
#include "Radio/radio_link.hpp"

#include "Canbus/GaraunteedDelivery/gdelivery_can.hpp"

#include "Drivers/CC2500/cc2500.hpp"
#include "Logger/usb_serial_port.hpp"
#include "Logger/logger.hpp"

/*
 * DEFINES
 */
#define CONTROL_SYSTEM_FREQ			100		// Frequency to run control system thread
#define DRONE_STATE_TIMEOUT_MS		50		// Timeout in ms for invalid (stale) drone state

/*
 * GLOBAL
 */
static USBSerialPort serial_port(usbMutexHandle);
static Logger logger(serial_port);
static GaraunteedDeliveryCAN canbus{&hfdcan1, logger};

/*
 * TIMING
 */
const uint32_t system_period_ms = 1.0f / CONTROL_SYSTEM_FREQ * 1000.0f;

/*
 *
 * THREADS
 *
 */
void control_system_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.info("--- CONTROL SYSTEM THREAD STARTING ---");
	osDelay(10);

	// Setpoints
	uint16_t throttle = 0;

	// ESC heartbeat
	const unsigned esc_heartbeat_tick_delta = osKernelGetTickFreq() / constants::REQUIRED_ESC_HEARTBEAT_HZ;
	unsigned esc_last_heartbeat_tick = osKernelGetTickCount();

	uint32_t wakeup_time = osKernelGetTickCount();

	/*
	 * RUN THE CONTROL SYSTEM
	 */
	while (1)
	{
		wakeup_time += system_period_ms;
		osDelayUntil(wakeup_time);

		// -------------------------
		// Maintain ESC heartbeat
		// -------------------------
		if (osKernelGetTickCount() - esc_last_heartbeat_tick > esc_heartbeat_tick_delta)
		{
			canbus.transmit_heartbeat();
			esc_last_heartbeat_tick = osKernelGetTickCount();
		}

		// -------------------------
		// Receive radio packet
		// -------------------------
		cc2500_packet_t rx_packet{};
		osStatus_t osStatus = osMessageQueueGet(radioQueueHandle, &rx_packet, NULL, 0);		// try semantics

		if (osStatus == osOK)
		{
			switch (static_cast<RadioMsgID>(rx_packet.id))
			{
				case RadioMsgID::Heartbeat:
				{
					logger.error("CONTROL SYSTEM: Received HEARTBEAT message.");
					break;
				}
				case RadioMsgID::LossOfLink:
				{
					LossOfLinkMsg msg;
					RadioLink::loss_of_link_msg_decode(rx_packet, msg);
					if (msg.loss_of_link)
					{
						logger.warn("CONTROL SYSTEM: LOSS-OF-LINK.");
						canbus.transmit_disarm();
					}
					else
					{
						logger.warn("CONTROL SYSTEM: LINK RESTORED.");
					}
					break;
				}
				case RadioMsgID::ArmDisarm:
				{
					ArmDisarmMsg msg;
					RadioLink::arm_disarm_msg_decode(rx_packet, msg);
					if (msg.armed)
					{
						logger.info("CONTROL SYSTEM: Arming drone.");
						canbus.transmit_arm();
					}
					else
					{
						logger.info("CONTROL SYSTEM: Disarming drone.");
						canbus.transmit_disarm();
					}
					break;
				}
				case RadioMsgID::Angle:
				{
					logger.error("CONTROL SYSTEM: Received ANGLE message.");
					break;
				}
				case RadioMsgID::Throttle:
				{
					ThrottleMsg msg;
					RadioLink::throttle_msg_decode(rx_packet, msg);
					throttle = msg.throttle;

					logger.info("CONTROL SYSTEM: Received new throttle: {}", msg.throttle);
					break;
				}
			}
		}

		// -------------------------
		// Get drone state
		// -------------------------
		drone_state_t state{};
		bool stateStatus = state_topic.receive(state);

		bool timeout = stateStatus && (osKernelGetTickCount() - state.timestamp > DRONE_STATE_TIMEOUT_MS);
		if (!stateStatus || timeout)
		{
			if (!stateStatus)
			{
				logger.error("CONTROL SYSTEM: No drone state received.");
			}
			else
			{
				logger.error("CONTROL SYSTEM: Drone state invalid. Timeout occured.");
			}
			canbus.transmit_disarm();
			continue;
		}

		// -------------------------
		// Run controller
		// -------------------------
		canbus.transmit_speed(throttle);
	}
}

