/*
 * IIRFilter.hpp
 *
 *  Created on: Jan 31, 2025
 *      Author: Nikolai Philipenko
 */
#pragma once

#include <cstdint>
#include <array>

class IIRFilter
{
	/*
	 * Class to implement a Butterworth second-order low-pass filter
	 */
public:
	IIRFilter(float cutoff_frequency, float sampling_frequency);

	float update(float input);

private:
	// Parameters
	const float cutoff_frequency;		// Hz
	const float sampling_frequency;		// Hz
	const float K;
	const float Q = 0.7071;				// Butterworth response quality factor

	// Filter coefficients
	std::array<float, 3> a {};
	std::array<float, 2> b {};

	// Previous input / outputs
	std::array<float, 2> prev_input {};
	std::array<float, 2> prev_output {};
};


