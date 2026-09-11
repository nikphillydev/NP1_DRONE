/*
 * fcc_topics.hpp
 *
 *  Created on: Aug 24, 2026
 *      Author: Nikolai Philipenko
 */
#pragma once

#include "main.h"
#include "Utility/topic.hpp"
#include "Threads/sensor_fusion_thread.hpp"

/*
 * FCC Publish/Receive Topics
 */

extern Topic<drone_state_t> state_topic;
