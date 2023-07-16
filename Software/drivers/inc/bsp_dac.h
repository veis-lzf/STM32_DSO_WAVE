#ifndef __BSP_DAC_H
#define __BSP_DAC_H	

#include "main.h"
#include "app_cfg.h"

#define WAVE_BUFFER_SIZE	128

/* DAC初始化及基本操做接口函数 */
void bsp_dac_init(void);
void bsp_set_dac_value(uint16_t value);
void bsp_set_dac_voltage(float value);

/* 波形发生接口函数 */
void dacl_SetSinWave(uint32_t vpp, uint32_t freq);
void dacl_SetTriWave(uint16_t _low, uint16_t _high, uint32_t freq, uint8_t _duty);
void dacl_SetRectWave(uint16_t _low, uint16_t _high, uint32_t freq, uint8_t _duty);
void dacl_SetSawtoothWave(uint16_t _low, uint16_t _high, uint32_t freq);

#endif /* __BSP_ADC_H */
