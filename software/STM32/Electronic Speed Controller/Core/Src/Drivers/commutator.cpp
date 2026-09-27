/*
 * commutator.cpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#include "Drivers/commutator.hpp"
#include "stm32g4xx_hal.h"
#include "tim.h"


Commutator::Commutator()
{
	// Charge the pumps on the mosfet gate drivers

	phase_control(Phase::A, PhaseMode::Sink);
	phase_control(Phase::B, PhaseMode::Sink);
	phase_control(Phase::C, PhaseMode::Sink);

	osDelay(100);

	disable_all_phases();
}

void Commutator::set_speed_percent(float speed_perc)
{
	/*
	 * The speed percentage becomes the duty cycle on the sourcing phase
	 */

	if (speed_perc > MAX_PWM_SOURCE_DUTY_CYCLE) speed_perc = MAX_PWM_SOURCE_DUTY_CYCLE;
	if (speed_perc < 0) speed_perc = 0;

	source_duty_cycle = speed_perc;

	// Update any currently sourcing phases
	if (TIM1->CCR1 != 0) TIM1->CCR1 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;
	if (TIM1->CCR2 != 0) TIM1->CCR2 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;
	if (TIM1->CCR3 != 0) TIM1->CCR3 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;

	// Update BEMF zero-crossing detection polling	-> for HAL_TIM_PWM_PulseFinishedCallback()
	TIM1->CCR4 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0 * BEMF_POLLING_RATIO;

	// Conserve power if necessary
	if (source_duty_cycle == 0) disable_all_phases();
}

void Commutator::bldc_step_open_loop()
{
	// Disable all outputs before new step (safety)
	disable_all_phases();

	// Increment step
	int step_increment = static_cast<int>(commutation_step) + 1 % 6;
	commutation_step = static_cast<BldcStep>(step_increment);

	// Open-Loop 6-Step Trapezoidal commutation
	switch (commutation_step) {
		case BldcStep::AH_BL:
			phase_control(Phase::A, PhaseMode::Source);
			phase_control(Phase::B, PhaseMode::Sink);
			break;
		case BldcStep::AH_CL:
			phase_control(Phase::A, PhaseMode::Source);
			phase_control(Phase::C, PhaseMode::Sink);
			break;
		case BldcStep::BH_CL:
			phase_control(Phase::B, PhaseMode::Source);
			phase_control(Phase::C, PhaseMode::Sink);
			break;
		case BldcStep::BH_AL:
			phase_control(Phase::B, PhaseMode::Source);
			phase_control(Phase::A, PhaseMode::Sink);
			break;
		case BldcStep::CH_AL:
			phase_control(Phase::C, PhaseMode::Source);
			phase_control(Phase::A, PhaseMode::Sink);
			break;
		case BldcStep::CH_BL:
			phase_control(Phase::C, PhaseMode::Source);
			phase_control(Phase::B, PhaseMode::Sink);
			break;
	}

}

void Commutator::bldc_step_closed_loop()
{
	// Closed-Loop 6-Step Trapezoidal commutation

	// Uses BEMF zero-crossing detection to switch to next commutation state

	switch (commutation_step) {
		case BldcStep::AH_BL: {
			// Falling-edge phase C
			uint32_t level = HAL_COMP_GetOutputLevel(phc_comp);
			if (level == COMP_OUTPUT_LEVEL_LOW) {
				bldc_step_open_loop();
			}
			break;
		}
		case BldcStep::AH_CL: {
			// Rising-edge phase B
			uint32_t level = HAL_COMP_GetOutputLevel(phb_comp);
			if (level == COMP_OUTPUT_LEVEL_HIGH) {
				bldc_step_open_loop();
			}
			break;
		}
		case BldcStep::BH_CL: {
			// Falling-edge phase A
			uint32_t level = HAL_COMP_GetOutputLevel(pha_comp);
			if (level == COMP_OUTPUT_LEVEL_LOW) {
				bldc_step_open_loop();
			}
			break;
		}
		case BldcStep::BH_AL: {
			// Rising-edge phase C
			uint32_t level = HAL_COMP_GetOutputLevel(phc_comp);
			if (level == COMP_OUTPUT_LEVEL_HIGH) {
				bldc_step_open_loop();
			}
			break;
		}
		case BldcStep::CH_AL: {
			// Falling-edge phase B
			uint32_t level = HAL_COMP_GetOutputLevel(phb_comp);
			if (level == COMP_OUTPUT_LEVEL_LOW) {
				bldc_step_open_loop();
			}
			break;
		}
		case BldcStep::CH_BL: {
			// Rising-edge phase A
			uint32_t level = HAL_COMP_GetOutputLevel(pha_comp);
			if (level == COMP_OUTPUT_LEVEL_HIGH) {
				bldc_step_open_loop();
			}
			break;
		}
	}
}

void Commutator::enable_bldc_step_closed_loop()
{
	// Enable timer interrupt for BEMF zero-crossing detection polling
	TIM1->DIER |= TIM_DIER_CC4IE;
}

void Commutator::disable_bldc_step_closed_loop()
{
	// Disable timer interrupt for BEMF zero-crossing detection polling
	TIM1->DIER &= ~TIM_DIER_CC4IE;
}

/*
 *
 * PRIVATE METHODS
 *
 */
void Commutator::phase_control(Phase phase, PhaseMode mode)
{
	/*
	 * 	Each mosfet half-bridge (per phase) uses complementary PWM.
	 * 	The non-inverted PWM signal is connected to the high-side
	 * 	mosfet and the inverted PWM signal to the low-side mosfet.
	 *
	 *	When the non-inverted PWM is logic high, the inverted PWM 
	 *  is logic low and vice versa.
	 *
	 *	The non-inverted PWM (controlling the high-side mosfet) is
	 *	configurable to be logic high for some number of counts in
	 *	the PWM counter period.
	 *
	 *	The inverted PWM (controlling the low-side mosfet) is logic
	 *	high for the remainder of the counts in the PWM counter period.
	 */

	uint32_t high_side_pwm_count = 0;

	switch (mode) {
		case PhaseMode::Source: {
			high_side_pwm_count = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0f;
			break;
		}
		case PhaseMode::Sink: {
			high_side_pwm_count = 0;
			break;
		}
	}

	switch (phase) {
		case Phase::A: {
			TIM1->CCR1 = high_side_pwm_count;
			TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;	// enables PWM channel
			break;
		}
		case Phase::B: {
			TIM1->CCR2 = high_side_pwm_count;
			TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;
			break;
		}
		case Phase::C: {
			TIM1->CCR3 = high_side_pwm_count;
			TIM1->CCER |= TIM_CCER_CC3E | TIM_CCER_CC3NE;
			break;
		}
	}
}

void Commutator::disable_all_phases()
{
	// Set inverted PWM (controlling the low-side mosfet) logic high for full PWM period
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;

	// Disable all PWM channels
	TIM1->CCER &= ~TIM_CCER_CC1E;
	TIM1->CCER &= ~TIM_CCER_CC1NE;
	TIM1->CCER &= ~TIM_CCER_CC2E;
	TIM1->CCER &= ~TIM_CCER_CC2NE;
	TIM1->CCER &= ~TIM_CCER_CC3E;
	TIM1->CCER &= ~TIM_CCER_CC3NE;
}


