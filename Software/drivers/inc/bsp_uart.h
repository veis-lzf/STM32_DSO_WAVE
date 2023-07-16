#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "main.h"

#define RX_BUFFER_SIZE	(1*1024)
#define ACK_BUFFER_LENGTH	(5*1024)

enum enumBaudRate
{
	BAUD_1200 = 1200,
	BAUD_2400 = 2400,
	BAUD_4800 = 4800,
	BAUD_9600 = 9600,
	BAUD_115200 = 115200,
	BAUD_230400 = 230400,
	BAUD_460800 = 460800,
	BAUD_921600 = 921600,
	BAUD_1500000 = 1500000,
	BAUD_2000000 = 2000000,
	BAUD_3000000 = 3000000,
	BAUD_INVALID = 0xffffff
};

void bsp_uart1_init(uint32_t baund);

#endif


