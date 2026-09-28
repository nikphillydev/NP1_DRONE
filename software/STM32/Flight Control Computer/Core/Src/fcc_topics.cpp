/*
 * fcc_topics.cpp
 *
 *  Created on: Aug 27, 2026
 *      Author: Nikolai Philipenko
 */
#include "fcc_topics.hpp"

/*
 * FCC Publish/Receive Topics
 */

Topic<DroneState> state_topic{stateTopicMutexHandle};

