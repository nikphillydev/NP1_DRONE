/*
 * main_thread.cpp
 *
 *  Created on: Jun 2, 2026
 *      Author: Nikolai Philipenko
 */

#include "main.h"
#include "fdcan.h"
#include "comp.h"

#include "Threads/main_thread.hpp"
#include "Drivers/commutator.hpp"
#include "constants.hpp"

/*
 *
 * PROTOTYPES
 *
 */
void handle_standby_state(esc_state_t& state, thread_input_t& input, Commutator& comm);
void handle_arming_state(esc_state_t& state, thread_input_t& input, Commutator& comm);
void handle_armed_state(esc_state_t& state, thread_input_t& input, Commutator& comm);

void Delay_us(uint32_t us);

/*
 *
 * GLOBALS
 *
 */
FDCAN_RxHeaderTypeDef rx_header;
uint8_t rx_data[CAN_BUFFER_SIZE];

const float ALIGN_SPEED_PERC 		= 5.0;
const float MIN_ARMED_SPEED_PERC	= 25.0;

const int ESC_ID					= 0;	// 0-3

/*
 *
 * THREADS
 *
 */
void main_thread()
{
	Commutator comm{};
	esc_state_t current_state = STAND_BY;

	const unsigned heartbeat_tick_delta = osKernelGetTickFreq() / REQUIRED_ESC_HEARTBEAT_HZ * HEARTBEAT_RX_TOLERANCE_MULTIPLIER;
	unsigned last_heartbeat_tick_count = 0;

	while(1)
	{
		// Try semantics
		thread_input_t input{ NO_MESSAGE };
		osMessageQueueGet(threadInputQueueHandle, &input, NULL, 0);

		// Check FCC heartbeat

		if (input.type == CAN_MSG_HEARTBEAT)
		{
			last_heartbeat_tick_count = osKernelGetTickCount();
		}
		if (osKernelGetTickCount() - last_heartbeat_tick_count > heartbeat_tick_delta)
		{
			thread_input_t input{ CAN_MSG_DISARM };
			osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
		}

		// ESC state machine

		switch (current_state)
		{
			case STAND_BY: {
				handle_standby_state(current_state, input, comm);
				break;
			}
			case ARMING: {
				handle_arming_state(current_state, input, comm);
				break;
			}
			case ARMED: {
				handle_armed_state(current_state, input, comm);
				break;
			}
		}
	}
}

/*
 *
 * STATE FUNCTIONS
 *
 */
void handle_standby_state(esc_state_t& current_state, thread_input_t& input, Commutator& comm)
{
	// Handle input
	if (input.type == CAN_MSG_ARM)
	{
		comm.precharge_drivers();
		current_state = ARMING;
		return;
	}

	// Responsibility
	comm.set_speed_percent(0);
}

void handle_arming_state(esc_state_t& current_state, thread_input_t& input, Commutator& comm)
{
	// ALIGN step tuning parameters
	const float align_time_seconds = 0.8;
	const unsigned align_ticks_delta = osKernelGetTickFreq() * align_time_seconds;

	// OPEN-LOOP Ramp up tuning parameters
	const int startup_delay_us 	= 5000;
	const int target_delay_us 	= 100;
	const int start_speed_perc 	= ALIGN_SPEED_PERC;
	const int target_speed_perc	= MIN_ARMED_SPEED_PERC;
	const int ramp_step			= 50;

	// Static function parameters
	static bool align_init = false;
	static int align_ticks_begin = 0;
	static int current_delay = startup_delay_us;
	static int current_speed = start_speed_perc;

	// Handle input
	if (input.type == CAN_MSG_DISARM || input.type == ARMING_COMPLETE)
	{
		// Reset static variables for next ARMING sequence
		align_init = false;
		align_ticks_begin = 0;
		current_delay = startup_delay_us;
		current_speed = start_speed_perc;

		if (input.type == CAN_MSG_DISARM)
		{
			current_state = STAND_BY;
			return;
		}
		else if (input.type == ARMING_COMPLETE)
		{
			comm.enable_closed_loop_bldc_step();
			current_state = ARMED;
			return;
		}
	}

	// Responsibility

	// ALIGN step

	if(!align_init)
	{
		comm.set_speed_percent(start_speed_perc);
		comm.open_loop_bldc_step();
		align_init = true;
		align_ticks_begin = osKernelGetTickCount();
	}

	if(osKernelGetTickCount() - align_ticks_begin > align_ticks_delta)
	{
		// OPEN-LOOP Ramp up step

		// current_speed goes from low to high
		// current_delay goes from high to low

		// both interpolate linearly between their bounds

		if(current_delay > target_delay_us)
		{
			comm.set_speed_percent(current_speed);
			comm.open_loop_bldc_step();

			Delay_us(current_delay);

			// Decrease delay (increase frequency) with ramp step
			current_delay -= ramp_step;

			// Linear interpolation with delay to increase speed
			current_speed = start_speed_perc + (float)(current_delay - startup_delay_us) * (target_speed_perc - start_speed_perc) / (target_delay_us - startup_delay_us);
		}
		else
		{
			thread_input_t input{ ARMING_COMPLETE };
			osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
		}
	}
}

void handle_armed_state(esc_state_t& current_state, thread_input_t& input, Commutator& comm)
{
	// Handle input
	if (input.type == CAN_MSG_DISARM)
	{
		comm.disable_closed_loop_bldc_step();
		current_state = STAND_BY;
		return;
	}

	// CLOSED-LOOP BLDC control

	else if (input.type == ISR_BEMF_POLL)
	{
		comm.closed_loop_bldc_step();
	}
	else if (input.type == CAN_MSG_SPEED)
	{
		// BIG ENDIAN
		uint16_t raw_speed_int = static_cast<uint16_t>(input.payload[0] << 8) | static_cast<uint16_t>(input.payload[1]);
		float raw_speed_frac = static_cast<float>(raw_speed_int) / UINT16_MAX;

		// raw speed 0% -> min allowed armed speed
		// raw speed 100% -> max speed

		float applied_speed_perc = (100.0 - MIN_ARMED_SPEED_PERC) * raw_speed_frac + MIN_ARMED_SPEED_PERC;

		comm.set_speed_percent(applied_speed_perc);
	}
}

void Delay_us (uint32_t us)
{
	// Reset the counter value
	TIM2->CNT = 0;

	// Perform wait
	while (TIM2->CNT < us);
}

/*
 *
 * CALLBACKS
 *
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		thread_input_t input{ ISR_BEMF_POLL };
		osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if (hfdcan->Instance == FDCAN2)
	{
		if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
		{
		    /* Retrieve Rx message from RX FIFO0 */
		    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK)
		    {
		    	Error_Handler();
		    }

		    switch (rx_header.Identifier)
		    {
				case 0: {
					thread_input_t input{ CAN_MSG_DISARM };
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
				case 1: {
					thread_input_t input{ CAN_MSG_ARM };
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
				case 2: {
					thread_input_t input { CAN_MSG_SPEED };
					input.payload[0] = rx_data[ESC_ID * 2];
					input.payload[1] = rx_data[(ESC_ID + 1) * 2 - 1];
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
				case 3: {
					thread_input_t input{ CAN_MSG_HEARTBEAT };
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
		    }
		}
	}
}
