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
#include "Threads/main_thread_types.hpp"
#include "Drivers/commutator.hpp"
#include "Message/can_message.hpp"
#include "constants.hpp"

/*
 *
 * PROTOTYPES
 *
 */
void handle_standby_state(EscState& current_state, ThreadInput& input, Commutator& comm);
void handle_arming_state(EscState& current_state, ThreadInput& input, Commutator& comm);
void handle_armed_state(EscState& current_state, ThreadInput& input, Commutator& comm);

void Delay_us(uint32_t us);

/*
 *
 * GLOBAL
 *
 */
FDCAN_RxHeaderTypeDef rx_header;
uint8_t rx_data[CAN_BUFFER_SIZE];

const int MIN_ARMED_SPEED_PERC		= 25;

const int ESC_ID					= 0;	// 0-3

/*
 *
 * THREADS
 *
 */
void main_thread()
{
	Commutator comm{};
	EscState current_state = EscState_Standby;

	const unsigned esc_heartbeat_tick_delta = osKernelGetTickFreq() / constants::REQUIRED_ESC_HEARTBEAT_HZ * constants::HEARTBEAT_RX_TOLERANCE_MULTIPLIER;
	unsigned esc_last_heartbeat_tick_count = 0;

	/*
	 * RUN ESC STATE MACHINE
	 */
	while(1)
	{
		// Try semantics
		ThreadInput input{ InputType_None };
		osMessageQueueGet(threadInputQueueHandle, &input, NULL, 0);

		// Check FCC heartbeat

		if (input.type == InputType_CanHeartbeat)
		{
			esc_last_heartbeat_tick_count = osKernelGetTickCount();
		}
		if (osKernelGetTickCount() - esc_last_heartbeat_tick_count > esc_heartbeat_tick_delta)
		{
			ThreadInput input{ InputType_CanDisarm };
			osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
		}

		// State machine

		switch (current_state)
		{
			case EscState_Standby: {
				handle_standby_state(current_state, input, comm);
				break;
			}
			case EscState_Arming: {
				handle_arming_state(current_state, input, comm);
				break;
			}
			case EscState_Armed: {
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
void handle_standby_state(EscState& current_state, ThreadInput& input, Commutator& comm)
{
	// Handle input
	if (input.type == InputType_CanArm)
	{
		current_state = EscState_Arming;
		return;
	}

	// Responsibility
	comm.set_speed_percent(0);
}

void handle_arming_state(EscState& current_state, ThreadInput& input, Commutator& comm)
{
	// ALIGN step tuning parameters
	const float align_time_seconds = 0.8;
	const unsigned align_ticks_delta = osKernelGetTickFreq() * align_time_seconds;

	// OPEN-LOOP Ramp up tuning parameters
	const int start_delay_us 	= 5000;
	const int target_delay_us 	= 100;
	const int start_speed_perc 	= 5;
	const int target_speed_perc	= MIN_ARMED_SPEED_PERC;
	const int ramp_step			= 50;

	// Static function parameters
	static bool align_init = false;
	static int align_ticks_begin = 0;
	static int current_delay = start_delay_us;
	static int current_speed = start_speed_perc;

	// Handle input
	if (input.type == InputType_CanDisarm || input.type == InputType_ArmingComplete)
	{
		// Reset static variables for next ARMING sequence
		align_init = false;
		align_ticks_begin = 0;
		current_delay = start_delay_us;
		current_speed = start_speed_perc;

		if (input.type == InputType_CanDisarm)
		{
			current_state = EscState_Standby;
			return;
		}
		else if (input.type == InputType_ArmingComplete)
		{
			comm.enable_bldc_step_closed_loop();
			current_state = EscState_Armed;
			return;
		}
	}

	// Responsibility

	// ALIGN step

	if(!align_init)
	{
		comm.set_speed_percent(start_speed_perc);
		comm.bldc_step_open_loop();
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
			comm.bldc_step_open_loop();

			Delay_us(current_delay);

			// Decrease delay (increase frequency) with ramp step
			current_delay -= ramp_step;

			// Linear interpolation with delay to increase speed
			current_speed = start_speed_perc + (float)(current_delay - start_delay_us) * (target_speed_perc - start_speed_perc) / (target_delay_us - start_delay_us);
		}
		else
		{
			ThreadInput input{ InputType_ArmingComplete };
			osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
		}
	}
}

void handle_armed_state(EscState& current_state, ThreadInput& input, Commutator& comm)
{
	// Handle input
	if (input.type == InputType_CanDisarm)
	{
		comm.disable_bldc_step_closed_loop();
		current_state = EscState_Standby;
		return;
	}

	// CLOSED-LOOP BLDC control

	else if (input.type == InputType_IsrBemfPoll)
	{
		comm.bldc_step_closed_loop();
	}
	else if (input.type == InputType_CanSpeed)
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
		ThreadInput input{ InputType_IsrBemfPoll };
		osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if (hfdcan->Instance == FDCAN2)
	{
		if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
		{
		    // Retrieve Rx message from RX FIFO0
		    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK)
		    {
		    	return;
//		    	Error_Handler();	// Do not want to just die
		    }

		    switch (static_cast<CanbusMsgID>(rx_header.Identifier))
		    {
				case CanbusMsgID::Disarm: {
					ThreadInput input{ InputType_CanDisarm };
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
				case CanbusMsgID::Arm: {
					ThreadInput input{ InputType_CanArm };
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
				case CanbusMsgID::Speed: {
					ThreadInput input { InputType_CanSpeed };
					input.payload[0] = rx_data[ESC_ID * 2];
					input.payload[1] = rx_data[(ESC_ID + 1) * 2 - 1];
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
				case CanbusMsgID::Heartbeat: {
					ThreadInput input{ InputType_CanHeartbeat };
					osMessageQueuePut(threadInputQueueHandle, &input, 0, 0);
					break;
				}
		    }
		}
	}
}
