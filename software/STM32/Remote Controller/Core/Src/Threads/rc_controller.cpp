/*
 * rc_controller.cpp
 *
 *  Created on: Jun 1, 2026
 *      Author: Nikolai Philipenko
 */

#include "rc_controller.hpp"
#include "main.h"
#include "fdcan.h"
#include "adc.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "Drivers/usb.hpp"
#include "Drivers/CC2500/cc2500.hpp"
#include "Radio/radio_link.hpp"
#include "Utility/MovingAverageFilter.hpp"
#include "constants.hpp"

#define SEND_THROTTLE_COMMAND_HZ	50

#define ADC_MIN_VALUE				0
#define ADC_MAX_VALUE				4096
#define THROTTLE_MIN_VALUE			0
#define THROTTLE_MAX_VALUE			65536

void transmit_packet(const cc2500_packet_t& packet, CC2500& transceiver, USB_Logger& logger);

USB_Logger logger{usbMutexHandle};
CC2500 transceiver(&hspi1, spi1MutexHandle, CC2500_CS_GPIO_Port, CC2500_CS_Pin, logger);

/*
 *
 * THREADS
 *
 */
void rc_controller_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.log("--- RC CONTROLLER THREAD STARTING ---", CRITICAL);
	osDelay(100);

	/*
	 * INITIALIZATION
	 */
	bool modem_init = transceiver.init();
	if (!modem_init)
	{
		logger.log("RC CONTROLLER THREAD: Failed to init modem", ERR);
		osDelay(10);

		// Delete this thread
		vTaskDelete( NULL );
	}

	// Enter transmit mode
	while(!transceiver.enter_tx_mode())
	{
		logger.log("CC2500 failed to enter TX mode", ERR);
	}

	/* ADC SETUP */
	uint16_t adc_raw;
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_raw, 1);

	MovingAverageFilter<float> adc_filter{10};

	/* THREAD TICK PARAMETERS */
	const unsigned heartbeat_tick_delta = osKernelGetTickFreq() / REQUIRED_GCS_HEARTBEAT_HZ;
	unsigned last_heartbeat_tick = osKernelGetTickCount();

	const unsigned send_throttle_command_tick_delta = osKernelGetTickFreq() / SEND_THROTTLE_COMMAND_HZ;
	unsigned last_send_throttle_command_tick = osKernelGetTickCount();

	/*
	 * MAIN LOOP
	 */
	while(1)
	{
		/* Determine if heartbeat needs to be sent */
		if (osKernelGetTickCount() - last_heartbeat_tick > heartbeat_tick_delta)
		{
			thread_input_t input{ CMD_HEARTBEAT };
			osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
			last_heartbeat_tick = osKernelGetTickCount();
		}

		/* Determine if angle command needs to be sent */
		if (osKernelGetTickCount() - last_send_throttle_command_tick > send_throttle_command_tick_delta)
		{
			thread_input_t input{ CMD_THROTTLE };
			osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
			last_send_throttle_command_tick = osKernelGetTickCount();
		}

		/* Respond to commands */
		thread_input_t input{ NO_INPUT };
		osMessageQueueGet(rcControllerQueueHandle, &input, NULL, 0);

		switch (input) {
			case CMD_HEARTBEAT: {
//				logger.log("Sending heartbeat", INFO);
				cc2500_packet_t packet = NP1RadioLink::heartbeat_msg_pack();
				transmit_packet(packet, transceiver, logger);
				break;
			}
			case CMD_ARM: {
				logger.log("Sending arm", INFO);
				arm_disarm_msg_t msg{};
				msg.armed = true;
				cc2500_packet_t packet = NP1RadioLink::arm_disarm_msg_pack(msg);
				transmit_packet(packet, transceiver, logger);
				break;
			}
			case CMD_DISARM: {
				logger.log("Sending disarm", INFO);
				arm_disarm_msg_t msg{};
				msg.armed = false;
				cc2500_packet_t packet = NP1RadioLink::arm_disarm_msg_pack(msg);
				transmit_packet(packet, transceiver, logger);
				break;
			}
			case CMD_THROTTLE: {
//				logger.log("Sending throttle command", INFO);
				uint16_t adc_filtered = (uint16_t)adc_filter.update((float)adc_raw);
				uint16_t throttle = (adc_filtered - ADC_MIN_VALUE) * (THROTTLE_MAX_VALUE - THROTTLE_MIN_VALUE) / (ADC_MAX_VALUE - ADC_MIN_VALUE) + THROTTLE_MIN_VALUE;
				throttle_msg_t msg{};
				msg.throttle = throttle;
				cc2500_packet_t packet = NP1RadioLink::throttle_msg_pack(msg);
				transmit_packet(packet, transceiver, logger);
				break;
			}
			default:
				break;
		}
	}
}

/*
 *
 * HELPERS
 *
 */
void transmit_packet(const cc2500_packet_t& packet, CC2500& transceiver, USB_Logger& logger)
{
	if (!transceiver.transmit_packet(packet))
	{
		char debug[256];
		snprintf(debug, sizeof(debug), "RC Controller: Failed to transmit message ID: %d", packet.id);
		logger.log(debug, ERR);
	}
}

/*
 *
 * CALLBACKS
 *
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO4_DISARM_EXTI4_Pin)
	{
		thread_input_t input{ CMD_DISARM };
		osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
	}
	else if (GPIO_Pin == GPIO6_ARM_EXTI10_Pin)
	{
		thread_input_t input{ CMD_ARM };
		osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
	}
}

