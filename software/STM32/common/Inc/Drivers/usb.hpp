/*
 * usb.h
 *
 *  Created on: Oct 26, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "cmsis_os.h"
#include "string"

/*
 * TYPES
 */
typedef enum Log_Level {
  INFO,
  CRITICAL,
  ERR,
  SENSOR,
  STATE,
  RAW
} Log_Level;

/*
 * LOGGER
 */
class USB_Logger
{
public:
	USB_Logger(osMutexId_t& mutex);

	void log(std::string message, Log_Level level);

private:
	osMutexId_t mutex;
};
