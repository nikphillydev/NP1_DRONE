/*
 * phase_controller.hpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#include <cstdint>


class PhaseController
{
public:
	// Phase control
	void pha_source();
	void pha_sink();
	void phb_source();
	void phb_sink();
	void phc_source();
	void phc_sink();

	void disable_all_phases();

	// Closed-loop control
	void enable_bemf_irq_polling();
	void disable_bemf_irq_polling();

	// Getters / setters
	void set_source_duty_cycle(float duty_cycle);
	float get_source_duty_cycle();

private:
	// Members
	float source_duty_cycle = 0;

	// Per-Phase state
	bool pha_sourcing = false;
	bool phb_sourcing = false;
	bool phc_sourcing = false;
	bool bemf_irq_polling = false;

	// Constants
	const float MAX_PWM_SOURCE_DUTY_CYCLE = 100.0f;
	const uint32_t PWM_COUNTER_PERIOD = 5311 + 1;	// Must match exactly Period+1 in tim.c
	const float BEMF_POLLING_RATIO = 0.9;
};
