/*
 * LIS3MDL.cpp
 *
 *  Created on: Dec 17, 2024
 *      Author: Nikolai Philipenko
 */

#include "Drivers/LIS3MDL.hpp"
#include "Drivers/usb.hpp"
#include "Utility/lock_guard.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>


LIS3MDL::LIS3MDL(I2C_HandleTypeDef* i2c_handle, osMutexId_t& i2c_mutex, osMutexId_t& mag_data_mutex)
	:i2c_handle(i2c_handle),
	 i2c_mutex(i2c_mutex),
	 mag_data_mutex(mag_data_mutex) {}

bool LIS3MDL::init()
{
	bool status = false;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];
	memset(tx_data, 0, sizeof(tx_data));
	memset(rx_data, 0, sizeof(rx_data));

	// Check chip ID
	rx_data[0] = 0x00;
	status = read_register(REG_WHO_AM_I, rx_data, 1);
	if (status && rx_data[0] == 0x3D)
	{
		USB_Log("Found LIS3MDL, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find LIS3MDL. Initialization failed.", ERR);
		return false;
	}
	osDelay(10);

	// Perform soft-reset of device
	tx_data[0] = 0x04;
	status = write_register(REG_CTRL_REG2, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Set +-4G range
	tx_data[0] = 0x00;
	status = write_register(REG_CTRL_REG2, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Set X,Y axes ultra-high performance mode, 155Hz ODR, disable temperature sensor
	tx_data[0] = 0x62;
	status = write_register(REG_CTRL_REG1, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Set Z axis ultra-high performance mode
	tx_data[0] = 0x0C;
	status = write_register(REG_CTRL_REG4, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Initialize IIR Filters
	float ODR = 155;				// Hz
	for (size_t i = 0; i < filters.size(); i++)
	{
		filters[i] = std::make_unique<IIRFilter>(CUTOFF, ODR);
	}

	// Enable block data update
	tx_data[0] = 0x40;
	status = write_register(REG_CTRL_REG5, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Switch to continuous-conversion mode
	tx_data[0] = 0x00;
	status = write_register(REG_CTRL_REG3, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Read data to clear DRDY interrupt
	service_irq();

	USB_Log("LIS3MDL initialized OK.", CRITICAL);
	osDelay(100);

	return status;
}

bool LIS3MDL::service_irq()
{
	uint8_t raw_data[6];
	bool status = read_register(REG_OUT_X_L, raw_data, sizeof(raw_data));

	if (status)
	{
		int16_t raw_x_int16 = (raw_data[1] << 8) | raw_data[0];
		int16_t raw_y_int16 = (raw_data[3] << 8) | raw_data[2];
		int16_t raw_z_int16 = (raw_data[5] << 8) | raw_data[4];

		float LIS3_x = filters[0]->update(raw_x_int16 / LSB_per_gauss - calib_data.x_offset);
		float LIS3_y = filters[1]->update(raw_y_int16 / LSB_per_gauss - calib_data.y_offset);
		float LIS3_z = filters[2]->update(raw_z_int16 / LSB_per_gauss - calib_data.z_offset);

		np::lock_guard lock(mag_data_mutex);
		axis_intensities[0] = LIS3_x;
		axis_intensities[1] = LIS3_y;
		axis_intensities[2] = LIS3_z;

		// DRDY interrupt cleared automatically after read
	}
	else
	{
		USB_Log("ERR reading LIS3MDL data.", ERR);
	}

	return status;
}

void LIS3MDL::log_data_to_gcs()
{
	char string[128];
	{
		np::lock_guard lock(mag_data_mutex);
		snprintf(string, sizeof(string), "LIS3MDL %.2f %.2f %.2f", axis_intensities[0], axis_intensities[1], axis_intensities[2]);
	}
	USB_Log(string, SENSOR);
}

std::array<float, 3> LIS3MDL::get_axis_intensities()
{
	np::lock_guard lock(mag_data_mutex);
	return axis_intensities;
}

/*
 *
 *  Low-level register read / write
 *
 */
bool LIS3MDL::read_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	bool status = false;

	if (data_len > 1) reg_addr |= LIS3MDL_READ_BURST;

	{
		np::lock_guard lock(i2c_mutex);
		status = (HAL_I2C_Mem_Read(i2c_handle, (LIS3MDL_ADDRESS << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, rx_data, data_len, HAL_MAX_DELAY) == HAL_OK);
	}

	if (!status)
	{
		USB_Log("LIS3MDL register read failed.", ERR);
	}

	return status;
}

bool LIS3MDL::write_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	bool status = false;
	{
		np::lock_guard lock(i2c_mutex);
		status = (HAL_I2C_Mem_Write(i2c_handle, (LIS3MDL_ADDRESS << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, tx_data, data_len, HAL_MAX_DELAY) == HAL_OK);
	}

	if (!status)
	{
		USB_Log("LIS3MDL register write failed.", ERR);
	}

	return status;
}



