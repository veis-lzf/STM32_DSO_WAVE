#ifndef __WINMAIN_H__
#define __WINMAIN_H__

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


/*********************************���ͷ�ļ�*********************************/
#include "sysType.h" // ����ϵͳ����ֲ��������
#include "debug.h" // ������־���
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

/* ����ʱ��Ƭ��С,1ms */
#define TIME_SLICE	1000

/* ������ƿ����� */
typedef struct
{
	/* ������ָ�� */
	void (*task)(void);
	/* ������ʱ */
	uint32_t delay_t;
	/* �������� */
	uint32_t period_t;
} Task_PCB;

void InitSystem(void);
void SysTickCallback(void);
extern uint32_t sysTick_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
