#include "stm32f4xx_hal.h"

static GPIO_InitTypeDef  GPIO_InitStruct;


void led_gpio_init(void)
{     
  /*##-2- Configure PA05 IO in output push-pull mode to drive external LED ###*/  
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

}

void led_toggle(void)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}