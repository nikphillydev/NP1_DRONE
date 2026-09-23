/*
 * usb.cpp
 *
 *  Created on: Sep 20, 2026
 *      Author: Nikolai Philipenko
 */
#include "Drivers/Logger/usb_serial_port.hpp"
#include "Utility/lock_guard.hpp"
#include "usbd_cdc_if.h"
#include "usbd_def.h"


bool USBSerialPort::write_bytes(std::vector<uint8_t>&& data)
{
	const uint32_t timeout_s = 60;
	const uint32_t timeout_ticks = timeout_s * 1000;
	const uint32_t start_tick = osKernelGetTickCount();

	USBD_StatusTypeDef status = USBD_BUSY;
	{
		np::lock_guard lock(mutex);
		status = static_cast<USBD_StatusTypeDef>(CDC_Transmit_FS(data.data(), data.size()));
	}

	while (status == USBD_BUSY)
	{
		if (osKernelGetTickCount() - start_tick > timeout_ticks) break;

		osDelay(1);

		{
			np::lock_guard lock(mutex);
			status = static_cast<USBD_StatusTypeDef>(CDC_Transmit_FS(data.data(), data.size()));
		}
	}

	if (status == USBD_BUSY)
	{
		return false;
	}

	return true;
}

