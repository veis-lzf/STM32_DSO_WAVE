#include "protocol.h"
#include "bsp_uart.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
#include "debug.h"
#include "bsp_dsp_fft.h"

extern float32_t testInput_fft_2048[2048];
extern uint16_t g_usWaveBuff[WAVE_BUFFER_SIZE];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DAC_HandleTypeDef hdac;
extern DMA_HandleTypeDef hdma_dac1;
extern __IO uint16_t uhADC1ConvertedValue[ADC_BUFFER_SIZE];
extern __IO uint16_t uhADC2ConvertedValue[ADC_BUFFER_SIZE];
extern sDataType dac_wave_info;
extern sDataType adc_wave_info;
extern uint32_t rx_len;
extern uint8_t RX_BUFFER[RX_BUFFER_SIZE];
extern UART_HandleTypeDef huart1;
extern uint32_t sysTick_t;
uint8_t ack_buffer[ACK_BUFFER_LENGTH] = {0}; // 应答数据缓存
extern uint8_t  TimeBaseId;

static uint8_t checksum(uint8_t *arry, uint32_t len)
{
	uint8_t cks = arry[0];
	uint32_t i;
	for(i = 1; i < len; i++)
	{
		cks ^= arry[i];
	}
	return cks;
}

#if 0
/*
获取一个96位的CpuID，用数组保存;
以全字(32位)读取;
基地址：0x1FFF7A10;
*/
static void GetSTM32_CpuID(uint32_t* CpuID)
{
	//获取CPU唯一ID
	CpuID[0] = *(uint32_t*)(0x1FFF7A10);
	CpuID[1] = *(uint32_t*)(0x1FFF7A14);
	CpuID[2] = *(uint32_t*)(0x1FFF7A18);
}
#endif

static int OnCfgDebug(uint32_t vp_Type, uint32_t vp_P1, uint32_t vp_P2, uint32_t vp_P3)
{
//	p_info("info:OnCfgDebug:Type=%d,P1=%d,P2=%d,P3=%d.", vp_Type, vp_P1, vp_P2, vp_P3);
	
	switch(vp_Type)
	{
	case 1:
			HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
		break;
	case 2:
			dacl_SetSinWave(vp_P1, vp_P2);
		break;
	case 3:
			dacl_SetTriWave(0, vp_P1, vp_P2, 50);
		break;
	case 4:
			dacl_SetRectWave(0, vp_P1, vp_P2, vp_P3);;
		break;
	case 5:
			dacl_SetSawtoothWave(0, vp_P1, vp_P2);
		break;
	case 6:
		while(!uhADCConverFinish);
		uhADCConverFinish = 0;
		for(int i =0; i < ADC_BUFFER_SIZE; i++) {
			printf("%d\n", uhADC1ConvertedValue[i]);
		}
		break;
	case 7:
		{
			float32_t *ptestOutput_fft;
			for(int i =0 ; i < ADC_BUFFER_SIZE; i++)
			{
				testInput_fft_2048[i] = uhADC1ConvertedValue[i];
			}
			
			ptestOutput_fft = arm_rfft_fast_f32_app(testInput_fft_2048, ADC_BUFFER_SIZE, GetADCSampleRate(), 0);
			for(int i =0 ; i < 1024; i++)
			{
				printf("%f\n", ptestOutput_fft[i]);
			}
		}
		break;
	default:
		p_info("warn:PARAM INVALID!");
		break;
	}
	return 0;
}

// 格式：AT+cmdCfg=vl_CmdId,vl_Type,vl_P1,vl_P2,vl_P3
// 设置定时器命令：666
// 设置关闭和开始时间类型：1
// 开启时间和关闭时间：vl_P1，vl_P2
static int AT_DeviceHandle(const unsigned char *data_buf)
{
	uint32_t i, vl_CmdId, vl_Type, vl_P1, vl_P2, vl_P3;
	uint32_t nlen = strlen((const char *)data_buf);
	char vl_FormateStr[64];
	
	vl_CmdId = 0;
	vl_Type = 0;
	vl_P1 = 0;
	vl_P2 = 0;
	vl_P3 = 0;
	
//	p_dbg("data_buf=%s", data_buf);
	if(!strstr((const char *)data_buf, "="))
		goto RETURN;

	memset(vl_FormateStr, 0, sizeof(vl_FormateStr)/sizeof(vl_FormateStr[0]));
	memcpy(vl_FormateStr, "AT+cmdCfg=%d", strlen("AT+cmdCfg=%d"));
	
//	p_dbg("nlen=%d", nlen);
	for (i = 0; i < nlen; i++)
	{
		if ((',' == data_buf[i]) && (i < nlen - 1))
			memcpy(vl_FormateStr + strlen(vl_FormateStr), ",%d", strlen(",%d"));
	}
//	p_dbg("vl_FormateStr=%s", vl_FormateStr);
	sscanf((const char *)data_buf, vl_FormateStr, &vl_CmdId, &vl_Type, &vl_P1, &vl_P2, &vl_P3);
	
	memset((char *)data_buf, 0, nlen);
	
//	p_dbg("vl_CmdId=%d, vl_Type=%d, vl_P1=%d, vl_P2=%d, vl_P3=%d", vl_CmdId, vl_Type, vl_P1, vl_P2, vl_P3);
	
	return OnCfgDebug(vl_Type, vl_P1, vl_P2, vl_P3);

RETURN:
	return -1;
}

// 解析串口协议
void Receive_Handle(void)
{
	uint8_t cks = 0, i = 0;
	uint16_t len = 0;
//	p_dbg_track;
//	p_dbg("%s", RX_BUFFER);
	if(RX_BUFFER[0] == HEARD_ID)
	{
		len = (RX_BUFFER[1] << 8) | RX_BUFFER[2];
		cks = RX_BUFFER[0] ^ RX_BUFFER[1] ^ RX_BUFFER[2];
		for(i = 0; i < len; i++)
			cks ^= RX_BUFFER[3+i];
		
		if(cks != RX_BUFFER[rx_len - 1])
		{
			//p_err("命令校验错误");
			return;
		}
		switch(RX_BUFFER[3])
		{
			case SEND_ID:
			{
				switch(RX_BUFFER[4])
				{
					case HAND_CODE: // 握手应答
					{
						// AA xx 01 80 CPUID(4字节)  xx	
						uint8_t i = 0;
						ack_buffer[i++] = HEARD_ID;
						ack_buffer[i++] = 0; // len[1]
						ack_buffer[i++] = 2+4; // len[0]
						ack_buffer[i++] = DATA_ACK_ID;
						ack_buffer[i++] = CMD_ACK_ID;
						ack_buffer[i++] = (uint8_t)(((*CPU_ID) >> 24));
						ack_buffer[i++] = (uint8_t)(((*CPU_ID) >> 16));
						ack_buffer[i++] = (uint8_t)(((*CPU_ID) >> 8));
						ack_buffer[i++] = (uint8_t)(*CPU_ID);
						ack_buffer[i] 	= checksum(ack_buffer, i);
						i++;
						HAL_UART_Transmit(&huart1, ack_buffer, i, 0xff);
					}
					break;

					case GET_OUTPUTWAVE: // 获取输出波形的数据
					{
						// AA uLen 01 79 [result] 校验
						uint32_t i = 0, len = 0;
						ack_buffer[i++] = HEARD_ID;
						ack_buffer[i++] = (2 + WAVE_BUFFER_SIZE*2)>>8;
						ack_buffer[i++] = (2 + WAVE_BUFFER_SIZE*2)&0xff;
						ack_buffer[i++] = DATA_ACK_ID;
						ack_buffer[i++] = GET_OUTPUTWAVE;

						while(len < WAVE_BUFFER_SIZE)
						{
							ack_buffer[i++] = (uint8_t)(g_usWaveBuff[len] >> 8);
							ack_buffer[i++] = (uint8_t)g_usWaveBuff[len++];
						}
						ack_buffer[i] = checksum(ack_buffer, i);
						i++;
						HAL_UART_Transmit_DMA(&huart1, ack_buffer, i); // 采用DMA传输
					}
					break;
					
					case OUTPUT_CMD: // 设置输出波形应答
					{
						uint8_t ret = RET_OK;
						uint8_t i = 0;
						uint16_t val = 0;
						uint32_t freq = 0;
						uint16_t duty = 0;
						// 提取波形数据
						dac_wave_info.WaveType 	= RX_BUFFER[5];
						dac_wave_info.Freq[1] 	= RX_BUFFER[6];
						dac_wave_info.Freq[0] 	= RX_BUFFER[7];
						dac_wave_info.Duty 		= RX_BUFFER[8];
						dac_wave_info.Amplitude = RX_BUFFER[9];
						// 无效类型
						if(dac_wave_info.WaveType >= NULL_WAVE || dac_wave_info.WaveType < SIN_WAVE)
							ret = RET_ERROR;
						// 无效频率
						if(((dac_wave_info.Freq[1] << 8) | dac_wave_info.Freq[0]) > MAX_FREQ)
							ret = RET_ERROR;
						// 无效占空比
						if(dac_wave_info.Duty > MAX_DUTY || dac_wave_info.Duty < MIN_DUTY )
							ret = RET_ERROR;
						// 无效幅度
						if(dac_wave_info.Amplitude > MAX_AMP)
							ret = RET_ERROR;
						
						// 设置波形输出
						if(ret != RET_ERROR)
						{
							freq = ((dac_wave_info.Freq[1] << 8) | dac_wave_info.Freq[0]);
							val = (uint16_t)((float)(dac_wave_info.Amplitude / 33.0) * 4095);
							duty = dac_wave_info.Duty;
							
							switch(dac_wave_info.WaveType)
							{
								case SIN_WAVE:
									dacl_SetSinWave(val, freq);
									break;
								
								case DELTA_WAVE:
									dacl_SetTriWave(0, val, freq, duty);
									break;
								
								case SQUARD_WARE:
									dacl_SetRectWave(0, val, freq, duty);
									break;
								
								case SAWTOOTH_WAVE:
									dacl_SetSawtoothWave(0, val, freq);
									break;
								
								default:
									ret = RET_ERROR;
									break;
							}
						}
						
						// AA uLen 01 7F [result] 校验
						// 0x00：设置失败，参数无效
						// 0x01：设置成功		
						ack_buffer[i++] = HEARD_ID;
						ack_buffer[i++] = 0;
						ack_buffer[i++] = 2+1;
						ack_buffer[i++] = DATA_ACK_ID;
						ack_buffer[i++] = OUTPUT_CMD;
						ack_buffer[i++] = ret;
						ack_buffer[i] 	= checksum(ack_buffer, i);
						i++;
						HAL_UART_Transmit_DMA(&huart1, ack_buffer, i);
					}
					break;
					
					case COOLECT_CMD: // 采集波形数据应答
					{
						// AA uLen 01 7E [fre] [vpp] [result] 校验
						uint32_t i = 0, len = 0;
						ack_buffer[i++] = HEARD_ID;
						ack_buffer[i++] = (2+3+ADC_BUFFER_SIZE*2)>>8;
						ack_buffer[i++] = (2+3+ADC_BUFFER_SIZE*2)&0xff;
						ack_buffer[i++] = DATA_ACK_ID;
						ack_buffer[i++] = OUTPUT_CMD;
						ack_buffer[i++] = adc_wave_info.Freq[1];
						ack_buffer[i++] = adc_wave_info.Freq[0];
						ack_buffer[i++] = adc_wave_info.Amplitude;
						
//						HAL_ADC_Start_DMA(&hadc1, (uint32_t *)uhADC1ConvertedValue, ADC_BUFFER_SIZE);
						#if defined(TEST_ADC_DATA_RAND)
							srand(sysTick_t); // 初始化随机数
						#endif
						if(uhADCConverFinish)
						{
							while(len < ADC_BUFFER_SIZE)
							{
								#if defined(TEST_ADC_DATA_RAND)
									uhADC1ConvertedValue[len] = rand() % 4096;
								#endif
								ack_buffer[i++] = (uint8_t)(uhADC1ConvertedValue[len] >> 8);
								ack_buffer[i++] = (uint8_t)uhADC1ConvertedValue[len++];

								testInput_fft_2048[i] = uhADC1ConvertedValue[i];
							}
							// 计算FFT，得到输入信号的频率、峰峰值、最大值、最小值、平均值
							arm_rfft_fast_f32_app(testInput_fft_2048, ADC_BUFFER_SIZE, GetADCSampleRate(), 0);
							adc_wave_info.Freq[1] = ((uint16_t)(g_WaveFreq * 10) >> 8);
							adc_wave_info.Freq[0] = ((uint16_t)(g_WaveFreq * 10) & 0x00ff);
							adc_wave_info.Amplitude = g_WavePkPk * 10;
							
							ack_buffer[5] = adc_wave_info.Freq[1];
							ack_buffer[6] = adc_wave_info.Freq[0];
							ack_buffer[7] = adc_wave_info.Amplitude;
						}
						else
						{
							memset(&ack_buffer[i], 0, ADC_BUFFER_SIZE*2);
						}
						ack_buffer[i] = checksum(ack_buffer, i);
						i++;
						HAL_UART_Transmit_DMA(&huart1, ack_buffer, i); // 采用DMA传输
						uhADCConverFinish = 0;
					}
					break;
					
					case CHANG_BAUND_ID: // 修改波特率
					{
						// 00：设置
						// 01：读取
						switch(RX_BUFFER[5])
						{
							case SETBAUND_CMD:
								{
									uint32_t baund = 0;
									uint8_t ret;
									baund = (RX_BUFFER[6] << 24)|(RX_BUFFER[7] << 16)|(RX_BUFFER[8] << 8)|RX_BUFFER[9];
									switch(baund)
									{
										case BAUD_1200:
										case BAUD_2400:
										case BAUD_4800:
										case BAUD_9600:
										case BAUD_115200:
										case BAUD_230400:
										case BAUD_460800:
										case BAUD_921600:
										case BAUD_1500000:
										case BAUD_2000000:
										case BAUD_3000000:
											ret = RET_OK;
											break;
										default:
											ret = RET_ERROR;
											break;
									}
									uint32_t i = 0;
									ack_buffer[i++] = HEARD_ID;
									ack_buffer[i++] = 0;
									ack_buffer[i++] = 2+1;
									ack_buffer[i++] = CHANG_BAUND_ID;
									ack_buffer[i++] = 0; // 00：无需返回数据 01：返回读取的结果
									ack_buffer[i++] = ret;
									ack_buffer[i] = checksum(ack_buffer, i);
									i++;
									HAL_UART_Transmit(&huart1, ack_buffer, i, 0xffffffff); // 阻塞，确保发送完成再修改波特率
									if(ret)
									{
										bsp_uart1_init(baund);
									}
								}
								break;
							case READBAUND_CMD:
								break;
						}
					}
					break;
				}
			}
			break;
			
			case DEBUG_ID: // 调试指令
			{
			}
			break;			
		}
		
	}
	else
	{
		AT_DeviceHandle(RX_BUFFER); // 调用AT命令解析接口函数
	}
}
