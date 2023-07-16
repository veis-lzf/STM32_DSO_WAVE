#ifndef __BSP_LED_H
#define __BSP_LED_H	

#include "main.h"
#include "app_cfg.h"

typedef enum
{
	LED_OFF,
	LED_ON,
} LEDStatus;

typedef enum
{
	LED0,
} LEDIndex;

void bsp_led_init(void);
void led_set_status(LEDIndex led_index, LEDStatus led_status);
void led_toggle_status(LEDIndex led_index);

#endif /* __BSP_ADC_H */















