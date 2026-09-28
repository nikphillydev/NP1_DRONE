/*
 * control_system_thread.cpp
 *
 *  Created on: Aug 24, 2026
 *      Author: Nikolai Philipenko
 *
 *  Control system for the NP1 Drone, requiring drone state and pilot input.
 *  Performs flight termination on loss-of-link signal.
 */
#include "main.h"
#include "fdcan.h"
#include "fcc_topics.hpp"

#include "Threads/control_system_thread.hpp"
#include "Threads/radio_thread.hpp"

#include "Canbus/GaraunteedDelivery/gdelivery_can.hpp"
#include "Logger/usb_serial_port.hpp"
#include "Logger/logger.hpp"
#include "constants.hpp"

/*
 * DEFINES
 */
#define CONTROL_SYSTEM_FREQ			250		// Frequency to run control system thread
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

	// Radio Input
	RadioInput radio_input{};
	RadioInput prev_radio_input{};

	// ESC heartbeat (transmit)
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

		// Maintain ESC heartbeat
		if (osKernelGetTickCount() - esc_last_heartbeat_tick > esc_heartbeat_tick_delta)
		{
			canbus.transmit_heartbeat();
			esc_last_heartbeat_tick = osKernelGetTickCount();
		}

		// -------------------------
		// Get latest radio input
		// -------------------------
		prev_radio_input = radio_input;
		osMessageQueueGet(radioQueueHandle, &radio_input, NULL, 0);

		// LOSS-OF-LINK CHECK
		if (radio_input.loss_of_link && !prev_radio_input.loss_of_link)
		{
			logger.warn("CONTROL SYSTEM: LOSS-OF-LINK.");
			canbus.transmit_disarm();
		}
		else if (!radio_input.loss_of_link && prev_radio_input.loss_of_link)
		{
			logger.warn("CONTROL SYSTEM: LINK RESTORED.");
		}

		// ARM / DISARM CHECK
		if (!radio_input.armed && prev_radio_input.armed)
		{
			canbus.transmit_disarm();
		}
		else if (radio_input.armed && !prev_radio_input.armed)
		{
			canbus.transmit_arm();
		}

		// -------------------------
		// Get drone state
		// -------------------------
		DroneState state{};
		bool recv = state_topic.receive(state);
		bool timeout = recv && (osKernelGetTickCount() - state.timestamp > DRONE_STATE_TIMEOUT_MS);
		if (!recv || timeout)
		{
			if (!recv)
			{
				logger.error("CONTROL SYSTEM: No drone state received.");
			}
			else
			{
				logger.error("CONTROL SYSTEM: Drone state timeout (stale).");
			}
			canbus.transmit_disarm();
			continue;
		}

		// -------------------------
		// Run controller
		// -------------------------
		canbus.transmit_speed(radio_input.throttle);
	}
}

