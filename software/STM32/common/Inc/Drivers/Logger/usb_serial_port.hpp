/*
 * usb.hpp
 *
 *  Created on: Sep 20, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "cmsis_os.h"
#include <vector>


class USBSerialPort {
public:
	USBSerialPort(osMutexId_t& mutex) : mutex{mutex} {};

	bool write_bytes(std::vector<uint8_t>&& data);

private:
	osMutexId_t& mutex;
};
