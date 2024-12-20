/*
 * usb.h
 *
 *  Created on: Oct 26, 2024
 *      Author: nikolai
 */

#ifndef INC_DRIVERS_USB_H_
#define INC_DRIVERS_USB_H_

#include <stdint.h>

typedef enum Log_Level {
  INFO,
  CRITICAL,
  ERR,
  SENSOR,
  RAW
} Log_Level;

uint8_t USB_Log(const char* string, Log_Level level);

#endif /* INC_DRIVERS_USB_H_ */
