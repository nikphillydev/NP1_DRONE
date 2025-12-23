/*
 * radio.hpp
 *
 *  Created on: May 28, 2025
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
void radio_thread();

/*
 *
 * FUNCTIONS
 *
 */
void service_CC2500_receive_irq();

#ifdef __cplusplus
}
#endif
