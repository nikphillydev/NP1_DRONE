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
#include "Drivers/bmp388.h"
#include "Drivers/lis3mdl.h"
#include "spi.h"
#include "i2c.h"
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
BMP388 barometer;
BMP388_CalibrationData calib_data;
LIS3MDL magnetometer;

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
uint32_t accIRQTaskBuffer[ 256 ];
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
uint32_t gyroIRQTaskBuffer[ 256 ];
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
/* Definitions for pressureIRQTask */
osThreadId_t pressureIRQTaskHandle;
uint32_t pressureIRQTaskBuffer[ 256 ];
osStaticThreadDef_t pressureIRQTaskControlBlock;
const osThreadAttr_t pressureIRQTask_attributes = {
  .name = "pressureIRQTask",
  .stack_mem = &pressureIRQTaskBuffer[0],
  .stack_size = sizeof(pressureIRQTaskBuffer),
  .cb_mem = &pressureIRQTaskControlBlock,
  .cb_size = sizeof(pressureIRQTaskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for magIRQTask */
osThreadId_t magIRQTaskHandle;
uint32_t magIRQTaskBuffer[ 256 ];
osStaticThreadDef_t magIRQTaskControlBlock;
const osThreadAttr_t magIRQTask_attributes = {
  .name = "magIRQTask",
  .stack_mem = &magIRQTaskBuffer[0],
  .stack_size = sizeof(magIRQTaskBuffer),
  .cb_mem = &magIRQTaskControlBlock,
  .cb_size = sizeof(magIRQTaskControlBlock),
  .priority = (osPriority_t) osPriorityHigh,
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
/* Definitions for i2c2Mutex */
osMutexId_t i2c2MutexHandle;
osStaticMutexDef_t myMutex03ControlBlock;
const osMutexAttr_t i2c2Mutex_attributes = {
  .name = "i2c2Mutex",
  .cb_mem = &myMutex03ControlBlock,
  .cb_size = sizeof(myMutex03ControlBlock),
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
/* Definitions for pressureIRQSemaphore */
osSemaphoreId_t pressureIRQSemaphoreHandle;
osStaticSemaphoreDef_t pressureIRQSemaphoreControlBlock;
const osSemaphoreAttr_t pressureIRQSemaphore_attributes = {
  .name = "pressureIRQSemaphore",
  .cb_mem = &pressureIRQSemaphoreControlBlock,
  .cb_size = sizeof(pressureIRQSemaphoreControlBlock),
};
/* Definitions for magIRQSemaphore */
osSemaphoreId_t magIRQSemaphoreHandle;
osStaticSemaphoreDef_t magIRQSemaphoreControlBlock;
const osSemaphoreAttr_t magIRQSemaphore_attributes = {
  .name = "magIRQSemaphore",
  .cb_mem = &magIRQSemaphoreControlBlock,
  .cb_size = sizeof(magIRQSemaphoreControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void start_application_task(void *argument);
void start_acc_irq_task(void *argument);
void start_gyro_irq_task(void *argument);
void start_logging_task(void *argument);
void start_pressure_irq_task(void *argument);
void start_mag_irq_task(void *argument);

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

  /* creation of i2c2Mutex */
  i2c2MutexHandle = osMutexNew(&i2c2Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of accIRQSemaphore */
  accIRQSemaphoreHandle = osSemaphoreNew(64, 0, &accIRQSemaphore_attributes);

  /* creation of gyroIRQSemaphore */
  gyroIRQSemaphoreHandle = osSemaphoreNew(64, 0, &gyroIRQSemaphore_attributes);

  /* creation of pressureIRQSemaphore */
  pressureIRQSemaphoreHandle = osSemaphoreNew(64, 0, &pressureIRQSemaphore_attributes);

  /* creation of magIRQSemaphore */
  magIRQSemaphoreHandle = osSemaphoreNew(64, 0, &magIRQSemaphore_attributes);

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

  /* creation of pressureIRQTask */
  pressureIRQTaskHandle = osThreadNew(start_pressure_irq_task, NULL, &pressureIRQTask_attributes);

  /* creation of magIRQTask */
  magIRQTaskHandle = osThreadNew(start_mag_irq_task, NULL, &magIRQTask_attributes);

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
	osDelay(400);
	USB_Log("------------ APPLICATION STARTING ------------", CRITICAL);
	osDelay(100);

	/* Initialize sensor drivers */
	BMI088_Init(&imu, &hspi1, &spi1MutexHandle, ACCEL_CS_GPIO_Port, GYRO_CS_GPIO_Port, ACCEL_CS_Pin, GYRO_CS_Pin, ACCEL_INT_Pin, GYRO_INT_Pin);
	BMP388_Init(&barometer, &calib_data, &hi2c2, &i2c2MutexHandle, BMP_INT_Pin);
	LIS3MDL_Init(&magnetometer, &hi2c2, &i2c2MutexHandle, MAG_DRDY_Pin);
	USB_Log("ALL SENSORS INITIALIZED", CRITICAL);

	/* Infinite loop */
	for(;;)
	{
//		// Raw format
//		float mag_x_raw = magnetometer.intensity_x_gauss*100.0*10.0;
//		float mag_y_raw = magnetometer.intensity_y_gauss*100.0*10.0;
//		float mag_z_raw = magnetometer.intensity_z_gauss*100.0*10.0;
//		// Uni format
//		float mag_x_uni = magnetometer.intensity_x_gauss*100.0;
//		float mag_y_uni = magnetometer.intensity_y_gauss*100.0;
//		float mag_z_uni = magnetometer.intensity_z_gauss*100.0;
//		char str[512];
//		snprintf(str, 512, "Uni:0,0,0,0,0,0,%f,%f,%f\r", mag_x_uni, mag_y_uni, mag_z_uni);
//		USB_Log(str, RAW);
//		snprintf(str, 512, "Raw:0,0,0,0,0,0,%d,%d,%d\r", (int)mag_x_raw, (int)mag_y_raw, (int)mag_z_raw);
//		USB_Log(str, RAW);


		char str[512];
		snprintf(str, 512, "Heading: %f", magnetometer.heading);
		USB_Log(str, RAW);
		osDelay(100);
//		USB_Log("nikolai informational log", INFO);
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
//		BMI088_LogAccData(&imu);
//		osDelay(2);
//		BMI088_LogGyroData(&imu);
//		osDelay(2);
//		BMP388_LogData(&barometer);
//		osDelay(2);
//		LIS3MDL_LogData(&magnetometer);
		osDelay(250);
	}
  /* USER CODE END start_logging_task */
}

/* USER CODE BEGIN Header_start_pressure_irq_task */
/**
* @brief Function implementing the pressureIRQTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_pressure_irq_task */
void start_pressure_irq_task(void *argument)
{
  /* USER CODE BEGIN start_pressure_irq_task */
	/* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(pressureIRQSemaphoreHandle, osWaitForever);
		BMP388_ReadData(&barometer);
	}
  /* USER CODE END start_pressure_irq_task */
}

/* USER CODE BEGIN Header_start_mag_irq_task */
/**
* @brief Function implementing the magIRQTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_mag_irq_task */
void start_mag_irq_task(void *argument)
{
  /* USER CODE BEGIN start_mag_irq_task */
	/* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(magIRQSemaphoreHandle, osWaitForever);
		LIS3MDL_ReadData(&magnetometer);
	}
  /* USER CODE END start_mag_irq_task */
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
	else if (GPIO_Pin == barometer.irq_pin)
	{
		osSemaphoreRelease(pressureIRQSemaphoreHandle);
	}
	else if (GPIO_Pin == magnetometer.irq_pin)
	{
		osSemaphoreRelease(magIRQSemaphoreHandle);
	}
}

/* USER CODE END Application */

