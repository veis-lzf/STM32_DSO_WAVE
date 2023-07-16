#ifndef __BSP_ADC_H
#define __BSP_ADC_H	

#include "main.h"
#include "app_cfg.h"

// 宏定义
#define ADC_BUFFER_SIZE		(2*1024)
#define Vref (3.301f)
#define ADCRand	(4095)
#define DIV_VOL	(1) // 分压比

void bsp_InitADC(void);
void bsp_StopADC(void);
void bsp_StartADC(void);
uint16_t bsp_GetAdcValue(void);
void bsp_timer1_init(uint32_t freq);
float GetADCSampleRate(void);

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern __IO uint16_t uhADC1ConvertedValue[ADC_BUFFER_SIZE];
extern __IO uint16_t uhADCConverFinish;

#endif /* __BSP_ADC_H */















