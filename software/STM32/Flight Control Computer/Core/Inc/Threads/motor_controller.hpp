/*
 * motor_controller.cpp
 *
 *  Created on: Jun 1, 2026
 *      Author: Nikolai Philipenko
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/*
 * This #ifdef clause is needed because if a Cpp file defines a function declaration / prototype,
 * than that declaration cannot be used in a C file unless extern "C" is used.
 */

/*
 *
 * THREADS
 *
 */
void motor_controller_thread();

#ifdef __cplusplus
}
#endif
