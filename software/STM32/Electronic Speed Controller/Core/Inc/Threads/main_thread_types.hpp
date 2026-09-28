/*
 * main_thread_types.hpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include <stdint.h>

/*
 *
 * MAIN THREAD TYPES
 *
 */
typedef enum {
	InputType_None,
	InputType_CanArm,
	InputType_CanDisarm,
	InputType_CanSpeed,
	InputType_CanHeartbeat,
	InputType_ArmingComplete,
	InputType_IsrBemfPoll
} InputType;

typedef struct {
	InputType type;
	uint8_t payload[2];		// optional depending on type
} ThreadInput;

#ifdef __cplusplus
enum class EscState
{
	StandBy,
	Arming,
	Armed,
};
#endif
