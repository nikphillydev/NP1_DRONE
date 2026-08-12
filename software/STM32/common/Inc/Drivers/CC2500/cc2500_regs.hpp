/*
 * cc2500_regs.hpp
 *
 *  Created on: Jul 23, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

/*
 *
 * REGISTERS
 *
 */

/* CONFIGURATION REGISTERS - R/W */
#define REG_IOCFG2           		0x00
#define REG_IOCFG1           		0x01
#define REG_IOCFG0           		0x02
#define REG_FIFOTHR          		0x03
#define REG_SYNC1            		0x04
#define REG_SYNC0            		0x05
#define REG_PKTLEN           		0x06
#define REG_PKTCTRL1         		0x07
#define REG_PKTCTRL0         		0x08
#define REG_ADDR             		0x09
#define REG_CHANNR           		0x0A
#define REG_FSCTRL1          		0x0B
#define REG_FSCTRL0          		0x0C
#define REG_FREQ2            		0x0D
#define REG_FREQ1            		0x0E
#define REG_FREQ0            		0x0F
#define REG_MDMCFG4          		0x10
#define REG_MDMCFG3          		0x11
#define REG_MDMCFG2          		0x12
#define REG_MDMCFG1          		0x13
#define REG_MDMCFG0          		0x14
#define REG_DEVIATN          		0x15
#define REG_MCSM2            		0x16
#define REG_MCSM1            		0x17
#define REG_MCSM0            		0x18
#define REG_FOCCFG           		0x19
#define REG_BSCFG            		0x1A
#define REG_AGCCTRL2         		0x1B
#define REG_AGCCTRL1         		0x1C
#define REG_AGCCTRL0         		0x1D
#define REG_WOREVT1          		0x1E
#define REG_WOREVT0          		0x1F
#define REG_WORCTRL          		0x20
#define REG_FREND1           		0x21
#define REG_FREND0           		0x22
#define REG_FSCAL3           		0x23
#define REG_FSCAL2           		0x24
#define REG_FSCAL1           		0x25
#define REG_FSCAL0           		0x26
#define REG_RCCTRL1          		0x27
#define REG_RCCTRL0          		0x28
#define REG_FSTEST           		0x29
#define REG_PTEST            		0x2A
#define REG_AGCTEST          		0x2B
#define REG_TEST2            		0x2C
#define REG_TEST1            		0x2D
#define REG_TEST0            		0x2E

/* STATUS REGISTERS - RO (BURST BIT MUST BE SET) */
#define REG_PARTNUM          		0x30
#define REG_VERSION          		0x31
#define REG_FREQEST          		0x32
#define REG_LQI              		0x33
#define REG_RSSI             		0x34
#define REG_MARCSTATE        		0x35
#define REG_WORTIME1         		0x36
#define REG_WORTIME0         		0x37
#define REG_PKTSTATUS        		0x38
#define REG_VCO_VC_DAC       		0x39
#define REG_TXBYTES          		0x3A
#define REG_RXBYTES          		0x3B
#define REG_RCCTRL1_STATUS   		0x3C
#define REG_RCCTRL0_STATUS   		0x3D

/* MULTI-BYTE REGISTERS */
#define REG_PATABLE					0x3E	// Power output table
#define REG_TX_FIFO					0x3F	// Transmit FIFO
#define REG_RX_FIFO					0x3F	// Receive FIFO

/*
 *
 * REGISTER ACCESS
 *
 */

/* COMMAND STROBES (BURST BIT MUST NOT BE SET) */
#define CMD_SRES					0x30	// Reset chip
#define CMD_SFSTXON					0x31	// Enable and calibrate frequency synthesizer
#define CMD_SXOFF					0x32	// Turn off crystal oscillator
#define CMD_SCAL					0x33	// Calibrate frequency synthesizer and turn it off
#define CMD_SRX						0x34	// Enable RX
#define CMD_STX						0x35	// Enable TX
#define CMD_SIDLE					0x36	// Exit Radio mode
#define CMD_SWOR					0x38	// Start automatic RX polling
#define CMD_SPWD					0x39	// Enter power down mode when CSn goes high.
#define CMD_SFRX					0x3A	// Flush the RX FIFO buffer.
#define CMD_SFTX					0x3B	// Flush the TX FIFO buffer.
#define CMD_SWORRST					0x3C	// Reset real time clock
#define CMD_SNOP					0x3D	// No operation. May be used to get access to the chip status byte.

/* ACCESS TYPES */
#define CC2500_READ					0x80
#define CC2500_WRITE				0x00
#define CC2500_BURST				0x40

/* TX / RX FIFO SIZES */
#define CC2500_FIFO_SIZE		64

/*
 *
 * BITMASKS AND SHIFTS
 *
 */

/* STATUS BYTE BITMASKS AND SHIFTS*/
#define CC2500_STATUS_CHIP_RDY_BITMASK   		0x80
#define CC2500_STATUS_CHIP_RDY_SHIFT   			7
#define CC2500_STATUS_STATE_BITMASK				0x70
#define CC2500_STATUS_STATE_SHIFT				4
#define CC2500_STATUS_FIFO_BYTES_BITMASK		0x0F
#define CC2500_STATUS_FIFO_BYTES_SHIFT			0

/* TXBYTES REGISTER BITMASKS AND SHIFTS */
#define CC2500_TXBYTES_UNDERFLOW_BITMASK		0x80
#define CC2500_TXBYTES_UNDERFLOW_SHIFT			7
#define CC2500_TXBYTES_NUM_TXBYTES_BITMASK		0x7F
#define CC2500_TXBYTES_NUM_TXBYTES_SHIFT		0

/* RXBYTES REGISTER BITMASKS AND SHIFTS */
#define CC2500_RXBYTES_OVERFLOW_BITMASK			0x80
#define CC2500_RXBYTES_OVERFLOW_SHIFT			7
#define CC2500_RXBYTES_NUM_RXBYTES_BITMASK		0x7F
#define CC2500_RXBYTES_NUM_RXBYTES_SHIFT		0

/* RECEIVED PACKET STATUS BYTE BITMASKS AND SHIFTS */
#define CC2500_RX_PACKET_LQI_BITMASK			0x7F
#define CC2500_RX_PACKET_LQI_SHIFT				0
#define CC2500_RX_PACKET_CRC_OK_BITMASK			0x80
#define CC2500_RX_PACKET_CRC_OK_SHIFT			7

