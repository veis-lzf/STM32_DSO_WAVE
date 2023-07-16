#include "bsp_adc.h"
#include "debug.h"
#include "protocol.h"

uint8_t  TimeBaseId;
/* 用于配制定时器TIM1的周期和分频****************************************************************************/
/* 
	单通道采样率， 2.8Msps到1sps 
    三通道采样率， 8.4Msps到1sps
*/

typedef enum
{
	RATE_2_8MSPS,
	RATE_2MSPS,
	RATE_1MSPS,
	RATE_500KSPS,
	RATE_200KSPS,
	RATE_100KSPS,
	RATE_50KSPS,
	RATE_20KSPS,
	RATE_10KSPS,
	RATE_5KSPS,
	RATE_2KSPS,
	RATE_1KSPS,
	RATE_500SPS,
	RATE_200SPS,
	RATE_100SPS,
	RATE_50SPS,
	RATE_20SPS,
	RATE_10SPS,
	RATE_5SPS,
	RATE_2SPS,
	RATE_1SPS,
}ADC_SampleRate;

const uint32_t g_SampleFreqTable[][2] =
{
	{2800000,  1},     //2.8Msps 0
	{2000000,  1},     //2Msps 1
	{1000000,  1},     //1Msps 2
	{500000,   1},     //500Ksps 3
	{200000,   1},     //200Ksps 4
	
	{100000,   1},     //100Ksps 5
	{50000,    1},     //50Ksps 6
	{20000,    1},     //20Ksps 7
	{10000,    1},     //10Ksps 8
	{5000,     1},     //5Ksps 9
	
	{16800,      5},    //2Ksps 10
	{16800,     10},    //1Ksps 11
	{16800,     20},    //500sps 12
	{16800,     50},    //200sps 13
	{16800,     100},   //100sps 14
	
	{16800,     200},   //50sps 15
	{16800,     500},   //20sps 16
	{16800,     1000},  //10sps 17
	{16800,     2000},  //5sps 18
	{16800,     5000},  //2sps 19
	{16800,     10000}, //1sps 20
};

#define ADC_DUALMODE_ENABLE 0
#define ADCDMA_IRQ_ENABLE 1

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;
TIM_HandleTypeDef htim1;
ADC_SampleRate gSample_rate;
sDataType adc_wave_info;
__IO uint16_t uhADC1ConvertedValue[ADC_BUFFER_SIZE];	// ADC采集缓冲区
#if ADC_DUALMODE_ENABLE
__IO uint16_t uhADC2ConvertedValue[ADC_BUFFER_SIZE];	// ADC采集缓冲区
#endif
__IO uint16_t uhADCDualConver = 0;
__IO uint16_t uhADCConverFinish = 0;


float GetADCSampleRate(void)
{
	switch(gSample_rate)
	{
		case RATE_2_8MSPS:{return 2800000.0;};
		case RATE_2MSPS:{return 2000000.0;};
		case RATE_1MSPS:{return 1000000.0;};
		case RATE_500KSPS:{return 500000.0;};
		case RATE_200KSPS:{return 200000.0;};
		case RATE_100KSPS:{return 100000.0;};
		case RATE_50KSPS:{return 50000.0;};
		case RATE_20KSPS:{return 20000.0;};
		case RATE_10KSPS:{return 10000.0;};
		case RATE_5KSPS:{return 5000.0;};
		case RATE_2KSPS:{return 2000.0;};
		case RATE_1KSPS:{return 1000.0;};
		case RATE_500SPS:{return 500.0;};
		case RATE_200SPS:{return 200.0;};
		case RATE_100SPS:{return 100.0;};
		case RATE_50SPS:{return 50.0;};
		case RATE_20SPS:{return 20.0;};
		case RATE_10SPS:{return 10.0;};
		case RATE_5SPS:{return 5.0;};
		case RATE_2SPS:{return 2.0;};
		case RATE_1SPS:{return 1.0;};
	}
	return 0.0;
}
/**
 * @name	  bsp_InitADC
 * @brief     ADC1_IN1和ADC2_IN1 采用DMA交替采样方式，ADC3采用普通方式
 * @param	  空
 * @return    空
 * @DateTime  2023-04-20
 */
void bsp_InitADC()
{
	ADC_ChannelConfTypeDef sConfig = {0};
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_MultiModeTypeDef multimode = {0};
	
	__HAL_RCC_DMA2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();
//	__HAL_RCC_ADC2_CLK_ENABLE();
//	__HAL_RCC_ADC3_CLK_ENABLE();
	
#if ADCDMA_IRQ_ENABLE
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
#if ADC_DUALMODE_ENABLE
	HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
#endif
 #endif
	
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK){
		Error_Handler();
	}
	
	/* ADC1 GPIO Configuration */
    GPIO_InitStruct.Pin = ADC123_IN1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC123_IN1_GPIO_Port, &GPIO_InitStruct);
	
	/* ADC1 DMA Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK){
		Error_Handler();
    }
    __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
	
#if ADC_DUALMODE_ENABLE
	// 配置多通道ADC采样,交替模式
  	multimode.Mode = ADC_DUALMODE_INTERL;
	multimode.DMAAccessMode = ADC_DMAACCESSMODE_2;
	multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK){
		Error_Handler();
	}
#else
	multimode.Mode = ADC_MODE_INDEPENDENT;
	multimode.DMAAccessMode = ADC_DMAACCESSMODE_DISABLED;
	multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK){
		Error_Handler();
	}
#endif
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
		Error_Handler();
	}
	
#if ADC_DUALMODE_ENABLE
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
	hadc2.Init.ScanConvMode = DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.NbrOfConversion = 1;
	hadc2.Init.DMAContinuousRequests = ENABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}
	hdma_adc2.Instance = DMA2_Stream3;
    hdma_adc2.Init.Channel = DMA_CHANNEL_1;
    hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc2.Init.Mode = DMA_CIRCULAR;
    hdma_adc2.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&hadc2,DMA_Handle,hdma_adc2);
	
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
#endif
    /* ADC3 GPIO Configuration */
    GPIO_InitStruct.Pin = VER_ADC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(VER_ADC_GPIO_Port, &GPIO_InitStruct);

	hadc3.Instance = ADC3;
	hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc3.Init.Resolution = ADC_RESOLUTION_12B;
	hadc3.Init.ScanConvMode = DISABLE;
	hadc3.Init.ContinuousConvMode = DISABLE;
	hadc3.Init.DiscontinuousConvMode = DISABLE;
	hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc3.Init.NbrOfConversion = 1;
	hadc3.Init.DMAContinuousRequests = DISABLE;
	hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc3) != HAL_OK){
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK){
		Error_Handler();
	}
	
	/* ADC3 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
	HAL_ADC_Start_IT(&hadc3);

#if ADC_DUALMODE_ENABLE
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)uhADC1ConvertedValue, ADC_BUFFER_SIZE);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)uhADC2ConvertedValue, ADC_BUFFER_SIZE);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t *)uhADC1ConvertedValue, ADC_BUFFER_SIZE);
#else
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)uhADC1ConvertedValue, ADC_BUFFER_SIZE);
#endif
	gSample_rate = RATE_5KSPS;
	bsp_timer1_init(gSample_rate); // 5k
}

// 启动ADC3转换
void bsp_StartADC3()
{
	HAL_ADC_Start_IT(&hadc3);
}

// 停止ADC转换
void bsp_StopADC3()
{
	HAL_ADC_Stop_IT(&hadc3);
}

// 获取ADC原始值
uint16_t bsp_GetAdc3Value()
{
	return uhADCDualConver;
}


/* ADC转换完成回调函数 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
//	p_dbg_enter;
	if(hadc->DMA_Handle == &hdma_adc1)
	{
		uhADCConverFinish = 1;
	}
	if(hadc->DMA_Handle == &hdma_adc2)
	{
		HAL_DMA_Abort(&hdma_adc1);
	}
	if(hadc == &hadc3)
	{
		uhADCDualConver = HAL_ADC_GetValue(&hadc3);
	}
}

/*
 ********************************************************************************
system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

HCLK = SYSCLK / 1     (AHB1Periph)
PCLK2 = HCLK / 2      (APB2Periph)
PCLK1 = HCLK / 4      (APB1Periph)

因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	  
TIM1 更新周期是 = TIM1CLK / （TIM_Period + 1）/（TIM_Prescaler + 1）
********************************************************************************
*/
void bsp_timer1_init(uint32_t freq)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	
	__HAL_RCC_TIM1_CLK_ENABLE();

	if(freq > 20)
		freq = 0;
	
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = g_SampleFreqTable[freq][1] - 1;
	htim1.Init.Period = 168000000 / g_SampleFreqTable[freq][0] - 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK){
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK){
		Error_Handler();
	}
	
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK){
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK){
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (htim1.Init.Period + 1)/ 2;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}

	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}
