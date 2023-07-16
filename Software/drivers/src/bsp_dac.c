#include "bsp_dac.h"
#include "debug.h"
#include "protocol.h"

TIM_HandleTypeDef htim6;
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;
sDataType dac_wave_info;

uint16_t g_usWaveBuff[WAVE_BUFFER_SIZE];     /* ���λ��� */

// ���ұ�
uint16_t g_SineWave128[128] = 
{
	0x0800,0x0864,0x08c9,0x092c,0x098f,0x09f1,0x0a52,0x0ab2,0x0b0f,0x0b6b,0x0bc5,0x0c1c,0x0c71,0x0cc3,0x0d13,0x0d5f,
	0x0da7,0x0ded,0x0e2e,0x0e6c,0x0ea6,0x0edc,0x0f0d,0x0f3a,0x0f63,0x0f87,0x0fa7,0x0fc2,0x0fd8,0x0fe9,0x0ff5,0x0ffd,
	0x0fff,0x0ffd,0x0ff5,0x0fe9,0x0fd8,0x0fc2,0x0fa7,0x0f87,0x0f63,0x0f3a,0x0f0d,0x0edc,0x0ea6,0x0e6c,0x0e2e,0x0ded,
	0x0da7,0x0d5f,0x0d13,0x0cc3,0x0c71,0x0c1c,0x0bc5,0x0b6b,0x0b0f,0x0ab2,0x0a52,0x09f1,0x098f,0x092c,0x08c9,0x0864,
	0x0800,0x079c,0x0737,0x06d4,0x0671,0x060f,0x05ae,0x054e,0x04f1,0x0495,0x043b,0x03e4,0x038f,0x033d,0x02ed,0x02a1,
	0x0259,0x0213,0x01d2,0x0194,0x015a,0x0124,0x00f3,0x00c6,0x009d,0x0079,0x0059,0x003e,0x0028,0x0017,0x000b,0x0003,
	0x0001,0x0003,0x000b,0x0017,0x0028,0x003e,0x0059,0x0079,0x009d,0x00c6,0x00f3,0x0124,0x015a,0x0194,0x01d2,0x0213,
	0x0259,0x02a1,0x02ed,0x033d,0x038f,0x03e4,0x043b,0x0495,0x04f1,0x054e,0x05ae,0x060f,0x0671,0x06d4,0x0737,0x079c,
};

#define MAX_DAC_VALUE	(4095)
#define MAX_DAC_VOLTAGE	(3.3f)

#define TEST_TriangleWaveGenerate	(0)

// �������ǲ����ʹ��
#if TEST_TriangleWaveGenerate
// APB1 MAX 42Mhz
static void bsp_time6_init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	__HAL_RCC_TIM6_CLK_ENABLE();

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 42-1;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 2-1;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}
#endif

/**
 *	@brief	���ʼ��DACͨ��1
 */
void bsp_dac_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	DAC_ChannelConfTypeDef sConfig = {0};
	
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
	// ��ʼ��GPIO
	GPIO_InitStruct.Pin = DAC_OUT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC_OUT1_GPIO_Port, &GPIO_InitStruct);
	
	// ����DAC����ʼ��λ�����������
	hdac.Instance = DAC;
	if (HAL_DAC_Init(&hdac) != HAL_OK)
	{
		Error_Handler();
	}
#if TEST_TriangleWaveGenerate
	sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
#else
	sConfig.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
#endif
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; // �ر���������������������������
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
#if TEST_TriangleWaveGenerate
	// ������ǲ�
	if (HAL_DACEx_TriangleWaveGenerate(&hdac, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_4095) != HAL_OK)
	{
		Error_Handler();
	}
#endif
    hdma_dac1.Instance = DMA1_Stream5;
    hdma_dac1.Init.Channel = DMA_CHANNEL_7;
    hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_dac1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_dac1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac1);
	
#if TEST_TriangleWaveGenerate	
	bsp_time6_init();
	HAL_TIM_Base_Start(&htim6);
	HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
#else
	HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, 0);
	HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
#endif
}

/**
 *	@brief	����DAC���Ƶ�ʺ�����DMA����
 *	@param	buffer_addr����������εĻ�������ַ
 *	@param	count������ĵ���
 *	@param	freq�����Ƶ��
 */
static void dac1_initfor_dma(uint32_t buffer_addr, uint32_t count, uint32_t freq)
{
	__IO uint32_t tim_clk;
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	DAC_ChannelConfTypeDef sConfig = {0};
	
	// ֹͣDMA�Ͷ�ʱ���ٽ�������
	HAL_TIM_Base_Stop(&htim6);
	HAL_DAC_Stop_DMA(&hdac, DAC1_CHANNEL_1);
	
	/* ��ʱ������ */
	__HAL_RCC_TIM6_CLK_ENABLE();
	// APB1 timerʱ��=sysclk/1/4x2������ƵΪ168MHzʱ��APB1�Ķ�ʱ��ʱ��Ϊ84MHz
	tim_clk =  SystemCoreClock / 2;
	// ���õĶ�ʱ������Ƶ�� / DMA�Ļ������ = �������Ƶ��
	freq = freq * count / 10;
	
	if(freq < 100)
	{
		htim6.Init.Prescaler = 10000 - 1; // ��Ƶ��=10000
		htim6.Init.Period = (tim_clk / 10000) / freq - 1; // �Զ���װ��ֵ
	}
	else if(freq < 3000)
	{
		htim6.Init.Prescaler = 100 - 1; // ��Ƶ��=100
		htim6.Init.Period = (tim_clk / 100) / freq - 1; // �Զ���װ��ֵ
	}
	else // ����4k��Ƶ�ʣ������Ƶ
	{
		htim6.Init.Prescaler = 0;
		htim6.Init.Period = tim_clk / freq - 1; // �Զ���װ��ֵ
	}
	
	htim6.Instance = TIM6;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK){
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK){
		Error_Handler();
	}
	
	/* ����DAC����ʼ��λ����������� */
	__HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = DAC_OUT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC_OUT1_GPIO_Port, &GPIO_InitStruct);
	
	hdac.Instance = DAC;
	if (HAL_DAC_Init(&hdac) != HAL_OK){
		Error_Handler();
	}
	sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; // �ر���������������������������
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}
	
	/* ����DMA */
	__HAL_RCC_DMA1_CLK_ENABLE(); // ʹ��DMA1ʱ��
	
	hdma_dac1.Instance = DMA1_Stream5;
    hdma_dac1.Init.Channel = DMA_CHANNEL_7;
    hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_dac1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_dac1) != HAL_OK){
      Error_Handler();
    }
    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac1);
	
	HAL_TIM_Base_Start(&htim6);
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)buffer_addr, count, DAC_ALIGN_12B_R);
}

/**
 *	@brief	����DAC�ĵ�ѹֵ����Χ��0~3.3V
 */
void bsp_set_dac_voltage(float value)
{
	if(value > MAX_DAC_VOLTAGE)
		return;
	
	bsp_set_dac_value((uint16_t)(value * MAX_DAC_VALUE / MAX_DAC_VOLTAGE));
}

/**
 *	@brief	����DAC��ADCֵ����Χ��0~4095
 */
void bsp_set_dac_value(uint16_t value)
{
	if(value > MAX_DAC_VALUE)
		return;
	
	HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, value);
}

/*
******************************************************************************************************** 
* �� �� ��: dac1_SetSinWave 
* ����˵�� : DAC1 ������Ҳ� 
* �� ��: _vpp ���� 0-4095; 
* _freq : Ƶ�� 
* �� �� ֵ: �� 
*************************************************** ******************************************************
*/
void dacl_SetSinWave(uint32_t vpp, uint32_t freq)
{
	uint16_t i;
	uint16_t dac;
	
	/* �������Ҳ����� */
	for (i = 0; i < 128; i++)
	{
		dac = (vpp * g_SineWave128[i]  / 4095);
		if (dac > 4095) dac = 4095;
		g_usWaveBuff[i] = dac;
	}
	dac1_initfor_dma((uint32_t)&g_usWaveBuff, WAVE_BUFFER_SIZE, freq);
}

/*
******************************************************************************************************** 
* �� �� ��: dacl_SetRectWave 
* ����˵�� : DAC1 �������
* �� ��: _low ���͵�ƽֵ; 
* _high : �ߵ�ƽֵ 
* freq�����Ƶ��
* _duty����ռ�ձȣ�1%~100%��������1%
* �� �� ֵ: �� 
*************************************************** ******************************************************
*/
void dacl_SetRectWave(uint16_t _low, uint16_t _high, uint32_t freq, uint8_t _duty)
{
	uint16_t i = 0;
	
	if (_high > 4095) _high = 4095;
	
	if(_duty >= 100) // 100%
	{
		for (i = 0; i < 128; i++)
		{
			g_usWaveBuff[i] = _high;
		}
	}
	else if(_duty == 0) // 0%
	{
		for (i = 0; i < 128; i++)
		{
			g_usWaveBuff[i] = _low;
		}
	}
	else if(_duty == 99)
	{
		g_usWaveBuff[i] = _low;
		for (i = 1; i < 128; i++)
		{
			g_usWaveBuff[i] = _high;
		}
	}
	else
	{
		for (i = 0; i < (_duty * 128) / 100; i++)
		{
			g_usWaveBuff[i] = _high;
		}
		for (; i < 128; i++)
		{
			g_usWaveBuff[i] = _low;
		}
	}
	dac1_initfor_dma((uint32_t)&g_usWaveBuff, WAVE_BUFFER_SIZE, freq);
}
/*
********************************************************************************************************* 
* �� �� ��: dac1_SetTriWave 
* ����˵�� : DAC1 ������ǲ� 
* �� ��: _low �͵�ƽʱ DAC, 
* _high : �ߵ�ƽʱ DAC 
* _freq : Ƶ�� Hz 
* _duty : ռ�ձ� 
* �� �� ֵ: �� 
*********************************************************************************************************
*/
void dacl_SetTriWave(uint16_t _low, uint16_t _high, uint32_t freq, uint8_t _duty)
{
	uint32_t i = 0;
	uint16_t dac;
	uint16_t m;
	
	m = (_duty * 128) /100;
	if(m == 0) m = 1;
	if(m > 127) m = 127;
	
	for (i = 0; i < m; i++)
	{
		dac = _low + ((_high - _low) * i) / m;
		if (dac > 4095) dac = 4095;
		g_usWaveBuff[i] = dac;
	}
	for (; i < 128; i++)
	{
		dac = _high - ((_high - _low) * (i - m)) / (128 - m);
		if (dac > 4095) dac = 4095;
		g_usWaveBuff[i] = dac;	
	}
	dac1_initfor_dma((uint32_t)&g_usWaveBuff, WAVE_BUFFER_SIZE, freq);
}
/*
********************************************************************************************************* 
* �� �� ��: dac1_SetTriWave 
* ����˵�� : DAC1 �����ݲ� 
* �� ��: _low �͵�ƽʱ DAC, 
* _high : �ߵ�ƽʱ DAC 
* _freq : Ƶ�� Hz 
* �� �� ֵ: �� 
*********************************************************************************************************
*/
void dacl_SetSawtoothWave(uint16_t _low, uint16_t _high, uint32_t freq)
{
	uint32_t i;
	uint16_t dac;
	
	for (i = 0;i < 128; i++)
	{
		dac = _low + ((_high - _low) * i) / 127;
		if (dac > 4095) dac = 4095;
		g_usWaveBuff[i] = dac;
	}
	dac1_initfor_dma((uint32_t)&g_usWaveBuff, WAVE_BUFFER_SIZE, freq);
}
