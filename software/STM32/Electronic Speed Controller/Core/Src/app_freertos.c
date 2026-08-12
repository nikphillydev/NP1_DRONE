/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

#include "Threads/main_thread.hpp"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
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
/* Definitions for mainTask */
osThreadId_t mainTaskHandle;
uint32_t mainTaskBuffer[ 1024 ];
osStaticThreadDef_t mainTaskControlBlock;
const osThreadAttr_t mainTask_attributes = {
  .name = "mainTask",
  .stack_mem = &mainTaskBuffer[0],
  .stack_size = sizeof(mainTaskBuffer),
  .cb_mem = &mainTaskControlBlock,
  .cb_size = sizeof(mainTaskControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for threadInputQueue */
osMessageQueueId_t threadInputQueueHandle;
uint8_t threadInputQueueBuffer[ 64 * sizeof( thread_input_t ) ];
osStaticMessageQDef_t threadInputQueueControlBlock;
const osMessageQueueAttr_t threadInputQueue_attributes = {
  .name = "threadInputQueue",
  .cb_mem = &threadInputQueueControlBlock,
  .cb_size = sizeof(threadInputQueueControlBlock),
  .mq_mem = &threadInputQueueBuffer,
  .mq_size = sizeof(threadInputQueueBuffer)
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void start_main_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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
  /* creation of threadInputQueue */
  threadInputQueueHandle = osMessageQueueNew (64, sizeof(thread_input_t), &threadInputQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of mainTask */
  mainTaskHandle = osThreadNew(start_main_task, NULL, &mainTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_start_main_task */
/**
  * @brief  Function implementing the mainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_main_task */
void start_main_task(void *argument)
{
  /* USER CODE BEGIN start_main_task */
  /* Infinite loop */
	main_thread();
  /* USER CODE END start_main_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

