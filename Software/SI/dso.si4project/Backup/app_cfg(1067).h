#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "debug.h"
//#include "sysType.h"

// OLED��ʾģ��
#ifndef OLED_APP_CFG
	#define OLED_APP_CFG 			(1)
#endif

// ����ת����ģ��
#ifndef TTS_APP_CFG
	#define TTS_APP_CFG				(0)
#endif

// ADC����DMAģʽ,1:����DMAģʽ��0��ȱʡ�ж�ģʽ
#ifndef ADC1_DMA_ENABLE
	#define ADC1_DMA_ENABLE				(1)
#endif

// ����ģ��ADC�ɼ������ݣ���䵽ADC������
//#define TEST_ADC_DATA_RAND


#ifndef DSP_FPU_ENABLE
#define DSP_FPU_ENABLE	(1)
#endif

#if DSP_FPU_ENABLE
#include "arm_math.h"
#include ""
#endif

/*
*********************************************************************************************************
*                                      �����ж��������ַ
* ���16k��ַ����bootloader�����ǵĴ�����Ҫ��16k��ʼ��������Ҫ������ʱ���ж���������ӳ�䵽16k��ַ
* Ŀǰֻ��F205��Ч��F103�汾��ʹ��bootloader
*********************************************************************************************************
*/
//#define IVECTOR_ADDR 					(16*1024)
#define IVECTOR_ADDR 					(0)		//�����ʹ��bootloaer��ͬʱ��IROM1��start��Ϊ0x8000000��

