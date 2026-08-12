/*
 * bmp388.hpp
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

/* BMP388 REGISTERS */
#define REG_CHIP_ID			    0x00
#define REG_ERR_REG			    0x02
#define REG_STATUS			    0x03
#define REG_DATA_0			    0x04
#define REG_DATA_1			    0x05
#define REG_DATA_2			    0x06
#define REG_DATA_3			    0x07
#define REG_DATA_4			    0x08
#define REG_DATA_5			    0x09
#define REG_SENSORTIME_0	    0x0C
#define REG_SENSORTIME_1	    0x0D
#define REG_SENSORTIME_2	    0x0E
#define REG_EVENT			    0x10
#define REG_INT_STATUS		    0x11
#define REG_FIFO_LENGTH_0	    0x12
#define REG_FIFO_LENGTH_1	    0x13
#define REG_FIFO_DATA		    0x14
#define REG_FIFO_WTM_0		    0x15
#define REG_FIFO_WTM_1		    0x16
#define REG_FIFO_CONFIG_1	    0x17
#define REG_FIFO_CONFIG_2	    0x18
#define REG_INT_CTRL		    0x19
#define REG_IF_CONF			    0x1A
#define REG_PWR_CTRL		    0x1B
#define REG_OSR				    0x1C
#define REG_ODR				    0x1D
#define REG_CONFIG			    0x1F
#define REG_CMD				    0x7E

/* BMP388 CALIBRATION REGISTERS
 * 		Non-volatile memory trimming coefficient registers */
#define REG_NVM_PAR_T1_LSB		0x31
#define REG_NVM_PAR_T1_MSB		0x32
#define REG_NVM_PAR_T2_LSB		0x33
#define REG_NVM_PAR_T2_MSB		0x34
#define REG_NVM_PAR_T3			0x35
#define REG_NVM_PAR_P1_LSB		0x36
#define REG_NVM_PAR_P1_MSB		0x37
#define REG_NVM_PAR_P2_LSB		0x38
#define REG_NVM_PAR_P2_MSB		0x39
#define REG_NVM_PAR_P3			0x3A
#define REG_NVM_PAR_P4			0x3B
#define REG_NVM_PAR_P5_LSB		0x3C
#define REG_NVM_PAR_P5_MSB		0x3D
#define REG_NVM_PAR_P6_LSB		0x3E
#define REG_NVM_PAR_P6_MSB		0x3F
#define REG_NVM_PAR_P7			0x40
#define REG_NVM_PAR_P8			0x41
#define REG_NVM_PAR_P9_LSB		0x42
#define REG_NVM_PAR_P9_MSB		0x43
#define REG_NVM_PAR_P10			0x44
#define REG_NVM_PAR_P11			0x45
