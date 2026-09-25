/*
 * can_controller.cpp
 *
 *  Created on: Aug 31, 2026
 *      Author: Nikolai Philipenko
 */
#include "Controllers/CANController/can_controller.hpp"


bool CANController::send_heartbeat()
{
	switch (get_canbus_state()) {
		case CANBUS_STATE::NORMAL:
		{
			while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) <= 0)
			{
				logger.warn("CANController: CANBUS Tx Fifo full, send_heartbeat() waiting...");
				osDelay(CANBUS_DELAY_MS_TX_FIFO_FULL);
			}

			TxHeaderCan1.Identifier = CANBUS_MSG::HEARTBEAT;
			uint8_t tx_data[8]{};
			if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeaderCan1, tx_data) == HAL_OK)
			{
				return true;
			}
			logger.error("CANController: send_heartbeat() CANBUS transmit failure");
			break;
		}
		case CANBUS_STATE::ACTIVE_ERROR:
		{
			logger.error("CANController: Error on CANBUS");
			break;
		}
	}
	return false;
}

bool CANController::send_arm()
{
	switch (get_canbus_state()) {
		case CANBUS_STATE::NORMAL:
		{
			while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) <= 0)
			{
				logger.warn("CANController: CANBUS Tx Fifo full, send_arm() waiting...");
				osDelay(CANBUS_DELAY_MS_TX_FIFO_FULL);
			}

			TxHeaderCan1.Identifier = CANBUS_MSG::ARM;
			uint8_t tx_data[8]{};
			if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeaderCan1, tx_data) == HAL_OK)
			{
				return true;
			}
			logger.error("CANController: send_arm() CANBUS transmit failure");
			break;
		}
		case CANBUS_STATE::ACTIVE_ERROR:
		{
			logger.error("CANController: Error on CANBUS");
			break;
		}
	}
	return false;
}

bool CANController::send_disarm()
{
	switch (get_canbus_state()) {
		case CANBUS_STATE::NORMAL:
		{
			while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) <= 0)
			{
				logger.warn("CANController: CANBUS Tx Fifo full, send_disarm() waiting...");
				osDelay(CANBUS_DELAY_MS_TX_FIFO_FULL);
			}

			TxHeaderCan1.Identifier = CANBUS_MSG::DISARM;
			uint8_t tx_data[8]{};
			if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeaderCan1, tx_data) == HAL_OK)
			{
				return true;
			}
			logger.error("CANController: send_disarm() CANBUS transmit failure");
			break;
		}
		case CANBUS_STATE::ACTIVE_ERROR:
		{
			logger.error("CANController: Error on CANBUS");
			break;
		}
	}
	return false;
}

bool CANController::send_speed(uint16_t speed)
{
	switch (get_canbus_state()) {
		case CANBUS_STATE::NORMAL:
		{
			while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) <= 0)
			{
				logger.warn("CANController: CANBUS Tx Fifo full, send_speed() waiting...");
				osDelay(CANBUS_DELAY_MS_TX_FIFO_FULL);
			}

			TxHeaderCan1.Identifier = CANBUS_MSG::SPEED;
			uint8_t tx_data[8]{};
			tx_data[0] = (speed >> 8) & 0xFF;
			tx_data[1] = speed & 0xFF;
			if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeaderCan1, tx_data) == HAL_OK)
			{
				return true;
			}
			logger.error("CANController: send_speed() CANBUS transmit failure");
			break;
		}
		case CANBUS_STATE::ACTIVE_ERROR:
		{
			logger.error("CANController: Error on CANBUS");
			break;
		}
	}
	return false;
}

/*
 *
 * PRIVATE
 *
 */
CANBUS_STATE CANController::get_canbus_state()
{
	FDCAN_ProtocolStatusTypeDef protocol_status;
	if (HAL_FDCAN_GetProtocolStatus(hfdcan, &protocol_status) == HAL_OK)
	{
		if (protocol_status.ErrorPassive == 1)
		{
			return CANBUS_STATE::ACTIVE_ERROR;
		}
		else
		{
			return CANBUS_STATE::NORMAL;
		}
	}

	logger.error("CANController: Failed to get CANBUS state");
	return CANBUS_STATE::ACTIVE_ERROR;
}

