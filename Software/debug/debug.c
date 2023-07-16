#include "debug.h"
#include "bsp_uart.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;
// 调试等级
int dbg_level = DEBUG;


/**
 * @brief      获取系统时间基准
 *
 * @return     返回系统CPU运行时间
 */
uint32_t os_time_get(void)
{
	return HAL_GetTick();
}

/**
 * @brief      重写fputc
 *
 * @param[in]  ch    待发送参数
 * @param      f     设备文件
 *
 * @return     返回发送的字符
 */
int fputc(int ch, FILE *f)
{
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&ch, 1);
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
	return (ch);
}

void DebugLevelInit(iot_log_level_t level)
{
	dbg_level = level;
}
