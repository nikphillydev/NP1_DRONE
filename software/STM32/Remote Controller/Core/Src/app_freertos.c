/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fdcan.h"

#include "Threads/rc_controller.hpp"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for rcControllerTask */
osThreadId_t rcControllerTaskHandle;
uint32_t rcControllerTaskBuffer[ 2048 ];
osStaticThreadDef_t rcControllerTaskControlBlock;
const osThreadAttr_t rcControllerTask_attributes = {
  .name = "rcControllerTask",
  .stack_mem = &rcControllerTaskBuffer[0],
  .stack_size = sizeof(rcControllerTaskBuffer),
  .cb_mem = &rcControllerTaskControlBlock,
  .cb_size = sizeof(rcControllerTaskControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for rcControllerQueue */
osMessageQueueId_t rcControllerQueueHandle;
uint8_t rcControllerQueueBuffer[ 64 * sizeof( thread_input_t ) ];
osStaticMessageQDef_t rcControllerQueueControlBlock;
const osMessageQueueAttr_t rcControllerQueue_attributes = {
  .name = "rcControllerQueue",
  .cb_mem = &rcControllerQueueControlBlock,
  .cb_size = sizeof(rcControllerQueueControlBlock),
  .mq_mem = &rcControllerQueueBuffer,
  .mq_size = sizeof(rcControllerQueueBuffer)
};
/* Definitions for spi1Mutex */
osMutexId_t spi1MutexHandle;
osStaticMutexDef_t spi1MutexControlBlock;
const osMutexAttr_t spi1Mutex_attributes = {
  .name = "spi1Mutex",
  .cb_mem = &spi1MutexControlBlock,
  .cb_size = sizeof(spi1MutexControlBlock),
};
/* Definitions for usbMutex */
osMutexId_t usbMutexHandle;
osStaticMutexDef_t usbMutexControlBlock;
const osMutexAttr_t usbMutex_attributes = {
  .name = "usbMutex",
  .cb_mem = &usbMutexControlBlock,
  .cb_size = sizeof(usbMutexControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void start_rc_controller_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void) { }
__weak unsigned long getRunTimeCounterValue(void) { return 0; }
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of spi1Mutex */
  spi1MutexHandle = osMutexNew(&spi1Mutex_attributes);

  /* creation of usbMutex */
  usbMutexHandle = osMutexNew(&usbMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of rcControllerQueue */
  rcControllerQueueHandle = osMessageQueueNew (64, sizeof(thread_input_t), &rcControllerQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of rcControllerTask */
  rcControllerTaskHandle = osThreadNew(start_rc_controller_task, NULL, &rcControllerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_start_rc_controller_task */
/**
  * @brief  Function implementing the rcControllerTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_rc_controller_task */
void start_rc_controller_task(void *argument)
{
  /* USER CODE BEGIN start_rc_controller_task */
  /* Infinite loop */
  rc_controller_thread();
  /* USER CODE END start_rc_controller_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

