/*
 * bmi088.hpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "cmsis_os.h"
#include "stm32g4xx_hal.h"

#include <array>
#include <memory>
#include <cmath>
#include <cstdint>

#include "Utility/IIRFilter.hpp"
#include "Drivers/usb.hpp"


class BMI088
{
public:
	BMI088(SPI_HandleTypeDef* spi_handle, osMutexId_t& spi_mutex, GPIO_TypeDef* acc_cs_port,
			GPIO_TypeDef* gyro_cs_port, uint16_t acc_cs_pin, uint16_t gyro_cs_pin, osMutexId_t& accel_data_mutex,
			osMutexId_t& gyro_data_mutex, USB_Logger& logger);

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
	const float acc_conversion = 9.80665 / 32768.0f * powf(2.0, 0x03 + 1) * 1.5f; 		// Based on +-24g range
	const float gyro_conversion = M_PI / 180.0f * 2000.0f / 32768.0f; 					// Based on +-2000 deg/s range

	// IMU data

	osMutexId_t& accel_data_mutex;
	std::array<float, 3> linear_accelerations {};	// Current linear accelerations [x, y, z] (m/s^2)
	float temperature;								// Current temperature 			(degC)

	osMutexId_t& gyro_data_mutex;
	std::array<float, 3> angular_velocities {};		// Current angular velocities 	[x, y, z] (rad/s)

	// IIR Filters

	const float ACCEL_CUTOFF_FREQ_HZ	= 15.0f;
	std::array<std::unique_ptr<IIRFilter>, 3> accel_filters {};

	const float GYRO_CUTOFF_FREQ_HZ		= 80.0f;
	std::array<std::unique_ptr<IIRFilter>, 3> gyro_filters {};

	// Logger
	USB_Logger& logger;
};




