#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "debug.h"
//#include "sysType.h"

// OLED显示模块
#ifndef OLED_APP_CFG
	#define OLED_APP_CFG 			(1)
#endif

// 文字转语音模块
#ifndef TTS_APP_CFG
	#define TTS_APP_CFG				(0)
#endif

// ADC启动DMA模式,1:启用DMA模式，0：缺省中断模式
#ifndef ADC1_DMA_ENABLE
	#define ADC1_DMA_ENABLE				(1)
#endif

// 测试模拟ADC采集的数据，填充到ADC缓冲区
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
*                                      定义中断向量表地址
* 因低16k地址用于bootloader，我们的代码需要从16k开始，所以需要在启动时将中断向量重新映射到16k地址
* 目前只对F205有效，F103版本不使用bootloader
*********************************************************************************************************
*/
//#define IVECTOR_ADDR 					(16*1024)
#define IVECTOR_ADDR 					(0)		//如果不使用bootloaer（同时将IROM1的start改为0x8000000）

