#include "debug.h"
#include "bsp_uart.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;
// ���Եȼ�
int dbg_level = DEBUG;


/**
 * @brief      ��ȡϵͳʱ���׼
 *
 * @return     ����ϵͳCPU����ʱ��
 */
uint32_t os_time_get(void)
{
	return HAL_GetTick();
}

/**
 * @brief      ��дfputc
 *
 * @param[in]  ch    �����Ͳ���
 * @param      f     �豸�ļ�
 *
 * @return     ���ط��͵��ַ�
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
