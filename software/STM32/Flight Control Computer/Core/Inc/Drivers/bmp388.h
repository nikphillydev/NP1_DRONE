/*
 * bmp388.h
 *
 *  Created on: Nov 11, 2024
 *      Author: Nikolai Philipenko
 */

#ifndef INC_DRIVERS_BMP388_H_
#define INC_DRIVERS_BMP388_H_

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include <stdint.h>

/*
 *
 * BMP388 REGISTERS
 *
 */
#define REG_CHIP_ID			0x00
#define REG_ERR_REG			0x02
#define REG_STATUS			0x03
#define REG_DATA_0			0x04
#define REG_DATA_1			0x05
#define REG_DATA_2			0x06
#define REG_DATA_3			0x07
#define REG_DATA_4			0x08
#define REG_DATA_5			0x09
#define REG_SENSORTIME_0	0x0C
#define REG_SENSORTIME_1	0x0D
#define REG_SENSORTIME_2	0x0E
#define REG_EVENT			0x10
#define REG_INT_STATUS		0x11
#define REG_FIFO_LENGTH_0	0x12
#define REG_FIFO_LENGTH_1	0x13
#define REG_FIFO_DATA		0x14
#define REG_FIFO_WTM_0		0x15
#define REG_FIFO_WTM_1		0x16
#define REG_FIFO_CONFIG_1	0x17
#define REG_FIFO_CONFIG_2	0x18
#define REG_INT_CTRL		0x19
#define REG_IF_CONF			0x1A
#define REG_PWR_CTRL		0x1B
#define REG_OSR				0x1C
#define REG_ODR				0x1D
#define REG_CONFIG			0x1F
#define REG_CMD				0x7E

/*
 *
 * BMP388 CALIBRATION
 *
 */
// Non-volatile memory trimming coefficient registers
#define NVM_PAR_T1_LSB		0x31
#define NVM_PAR_T1_MSB		0x32
#define NVM_PAR_T2_LSB		0x33
#define NVM_PAR_T2_MSB		0x34
#define NVM_PAR_T3			0x35
#define NVM_PAR_P1_LSB		0x36
#define NVM_PAR_P1_MSB		0x37
#define NVM_PAR_P2_LSB		0x38
#define NVM_PAR_P2_MSB		0x39
#define NVM_PAR_P3			0x3A
#define NVM_PAR_P4			0x3B
#define NVM_PAR_P5_LSB		0x3C
#define NVM_PAR_P5_MSB		0x3D
#define NVM_PAR_P6_LSB		0x3E
#define NVM_PAR_P6_MSB		0x3F
#define NVM_PAR_P7			0x40
#define NVM_PAR_P8			0x41
#define NVM_PAR_P9_LSB		0x42
#define NVM_PAR_P9_MSB		0x43
#define NVM_PAR_P10			0x44
#define NVM_PAR_P11			0x45

typedef struct BMP388_CalibrationData
{
	float par_t1;
	float par_t2;
	float par_t3;
	float par_p1;
	float par_p2;
	float par_p3;
	float par_p4;
	float par_p5;
	float par_p6;
	float par_p7;
	float par_p8;
	float par_p9;
	float par_p10;
	float par_p11;

} BMP388_CalibrationData;

/*
 *
 * BMP388 INSTANCE
 *
 */
#define BMP388_ADDRESS		0x76

typedef struct BMP388
{
	BMP388_CalibrationData* calib_data;

	I2C_HandleTypeDef* i2c_handle;
	osMutexId_t* i2c_mutex;
	uint16_t irq_pin;

	// Current temperature (degC)
	float temperature;

	// Current atmospheric pressure (Pa)
	float pressure;
	float startup_pressure; // For initial altitude reference

	// Current altitude (based on startup_pressure) (meters)
	float altitude;

} BMP388;

/*
 *
 * BMP388 INTERFACE
 *
 */
uint8_t BMP388_Init(BMP388* sensor, BMP388_CalibrationData* calib_data, I2C_HandleTypeDef* i2c_handle, osMutexId_t* i2c_mutex, uint16_t irq_pin);

uint8_t BMP388_ReadData(BMP388* sensor);
void BMP388_LogData(BMP388* sensor);

/*
 *
 * BMP388 LOW LEVEL REGISTER FUNCTIONS
 *
 */
uint8_t BMP388_ReadRegister(BMP388* sensor, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
uint8_t BMP388_WriteRegister(BMP388* sensor, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);
uint8_t BMP388_ReadCalibrationData(BMP388* sensor);
uint8_t BMP388_ComputeStartupPressure(BMP388* sensor);


#endif /* INC_DRIVERS_BMP388_H_ */
