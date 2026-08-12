/*
 * commutator.cpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#include "Drivers/commutator.hpp"
#include "stm32g4xx_hal.h"


void Commutator::precharge_drivers()
{
	controller.pha_sink();
	controller.phb_sink();
	controller.phc_sink();

	// Delay to charge pumps
	osDelay(100);

	controller.disable_all_phases();
}

void Commutator::set_speed_percent(float speed_perc)
{
	if (speed_perc > 100) speed_perc = 100;
	if (speed_perc < 0) speed_perc = 0;

	// speed is directly proportional to duty cycle on sourcing phase

	controller.set_source_duty_cycle(speed_perc);

	if (speed_perc == 0)
	{
		// if no speed (0% duty cycle on sourcing phase), disable phases to conserve power
		controller.disable_all_phases();
	}
}

void Commutator::open_loop_bldc_step()
{
	// Disable all outputs before new step (safety)
	controller.disable_all_phases();

	// Increment step
	commutation_step = static_cast<bldc_step_t>((commutation_step + 1) % 6);

	// Open-Loop 6-Step Trapezoidal commutation
	switch (commutation_step) {
		case AH_BL:
			controller.pha_source();
			controller.phb_sink();
			break;
		case AH_CL:
			controller.pha_source();
			controller.phc_sink();
			break;
		case BH_CL:
			controller.phb_source();
			controller.phc_sink();
			break;
		case BH_AL:
			controller.phb_source();
			controller.pha_sink();
			break;
		case CH_AL:
			controller.phc_source();
			controller.pha_sink();
			break;
		case CH_BL:
			controller.phc_source();
			controller.phb_sink();
			break;
		default:
			break;
	}

}

void Commutator::enable_closed_loop_bldc_step()
{
	controller.enable_bemf_irq_polling();
}

void Commutator::disable_closed_loop_bldc_step()
{
	controller.disable_bemf_irq_polling();
}

void Commutator::closed_loop_bldc_step()
{
	// Closed-Loop 6-Step Trapezoidal commutation
	// Check for BEMF zero-crossing to switch to next commutation state
	switch (commutation_step) {
		case AH_BL: {
			// Falling-edge phase C
			uint32_t level = HAL_COMP_GetOutputLevel(phc_comp);
			if (level == COMP_OUTPUT_LEVEL_LOW) {
				open_loop_bldc_step();
			}
			break;
		}
		case AH_CL: {
			// Rising-edge phase B
			uint32_t level = HAL_COMP_GetOutputLevel(phb_comp);
			if (level == COMP_OUTPUT_LEVEL_HIGH) {
				open_loop_bldc_step();
			}
			break;
		}
		case BH_CL: {
			// Falling-edge phase A
			uint32_t level = HAL_COMP_GetOutputLevel(pha_comp);
			if (level == COMP_OUTPUT_LEVEL_LOW) {
				open_loop_bldc_step();
			}
			break;
		}
		case BH_AL: {
			// Rising-edge phase C
			uint32_t level = HAL_COMP_GetOutputLevel(phc_comp);
			if (level == COMP_OUTPUT_LEVEL_HIGH) {
				open_loop_bldc_step();
			}
			break;
		}
		case CH_AL: {
			// Falling-edge phase B
			uint32_t level = HAL_COMP_GetOutputLevel(phb_comp);
			if (level == COMP_OUTPUT_LEVEL_LOW) {
				open_loop_bldc_step();
			}
			break;
		}
		case CH_BL: {
			// Rising-edge phase A
			uint32_t level = HAL_COMP_GetOutputLevel(pha_comp);
			if (level == COMP_OUTPUT_LEVEL_HIGH) {
				open_loop_bldc_step();
			}
			break;
		}
		default:
			break;
	}
}

