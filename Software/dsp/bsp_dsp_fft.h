#ifndef __BSP_DSP_FFT_H
#define __BSP_DSP_FFT_H

#include <main.h>
#include "app_cfg.h"

extern float32_t  g_WaveMean;
extern float32_t  g_WavePkPk;
extern float32_t  g_WaveFreq;
extern float32_t  g_WaveMax;
extern float32_t  g_WaveMin;

// 汇编FFT库，点数有限
uint32_t * DSP_FFTPhase_ASMLIB(uint32_t * input, uint32_t * output, uint16_t _usFFTPoints);
uint32_t * DSP_FFT_ASMLIB(uint32_t * input, uint32_t * output, uint16_t _usFFTPoints);

// 复数FFT和IFFT
float32_t * arm_cfft_f32_app(float32_t *pData,uint32_t ifftFlag);
// 调用示例
void arm_cfft_32_test(uint8_t flag);
void arm_cfft_radix2_q31_app(void);
void arm_cfft_radix2_q15_app(void);
void arm_cfft_radix4_q15_app(void);

void GetMeasureResult(float32_t *pSampData, uint32_t Size);

// 实数FFT和IFFT
float32_t * arm_rfft_fast_f32_app(float32_t *pSampData, uint32_t fftSize, float32_t sample_rate, uint32_t flag);
// 调用示例
void arm_rfft_q15_app(void);
void arm_rfft_q31_app(void);

#endif /* __BSP_DSP_FFT_H */
