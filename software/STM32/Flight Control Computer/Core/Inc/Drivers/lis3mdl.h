/*
 * lis3mdl.h
 *
 *  Created on: Dec 3, 2024
 *      Author: nikolai
 */

#ifndef INC_DRIVERS_LIS3MDL_H_
#define INC_DRIVERS_LIS3MDL_H_

#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include <stdint.h>

/*
 *
 * LIS3MDL REGISTERS
 *
 */
#define OFFSET_X_REG_L_M		0x05
#define OFFSET_X_REG_H_M		0x06
#define OFFSET_Y_REG_L_M		0x07
#define OFFSET_Y_REG_H_M		0x08
#define OFFSET_Z_REG_L_M		0x09
#define OFFSET_Z_REG_H_M		0x0A
#define WHO_AM_I				0x0F
#define CTRL_REG1				0x20
#define CTRL_REG2				0x21
#define CTRL_REG3				0x22
#define CTRL_REG4				0x23
#define CTRL_REG5				0x24
#define STATUS_REG				0x27
#define OUT_X_L					0x28
#define OUT_X_H					0x29
#define OUT_Y_L					0x2A
#define OUT_Y_H					0x2B
#define OUT_Z_L					0x2C
#define OUT_Z_H					0x2D
#define TEMP_OUT_L				0x2E
#define TEMP_OUT_H				0x2F
#define INT_CFG					0x30
#define INT_SRC					0x31
#define INT_THS_L				0x32
#define INT_THS_H				0x33

/*
 *
 * LIS3MDL INSTANCE
 *
 */
#define LIS3MDL_ADDRESS			0x1C

typedef struct LIS3MDL {

	I2C_HandleTypeDef* i2c_handle;
	osMutexId_t* i2c_mutex;
	uint16_t irq_pin;

	// Axis intensities (in gauss)
	float intensity_x_gauss;
	float intensity_y_gauss;
	float intensity_z_gauss;

	// Current heading (XY only)
	float heading;

	// Conversion constant
	float LSB_per_gauss;

	// Hard-iron offsets (in gauss)
	float x_offset;
	float y_offset;
	float z_offset;

} LIS3MDL;

/*
 *
 * LIS3MDL INTERFACE
 *
 */
uint8_t LIS3MDL_Init(LIS3MDL* sensor, I2C_HandleTypeDef* i2c_handle, osMutexId_t* i2c_mutex, uint16_t irq_pin);

uint8_t LIS3MDL_ReadData(LIS3MDL* sensor);
void LIS3MDL_LogData(LIS3MDL* sensor);

/*
 *
 * LIS3MDL LOW-LEVEL REGISTER FUNCTIONS
 *
 */
uint8_t LIS3MDL_ReadRegister(LIS3MDL* sensor, uint8_t reg_addr, uint8_t* rx_data, uint16_t data_len);
uint8_t LIS3MDL_WriteRegister(LIS3MDL* sensor, uint8_t reg_addr, uint8_t* tx_data, uint16_t data_len);

#endif /* INC_DRIVERS_LIS3MDL_H_ */
