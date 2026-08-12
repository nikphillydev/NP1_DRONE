/*
 * MovingAverageFilter.hpp
 *
 *  Created on: Mar 10, 2025
 *      Author: Nikolai Philipenko
 */

#pragma once

#include <vector>
#include <algorithm>

/*
 * Interface
 */
template <typename T>
class MovingAverageFilter
{
	/*
	 * Class to implement a generic moving average filter for numeric types
	 */
public:
	MovingAverageFilter(unsigned max_size);

	T update(T input);

private:
	std::vector<T> buffer{};
	const unsigned MAX_SIZE;
	unsigned index = 0;
	bool initialized = false;
};

/*
 * Implementation
 */
template <typename T>
MovingAverageFilter<T>::MovingAverageFilter(unsigned max_size)
	: MAX_SIZE(max_size)
{
	buffer.resize(max_size, 0);
}

template <typename T>
T MovingAverageFilter<T>::update(T input)
{
	if (!initialized)
	{
		// Initialize buffer with the same value
		std::fill(buffer.begin(), buffer.end(), input);
		initialized = true;
	}
	else
	{
		// Replace oldest element in buffer with input (rolling buffer topology)
		buffer.at(index++) = input;
		index = index % MAX_SIZE;
	}

	// Compute average
	T sum = 0;
	for (const T& value : buffer)
	{
		sum += value;
	}
	return static_cast<T>(sum / buffer.size());
}
