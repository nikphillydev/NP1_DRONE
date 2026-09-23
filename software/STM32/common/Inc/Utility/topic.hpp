/*
 * topic.hpp
 *
 *  Created on: Aug 24, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "cmsis_os.h"
#include <stack>
#include "Utility/lock_guard.hpp"


template <typename T>
class Topic {
	/*
	 * Class to implement a thread-safe, publish/receive messaging service.
	 */
public:
	Topic(osMutexId_t& mutex) : mutex(mutex) {}

	// Publish a message to the topic
	void publish(const T& tx_msg);

	// Try to receive the latest message from the topic, if one exists
	bool receive(T& rx_msg);

private:
	osMutexId_t& mutex;
	std::stack<T> stack{};
};


/*
 *
 * TEMPLATE IMPLEMENTATION
 *
 */
template <typename T>
void Topic<T>::publish(const T& tx_msg)
{
	np::lock_guard lock(mutex);

	while (!stack.empty())
	{
		stack.pop();
	}

	stack.push(tx_msg);
}

template <typename T>
bool Topic<T>::receive(T& rx_msg)
{
	np::lock_guard lock(mutex);

	if (stack.empty())
	{
		return false;
	}

	rx_msg = stack.top();
	return true;
}

