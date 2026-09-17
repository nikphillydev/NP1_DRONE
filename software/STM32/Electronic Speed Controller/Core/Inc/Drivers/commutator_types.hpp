/*
 * commutator_types.hpp
 *
 *  Created on: Sep 15, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

typedef enum {
	AH_BL,
	AH_CL,
	BH_CL,
	BH_AL,
	CH_AL,
	CH_BL,
} bldc_step_t;

typedef enum {
	A,
	B,
	C
} phase_t;

typedef enum {
	SOURCE,
	SINK
} phase_mode_t;
