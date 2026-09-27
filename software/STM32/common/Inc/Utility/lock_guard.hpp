/*
 * lock_guard.hpp
 *
 *  Created on: Dec 15, 2024
 *      Author: Nikolai Philipenko
 */

#pragma once

#include "cmsis_os.h"

namespace np
{
/*
 * Class to implement mutex RAII for CMSIS-V2
 */
class lock_guard
{
public:
	lock_guard(osMutexId_t& mutex) : mutex(mutex) { osMutexAcquire(this->mutex, osWaitForever); }
	~lock_guard() { osMutexRelease(mutex); }
private:
	osMutexId_t& mutex;
};
} // namespace np

