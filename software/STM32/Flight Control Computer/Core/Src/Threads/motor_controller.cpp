/*
 * motor_controller.cpp
 *
 *  Created on: Jun 1, 2026
 *      Author: Nikolai Philipenko
 */

#include "main.h"
#include "fdcan.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "Threads/motor_controller.hpp"
#include "Drivers/usb.hpp"
#include "Utility/MovingAverageFilter.hpp"

/*
 * Sensors
 */
static USB_Logger logger{usbMutexHandle};

/*
 *
 * THREADS
 *
 */
void motor_controller_thread()
{
	osDelay(THREAD_START_DELAY_MS);
	logger.log("--- MOTOR CONTROLLER THREAD STARTING ---", CRITICAL);
	osDelay(10);

	vTaskDelete( NULL );

	while(1)
	{
//		if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0)
//		{
//			//------------------------
//			// Send Heartbeat
//			//------------------------
//
//			TxHeaderCan1.Identifier = 3;
//			uint8_t tx_data[8] = {};
//			if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//			{
//				Error_Handler();
//			}
//		}
//
//
//		if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0)
//		{
//			//------------------------
//			// Send Speed
//			//------------------------
//
//			uint16_t adc_filt = (uint16_t)filter.update((float)adc_raw);
//			uint16_t speed = (adc_filt - ADC_MIN) * (SPEED_MAX - SPEED_MIN) / (ADC_MAX - ADC_MIN) + SPEED_MIN;
//
//			char message[256];
//			snprintf(message, 256, "ADC FILT: %d, 	SPEED PERC: %f", adc_filt, (float)speed / UINT16_MAX * 100.0);
//			USB_Log(message, INFO);
//
//			// BIG ENDIAN
//			uint8_t big_byte = 		(speed >> 8) 	& 0xFF;
//			uint8_t small_byte = 	(speed) 		& 0xFF;
//			uint8_t tx_data[8] = {};
//			tx_data[0] = big_byte;
//			tx_data[1] = small_byte;
//
//			TxHeaderCan1.Identifier = 2;
//			if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//			{
//				Error_Handler();
//			}
//		}

		osDelay(10);

//		if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0)
//		{
//			USB_Log("Motor Controller: Sending CAN message", CRITICAL);
//			uint8_t tx_data[8] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22};
//			if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//			{
//				Error_Handler();
//			}
//		}
//		osDelay(500);

//		USB_Log("Motor Controller: Waiting for 5 seconds...", CRITICAL);
//		osDelay(5000);
//
//		USB_Log("Motor Controller: Sending ARM command...", CRITICAL);
//		TxHeaderCan1.Identifier = 1;
//		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		osDelay(3000);
//
//		USB_Log("Motor Controller: Sending DISARM command...", CRITICAL);
//		TxHeaderCan1.Identifier = 0;
//		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		osDelay(3000);
//
//		USB_Log("Motor Controller: Sending ARM then immediately DISARM command...", CRITICAL);
//		TxHeaderCan1.Identifier = 1;
//		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		osDelay(100);
//		TxHeaderCan1.Identifier = 0;
//		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeaderCan1, tx_data) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		USB_Log("Motor Controller: Sequence complete...", CRITICAL);
//		osDelay(3000);
	}
}

