/*
 * main_thread.hpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 *
 *	My ESC software for commutating a BLDC motor. Uses 6-step trapezoidal control.
 *
 *  Design based on proposed embedded system design described in:
 *  "Managing Concurrency in Complex Embedded Systems" by David M. Cummings, NASA JPL.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * This #ifdef clause is needed because if a Cpp file defines a function declaration / prototype,
 * than that declaration cannot be used in a C file unless extern "C" is used.
 */

/*
 *
 * TYPES
 *
 */
typedef enum {
	NO_MESSAGE,
	CAN_MSG_ARM,
	CAN_MSG_DISARM,
	CAN_MSG_SPEED,
	CAN_MSG_HEARTBEAT,
	ARMING_COMPLETE,
	ISR_BEMF_POLL
} input_type_t;

typedef struct {
	input_type_t type;
	uint8_t payload[2];		// optional depending on type
} thread_input_t;

typedef enum {
	STAND_BY,
	ARMING,
	ARMED,
} esc_state_t;

/*
 *
 * THREADS
 *
 */
void main_thread();

#ifdef __cplusplus
}
#endif
