/*
 * pmw3901_regs.hpp
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

#define REG_PRODUCT_ID                  0x00
#define REG_REVISION_ID                 0x01
#define REG_MOTION                      0x02
#define REG_DELTA_X_L                   0x03
#define REG_DELTA_X_H                   0x04
#define REG_DELTA_Y_L                   0x05
#define REG_DELTA_Y_H                   0x06
#define REG_SQUAL                       0x07
#define REG_RAW_DATA_SUM                0x08
#define REG_MAXIMUM_RAW_DATA            0x09
#define REG_MINIMUM_RAW_DATA            0x0A
#define REG_SHUTTER_LOWER               0x0B
#define REG_SHUTTER_UPPER               0x0C
#define REG_OBSERVATION                 0x15
#define REG_MOTION_BURST                0x16
#define REG_POWER_UP_RESET              0x3A
#define REG_SHUTDOWN                    0x3B
#define REG_RAW_DATA_GRAB               0x58
#define REG_RAW_DATA_GRAB_STATUS        0x59
#define REG_INVERSE_PRODUCT_ID          0x5F

/* ACCESS TYPES */
#define PMW3901_READ					0x00
#define PMW3901_WRITE					0x80
