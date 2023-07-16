#include "bsp_led.h"
#include "debug.h"

/**
 *	@bref	led初始化
 *	@param	无
 *	@return	无
 */
void bsp_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = LED0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);
}

/**
 *	@bref	led状态设置
 *	@param	led_index[in]：led的序号
 *	@param	led_status[in]：led的状态
 *	@return	无
 */
void led_set_status(LEDIndex led_index, LEDStatus led_status)
{
	switch(led_index)
	{
		case LED0:
		{
			if(led_status == LED_OFF)
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
			else
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
		}
		break;
		
		default:
		break;
	}
}

/**
 *	@bref	led状态取反
 *	@param	led_index：[in]led的序号
 *	@return	无
 */
void led_toggle_status(LEDIndex led_index)
{
	switch(led_index)
	{
		case LED0:
			HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		break;
		
		default:
		break;
	}
}
