#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "main.h"
#include "app_cfg.h"

// ���ȷ�Χ
#define MAX_AMP		33
#define MIN_AMP		0
// ռ�ձȷ�Χ
#define MIN_DUTY	1
#define MAX_DUTY	100
// Ƶ�ʷ�Χ
#define MAX_FREQ	6500*10
// CPUID��ʼ��ַ
#define CPU_ID              ((unsigned int*)0x1fff7a10)

// Э��ͷ
#define HEARD_ID		0xAA	// Э��ͷ

// Э���ʶ
#define SEND_ID			0x9F	// ��λ�����ͱ�ʶ
#define DEBUG_ID		0xF1	// ������������ʶ
#define CHANG_BAUND_ID	0x88	// ���Ĵ��ڲ�����Э�飬������Ч

// Ӧ���ʶ
#define CMD_ACK_ID		0x80	// Ӧ�������ʶ
#define DATA_ACK_ID		0x01	// ��λ������Ӧ���ʶ
#define	HAND_CODE		0x80	// ������
#define	OUTPUT_CMD		0x7F	// ���ò����������
#define	COOLECT_CMD		0x7E	// ��λ���ɼ�����
#define GET_OUTPUTWAVE	0x79	// ��ȡ���������һ����������	

#define SETBAUND_CMD	0x00	// ���ò�����
#define READBAUND_CMD	0x01	// ��ȡ������

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
	uint8_t WaveType; // �������ͣ�1���ֽڣ���Χ��ֵ��enum eWaveType
	uint8_t	Freq[2]; // ���ٸ������������Ƶ��*10	2�ֽڣ��ȷ����ֽ�
	uint8_t Duty;	// ռ�ձȣ�1���ֽڣ���Χ��1~100%
	uint8_t Amplitude;	// ��ѹ���ȣ�1���ֽڣ���ѹ*10
} sDataType;

// ��������Э��
void Receive_Handle(void);

#endif	/* __PROTOCOL_H__ */
