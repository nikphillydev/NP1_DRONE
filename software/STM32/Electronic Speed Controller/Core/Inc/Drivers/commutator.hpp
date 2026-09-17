/*
 * commutator.hpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "Drivers/commutator_types.hpp"
#include "comp.h"

/*
 * Class to enable motor control.
 */
class Commutator
{
public:
	Commutator();

	void set_speed_percent(float speed_perc);

	/* Switch to the next commutation step */
	void bldc_step_open_loop();

	/* Switch to the next commutation step IF bemf zero-crossing detected */
	void bldc_step_closed_loop();

	void enable_bldc_step_closed_loop();
	void disable_bldc_step_closed_loop();

private:
	// Members
	bldc_step_t commutation_step 	= AH_BL;
	float source_duty_cycle 		= 0;

	const float MAX_PWM_SOURCE_DUTY_CYCLE 	= 100.0f;
	const uint32_t PWM_COUNTER_PERIOD 		= 5311 + 1;		// Must match exactly Period+1 in tim.c
	const float BEMF_POLLING_RATIO 			= 0.9;

	COMP_HandleTypeDef* pha_comp = &hcomp1;
	COMP_HandleTypeDef* phb_comp = &hcomp2;
	COMP_HandleTypeDef* phc_comp = &hcomp4;

	// Phase control
	void phase_control(phase_t phase, phase_mode_t mode);
	void disable_all_phases();
};
