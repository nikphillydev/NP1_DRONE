/*
 * commutator.hpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "Drivers/phase_controller.hpp"
#include "comp.h"


typedef enum {
	AH_BL,
	AH_CL,
	BH_CL,
	BH_AL,
	CH_AL,
	CH_BL,
} bldc_step_t;

/*
 * Class to enable motor control.
 */
class Commutator
{
public:
	void precharge_drivers();

	void set_speed_percent(float speed_perc);

	void open_loop_bldc_step();

	void enable_closed_loop_bldc_step();
	void disable_closed_loop_bldc_step();

	void closed_loop_bldc_step();

private:
	// Members
	PhaseController controller;
	bldc_step_t commutation_step = AH_BL;

	COMP_HandleTypeDef* pha_comp = &hcomp1;
	COMP_HandleTypeDef* phb_comp = &hcomp2;
	COMP_HandleTypeDef* phc_comp = &hcomp4;
};
