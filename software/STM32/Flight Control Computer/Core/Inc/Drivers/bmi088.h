/*
 * bmi088.h
 *
 *  Created on: Oct 26, 2024
 *      Author: Nikolai Philipenko
 */

#ifndef INC_DRIVERS_BMI088_H_
#define INC_DRIVERS_BMI088_H_

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include <stdint.h>

/*
 *
 * ACCELEROMETER REGISTERS
 *
 */
#define ACC_CHIP_ID			0x00
#define ACC_ERR_REG			0x02
#define ACC_STATUS			0x03
#define ACC_X_LSB			0x12
#define ACC_X_MSB			0x13
#define ACC_Y_LSB			0x14
#define ACC_Y_MSB			0x15
#define ACC_Z_LSB			0x16
#define ACC_Z_MSB			0x17
#define ACC_SENSORTIME_0	0x18
#define ACC_SENSORTIME_1	0x19
#define ACC_SENSORTIME_2	0x1A
#define ACC_INT_STAT_1		0x1D
#define	TEMP_MSB			0x22
#define	TEMP_LSB			0x23
#define	ACC_FIFO_LENGTH_0	0x24
#define	ACC_FIFO_LENGTH_1	0x25
#define	ACC_FIFO_DATA		0x26
#define ACC_CONF			0x40
#define ACC_RANGE			0x41
#define ACC_FIFO_DOWNS		0x45
#define	ACC_FIFO_WTM_0		0x46
#define	ACC_FIFO_WTM_1		0x47
#define	ACC_FIFO_CONFIG_0	0x48
#define	ACC_FIFO_CONFIG_1	0x49
#define	INT1_IO_CTRL		0x53
#define	INT2_IO_CTRL		0x54
#define INT_MAP_DATA		0x58
#define ACC_SELF_TEST		0x6D
#define ACC_PWR_CONF		0x7C
#define ACC_PWR_CTRL		0x7D
#define	ACC_SOFTRESET		0x7E

/*
 *
 * GYROSCOPE REGISTERS
 *
 */
#define GYRO_CHIP_ID		0x00
#define RATE_X_LSB			0x02
#define RATE_X_MSB			0x03
#define RATE_Y_LSB			0x04
#define RATE_Y_MSB 			0x05
#define RATE_Z_LSB			0x06
#define RATE_Z_MSB			0x07
#define GYRO_INT_STAT_1		0x0A
#define GYRO_FIFO_STATUS	0x0E
#define GYRO_RANGE			0x0F
#define GYRO_BANDWIDTH		0x10
#define GYRO_LPM1			0x11
#define GYRO_SOFTRESET		0x14
#define GYRO_INT_CTRL		0x15
#define INT3_INT4_IO_CONF	0x16
#define INT3_INT4_IO_MAP	0x18
#define GYRO_FIFO_WM_EN		0x1E
#define GYRO_FIFO_EXT_INT_S	0x34
#define GYRO_SELF_TEST		0x3C
#define GYRO_FIFO_CONFIG_0	0x3D
#define	GYRO_FIFO_CONFIG_1  0x3E
#define	GYRO_FIFO_DATA		0x3F

/*
 *
 * BMI088 INSTANCE
 *
 */
typedef struct BMI088
{
	SPI_HandleTypeDef* spi_handle;
	osMutexId_t* spi_mutex;
	GPIO_TypeDef* acc_cs_port;
	GPIO_TypeDef* gyro_cs_port;
	uint16_t acc_cs_pin;
	uint16_t gyro_cs_pin;
	uint16_t acc_irq_pin;
	uint16_t gyro_irq_pin;

	// Current linear accelerations (m/s^2)
	float accel_x;
	float accel_y;
	float accel_z;

	// Current angular velocities (rad/s)
	float rate_x;
	float rate_y;
	float rate_z;

	// Current temperature (degC)
	float temperature;

	// Conversion constants
	float acc_conversion;
	float gyro_conversion;

} BMI088;

/*
 *
 * BMI088 INTERFACE
 *
 */
uint8_t BMI088_Init(BMI088* imu, SPI_HandleTypeDef* spi_handle, osMutexId_t* spi_mutex, GPIO_TypeDef* acc_cs_port, GPIO_TypeDef* gyro_cs_port,
		uint16_t acc_cs_pin, uint16_t gyro_cs_pin, uint16_t acc_irq_pin, uint16_t gyro_irq_pin);

uint8_t BMI088_ReadAccData(BMI088* imu);
uint8_t BMI088_ReadGyroData(BMI088* imu);
uint8_t BMI088_ReadTempData(BMI088* imu);

void BMI088_LogAccData(BMI088* imu);
void BMI088_LogGyroData(BMI088* imu);

/*
 *
 * BMI088 LOW-LEVEL REGISTER FUNCTIONS
 *
 */
uint8_t BMI088_ReadAccRegister(BMI088* imu, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
uint8_t BMI088_WriteAccRegister(BMI088* imu, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

uint8_t BMI088_ReadGyroRegister(BMI088* imu, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
uint8_t BMI088_WriteGyroRegister(BMI088* imu, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

#endif /* INC_DRIVERS_BMI088_H_ */
