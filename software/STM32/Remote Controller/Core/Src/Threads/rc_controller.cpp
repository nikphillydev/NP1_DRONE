/*
 * rc_controller.cpp
 *
 *  Created on: Jun 1, 2026
 *      Author: Nikolai Philipenko
 */

#include "main.h"
#include "fdcan.h"
#include "adc.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "Threads/rc_controller.hpp"
#include "Threads/rc_controller_types.hpp"
#include "Logger/usb_serial_port.hpp"
#include "Logger/logger.hpp"
#include "Drivers/CC2500/cc2500.hpp"
#include "Radio/GaraunteedDelivery/gdelivery_radio.hpp"
#include "Radio/radio_link.hpp"
#include "Utility/moving_avg_filter.hpp"
#include "constants.hpp"

/*
 * DEFINES
 */
#define SEND_THROTTLE_COMMAND_HZ	50

#define ADC_MIN_VALUE				0
#define ADC_MAX_VALUE				4096
#define THROTTLE_MIN_VALUE			0
#define THROTTLE_MAX_VALUE			65536

/*
 * GLOBAL
 */
static USBSerialPort serial_port(usbMutexHandle);
static Logger logger(serial_port);
static CC2500 transmitter(&hspi1, spi1MutexHandle, CC2500_CS_GPIO_Port, CC2500_CS_Pin, logger);
static GaraunteedDeliveryRadio radio(transmitter, logger);

/*
 *
 * THREADS
 *
 */
void rc_controller_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.info("--- RC CONTROLLER THREAD STARTING ---");
	osDelay(100);

	/*
	 * INITIALIZATION
	 */
	if (!radio.init())
	{
		logger.error("RC CONTROLLER THREAD: Failed to init radio communications");
		// Delete this thread
		vTaskDelete( NULL );
	}

	// Setup ADC
	uint16_t adc_raw;
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_raw, 1);

	MovingAverageFilter<float> adc_filter{10};

	// Thread tick timing
	const unsigned gcs_heartbeat_tick_delta = osKernelGetTickFreq() / constants::REQUIRED_GCS_HEARTBEAT_HZ;
	unsigned gcs_last_heartbeat_tick = osKernelGetTickCount();

	const unsigned send_throttle_command_tick_delta = osKernelGetTickFreq() / SEND_THROTTLE_COMMAND_HZ;
	unsigned last_send_throttle_command_tick = osKernelGetTickCount();

	/*
	 * MAIN THREAD LOOP
	 */
	while(1)
	{
		// Maintain GCS heartbeat
		if (osKernelGetTickCount() - gcs_last_heartbeat_tick > gcs_heartbeat_tick_delta)
		{
			ThreadInput input{ ThreadInput_SendHeartbeat };
			osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
			gcs_last_heartbeat_tick = osKernelGetTickCount();
		}

		// Send throttle command
		if (osKernelGetTickCount() - last_send_throttle_command_tick > send_throttle_command_tick_delta)
		{
			ThreadInput input{ ThreadInput_SendThrottle };
			osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
			last_send_throttle_command_tick = osKernelGetTickCount();
		}

		// Respond to thread commands
		ThreadInput input{ ThreadInput_None };
		osMessageQueueGet(rcControllerQueueHandle, &input, NULL, 0);

		switch (input)
		{
			case ThreadInput_None: { break;	}
			case ThreadInput_SendHeartbeat:
			{
//				logger.info("Sending heartbeat");
				cc2500_packet_t packet = RadioLink::heartbeat_msg_pack();
				radio.transmit(packet);
				break;
			}
			case ThreadInput_SendArm:
			{
				logger.info("Sending arm");
				ArmDisarmMsg msg{};
				msg.armed = true;
				cc2500_packet_t packet = RadioLink::arm_disarm_msg_pack(msg);
				radio.transmit(packet);
				break;
			}
			case ThreadInput_SendDisarm:
			{
				logger.info("Sending disarm");
				ArmDisarmMsg msg{};
				msg.armed = false;
				cc2500_packet_t packet = RadioLink::arm_disarm_msg_pack(msg);
				radio.transmit(packet);
				break;
			}
			case ThreadInput_SendThrottle:
			{
//				logger.info("Sending throttle command");
				uint16_t adc_filtered = (uint16_t)adc_filter.update((float)adc_raw);
				uint16_t throttle = (adc_filtered - ADC_MIN_VALUE) * (THROTTLE_MAX_VALUE - THROTTLE_MIN_VALUE) / (ADC_MAX_VALUE - ADC_MIN_VALUE) + THROTTLE_MIN_VALUE;
				ThrottleMsg msg{};
				msg.throttle = throttle;
				cc2500_packet_t packet = RadioLink::throttle_msg_pack(msg);
				radio.transmit(packet);

				logger.info("Sending throttle. ADC: {}, Throttle: {}", adc_filtered, throttle);
				break;
			}
		}
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
		ThreadInput input{ ThreadInput_SendDisarm };
		osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
	}
	else if (GPIO_Pin == GPIO6_ARM_EXTI10_Pin)
	{
		ThreadInput input{ ThreadInput_SendArm };
		osMessageQueuePut(rcControllerQueueHandle, &input, 0, 0);
	}
}

