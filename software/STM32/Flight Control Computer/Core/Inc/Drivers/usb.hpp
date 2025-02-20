/*
 * usb.h
 *
 *  Created on: Oct 26, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

typedef enum Log_Level {
  INFO,
  CRITICAL,
  ERR,
  SENSOR,
  STATE,
  RAW
} Log_Level;

void USB_Log(const char* string, Log_Level level);
