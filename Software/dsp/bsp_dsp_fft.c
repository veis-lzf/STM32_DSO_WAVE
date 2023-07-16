#include <bsp_dsp_fft.h>
#include "bsp_adc.h"

/* �洢ƽ��ֵ�����ֵ��Ƶ�ʣ���Сֵ�����ֵ�ı��� */
float32_t   g_WaveMean = 0;
float32_t   g_WavePkPk = 0;
float32_t   g_WaveFreq = 0;
float32_t   g_WaveMax = 0;
float32_t   g_WaveMin = 0;

// �븴��FFT��ص�������������� 
#define TEST_LENGTH_SAMPLES 2048 

// FFT������������
float32_t testInput_fft_2048[2048];
float32_t *testOutput_fft_2048;

// ��ʵ��FFT��ص��������������
static float32_t rfft_testInput_f32[TEST_LENGTH_SAMPLES];
static float32_t rfft_testOutput_f32[TEST_LENGTH_SAMPLES];
static float32_t rfft_testOutput[TEST_LENGTH_SAMPLES];

/**
  * @name	   arm_rfft_fast_f32_app
  * @brief     �󸡵���2048�����FFT
  * @param	   pSampData����������������
			   flag��FFT(0)/IFFT(1)
  * @return    �������FFT�󷵻����ݵ�ַ
  * @DateTime  2019-7-27
  */
float32_t * arm_rfft_fast_f32_app(float32_t *pSampData, uint32_t fftSize,float32_t sample_rate, uint32_t flag)
{
	uint16_t i;
	uint16_t count = 0;
	float32_t fre = 0, max_amp = 0;
	arm_rfft_fast_instance_f32 S;
	
	// ��ʼ���ṹ��S�еĲ���
 	arm_rfft_fast_init_f32(&S, fftSize);
	
	// ����ʵ�����鲿��ʵ�����鲿..... ��˳��洢����
	for(i=0; i<fftSize; i++)
	{
	//	rfft_testInput_f32[i] = 1.2f*arm_sin_f32(2*3.1415926f*100*i/1000)+1;
		rfft_testInput_f32[i] = pSampData[i];
	}
	
	// 2048��ʵ���п���FFT������
	arm_rfft_fast_f32(&S, rfft_testInput_f32, rfft_testOutput_f32, 0);
	
	if(!flag) // ���任
	{
		// ��1024���ֵ
		arm_cmplx_mag_f32(rfft_testOutput_f32, rfft_testOutput, 1024);	
		
		// ����Ƶ��Ϊ1kHzʱ������Ƶ��
		max_amp = rfft_testOutput[1];
		for(i = 2; i < 1024; i++)
		{
			if(max_amp < rfft_testOutput[i])
			{
				max_amp  = rfft_testOutput[i];
				count = i;
			}
		}
		g_WaveFreq = fre = count*sample_rate/fftSize;
//		printf("�����ź�Ƶ�ʹ���ֵ��%f Hz\r\n", fre);
//		printf("-----------------------------------------------------\r\n");
		
		GetMeasureResult(pSampData, fftSize);
		
		return rfft_testOutput;
	}
	else if(flag) // ��任
	{
		arm_rfft_fast_f32(&S, rfft_testOutput_f32, rfft_testInput_f32, flag);
		return rfft_testInput_f32;
	}
	return 0;
}

void GetMeasureResult(float32_t *pSampData, uint32_t Size)
{
	uint32_t i;
	uint32_t avg = 0;
	g_WaveMin = 4095;
	g_WaveMax = 0.0f;
	g_WaveMean = 0.0f;
	/* ����ֵ�����ֵ����Сֵ */
	for (i = 0; i < Size; i++) 
	{
		if(pSampData[i] < g_WaveMin)
		{
			g_WaveMin = pSampData[i];
		}
		
		if(pSampData[i] > g_WaveMax)
		{
			g_WaveMax = pSampData[i];
		}
		avg += pSampData[i];
		
	}
	/* ��ƽ��ֵ */
	g_WaveMean = (avg / Size) * 3.3f / 4095;
	/* �����ֵ */
	g_WaveMax =  g_WaveMax * 3.3f / 4095;
	/* ����Сֵ */
	g_WaveMin = g_WaveMin * 3.3f / 4095;
	/* ����ֵ */
	g_WavePkPk = g_WaveMax - g_WaveMin;
}
