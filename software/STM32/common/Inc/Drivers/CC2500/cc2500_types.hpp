/*
 * cc2500_types.hpp
 *
 *  Created on: Jul 23, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include <cstdint>

/*
 *
 * CC2500 STATUS TYPES
 *
 */
enum class CC2500_State
{
	IDLE			=0b000,					// Idle state
	RX				=0b001,					// Receive mode
	TX				=0b010,					// Transmit mode
	FSTXON			=0b011,					// Frequency synthesizer is on, ready to start transmitting
	CALIBRATE		=0b100,					// Frequency synthesizer calibration is running
	SETTLING		=0b101,					// PLL is settling
	RXFIFO_OVERFLOW	=0b110,					// RX FIFO has overflowed. Read out any useful data, then flush the FIFO with SFRX
	TXFIFO_UNDERFLOW=0b111					// TX FIFO has underflowed. Acknowledge with SFTX
};

enum class CC2500_StatusUpdate
{
	RxFifoBytes,							// Update status with RX fifo bytes available
	TxFifoBytes								// Update status with TX fifo bytes free
};

struct CC2500_Status
{
	bool chip_ready;						// False until power and crystal have stabilized. Should always be true when using the SPI interface
	CC2500_State state;						// Indicates the current main state machine mode
	uint8_t fifo_bytes_available;			// The number of bytes available in the RX FIFO or free bytes in the TX FIFO (4 bit number, maximum 15 bytes)
};

/*
 *
 * APPLICATION TYPES
 *
 */
#define ADDRESS				0x69
#define PACKET_LENGTH		15
#define PAYLOAD_LENGTH		(PACKET_LENGTH - 1)

struct CC2500_Packet
{
	uint8_t id;								// Message ID
	uint8_t payload[PAYLOAD_LENGTH];		// Fixed-length payload
};
static_assert(PACKET_LENGTH == sizeof(CC2500_Packet), "CC2500 Packet size mismatch");

struct CC2500_PacketStatus
{
	float rssi;								// RSSI value, 0.5 dB resolution
	uint8_t lqi;							// The Link Quality Indicator estimates how easily a received signal can be demodulated
	bool crc_ok;							// True if CRC for received data OK (or	CRC disabled), false if CRC error in received data
};

