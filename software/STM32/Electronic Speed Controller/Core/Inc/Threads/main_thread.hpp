/*
 * main_thread.hpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 *
 *	My ESC software for commutating a BLDC motor.
 *	Uses 6-step trapezoidal control with bemf zero-crossing detection.
 *
 *  Software design based on proposed embedded system design described in:
 *  "Managing Concurrency in Complex Embedded Systems" by David M. Cummings, NASA JPL.
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
void main_thread();

#ifdef __cplusplus
}
#endif
