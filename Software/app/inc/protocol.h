#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "main.h"
#include "app_cfg.h"

// 幅度范围
#define MAX_AMP		33
#define MIN_AMP		0
// 占空比范围
#define MIN_DUTY	1
#define MAX_DUTY	100
// 频率范围
#define MAX_FREQ	6500*10
// CPUID起始地址
#define CPU_ID              ((unsigned int*)0x1fff7a10)

// 协议头
#define HEARD_ID		0xAA	// 协议头

// 协议标识
#define SEND_ID			0x9F	// 上位机发送标识
#define DEBUG_ID		0xF1	// 调试命令主标识
#define CHANG_BAUND_ID	0x88	// 更改串口波特率协议，立即生效

// 应答标识
#define CMD_ACK_ID		0x80	// 应答命令标识
#define DATA_ACK_ID		0x01	// 下位机数据应答标识
#define	HAND_CODE		0x80	// 握手码
#define	OUTPUT_CMD		0x7F	// 设置波形输出参数
#define	COOLECT_CMD		0x7E	// 上位机采集波形
#define GET_OUTPUTWAVE	0x79	// 读取波形输出的一个周期数据	

#define SETBAUND_CMD	0x00	// 设置波特率
#define READBAUND_CMD	0x01	// 读取波特率

enum eControlRet
{
	RET_ERROR,
	RET_OK,
};

enum eWaveType
{
	SIN_WAVE = 0x01,
	DELTA_WAVE,
	SQUARD_WARE,
	SAWTOOTH_WAVE,
	
	NULL_WAVE,
};

typedef struct 
{
// private:
	uint8_t WaveType; // 波形类型，1个字节，范围数值见enum eWaveType
	uint8_t	Freq[2]; // 减少浮点引入固区：频率*10	2字节，先发高字节
	uint8_t Duty;	// 占空比，1个字节，范围：1~100%
	uint8_t Amplitude;	// 电压幅度，1个字节，电压*10
} sDataType;

// 解析串口协议
void Receive_Handle(void);

#endif	/* __PROTOCOL_H__ */
