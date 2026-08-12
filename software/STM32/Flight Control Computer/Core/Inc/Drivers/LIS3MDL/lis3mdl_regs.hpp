/*
 * lis3mdl_regs.hpp
 *
 *  Created on: Jul 28, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

/*
 *
 * REGISTERS
 *
 */

#define REG_OFFSET_X_REG_L_M		0x05
#define REG_OFFSET_X_REG_H_M		0x06
#define REG_OFFSET_Y_REG_L_M		0x07
#define REG_OFFSET_Y_REG_H_M		0x08
#define REG_OFFSET_Z_REG_L_M		0x09
#define REG_OFFSET_Z_REG_H_M		0x0A
#define REG_WHO_AM_I				0x0F
#define REG_CTRL_REG1				0x20
#define REG_CTRL_REG2				0x21
#define REG_CTRL_REG3				0x22
#define REG_CTRL_REG4				0x23
#define REG_CTRL_REG5				0x24
#define REG_STATUS_REG				0x27
#define REG_OUT_X_L					0x28
#define REG_OUT_X_H					0x29
#define REG_OUT_Y_L					0x2A
#define REG_OUT_Y_H					0x2B
#define REG_OUT_Z_L					0x2C
#define REG_OUT_Z_H					0x2D
#define REG_TEMP_OUT_L				0x2E
#define REG_TEMP_OUT_H				0x2F
#define REG_INT_CFG					0x30
#define REG_INT_SRC					0x31
#define REG_INT_THS_L				0x32
#define REG_INT_THS_H				0x33

/* ACCESS TYPES */
#define LIS3MDL_READ_BURST			0x80
