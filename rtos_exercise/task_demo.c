/**
  ******************************************************************************
  * @file           : task_example.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include <string.h>
#include <stddef.h>
#include "led.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/


extern UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
void StartDefaultTask(void *argument);
void StartTask02(void *argument);

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int create_demo_tasks(void)
{

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* Infinite loop */
  int count = 0;
  char buf[32] = {};
  for(;;)
  {
	  sprintf(buf, "Task1 count %05d\n\r", ++count);
	  HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
      osDelay(1000);
#if ENABLE_ONBOARD_LED
      led_toggle();
#endif
  }
}

/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* Infinite loop */
  int count = 0;
  char buf[32] = {};
  for(;;)
  {
	sprintf(buf, "Task2 count %05d\n\r", ++count);
	HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
    osDelay(1000);
  }
}

