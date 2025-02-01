/*
 * usb.c
 *
 *  Created on: Oct 26, 2024
 *      Author: Nikolai Philipenko
 */

#include "Drivers/usb.hpp"
#include "main.h"
#include "Utility/lock_guard.hpp"
#include "usbd_cdc_if.h"
#include "usbd_def.h"
#include <cstdio>

void USB_Log(const char* string, Log_Level level)
{
	char str[512];
	float seconds = (float)osKernelGetTickCount() / (float)osKernelGetTickFreq();

	switch (level) {
		case INFO:
			snprintf(str, 512, "LOG %.2f INFO %s\n", seconds, string);
			break;
		case CRITICAL:
			snprintf(str, 512, "LOG %.2f CRIT %s\n", seconds, string);
			break;
		case ERR:
			snprintf(str, 512, "LOG %.2f ERR %s\n", seconds, string);
			break;
		case SENSOR:
			snprintf(str, 512, "SENSOR %.2f %s\n", seconds, string);
			break;
		case RAW:
			snprintf(str, 512, "%s\n", string);
			break;
		default:
			snprintf(str, 512, "LOG %.2f ERR INVALID LOG FORMATTING: %s\n", seconds, string);
			break;
	}


	np::lock_guard lock(usbMutexHandle);
	uint8_t status = CDC_Transmit_FS((uint8_t*)str, strlen(str));
	while (status == USBD_BUSY)
	{
		osDelay(1);
		status = CDC_Transmit_FS((uint8_t*)str, strlen(str));
	}
}
