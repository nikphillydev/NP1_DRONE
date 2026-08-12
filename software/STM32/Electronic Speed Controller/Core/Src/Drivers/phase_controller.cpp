/*
 * phase_controller.cpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#include "Drivers/phase_controller.hpp"
#include "tim.h"


void PhaseController::pha_source()
{
	pha_sourcing = true;

	// Set high-side PWM on-count pulse
	TIM1->CCR1 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;

	// Enable output
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;

	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

void PhaseController::pha_sink()
{
	pha_sourcing = false;

	// Low-side PWM fully on
	TIM1->CCR1 = 0;

	// Enable output
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;

	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
}

void PhaseController::phb_source()
{
	phb_sourcing = true;

	// Set high-side PWM on-count pulse
	TIM1->CCR2 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;

	// Enable output
	TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;

	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
}

void PhaseController::phb_sink()
{
	phb_sourcing = false;

	// Low-side PWM fully on
	TIM1->CCR2 = 0;

	// Enable output
	TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;

	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}

void PhaseController::phc_source()
{
	phc_sourcing = true;

	// Set high-side PWM on-count pulse
	TIM1->CCR3 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;

	// Enable output
	TIM1->CCER |= TIM_CCER_CC3E | TIM_CCER_CC3NE;

	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
}

void PhaseController::phc_sink()
{
	phc_sourcing = false;

	// Low-side PWM fully on
	TIM1->CCR3 = 0;

	// Enable output
	TIM1->CCER |= TIM_CCER_CC3E | TIM_CCER_CC3NE;

	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void PhaseController::disable_all_phases()
{
	pha_sourcing = false;
	phb_sourcing = false;
	phc_sourcing = false;

	// Low-side PWM fully on
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;

	// Disable all PWM outputs
	TIM1->CCER &= ~TIM_CCER_CC1E;
	TIM1->CCER &= ~TIM_CCER_CC1NE;
	TIM1->CCER &= ~TIM_CCER_CC2E;
	TIM1->CCER &= ~TIM_CCER_CC2NE;
	TIM1->CCER &= ~TIM_CCER_CC3E;
	TIM1->CCER &= ~TIM_CCER_CC3NE;

	// Turn off LEDs
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void PhaseController::enable_bemf_irq_polling()
{
	bemf_irq_polling = true;

	// Adjust for PWM PulseFinishedCallback
	TIM1->CCR4 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0 * BEMF_POLLING_RATIO;

	// Enable timer interrupt for BEMF zero-crossing synchronous polling
	TIM1->DIER |= TIM_DIER_CC4IE;
}

void PhaseController::disable_bemf_irq_polling()
{
	bemf_irq_polling = false;

	// Disable timer interrupt for BEMF zero-crossing synchronous polling
	TIM1->DIER &= ~TIM_DIER_CC4IE;
}

void PhaseController::set_source_duty_cycle(float duty_cycle)
{
	if (duty_cycle > MAX_PWM_SOURCE_DUTY_CYCLE) duty_cycle = MAX_PWM_SOURCE_DUTY_CYCLE;
	if (duty_cycle < 0) duty_cycle = 0;

	source_duty_cycle = duty_cycle;

	if (pha_sourcing) TIM1->CCR1 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;
	if (phb_sourcing) TIM1->CCR2 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;
	if (phc_sourcing) TIM1->CCR3 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0;
	if (bemf_irq_polling) TIM1->CCR4 = PWM_COUNTER_PERIOD * source_duty_cycle / 100.0 * BEMF_POLLING_RATIO;
}

float PhaseController::get_source_duty_cycle()
{
	return source_duty_cycle;
}
