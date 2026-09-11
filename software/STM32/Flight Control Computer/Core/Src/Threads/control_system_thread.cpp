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

#include "Radio/message.hpp"
#include "Radio/radio_link.hpp"

#include "Controllers/CANController/can_controller.hpp"

#include "Drivers/usb.hpp"
#include "Drivers/CC2500/cc2500.hpp"

#define CONTROL_SYSTEM_FREQ			100		// Frequency to run control system thread
#define DRONE_STATE_TIMEOUT_MS		50		// Timeout for invalid (old) drone state

/*
 * Sensors
 */
static USB_Logger logger{usbMutexHandle};
static CANController canbus{&hfdcan1, logger};

/*
 * Timing
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
	logger.log("--- CONTROL SYSTEM THREAD STARTING ---", CRITICAL);
	osDelay(10);

	// Setpoints
	uint16_t throttle = 0;

	// ESC heartbeat
	const unsigned heartbeat_tick_delta = osKernelGetTickFreq() / REQUIRED_ESC_HEARTBEAT_HZ;
	unsigned last_heartbeat_tick = osKernelGetTickCount();

	uint32_t wakeup_time = osKernelGetTickCount();

	while (1)
	{
		/*
		 * Run Control System
		 */
		wakeup_time += system_period_ms;
		osDelayUntil(wakeup_time);

		// -------------------------
		// Maintain ESC heartbeat
		// -------------------------
		if (osKernelGetTickCount() - last_heartbeat_tick > heartbeat_tick_delta)
		{
			canbus.send_heartbeat();
			last_heartbeat_tick = osKernelGetTickCount();
		}

		// -------------------------
		// Receive radio packet
		// -------------------------
		cc2500_packet_t rx_packet{};
		osStatus_t osStatus = osMessageQueueGet(radioQueueHandle, &rx_packet, NULL, 0);		// try semantics

		if (osStatus == osOK)
		{
			switch (rx_packet.id) {
				case MSG_ID_LOSS_OF_LINK:
				{
					loss_of_link_msg_t msg;
					if (NP1RadioLink::loss_of_link_msg_decode(rx_packet, msg))
					{
						if (msg.loss_of_link)
						{
							logger.log("CONTROL SYSTEM: LOSS-OF-LINK.", ERR);
							canbus.send_disarm();
						}
						else
						{
							logger.log("CONTROL SYSTEM: LINK RESTORED.", CRITICAL);
						}
					}

					break;
				}
				case MSG_ID_CMD_ARM_DISARM:
				{
					arm_disarm_msg_t msg;
					if (NP1RadioLink::arm_disarm_msg_decode(rx_packet, msg))
					{
						if (msg.armed)
						{
							logger.log("CONTROL SYSTEM: Arming drone.", INFO);
							canbus.send_arm();
						}
						else
						{
							logger.log("CONTROL SYSTEM: Disarming drone.", INFO);
							canbus.send_disarm();
						}
					}

					break;
				}
				case MSG_ID_CMD_ANGLE:
				{
					angle_msg_t msg;
					if (NP1RadioLink::angle_msg_decode(rx_packet, msg))
					{
						logger.log("CONTROL SYSTEM: Received angle. Not implemented.", INFO);
					}

					break;
				}
				case MSG_ID_CMD_THROTTLE:
				{
					throttle_msg_t msg;
					if (NP1RadioLink::throttle_msg_decode(rx_packet, msg))
					{
						char debug[128];
						snprintf(debug, sizeof(debug), "CONTROL SYSTEM: Received new throttle: %d", msg.throttle);
						logger.log(debug, INFO);

//						logger.log("CONTROL SYSTEM: Received new throttle.", INFO);

						throttle = msg.throttle;
					}

					break;
				}
				default:
				{
					char error[128];
					snprintf(error, sizeof(error), "CONTROL SYSTEM: Received invalid message ID: %d", rx_packet.id);
					logger.log(error, ERR);
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
				logger.log("CONTROL SYSTEM: No drone state received.", ERR);
			}
			else
			{
				logger.log("CONTROL SYSTEM: Drone state invalid. Timeout occured.", ERR);
			}
			canbus.send_disarm();
			continue;
		}

		// -------------------------
		// Run controller
		// -------------------------
		canbus.send_speed(throttle);
	}
}

