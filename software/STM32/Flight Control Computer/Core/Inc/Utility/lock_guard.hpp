/*
 * mutex.hpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "cmsis_os.h"

namespace np
{
class lock_guard
{
	/*
	 * Class to implement mutex RAII for CMSIS-V2
	 */
public:
	lock_guard(osMutexId_t& mutex) : mutex(mutex) { osMutexAcquire(this->mutex, osWaitForever); }
	~lock_guard() { osMutexRelease(mutex); }
private:
	osMutexId_t& mutex;
};
} // namespace np

