/*
 * lis3mdl.c
 *
 *  Created on: Dec 3, 2024
 *      Author: Nikolai Philipenko
 */

#include "Drivers/lis3mdl.h"
#include "Drivers/usb.hpp"
#include <math.h>
#include <stdio.h>

/*
 *
 * LIS3MDL INTERFACE
 *
 */
uint8_t LIS3MDL_Init(LIS3MDL* sensor, I2C_HandleTypeDef* i2c_handle, osMutexId_t* i2c_mutex, uint16_t irq_pin)
{
	sensor->i2c_handle = i2c_handle;
	sensor->i2c_mutex = i2c_mutex;
	sensor->irq_pin = irq_pin;
	sensor->LSB_per_gauss = 6842.0; /* Based on +-4G range */
	sensor->x_offset = -34.20 / 100.0; /* Micro-tesla to gauss */
	sensor->y_offset = -11.68 / 100.0;
	sensor->z_offset = -47.37 / 100.0;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];

	// Check chip ID
	rx_data[0] = 0x00;
	LIS3MDL_ReadRegister(sensor, WHO_AM_I, rx_data, 1);
	if (rx_data[0] == 0x3D)
	{
		USB_Log("Found LIS3MDL, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find BMP388. Initialization failed.", ERR);
		return 0;
	}
	osDelay(10);

	// Perform soft-reset of device
	tx_data[0] = 0x04;
	LIS3MDL_WriteRegister(sensor, CTRL_REG2, tx_data, 1);
	osDelay(10);

	// Set +-4G range
	tx_data[0] = 0x00;
	LIS3MDL_WriteRegister(sensor, CTRL_REG2, tx_data, 1);
	osDelay(10);

	// Set X,Y axes ultra-high performance mode, 40Hz ODR, enable temperature sensor
	tx_data[0] = 0xF8;
	LIS3MDL_WriteRegister(sensor, CTRL_REG1, tx_data, 1);
	osDelay(10);

	// Set Z axis ultra-high performance mode
	tx_data[0] = 0x0C;
	LIS3MDL_WriteRegister(sensor, CTRL_REG4, tx_data, 1);
	osDelay(10);

	// Switch to continuous-conversion mode
	tx_data[0] = 0x00;
	LIS3MDL_WriteRegister(sensor, CTRL_REG3, tx_data, 1);
	osDelay(10);

	// Read data to clear DRDY interrupt
	LIS3MDL_ReadData(sensor);

	USB_Log("LIS3MDL initialized OK.", CRITICAL);
	osDelay(100);

	return 1;
}

uint8_t LIS3MDL_ReadData(LIS3MDL* sensor)
{
	uint8_t raw_data[6];
	uint8_t status = LIS3MDL_ReadRegister(sensor, OUT_X_L, raw_data, sizeof(raw_data));

	if (status)
	{
		int16_t raw_x_int16 = (raw_data[1] << 8) | raw_data[0];
		int16_t raw_y_int16 = (raw_data[3] << 8) | raw_data[2];
		int16_t raw_z_int16 = (raw_data[5] << 8) | raw_data[4];

		sensor->intensity_x_gauss = (float)raw_x_int16 / sensor->LSB_per_gauss - sensor->x_offset;
		sensor->intensity_y_gauss = (float)raw_y_int16 / sensor->LSB_per_gauss - sensor->y_offset;
		sensor->intensity_z_gauss = (float)raw_z_int16 / sensor->LSB_per_gauss - sensor->z_offset;

		float declination = 11.8333; // Medicine Hat
		sensor->heading = atan2f(sensor->intensity_y_gauss, sensor->intensity_x_gauss) * (180.0 / M_PI);
		sensor->heading += declination;
		if (sensor->heading < 0)
		{
			sensor->heading += 360;
		}

		// DRDY interrupt cleared automatically after read
	}
	else
	{
		USB_Log("ERR reading LIS3MDL data.", ERR);
	}

	return 1;
}

void LIS3MDL_LogData(LIS3MDL* sensor)
{
	float x_utesla = sensor->intensity_x_gauss * 100;
	float y_utesla = sensor->intensity_y_gauss * 100;
	float z_utesla = sensor->intensity_z_gauss * 100;

	char string[128];
	snprintf(string, 128, "%.2f %.2f %.2f", x_utesla, y_utesla, z_utesla);
	USB_Log(string, SENSOR);
}

/*
 *
 * LIS3MDL LOW-LEVEL REGISTER FUNCTIONS
 *
 */
uint8_t LIS3MDL_ReadRegister(LIS3MDL* sensor, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	if (data_len > 1)
	{
		// In order to read multiple bytes, it is necessary to assert the most significant bit of the address field
		reg_addr |= 0x80;
	}

	osMutexAcquire(*sensor->i2c_mutex, osWaitForever);
	uint8_t status = (HAL_I2C_Mem_Read(sensor->i2c_handle, (LIS3MDL_ADDRESS << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, rx_data, data_len, HAL_MAX_DELAY) == HAL_OK);
	osMutexRelease(*sensor->i2c_mutex);

	if (!status)
	{
		USB_Log("LIS3MDL register read failed.\n", ERR);
	}

	return status;
}

uint8_t LIS3MDL_WriteRegister(LIS3MDL* sensor, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	osMutexAcquire(*sensor->i2c_mutex, osWaitForever);
	uint8_t status = (HAL_I2C_Mem_Write(sensor->i2c_handle, (LIS3MDL_ADDRESS << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, tx_data, data_len, HAL_MAX_DELAY) == HAL_OK);
	osMutexRelease(*sensor->i2c_mutex);

	if (!status)
	{
		USB_Log("LIS3MDL register write failed.\n", ERR);
	}

	return status;
}
