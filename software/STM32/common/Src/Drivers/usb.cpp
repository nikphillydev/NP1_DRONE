/*
 * usb.cpp
 *
 *  Created on: Oct 26, 2024
 *      Author: Nikolai Philipenko
 */

#include "usbd_cdc_if.h"
#include "usbd_def.h"

#include <cstdio>

#include "Drivers/usb.hpp"
#include "Utility/lock_guard.hpp"


USB_Logger::USB_Logger(osMutexId_t& mutex) : mutex(mutex) {}


void USB_Logger::log(std::string message, Log_Level level)
{
	char c_str[512];
	float seconds = (float)osKernelGetTickCount() / (float)osKernelGetTickFreq();

	switch (level) {
		case INFO:
			snprintf(c_str, 512, "LOG %.2f INFO %s\n", seconds, message.c_str());
			break;
		case CRITICAL:
			snprintf(c_str, 512, "LOG %.2f CRIT %s\n", seconds, message.c_str());
			break;
		case ERR:
			snprintf(c_str, 512, "LOG %.2f ERR %s\n", seconds, message.c_str());
			break;
		case SENSOR:
			snprintf(c_str, 512, "SENSOR %.2f %s\n", seconds, message.c_str());
			break;
		case STATE:
			snprintf(c_str, 512, "STATE %.2f %s\n", seconds, message.c_str());
			break;
		case RAW:
			snprintf(c_str, 512, "%s\n", message.c_str());
			break;
		default:
			snprintf(c_str, 512, "LOG %.2f ERR INVALID LOG FORMATTING: %s\n", seconds, message.c_str());
			break;
	}

	uint8_t status = USBD_BUSY;
	{
		np::lock_guard lock(mutex);
		status = CDC_Transmit_FS((uint8_t*)c_str, strlen(c_str));
	}

	while (status == USBD_BUSY)
	{
		osDelay(1);
		{
			np::lock_guard lock(mutex);
			status = CDC_Transmit_FS((uint8_t*)c_str, strlen(c_str));
		}
	}
}
