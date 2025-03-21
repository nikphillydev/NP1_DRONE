/*
 * bmp388.c
 *
 *  Created on: Nov 11, 2024
 *      Author: Nikolai Philipenko
 */

#include "Drivers/bmp388.h"
#include "Drivers/usb.hpp"
#include <math.h>
#include <stdio.h>

/*
 *
 * BMP388 INTERFACE
 *
 */
uint8_t BMP388_Init(BMP388* sensor, BMP388_CalibrationData* calib_data, I2C_HandleTypeDef* i2c_handle, osMutexId_t* i2c_mutex, uint16_t irq_pin)
{
	sensor->calib_data = calib_data;
	sensor->i2c_handle = i2c_handle;
	sensor->i2c_mutex = i2c_mutex;
	sensor->irq_pin = irq_pin;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];

	// Check chip ID
	rx_data[0] = 0x00;
	BMP388_ReadRegister(sensor, REG_CHIP_ID, rx_data, 1);
	if (rx_data[0] == 0x50)
	{
		USB_Log("Found BMP388, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find BMP388. Initialization failed.", ERR);
		return 0;
	}
	osDelay(10);

	// Perform soft-reset of device
	tx_data[0] = 0xB6;
	BMP388_WriteRegister(sensor, REG_CMD, tx_data, 1);
	osDelay(10);

	// Read, calculate, and store calibration coefficients
	BMP388_ReadCalibrationData(sensor);

	// Set pressure measurement to ultra-high resolution (x16 over-sampling, 20 bit / 0.17 Pa),
	// set temperature measurement to low power (x2 over-sampling, 17 bit / 0.0025 degC)
	tx_data[0] = 0x0C;
	BMP388_WriteRegister(sensor, REG_OSR, tx_data, 1);
	osDelay(10);

	// Set 25Hz ODR
	tx_data[0] = 0x03;
	BMP388_WriteRegister(sensor, REG_ODR, tx_data, 1);
	osDelay(10);

	// Set IIR filter coefficient to 15
	tx_data[0] = 0x04;
	BMP388_WriteRegister(sensor, REG_CONFIG, tx_data, 1);
	osDelay(10);

	// Compute startup pressure for initial altitude reference
	BMP388_ComputeStartupPressure(sensor);

	// Switch device into normal mode and enable pressure, temperature sensor
	tx_data[0] = 0x33;
	BMP388_WriteRegister(sensor, REG_PWR_CTRL, tx_data, 1);
	osDelay(10);

	// Enable data ready interrupt (temperature and pressure) and configure INT pin (active high, push-pull)
	tx_data[0] = 0x42;
	BMP388_WriteRegister(sensor, REG_INT_CTRL, tx_data, 1);
	osDelay(10);

	USB_Log("BMP388 initialized OK.", CRITICAL);
	osDelay(100);

	return 1;
}

uint8_t BMP388_ReadData(BMP388* sensor)
{
	// Compute compensated temperature

	uint8_t rx_temp_data[3];
	uint8_t status_temp = BMP388_ReadRegister(sensor, REG_DATA_3, rx_temp_data, 3);

	if (status_temp)
	{
		uint32_t temp_raw = (rx_temp_data[2] << 16) | (rx_temp_data[1] << 8) | rx_temp_data[0];

		float temp_partial_data1 = (float)temp_raw - sensor->calib_data->par_t1;
		float temp_partial_data2 = temp_partial_data1 * sensor->calib_data->par_t2;

		sensor->temperature = temp_partial_data2 + (temp_partial_data1 * temp_partial_data1) * sensor->calib_data->par_t3;
	}

	// Compute compensated pressure

	uint8_t rx_press_data[3];
	uint8_t status_pressure = BMP388_ReadRegister(sensor, REG_DATA_0, rx_press_data, 3);

	if (status_temp && status_pressure)
	{
		uint32_t press_raw = (rx_press_data[2] << 16) | (rx_press_data[1] << 8) | rx_press_data[0];

		float press_partial_data1 = sensor->calib_data->par_p6 * sensor->temperature;
		float press_partial_data2 = sensor->calib_data->par_p7 * (sensor->temperature * sensor->temperature);
		float press_partial_data3 = sensor->calib_data->par_p8 * (sensor->temperature * sensor->temperature * sensor->temperature);
		float press_partial_out1 = sensor->calib_data->par_p5 + press_partial_data1 + press_partial_data2 + press_partial_data3;

		press_partial_data1 = sensor->calib_data->par_p2 * sensor->temperature;
		press_partial_data2 = sensor->calib_data->par_p3 * (sensor->temperature * sensor->temperature);
		press_partial_data3 = sensor->calib_data->par_p4 * (sensor->temperature * sensor->temperature * sensor->temperature);
		float press_partial_out2 = (float)press_raw * (sensor->calib_data->par_p1 + press_partial_data1 + press_partial_data2 + press_partial_data3);

		press_partial_data1 = (float)press_raw * (float)press_raw;
		press_partial_data2 = sensor->calib_data->par_p9 + sensor->calib_data->par_p10 * sensor->temperature;
		press_partial_data3 = press_partial_data1 *	press_partial_data2;
		float press_partial_out3 = press_partial_data3 + ((float)press_raw * (float)press_raw * (float)press_raw) * sensor->calib_data->par_p11;

		sensor->pressure = press_partial_out1 + press_partial_out2 + press_partial_out3;

		// Compute altitude

		if (sensor->pressure && sensor->startup_pressure)
		{
			sensor->altitude = 44330 * (1 - powf(sensor->pressure / sensor->startup_pressure, 1.0 / 5.25579));
		}

		// Data ready INT cleared automatically 2.5 ms after the interrupt assertion
	}
	else
	{
		USB_Log("ERR reading BMP388 data.", ERR);
	}

	return status_temp && status_pressure;
}

void BMP388_LogData(BMP388* sensor)
{
	char string[128];
	snprintf(string, 128, "BAR %.2f %.2f %.2f", sensor->pressure, sensor->temperature, sensor->altitude);
	USB_Log(string, SENSOR);
}

/*
 *
 * BMP388 LOW LEVEL REGISTER FUNCTIONS
 *
 */
uint8_t BMP388_ReadRegister(BMP388* sensor, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	osMutexAcquire(*sensor->i2c_mutex, osWaitForever);
	uint8_t status = (HAL_I2C_Mem_Read(sensor->i2c_handle, (BMP388_ADDRESS << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, rx_data, data_len, HAL_MAX_DELAY) == HAL_OK);
	osMutexRelease(*sensor->i2c_mutex);

	if (!status)
	{
		USB_Log("BMP388 register read failed.\n", ERR);
	}

	return status;
}

uint8_t BMP388_WriteRegister(BMP388* sensor, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	osMutexAcquire(*sensor->i2c_mutex, osWaitForever);
	uint8_t status = (HAL_I2C_Mem_Write(sensor->i2c_handle, (BMP388_ADDRESS << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, tx_data, data_len, HAL_MAX_DELAY) == HAL_OK);
	osMutexRelease(*sensor->i2c_mutex);

	if (!status)
	{
		USB_Log("BMP388 register write failed.\n", ERR);
	}

	return status;
}

uint8_t BMP388_ReadCalibrationData(BMP388* sensor)
{
	uint8_t rx_data[2];

	// PAR T1
	BMP388_ReadRegister(sensor, NVM_PAR_T1_LSB, rx_data, 2);
	uint16_t par_t1_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_t1 = (float)par_t1_raw / powf(2, -8);

	// PAR T2
	BMP388_ReadRegister(sensor, NVM_PAR_T2_LSB, rx_data, 2);
	uint16_t par_t2_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_t2 = (float)par_t2_raw / powf(2, 30);

	// PAR T3
	BMP388_ReadRegister(sensor, NVM_PAR_T3, rx_data, 1);
	int8_t par_t3_raw = rx_data[0];
	sensor->calib_data->par_t3 = (float)par_t3_raw / powf(2, 48);

	// PAR P1
	BMP388_ReadRegister(sensor, NVM_PAR_P1_LSB, rx_data, 2);
	int16_t par_p1_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_p1 = ((float)par_p1_raw - powf(2, 14)) / powf(2, 20);

	// PAR P2
	BMP388_ReadRegister(sensor, NVM_PAR_P2_LSB, rx_data, 2);
	int16_t par_p2_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_p2 = ((float)par_p2_raw - powf(2, 14)) / powf(2, 29);

	// PAR P3
	BMP388_ReadRegister(sensor, NVM_PAR_P3, rx_data, 1);
	int8_t par_p3_raw = rx_data[0];
	sensor->calib_data->par_p3 = (float)par_p3_raw / powf(2, 32);

	// PAR P4
	BMP388_ReadRegister(sensor, NVM_PAR_P4, rx_data, 1);
	int8_t par_p4_raw = rx_data[0];
	sensor->calib_data->par_p4 = (float)par_p4_raw / powf(2, 37);

	// PAR P5
	BMP388_ReadRegister(sensor, NVM_PAR_P5_LSB, rx_data, 2);
	uint16_t par_p5_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_p5 = (float)par_p5_raw / powf(2, -3);

	// PAR P6
	BMP388_ReadRegister(sensor, NVM_PAR_P6_LSB, rx_data, 2);
	uint16_t par_p6_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_p6 = (float)par_p6_raw / powf(2, 6);

	// PAR P7
	BMP388_ReadRegister(sensor, NVM_PAR_P7, rx_data, 1);
	int8_t par_p7_raw = rx_data[0];
	sensor->calib_data->par_p7 = (float)par_p7_raw / powf(2, 8);

	// PAR P8
	BMP388_ReadRegister(sensor, NVM_PAR_P8, rx_data, 1);
	int8_t par_p8_raw = rx_data[0];
	sensor->calib_data->par_p8 = (float)par_p8_raw / powf(2, 15);

	// PAR P9
	BMP388_ReadRegister(sensor, NVM_PAR_P9_LSB, rx_data, 2);
	int16_t par_p9_raw = (rx_data[1] << 8) | rx_data[0];
	sensor->calib_data->par_p9 = (float)par_p9_raw / powf(2, 48);

	// PAR P10
	BMP388_ReadRegister(sensor, NVM_PAR_P10, rx_data, 1);
	int8_t par_p10_raw = rx_data[0];
	sensor->calib_data->par_p10 = (float)par_p10_raw / powf(2, 48);

	// PAR P11
	BMP388_ReadRegister(sensor, NVM_PAR_P11, rx_data, 1);
	int8_t par_p11_raw = rx_data[0];
	sensor->calib_data->par_p11 = (float)par_p11_raw / powf(2, 65);

	return 1;
}

uint8_t BMP388_ComputeStartupPressure(BMP388* sensor)
{
	/*
	 * Compute the average current pressure (for initial altitude reference)
	 */

	uint8_t tx_data[2];
	uint8_t sample_num = 50;
	float running_pressure = 0;

	for (int i = 0; i < sample_num; i++)
	{
		// Switch sensor into forced mode (take one reading, return to sleep)
		tx_data[0] = 0x13;
		BMP388_WriteRegister(sensor, REG_PWR_CTRL, tx_data, 1);
		osDelay(40);
		BMP388_ReadData(sensor);
		running_pressure += sensor->pressure;
	}

	sensor->startup_pressure = running_pressure / sample_num;
	return 1;
}







