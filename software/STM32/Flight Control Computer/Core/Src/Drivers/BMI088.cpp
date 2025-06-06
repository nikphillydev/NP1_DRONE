/*
 * BMI088.cpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 */

#include "Drivers/BMI088.hpp"
#include "Drivers/usb.hpp"
#include "Utility/lock_guard.hpp"
#include <cstdio>
#include <cstring>

BMI088::BMI088(SPI_HandleTypeDef* spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef* acc_cs_port, GPIO_TypeDef* gyro_cs_port,
			uint16_t acc_cs_pin, uint16_t gyro_cs_pin, osMutexId_t& accel_data_mutex, osMutexId_t& gyro_data_mutex)
	: spi_handle(spi_handle),
	  spi_mutex(spi_mutex),
	  acc_cs_port(acc_cs_port),
	  gyro_cs_port(gyro_cs_port),
	  acc_cs_pin(acc_cs_pin),
	  gyro_cs_pin(gyro_cs_pin),
	  accel_data_mutex(accel_data_mutex),
	  gyro_data_mutex(gyro_data_mutex) {}

bool BMI088::init()
{
	bool status = false;

	// Temporary buffers
	uint8_t tx_data[4];
	uint8_t rx_data[4];
	memset(tx_data, 0, sizeof(tx_data));
	memset(rx_data, 0, sizeof(rx_data));

	// Accelerometer requires rising edge on CS pin to switch to SPI mode
	HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_SET);
	osDelay(10);
	
	// Perform soft-reset of accelerometer
	tx_data[0] = 0xB6;
	status = write_accel_register(REG_ACC_SOFTRESET, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Perform soft-reset of gyroscope
	tx_data[0] = 0xB6;
	status = write_gyro_register(REG_GYRO_SOFTRESET, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	/*
	 * ACCELEROMETER SETUP
	 */

	// Accelerometer requires rising edge on CS pin to switch to SPI mode
	HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_SET);
	osDelay(10);

	// Check accelerometer chip ID
	rx_data[0] = 0;
	status = read_accel_register(REG_ACC_CHIP_ID, rx_data, 1);
	if (status && rx_data[0] == 0x1E)
	{
		USB_Log("Found BMI088 accelerometer, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find BMI088 accelerometer. Initialization failed.", ERR);
		return false;
	}
	osDelay(10);

	// Set 400Hz ODR, no over-sampling (145Hz BW)
	tx_data[0] = 0xAA;
	status = write_accel_register(REG_ACC_CONF, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Initialize accelerometer IIR Filters
	float ACCEL_ODR = 400;				// Hz
	float ACCEL_CUTOFF = 10;			// Hz
	for (size_t i = 0; i < accel_filters.size(); i++)
	{
		accel_filters[i] = std::make_unique<IIRFilter>(ACCEL_CUTOFF, ACCEL_ODR);
	}

	// Set +-6g range (5460 LSB/g)
	tx_data[0] = 0x01;
	status = write_accel_register(REG_ACC_RANGE, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Configure INT1 (output, push-pull, active high)
	tx_data[0] = 0x0A;
	status = write_accel_register(REG_INT1_IO_CTRL, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Map accelerometer data ready interrupt to INT1
	tx_data[0] = 0x04;
	status = write_accel_register(REG_INT_MAP_DATA, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Switch accelerometer into active mode (enable data acquisition)
	tx_data[0] = 0x00;
	status = write_accel_register(REG_ACC_PWR_CONF, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Turn accelerometer ON
	tx_data[0] = 0x04;
	status = write_accel_register(REG_ACC_PWR_CTRL, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	USB_Log("BMI088 accelerometer initialized OK.", CRITICAL);
	osDelay(100);

	/*
	 * GYROSCOPE SETUP
	 */

	// Check gyroscope chip ID
	rx_data[0] = 0;
	status = read_gyro_register(REG_GYRO_CHIP_ID, rx_data, 1);
	if (status && rx_data[0] == 0x0F)
	{
		USB_Log("Found BMI088 gyroscope, starting initialization.", CRITICAL);
	}
	else
	{
		USB_Log("Failed to find BMI088 gyroscope. Initialization failed.", ERR);
		return false;
	}
	osDelay(10);

	// Set 400Hz ODR (47Hz BW)
	tx_data[0] = 0x03;
	status = write_gyro_register(REG_GYRO_BANDWIDTH, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Initialize gyroscope IIR Filters
	float GYRO_ODR = 400;		// Hz
	float GYRO_CUTOFF = 50;		// Hz
	for (size_t i = 0; i < gyro_filters.size(); i++)
	{
		gyro_filters[i] = std::make_unique<IIRFilter>(GYRO_CUTOFF, GYRO_ODR);
	}

	// Set +-500 deg/s range (65.536 LSB/deg/s)
	tx_data[0] = 0x02;
	status = write_gyro_register(REG_GYRO_RANGE, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Configure INT3 (push-pull, active high)
	tx_data[0] = 0x01;
	status = write_gyro_register(REG_INT3_INT4_IO_CONF, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Map gyroscope data ready interrupt to INT3
	tx_data[0] = 0x01;
	status = write_gyro_register(REG_INT3_INT4_IO_MAP, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	// Enable gyroscope new data interrupt
	tx_data[0] = 0x80;
	status = write_gyro_register(REG_GYRO_INT_CTRL, tx_data, 1);
	if (!status) return status;
	osDelay(10);

	USB_Log("BMI088 gyroscope initialized OK.", CRITICAL);
	osDelay(100);

	return status;
}

bool BMI088::service_irq_accelerometer()
{
	// Read raw accelerometer data
	uint8_t rx_data[6];
	bool status = read_accel_register(REG_ACC_X_LSB, rx_data, sizeof(rx_data));

	if (status)
	{
		// Convert raw data into engineering units
		int16_t accel_x_int16 = (rx_data[1] << 8) | rx_data[0];
		int16_t accel_y_int16 = (rx_data[3] << 8) | rx_data[2];
		int16_t accel_z_int16 = (rx_data[5] << 8) | rx_data[4];
		
		float bmi_x = accel_filters[0]->update(accel_x_int16 * acc_conversion);
		float bmi_y = accel_filters[1]->update(accel_y_int16 * acc_conversion);
		float bmi_z = accel_filters[2]->update(accel_z_int16 * acc_conversion);

		{
			np::lock_guard lock(accel_data_mutex);
			linear_accelerations[0] = bmi_x;
			linear_accelerations[1] = bmi_y;
			linear_accelerations[2] = bmi_z;
		}

		// Clear data ready interrupt
		bool temp = read_accel_register(REG_ACC_INT_STAT_1, rx_data, 1);
		(void)temp;
	}
	else
	{
		USB_Log("ERROR reading BMI088 accelerometer data.", ERR);
	}

	return status;
}

bool BMI088::service_irq_gyroscope()
{
	// Read raw gyroscope data
	uint8_t rx_data[6];
	bool status = read_gyro_register(REG_RATE_X_LSB, rx_data, sizeof(rx_data));

	if (status)
	{
		// Convert raw data into engineering units
		int16_t rate_x_int16 = (rx_data[1] << 8) | rx_data[0];
		int16_t rate_y_int16 = (rx_data[3] << 8) | rx_data[2];
		int16_t rate_z_int16 = (rx_data[5] << 8) | rx_data[4];

		float bmi_x = gyro_filters[0]->update(rate_x_int16 * gyro_conversion);
		float bmi_y = gyro_filters[1]->update(rate_y_int16 * gyro_conversion);
		float bmi_z = gyro_filters[2]->update(rate_z_int16 * gyro_conversion);

		np::lock_guard lock(gyro_data_mutex);
		angular_velocities[0] = bmi_x;
		angular_velocities[1] = bmi_y;
		angular_velocities[2] = bmi_z;

		// Interrupt cleared automatically after 280-400us
	}
	else
	{
		USB_Log("ERROR reading BMI088 gyroscope data.", ERR);
	}

	return status;
}

bool BMI088::service_irq_temperature()
{
	// Read raw temperature data
	uint8_t rx_data[2];
	bool status = read_accel_register(REG_TEMP_MSB, rx_data, sizeof(rx_data));

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
		np::lock_guard lock(accel_data_mutex);
		temperature = temp_int11 * 0.125f + 23.0f;
	}
	else
	{
		USB_Log("ERROR reading BMI088 temperature data.", ERR);
	}

	return status;
}

void BMI088::log_data_to_gcs()
{
	char string[128];
	{
		np::lock_guard lock1(accel_data_mutex);
		np::lock_guard lock2(gyro_data_mutex);
		snprintf(string, sizeof(string), "BMI088 %.2f %.2f %.2f %.2f %.2f %.2f %.2f",
				linear_accelerations[0],
				linear_accelerations[1],
				linear_accelerations[2],
				angular_velocities[0],
				angular_velocities[1],
				angular_velocities[2],
				temperature);
	}
	USB_Log(string, SENSOR);
}

std::array<float, 3> BMI088::get_linear_accelerations()
{
	np::lock_guard lock(accel_data_mutex);
	return linear_accelerations;
}

std::array<float, 3> BMI088::get_angular_velocities()
{
	np::lock_guard lock(gyro_data_mutex);
	return angular_velocities;
}

float BMI088::get_temperature()
{
	np::lock_guard lock(accel_data_mutex);
	return temperature;
}

/*
 *
 *  Low-level register read / write
 *
 */
bool BMI088::read_accel_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 2;
	uint8_t tx_buffer[num_bytes];
	uint8_t rx_buffer[num_bytes];
	memset(tx_buffer, 0, num_bytes);
	memset(rx_buffer, 0, num_bytes);

	tx_buffer[0] = 0x80 | reg_addr;	// Read operation

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_TransmitReceive(spi_handle, tx_buffer, rx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_SET);
	}

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

bool BMI088::read_gyro_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	uint8_t rx_buffer[num_bytes];
	memset(tx_buffer, 0, num_bytes);
	memset(rx_buffer, 0, num_bytes);

	tx_buffer[0] = 0x80 | reg_addr;	// Read operation

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(gyro_cs_port, gyro_cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_TransmitReceive(spi_handle, tx_buffer, rx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(gyro_cs_port, gyro_cs_pin, GPIO_PIN_SET);
	}

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

bool BMI088::write_accel_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	memset(tx_buffer, 0, num_bytes);

	tx_buffer[0] = reg_addr;	// Write operation

	for (int i = 1; i < num_bytes; i++)
	{
		tx_buffer[i] = tx_data[i - 1];
	}

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_Transmit(spi_handle, tx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(acc_cs_port, acc_cs_pin, GPIO_PIN_SET);
	}

	if (!status)
	{
		USB_Log("BMI088 accelerometer register write failed.\n", ERR);
	}

	return status;
}

bool BMI088::write_gyro_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len)
{
	bool status = false;
	uint16_t num_bytes = data_len + 1;
	uint8_t tx_buffer[num_bytes];
	memset(tx_buffer, 0, num_bytes);

	tx_buffer[0] = reg_addr;	// Write operation

	for (int i = 1; i < num_bytes; i++)
	{
		tx_buffer[i] = tx_data[i - 1];
	}

	{
		np::lock_guard lock(spi_mutex);
		HAL_GPIO_WritePin(gyro_cs_port, gyro_cs_pin, GPIO_PIN_RESET);
		status = (HAL_SPI_Transmit(spi_handle, tx_buffer, num_bytes, HAL_MAX_DELAY) == HAL_OK);
		HAL_GPIO_WritePin(gyro_cs_port, gyro_cs_pin, GPIO_PIN_SET);
	}

	if (!status)
	{
		USB_Log("BMI088 gyroscope register write failed.\n", ERR);
	}

	return status;
}

