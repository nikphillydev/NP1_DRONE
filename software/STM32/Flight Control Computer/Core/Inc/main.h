/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* Export mutexes for everyone to use */
extern osMutexId_t spi1MutexHandle;
extern osMutexId_t usbMutexHandle;
extern osMutexId_t i2c2MutexHandle;
extern osMutexId_t accelDataMutexHandle;
extern osMutexId_t gyroDataMutexHandle;
extern osMutexId_t baroDataMutexHandle;
extern osMutexId_t magDataMutexHandle;
extern osMutexId_t stateMutexHandle;
extern osMutexId_t uart2MutexHandle;
extern osMutexId_t ultrasonicDataMutexHandle;
extern osMutexId_t flowDataMutexHandle;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO2_Pin GPIO_PIN_13
#define GPIO2_GPIO_Port GPIOC
#define FLOW_CS_Pin GPIO_PIN_14
#define FLOW_CS_GPIO_Port GPIOC
#define ACCEL_INT_Pin GPIO_PIN_15
#define ACCEL_INT_GPIO_Port GPIOC
#define ACCEL_INT_EXTI_IRQn EXTI15_10_IRQn
#define GYRO_CS_Pin GPIO_PIN_0
#define GYRO_CS_GPIO_Port GPIOA
#define GYRO_INT_Pin GPIO_PIN_1
#define GYRO_INT_GPIO_Port GPIOA
#define GYRO_INT_EXTI_IRQn EXTI1_IRQn
#define ACCEL_CS_Pin GPIO_PIN_4
#define ACCEL_CS_GPIO_Port GPIOA
#define CC2500_GDO0_Pin GPIO_PIN_0
#define CC2500_GDO0_GPIO_Port GPIOB
#define CC2500_CS_Pin GPIO_PIN_1
#define CC2500_CS_GPIO_Port GPIOB
#define CC2500_GDO2_Pin GPIO_PIN_2
#define CC2500_GDO2_GPIO_Port GPIOB
#define GPIO8_Pin GPIO_PIN_10
#define GPIO8_GPIO_Port GPIOB
#define GPIO9_Pin GPIO_PIN_11
#define GPIO9_GPIO_Port GPIOB
#define MAG_DRDY_Pin GPIO_PIN_12
#define MAG_DRDY_GPIO_Port GPIOB
#define MAG_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define MAG_INT_Pin GPIO_PIN_13
#define MAG_INT_GPIO_Port GPIOB
#define BMP_INT_Pin GPIO_PIN_14
#define BMP_INT_GPIO_Port GPIOB
#define BMP_INT_EXTI_IRQn EXTI15_10_IRQn
#define GPIO7_Pin GPIO_PIN_15
#define GPIO7_GPIO_Port GPIOB
#define GPIO6_Pin GPIO_PIN_10
#define GPIO6_GPIO_Port GPIOA
#define GPIO5_Pin GPIO_PIN_15
#define GPIO5_GPIO_Port GPIOA
#define GPIO4_Pin GPIO_PIN_4
#define GPIO4_GPIO_Port GPIOB
#define GPIO1_Pin GPIO_PIN_7
#define GPIO1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
