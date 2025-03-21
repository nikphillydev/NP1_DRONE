/*
 * bmi088.c
 *
 *  Created on: Oct 26, 2024
 *      Author: Nikolai Philipenko
 */

#include "Drivers/bmi088.h"
#include <math.h>
#include <stdio.h>
#include "Drivers/usb.hpp"

#define _USE_MATH_DEFINES

/*
 *
 * BMI088 INTERFACE
 *
 */

uint8_t BMI088_Init(BMI088* imu, SPI_HandleTypeDef* spi_handle, osMutexId_t* spi_mutex, GPIO_TypeDef* acc_cs_port, GPIO_TypeDef* gyro_cs_port,
		uint16_t acc_cs_pin, uint16_t gyro_cs_pin, uint16_t acc_irq_pin, uint16_t gyro_irq_pin)
{
	imu->spi_handle = spi_handle;
	imu->spi_mutex = spi_mutex;
	imu->acc_cs_port = acc_cs_port;
	imu->gyro_cs_port = gyro_cs_port;
	imu->acc_cs_pin = acc_cs_pin;
	imu->gyro_cs_pin = gyro_cs_pin;
	imu->acc_irq_pin = acc_irq_pin;
	imu->gyro_irq_pin = gyro_irq_pin;
	imu->acc_conversion = 9.80665 / 32768.0f * 2.0f * 1.5f; /* Based on +-3g range */
	imu->gyro_conversion = M_PI / 180.0f * 500.0f / 32768.0f; /* Based on +-500 deg/s range */

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];

	// Perform soft-reset of accelerometer (switch to SPI mode first)
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_SET);
	osDelay(10);
	tx_data[0] = 0xB6;
	BMI088_WriteAccRegister(imu, ACC_SOFTRESET, tx_data, 1);
	osDelay(10);

	// Perform soft-reset of gyroscope
	tx_data[0] = 0xB6;
	BMI088_WriteGyroRegister(imu, GYRO_SOFTRESET, tx_data, 1);
	osDelay(10);

	/*
	 * ACCELEROMETER SETUP
	 */

	// Accelerometer requires rising edge on CS pin to switch to SPI mode
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_SET);
	osDelay(10);

	// Check accelerometer chip ID
	rx_data[0] = 0;
	BMI088_ReadAccRegister(imu, ACC_CHIP_ID, rx_data, 1);
	if (rx_data[0] == 0x1E)
	{
		USB_Log("Found BMI088 accelerometer, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find BMI088 accelerometer. Initialization failed.", ERR);
		return 0;
	}
	osDelay(10);

	// Set 100Hz ODR, no over-sampling (40Hz BW)
	tx_data[0] = 0xA8;
	BMI088_WriteAccRegister(imu, ACC_CONF, tx_data, 1);
	osDelay(10);

	// Set +-3g range (10920 LSB/g)
	tx_data[0] = 0x00;
	BMI088_WriteAccRegister(imu, ACC_RANGE, tx_data, 1);
	osDelay(10);

	// Configure INT1 (output, push-pull, active high)
	tx_data[0] = 0x0A;
	BMI088_WriteAccRegister(imu, INT1_IO_CTRL, tx_data, 1);
	osDelay(10);

	// Map accelerometer data ready interrupt to INT1
	tx_data[0] = 0x04;
	BMI088_WriteAccRegister(imu, INT_MAP_DATA, tx_data, 1);
	osDelay(10);

	// Switch accelerometer into active mode (enable data acquisition)
	tx_data[0] = 0x00;
	BMI088_WriteAccRegister(imu, ACC_PWR_CONF, tx_data, 1);
	osDelay(10);

	// Turn accelerometer ON
	tx_data[0] = 0x04;
	BMI088_WriteAccRegister(imu, ACC_PWR_CTRL, tx_data, 1);
	osDelay(10);

	USB_Log("BMI088 accelerometer initialized OK.", CRITICAL);
	osDelay(100);

	/*
	 * GYROSCOPE SETUP
	 */

	// Check gyroscope chip ID
	rx_data[0] = 0;
	BMI088_ReadGyroRegister(imu, GYRO_CHIP_ID, rx_data, 1);
	if (rx_data[0] == 0x0F)
	{
		USB_Log("Found BMI088 gyroscope, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find BMI088 gyroscope. Initialization failed.", ERR);
		return 0;
	}
	osDelay(10);

	// Set 100Hz ODR (32Hz BW)
	tx_data[0] = 0x07;
	BMI088_WriteGyroRegister(imu, GYRO_BANDWIDTH, tx_data, 1);
	osDelay(10);

	// Set +-500 deg/s range (65.536 LSB/deg/s)
	tx_data[0] = 0x02;
	BMI088_WriteGyroRegister(imu, GYRO_RANGE, tx_data, 1);
	osDelay(10);

	// Configure INT3 (push-pull, active high)
	tx_data[0] = 0x01;
	BMI088_WriteGyroRegister(imu, INT3_INT4_IO_CONF, tx_data, 1);
	osDelay(10);

	// Map gyroscope data ready interrupt to INT3
	tx_data[0] = 0x01;
	BMI088_WriteGyroRegister(imu, INT3_INT4_IO_MAP, tx_data, 1);
	osDelay(10);

	// Enable gyroscope new data interrupt
	tx_data[0] = 0x80;
	BMI088_WriteGyroRegister(imu, GYRO_INT_CTRL, tx_data, 1);
	osDelay(10);

	USB_Log("BMI088 gyroscope initialized OK.", CRITICAL);
	osDelay(100);

	return 1;
}

uint8_t BMI088_ReadAccData(BMI088* imu)
{
	// Read raw accelerometer data
	uint8_t rx_data[6];
	uint8_t status = BMI088_ReadAccRegister(imu, ACC_X_LSB, rx_data, sizeof(rx_data));

	if (status)
	{
		// Convert raw data into engineering units
		int16_t accel_x_int16 = (rx_data[1] << 8) | rx_data[0];
		int16_t accel_y_int16 = (rx_data[3] << 8) | rx_data[2];
		int16_t accel_z_int16 = (rx_data[5] << 8) | rx_data[4];

		imu->accel_x = accel_x_int16 * imu->acc_conversion;
		imu->accel_y = accel_y_int16 * imu->acc_conversion;
		imu->accel_z = accel_z_int16 * imu->acc_conversion;

		// Clear data ready interrupt
		BMI088_ReadAccRegister(imu, ACC_INT_STAT_1, rx_data, 1);
	}
	else
	{
		USB_Log("ERR reading BMI088 accelerometer data.", ERR);
	}

	return status;
}

uint8_t BMI088_ReadGyroData(BMI088* imu)
{
	// Read raw gyroscope data
	uint8_t rx_data[6];
	uint8_t status = BMI088_ReadGyroRegister(imu, RATE_X_LSB, rx_data, sizeof(rx_data));

	if (status)
	{
		// Convert raw data into engineering units
		int16_t rate_x_int16 = (rx_data[1] << 8) | rx_data[0];
		int16_t rate_y_int16 = (rx_data[3] << 8) | rx_data[2];
		int16_t rate_z_int16 = (rx_data[5] << 8) | rx_data[4];

		imu->rate_x = rate_x_int16 * imu->gyro_conversion;
		imu->rate_y = rate_y_int16 * imu->gyro_conversion;
		imu->rate_z = rate_z_int16 * imu->gyro_conversion;

		// Interrupt cleared automatically after 280-400us
	}
	else
	{
		USB_Log("ERR reading BMI088 gyroscope data.", ERR);
	}

	return status;
}

uint8_t BMI088_ReadTempData(BMI088* imu)
{
	// Read raw temperature data
	uint8_t rx_data[2];
	uint8_t status = BMI088_ReadAccRegister(imu, TEMP_MSB, rx_data, sizeof(rx_data));

	if (status)
	{
		// Convert raw data into engineering units
		uint16_t temp_uint11 = (rx_data[0] << 3) | (rx_data[1] >> 5);
		int16_t temp_int11 = 0;
		if (temp_uint11 > 1023)
		{
			temp_int11 = temp_uint11 - 2048;
		}
		else
		{
			temp_int11 = temp_uint11;
		}
		imu->temperature = temp_int11 * 0.125f + 23.0f;
	}
	else
	{
		USB_Log("ERR reading BMI088 temperature data.", ERR);
	}

	return status;
}

void BMI088_LogAccData(BMI088* imu)
{
	char string[128];
	snprintf(string, 128, "ACC %.2f %.2f %.2f %.2f", imu->accel_x, imu->accel_y, imu->accel_z, imu->temperature);
	USB_Log(string, SENSOR);
}

void BMI088_LogGyroData(BMI088* imu)
{
	char string[128];
	snprintf(string, 128, "GYRO %.2f %.2f %.2f", imu->rate_x, imu->rate_y, imu->rate_z);
	USB_Log(string, SENSOR);
}

/*
 *
 * BMI088 LOW-LEVEL REGISTER FUNCTIONS
 *
 */

uint8_t BMI088_ReadAccRegister(BMI088* imu, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	uint16_t num_bytes = data_len + 2;
	uint8_t tx_buffer[num_bytes];
	uint8_t rx_buffer[num_bytes];

	for (int i = 0; i < num_bytes; i++)
	{
		tx_buffer[i] = 0;
		rx_buffer[i] = 0;
	}

	tx_buffer[0] = 0x80 | reg_addr;	// Read operation

	osMutexAcquire(*imu->spi_mutex, osWaitForever);
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_RESET);
	uint8_t status = (HAL_SPI_TransmitReceive(imu->spi_handle, tx_buffer, rx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_SET);
	osMutexRelease(*imu->spi_mutex);

	if (status)
	{
		// The first received byte is a dummy byte (ignore index i=1)
		for (int i = 2; i < num_bytes; i++)
		{
			rx_data[i - 2] = rx_buffer[i];
		}
	}
	else
	{
		USB_Log("BMI088 accelerometer register read failed.\n", ERR);
	}

	return status;
}

uint8_t BMI088_WriteAccRegister(BMI088* imu, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];

	tx_buffer[0] = reg_addr;	// Write operation

	for (int i = 1; i < num_bytes; i++)
	{
		tx_buffer[i] = tx_data[i - 1];
	}

	osMutexAcquire(*imu->spi_mutex, osWaitForever);
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_RESET);
	uint8_t status = (HAL_SPI_Transmit(imu->spi_handle, tx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(imu->acc_cs_port, imu->acc_cs_pin, GPIO_PIN_SET);
	osMutexRelease(*imu->spi_mutex);

	if (!status)
	{
		USB_Log("BMI088 accelerometer register write failed.\n", ERR);
	}

	return status;
}

uint8_t BMI088_ReadGyroRegister(BMI088* imu, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	uint8_t rx_buffer[num_bytes];

	for (int i = 0; i < num_bytes; i++)
	{
		tx_buffer[i] = 0;
		rx_buffer[i] = 0;
	}

	tx_buffer[0] = 0x80 | reg_addr;	// Read operation

	osMutexAcquire(*imu->spi_mutex, osWaitForever);
	HAL_GPIO_WritePin(imu->gyro_cs_port, imu->gyro_cs_pin, GPIO_PIN_RESET);
	uint8_t status = (HAL_SPI_TransmitReceive(imu->spi_handle, tx_buffer, rx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(imu->gyro_cs_port, imu->gyro_cs_pin, GPIO_PIN_SET);
	osMutexRelease(*imu->spi_mutex);

	if (status)
	{
		for (int i = 1; i < num_bytes; i++)
		{
			rx_data[i - 1] = rx_buffer[i];
		}
	}
	else
	{
		USB_Log("BMI088 gyroscope register read failed.\n", ERR);
	}

	return status;
}

uint8_t BMI088_WriteGyroRegister(BMI088* imu, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];

	tx_buffer[0] = reg_addr;	// Write operation

	for (int i = 1; i < num_bytes; i++)
	{
		tx_buffer[i] = tx_data[i - 1];
	}

	osMutexAcquire(*imu->spi_mutex, osWaitForever);
	HAL_GPIO_WritePin(imu->gyro_cs_port, imu->gyro_cs_pin, GPIO_PIN_RESET);
	uint8_t status = (HAL_SPI_Transmit(imu->spi_handle, tx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(imu->gyro_cs_port, imu->gyro_cs_pin, GPIO_PIN_SET);
	osMutexRelease(*imu->spi_mutex);

	if (!status)
	{
		USB_Log("BMI088 gyroscope register write failed.\n", ERR);
	}

	return status;
}
