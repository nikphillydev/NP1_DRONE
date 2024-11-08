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
#include "Drivers/usb.h"
#include "Drivers/bmi088.h"
#include "spi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
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

BMI088 imu;

/* USER CODE END Variables */
/* Definitions for applicationTask */
osThreadId_t applicationTaskHandle;
uint32_t applicationTaskBuffer[ 512 ];
osStaticThreadDef_t applicationTaskControlBlock;
const osThreadAttr_t applicationTask_attributes = {
  .name = "applicationTask",
  .stack_mem = &applicationTaskBuffer[0],
  .stack_size = sizeof(applicationTaskBuffer),
  .cb_mem = &applicationTaskControlBlock,
  .cb_size = sizeof(applicationTaskControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for accIRQTask */
osThreadId_t accIRQTaskHandle;
uint32_t accIRQTaskBuffer[ 512 ];
osStaticThreadDef_t accIRQTaskControlBlock;
const osThreadAttr_t accIRQTask_attributes = {
  .name = "accIRQTask",
  .stack_mem = &accIRQTaskBuffer[0],
  .stack_size = sizeof(accIRQTaskBuffer),
  .cb_mem = &accIRQTaskControlBlock,
  .cb_size = sizeof(accIRQTaskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for gyroIRQTask */
osThreadId_t gyroIRQTaskHandle;
uint32_t gyroIRQTaskBuffer[ 512 ];
osStaticThreadDef_t gyroIRQTaskControlBlock;
const osThreadAttr_t gyroIRQTask_attributes = {
  .name = "gyroIRQTask",
  .stack_mem = &gyroIRQTaskBuffer[0],
  .stack_size = sizeof(gyroIRQTaskBuffer),
  .cb_mem = &gyroIRQTaskControlBlock,
  .cb_size = sizeof(gyroIRQTaskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for loggingTask */
osThreadId_t loggingTaskHandle;
uint32_t loggingTaskBuffer[ 512 ];
osStaticThreadDef_t loggingTaskControlBlock;
const osThreadAttr_t loggingTask_attributes = {
  .name = "loggingTask",
  .stack_mem = &loggingTaskBuffer[0],
  .stack_size = sizeof(loggingTaskBuffer),
  .cb_mem = &loggingTaskControlBlock,
  .cb_size = sizeof(loggingTaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
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
/* Definitions for accIRQSemaphore */
osSemaphoreId_t accIRQSemaphoreHandle;
osStaticSemaphoreDef_t accIRQSemaphoreControlBlock;
const osSemaphoreAttr_t accIRQSemaphore_attributes = {
  .name = "accIRQSemaphore",
  .cb_mem = &accIRQSemaphoreControlBlock,
  .cb_size = sizeof(accIRQSemaphoreControlBlock),
};
/* Definitions for gyroIRQSemaphore */
osSemaphoreId_t gyroIRQSemaphoreHandle;
osStaticSemaphoreDef_t gyroIRQSemaphoreControlBlock;
const osSemaphoreAttr_t gyroIRQSemaphore_attributes = {
  .name = "gyroIRQSemaphore",
  .cb_mem = &gyroIRQSemaphoreControlBlock,
  .cb_size = sizeof(gyroIRQSemaphoreControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void start_application_task(void *argument);
void start_acc_irq_task(void *argument);
void start_gyro_irq_task(void *argument);
void start_logging_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* Create the semaphores(s) */
  /* creation of accIRQSemaphore */
  accIRQSemaphoreHandle = osSemaphoreNew(64, 0, &accIRQSemaphore_attributes);

  /* creation of gyroIRQSemaphore */
  gyroIRQSemaphoreHandle = osSemaphoreNew(64, 0, &gyroIRQSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of applicationTask */
  applicationTaskHandle = osThreadNew(start_application_task, NULL, &applicationTask_attributes);

  /* creation of accIRQTask */
  accIRQTaskHandle = osThreadNew(start_acc_irq_task, NULL, &accIRQTask_attributes);

  /* creation of gyroIRQTask */
  gyroIRQTaskHandle = osThreadNew(start_gyro_irq_task, NULL, &gyroIRQTask_attributes);

  /* creation of loggingTask */
  loggingTaskHandle = osThreadNew(start_logging_task, NULL, &loggingTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_start_application_task */
/**
  * @brief  Function implementing the applicationTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_application_task */
void start_application_task(void *argument)
{
  /* USER CODE BEGIN start_application_task */

	BMI088_Init(&imu, &hspi1, &spi1MutexHandle, ACCEL_CS_GPIO_Port, GYRO_CS_GPIO_Port, ACCEL_CS_Pin, GYRO_CS_Pin, ACCEL_INT_Pin, GYRO_INT_Pin);

	/* Infinite loop */
	for(;;)
	{
		osDelay(100);
		USB_Log("nikolai", INFO);
		osDelay(100);
		USB_Log("nikolai", WARN);
		osDelay(100);
		USB_Log("nikolai", ERR);
	}
  /* USER CODE END start_application_task */
}

/* USER CODE BEGIN Header_start_acc_irq_task */
/**
* @brief Deferred interrupt handling for BMI088 accelerometer.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_acc_irq_task */
void start_acc_irq_task(void *argument)
{
  /* USER CODE BEGIN start_acc_irq_task */
	/* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(accIRQSemaphoreHandle, osWaitForever);
		BMI088_ReadAccData(&imu);
		BMI088_ReadTempData(&imu); // TODO put this somewhere else?
	}
  /* USER CODE END start_acc_irq_task */
}

/* USER CODE BEGIN Header_start_gyro_irq_task */
/**
* @brief Deferred interrupt handling for BMI088 gyroscope.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_gyro_irq_task */
void start_gyro_irq_task(void *argument)
{
  /* USER CODE BEGIN start_gyro_irq_task */
	/* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(gyroIRQSemaphoreHandle, osWaitForever);
		BMI088_ReadGyroData(&imu);
	}
  /* USER CODE END start_gyro_irq_task */
}

/* USER CODE BEGIN Header_start_logging_task */
/**
* @brief Function implementing the loggingTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_logging_task */
void start_logging_task(void *argument)
{
	/* USER CODE BEGIN start_logging_task */
	/* Infinite loop */
	for(;;)
	{
		BMI088_LogAccData(&imu);
		osDelay(3);
		BMI088_LogGyroData(&imu);
		osDelay(3);
		BMI088_LogTempData(&imu);
		osDelay(3);
	}
	/* USER CODE END start_logging_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == imu.acc_irq_pin)
	{
		osSemaphoreRelease(accIRQSemaphoreHandle);
	}
	else if (GPIO_Pin == imu.gyro_irq_pin)
	{
		osSemaphoreRelease(gyroIRQSemaphoreHandle);
	}
}

/* USER CODE END Application */

