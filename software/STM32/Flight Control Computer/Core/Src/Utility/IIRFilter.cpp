/*
 * IIRFilter.cpp
 *
 *  Created on: Jan 31, 2025
 *      Author: Nikolai Philipenko
 */
#include "Utility/IIRFilter.hpp"
#include <cmath>


IIRFilter::IIRFilter(float cutoff_frequency, float sampling_frequency)
	: cutoff_frequency{cutoff_frequency},
	  sampling_frequency{sampling_frequency},
	  K{tanf(M_PI * (cutoff_frequency / sampling_frequency))}
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
		prev_input[0] = input;
		prev_input[1] = input;
		prev_output[0] = input;
		prev_output[1] = input;
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



