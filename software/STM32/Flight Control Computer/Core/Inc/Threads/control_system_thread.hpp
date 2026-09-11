/*
 * control_system_thread.hpp
 *
 *  Created on: Aug 24, 2026
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
void control_system_thread();

#ifdef __cplusplus
}
#endif
