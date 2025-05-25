/*
 * MovingAverageFilter.hpp
 *
 *  Created on: Mar 10, 2025
 *      Author: Nikolai Philipenko
 */

#pragma once

#include <vector>

template <typename T>
class MovingAverageFilter
{
	/*
	 * Class to implement a generic moving average filter
	 */
public:
	MovingAverageFilter(unsigned max_size);

	T update(T input);

private:
	std::vector<T> buffer{};
	const unsigned MAX_SIZE;
	unsigned index;
};

/*
 * Definitions
 */
template <typename T>
MovingAverageFilter<T>::MovingAverageFilter(unsigned max_size)
	: MAX_SIZE(max_size),
	  index(0)
{
	buffer.reserve(max_size);
}

template <typename T>
T MovingAverageFilter<T>::update(T input)
{
	if (buffer.size() < MAX_SIZE)
	{
		// Buffer not full, add input element to back
		buffer.push_back(input);
	}
	else
	{
		// Replace oldest element in buffer with input,
		// 		rolling buffer topology...
		buffer.at(index++) = input;
		if (index >= MAX_SIZE) index = 0;
	}

	// Compute average
	T sum = 0;
	for (const T& value : buffer)
	{
		sum += value;
	}
	return sum / buffer.size();
}
