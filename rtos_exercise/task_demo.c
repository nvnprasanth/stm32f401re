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
#include "shell.h"
#include "led.h"

/* Private includes ----------------------------------------------------------*/

/* Definitions for myTask01 */
osThreadId_t myTask01Handle;
const osThreadAttr_t myTask01_attributes = {
  .name = "myTask01",
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
void StartTask01(void *argument);
void StartTask02(void *argument);

/* Private user code ---------------------------------------------------------*/

char task_buf[512];  // adjust size depending on number of tasks

static void printTaskStatus(void) {
    vTaskList(task_buf); // fills buffer with task info
    printf("Task Name\tState\tPrio\tStack\tNum\n");
    printf("%s\n", task_buf);
    printf("State: R=Running/Ready, B=Blocked, S=Suspended, X=eXecuting\n");
}

static int task_cmd_handler(void *arg) {
    if (strcmp(arg, "resume") == 0) {
        osThreadResume(myTask01Handle);
        osThreadResume(myTask02Handle);
    } else if (strcmp(arg, "suspend") == 0) {
        osThreadSuspend(myTask01Handle);
        osThreadSuspend(myTask02Handle);
    } else if (strcmp(arg, "status") == 0) {
        printTaskStatus();
    } else {
        printf("Invalid task command. Use: task resume|suspend|status\n");
    }
    return 0;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int create_demo_tasks(void)
{

  /* Create the thread(s) */
  /* creation of myTask01 */
  myTask01Handle = osThreadNew(StartTask01, NULL, &myTask01_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);
  return register_command(CMD_TASK, "task", task_cmd_handler, "Control tasks: task resume|suspend|status");
  return 0;
}

/* USER CODE BEGIN Header_StartTask01 */
/**
  * @brief  Function implementing the Task01 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartmyTask01 */
void StartTask01(void *argument)
{
  /* Infinite loop */
  int count = 0;
  printf("Task01 suspending itself\n");
  osThreadSuspend(osThreadGetId());
  for(;;)
  {
      osDelay(1000);
#if ENABLE_ONBOARD_LED
      green_led_toggle();
#endif
  }
}

/**
* @brief Function implementing the Task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* Infinite loop */
  int count = 0;
  printf("Task02 suspending itself\n");
  osThreadSuspend(osThreadGetId());
  for(;;)
  {
	  //printf( "Task2 count %05d\n", ++count);
    osDelay(1000);
  }
}

