#ifndef __WINMAIN_H__
#define __WINMAIN_H__

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


/*********************************框架头文件*********************************/
#include "sysType.h" // 包含系统可移植数据类型
#include "debug.h" // 调试日志输出
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

/* 定义时间片大小,1ms */
#define TIME_SLICE	1000

/* 任务控制块类型 */
typedef struct
{
	/* 任务函数指针 */
	void (*task)(void);
	/* 任务延时 */
	uint32_t delay_t;
	/* 任务周期 */
	uint32_t period_t;
} Task_PCB;

void InitSystem(void);
void SysTickCallback(void);
extern uint32_t sysTick_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
