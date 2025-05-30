/*
 * bmi088.hpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "Utility/IIRFilter.hpp"
#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include <array>
#include <memory>
#include <cmath>
#include <cstdint>

/* ACCELEROMETER REGISTERS */
#define REG_ACC_CHIP_ID			0x00
#define REG_ACC_ERR_REG			0x02
#define REG_ACC_STATUS			0x03
#define REG_ACC_X_LSB			0x12
#define REG_ACC_X_MSB			0x13
#define REG_ACC_Y_LSB			0x14
#define REG_ACC_Y_MSB			0x15
#define REG_ACC_Z_LSB			0x16
#define REG_ACC_Z_MSB			0x17
#define REG_ACC_SENSORTIME_0	0x18
#define REG_ACC_SENSORTIME_1	0x19
#define REG_ACC_SENSORTIME_2	0x1A
#define REG_ACC_INT_STAT_1		0x1D
#define	REG_TEMP_MSB			0x22
#define	REG_TEMP_LSB			0x23
#define	REG_ACC_FIFO_LENGTH_0	0x24
#define	REG_ACC_FIFO_LENGTH_1	0x25
#define	REG_ACC_FIFO_DATA		0x26
#define REG_ACC_CONF			0x40
#define REG_ACC_RANGE			0x41
#define REG_ACC_FIFO_DOWNS		0x45
#define	REG_ACC_FIFO_WTM_0		0x46
#define	REG_ACC_FIFO_WTM_1		0x47
#define	REG_ACC_FIFO_CONFIG_0	0x48
#define	REG_ACC_FIFO_CONFIG_1	0x49
#define	REG_INT1_IO_CTRL		0x53
#define	REG_INT2_IO_CTRL		0x54
#define REG_INT_MAP_DATA		0x58
#define REG_ACC_SELF_TEST		0x6D
#define REG_ACC_PWR_CONF		0x7C
#define REG_ACC_PWR_CTRL		0x7D
#define	REG_ACC_SOFTRESET		0x7E

/* GYROSCOPE REGISTERS */
#define REG_GYRO_CHIP_ID		0x00
#define REG_RATE_X_LSB			0x02
#define REG_RATE_X_MSB			0x03
#define REG_RATE_Y_LSB			0x04
#define REG_RATE_Y_MSB 			0x05
#define REG_RATE_Z_LSB			0x06
#define REG_RATE_Z_MSB			0x07
#define REG_GYRO_INT_STAT_1		0x0A
#define REG_GYRO_FIFO_STATUS	0x0E
#define REG_GYRO_RANGE			0x0F
#define REG_GYRO_BANDWIDTH		0x10
#define REG_GYRO_LPM1			0x11
#define REG_GYRO_SOFTRESET		0x14
#define REG_GYRO_INT_CTRL		0x15
#define REG_INT3_INT4_IO_CONF	0x16
#define REG_INT3_INT4_IO_MAP	0x18
#define REG_GYRO_FIFO_WM_EN		0x1E
#define REG_GYRO_FIFO_EXT_INT_S	0x34
#define REG_GYRO_SELF_TEST		0x3C
#define REG_GYRO_FIFO_CONFIG_0	0x3D
#define	REG_GYRO_FIFO_CONFIG_1  0x3E
#define	REG_GYRO_FIFO_DATA		0x3F


class BMI088
{
public:
	BMI088(SPI_HandleTypeDef* spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef* acc_cs_port, GPIO_TypeDef* gyro_cs_port,
			uint16_t acc_cs_pin, uint16_t gyro_cs_pin, osMutexId_t& accel_data_mutex, osMutexId_t& gyro_data_mutex);

	// Initialization after startup
	[[nodiscard]] bool init();

	// IRQ service and logging
	bool service_irq_accelerometer();
	bool service_irq_gyroscope();
	bool service_irq_temperature();
	void log_data_to_gcs();

	// Getters
	std::array<float, 3> get_linear_accelerations();
	std::array<float, 3> get_angular_velocities();
	float get_temperature();

private:
	// Low-level register read / write
	[[nodiscard]] bool read_accel_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
	[[nodiscard]] bool read_gyro_register(uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
	[[nodiscard]] bool write_accel_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);
	[[nodiscard]] bool write_gyro_register(uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

	// SPI communication
	SPI_HandleTypeDef* spi_handle;
	osMutexId_t& spi_mutex;
	GPIO_TypeDef* acc_cs_port;
	GPIO_TypeDef* gyro_cs_port;
	uint16_t acc_cs_pin;
	uint16_t gyro_cs_pin;
	uint16_t acc_irq_pin;
	uint16_t gyro_irq_pin;

	// Conversion constants
	const float acc_conversion = 9.80665 / 32768.0f * powf(2.0, 0x01 + 1) * 1.5f; 		// Based on +-6g range
	const float gyro_conversion = M_PI / 180.0f * 500.0f / 32768.0f; 					// Based on +-500 deg/s range

	// IMU data mutexes
	osMutexId_t& accel_data_mutex;
	osMutexId_t& gyro_data_mutex;

	// IMU data
	std::array<float, 3> linear_accelerations {};	// Current linear accelerations [x, y, z] (m/s^2)
	std::array<float, 3> angular_velocities {};		// Current angular velocities [x, y, z] (rad/s)
	float temperature;								// Current temperature (degC)

	// IIR Filters
	std::array<std::unique_ptr<IIRFilter>, 3> accel_filters {};
	std::array<std::unique_ptr<IIRFilter>, 3> gyro_filters {};
};




