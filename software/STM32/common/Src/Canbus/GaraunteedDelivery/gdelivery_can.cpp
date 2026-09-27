/*
 * gdelivery_can.cpp
 *
 *  Created on: Sep 27, 2026
 *      Author: Nikolai Philipenko
 */
#include "Canbus/GaraunteedDelivery/gdelivery_can.hpp"
#include "Canbus/can_message.hpp"


void GaraunteedDeliveryCAN::transmit_heartbeat()
{
	TxHeaderCan1.Identifier = static_cast<uint8_t>(CanbusMsgID::Heartbeat);
	uint8_t tx_data[8]{};
	transmit(&TxHeaderCan1, tx_data);
}

void GaraunteedDeliveryCAN::transmit_arm()
{
	TxHeaderCan1.Identifier = static_cast<uint8_t>(CanbusMsgID::Arm);
	uint8_t tx_data[8]{};
	transmit(&TxHeaderCan1, tx_data);
}

void GaraunteedDeliveryCAN::transmit_disarm()
{
	TxHeaderCan1.Identifier = static_cast<uint8_t>(CanbusMsgID::Disarm);
	uint8_t tx_data[8]{};
	transmit(&TxHeaderCan1, tx_data);
}

void GaraunteedDeliveryCAN::transmit_speed(uint16_t speed)
{
	TxHeaderCan1.Identifier = static_cast<uint8_t>(CanbusMsgID::Speed);
	uint8_t tx_data[8]{};
	tx_data[0] = (speed >> 8) & 0xFF;
	tx_data[1] = speed & 0xFF;
	transmit(&TxHeaderCan1, tx_data);
}

/*
 *
 * PRIVATE
 *
 */
CanbusState GaraunteedDeliveryCAN::get_canbus_state()
{
	FDCAN_ProtocolStatusTypeDef protocol_status;
	if (HAL_FDCAN_GetProtocolStatus(hfdcan, &protocol_status) == HAL_OK)
	{
		if (protocol_status.ErrorPassive == 1)
		{
			return CanbusState::Error;
		}
		else
		{
			return CanbusState::Normal;
		}
	}

	logger.error("GaraunteedDeliveryCAN: Failed to get CANBUS state");
	return CanbusState::Error;
}

void GaraunteedDeliveryCAN::transmit(const FDCAN_TxHeaderTypeDef* header, const uint8_t* tx_data)
{
	uint32_t retry_count = 0;
	while (retry_count < MAX_RETRY_COUNT)
	{
		switch (get_canbus_state())
		{
			case CanbusState::Normal:
			{
				while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) <= 0)
				{
					logger.warn("GaraunteedDeliveryCAN: CANBUS Tx Fifo full, waiting...");
					osDelay(CAN_TX_FIFO_FULL_DELAY_MS);
				}

				if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, header, tx_data) == HAL_OK)
				{
					return;
				}
				logger.error("GaraunteedDeliveryCAN: CANBUS transmit failure, retrying...");
				break;
			}
			case CanbusState::Error:
			{
				logger.error("GaraunteedDeliveryCAN: Error on CANBUS, retrying...");
				break;
			}
		}
		retry_count++;
	}
	logger.error("GaraunteedDeliveryCAN: Failed to transmit on CANBUS.");
}


