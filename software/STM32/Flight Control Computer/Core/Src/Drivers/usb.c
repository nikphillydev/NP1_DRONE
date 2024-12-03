/*
 * usb.c
 *
 *  Created on: Oct 26, 2024
 *      Author: nikolai
 */

#include "Drivers/usb.h"
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"
#include "usbd_def.h"

uint8_t USB_Log(const char* string, Log_Level level)
{
	char str[512];
	float seconds = (float)osKernelGetTickCount() / (float)osKernelGetTickFreq();

	switch (level) {
		case INFO:
			snprintf(str, 256, "LOG %.2f INFO %s\n", seconds, string);
			break;
		case WARN:
			snprintf(str, 256, "LOG %.2f WARN %s\n", seconds, string);
			break;
		case ERR:
			snprintf(str, 256, "LOG %.2f ERR %s\n", seconds, string);
			break;
		case SENSOR:
			snprintf(str, 256, "SENSOR %.2f %s\n", seconds, string);
			break;
		default:
			snprintf(str, 256, "LOG %.2f ERR INVALID LOG FORMATTING: %s\n", seconds, string);
			break;
	}

	// Get USB mutex (shared resource)
	osMutexAcquire(usbMutexHandle, osWaitForever);
	uint8_t status = CDC_Transmit_FS((uint8_t*)str, strlen(str));
	while (status == USBD_BUSY)
	{
		osDelay(1);
		status = CDC_Transmit_FS((uint8_t*)str, strlen(str));
	}
	osMutexRelease(usbMutexHandle);

	return status;
}
