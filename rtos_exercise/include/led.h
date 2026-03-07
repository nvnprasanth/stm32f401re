
#ifndef __LED_H__
#define __LED_H__

#define ENABLE_ONBOARD_LED 1

void led_gpio_init(void);
void green_led(GPIO_PinState state);
void green_led_toggle(void);

#endif