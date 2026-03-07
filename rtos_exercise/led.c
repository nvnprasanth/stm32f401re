
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "shell.h"

static GPIO_InitTypeDef  GPIO_InitStruct;

static int led_cmd_handler(void *arg) {
    if (strcmp(arg, "on") == 0) {
        green_led(GPIO_PIN_SET);
    } else if (strcmp(arg, "off") == 0) {
        green_led(GPIO_PIN_RESET);
    } else if (strcmp(arg, "toggle") == 0) {
        green_led_toggle();
    } else {
        printf("Invalid LED command. Use: led on|off|toggle\n");
    }
    return 0;
}

int led_gpio_init(void)
{
  int ret = 0;

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  ret |= register_command(CMD_LED, "led", led_cmd_handler, "Control LED: led on|off");

  return ret;
}

void green_led(GPIO_PinState state)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, state);
}

void green_led_toggle(void)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}
