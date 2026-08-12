/*
 * IIRFilter.cpp
 *
 *  Created on: Jan 31, 2025
 *      Author: Nikolai Philipenko
 */
#include "Utility/IIRFilter.hpp"
#include <cmath>


IIRFilter::IIRFilter(float cutoff_freq_hz, float sampling_freq_hz)
	: cutoff_freq_hz{cutoff_freq_hz},
	  sampling_freq_hz{sampling_freq_hz},
	  K{tanf(M_PI * (cutoff_freq_hz / sampling_freq_hz))}
{
	a[0] = (K * K) / (K * K + K / Q + 1);
	a[1] = 2 * a[0];
	a[2] = a[0];

	b[0] = (2 * (K * K - 1)) / (K * K + K / Q + 1);
	b[1] = (K * K - K / Q + 1) / (K * K + K / Q + 1);
}

float IIRFilter::update(float input)
{
	if (!initialized)
	{
		std::fill(prev_input.begin(), prev_input.end(), input);
		std::fill(prev_output.begin(), prev_output.end(), input);
		initialized = true;
	}

	float sum1 = a[0] * input + a[1] * prev_input[0] + a[2] * prev_input[1];
	float sum2 = b[0] * prev_output[0] + b[1] * prev_output[1];
	float output = sum1 - sum2;

	prev_input[1] = prev_input[0];
	prev_input[0] = input;
	prev_output[1] = prev_output[0];
	prev_output[0] = output;
	return output;
}



