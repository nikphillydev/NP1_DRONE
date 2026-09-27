/*
 * rc_controller_types.hpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

/*
 *
 * RC CONTROLLER TYPES
 *
 */
typedef enum {
	ThreadInput_None,
	ThreadInput_SendHeartbeat,
	ThreadInput_SendArm,
	ThreadInput_SendDisarm,
	ThreadInput_SendThrottle
} ThreadInput;
