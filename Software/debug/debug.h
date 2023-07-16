  #ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include "main.h"

/* keil��Ĭ�ϲ�֧�����������壬����Ҫ������ */
#pragma anon_unions

#ifndef DEBUG_USART
#define DEBUG_USART		 USART1
#endif

typedef enum DebugLevel
{
    IOT_LOG_LEVEL_DEBUG = 0,     //������������ڿ����׶εĵ��ԣ�������ĳ�����߼��ؼ���ı���ֵ������������Ǻ�������ֵ����֤�ȵ�   
    IOT_LOG_LEVEL_INFO,          //�������������ҵ���¼���Ϣ������ĳ��ҵ������ϣ�����ҵ��������е�һЩ��Ϣ
    IOT_LOG_LEVEL_WARN,          //�������Ǳ�ڵĴ��󣬻��ߴ����������������Ĳ�����������Լ������У���������ע��
    IOT_LOG_LEVEL_ERROR,         //�������˱������ϴ���Ĵ��󡣴����������Ժ�����������������У�����������������������������ͻᵼ�²��������Ӧ��ҵ��
    IOT_LOG_LEVEL_FATAL,         //�������������صĴ��󣬻ᵼ����������ֹͣ��������Ҫ��������ֹͣ�����򵥵�˵���Ƿ��������ˡ�
    IOT_LOG_LEVEL_NONE,          //
} iot_log_level_t;


#define DEBUG 1
#define RELEASE_VERSION			0		// ��1�󽫹ر����д�ӡ��Ϣ

#if RELEASE_VERSION		
#undef DEBUG
#endif

#ifdef DEBUG
// ��ӡ������Ϣ����λ��ʶ��I
#define p_info(...)                                                  \
do                                                                   \
{                                                                    \
	if(dbg_level >= IOT_LOG_LEVEL_INFO){                              \
	printf("[I: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);\
	printf(__VA_ARGS__);                                             \
	printf("\r\n");}                                                 \
}while(0)

// ��ӡ������Ϣ����λ��ʶ��E
#define p_err(...)                                                   \
do																	 \
{																	 \
	if(dbg_level >= IOT_LOG_LEVEL_ERROR) {							 \
	printf("[E: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);\
	printf(__VA_ARGS__);											 \
	printf("\r\n");} 												 \
}while(0)

// ��ӡ������Ϣ����λ��ʶ��D
#define p_dbg(...)                                                   \
do																	 \
{																	 \
	if(dbg_level >= IOT_LOG_LEVEL_DEBUG) {							 \
	printf("[D: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);\
	printf(__VA_ARGS__);											 \
	printf("\r\n");} 												 \
}while(0)

// ��ӡʱ���
#define ERR_PRINT_TIME	printf("[E: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000)
#define DBG_PRINT_TIME	printf("[D: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000)

// ��λ����λ�ã��������С�״̬��
#define p_dbg_track do{if(dbg_level < IOT_LOG_LEVEL_DEBUG)break;printf("[D: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("%s,%d",  __FUNCTION__, __LINE__); printf("\r\n");}while(0)
#define p_dbg_enter do{if(dbg_level < IOT_LOG_LEVEL_DEBUG)break;printf("[D: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("enter %s\n", __FUNCTION__); printf("\r\n");}while(0)
#define p_dbg_exit do{if(dbg_level < IOT_LOG_LEVEL_DEBUG)break;printf("[D: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("exit %s\n", __FUNCTION__); printf("\r\n");}while(0)
#define p_dbg_status do{if(dbg_level < IOT_LOG_LEVEL_DEBUG)break;printf("[D: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("status %d\n", status); printf("\r\n");}while(0)

// ��λ����λ��
#define p_err_miss do{printf("[E: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("%s miss\n", __FUNCTION__); printf("\r\n");}while(0)
#define p_err_mem do{printf("[E: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("%s mem err\n", __FUNCTION__); printf("\r\n");}while(0)
#define p_err_fun do{printf("[E: %d.%03d] ",  os_time_get()/1000, os_time_get()%1000);printf("%s err in %d\n", __FUNCTION__, __LINE__); printf("\r\n");}while(0)

#else
#define ERR_PRINT_TIME	
#define DBG_PRINT_TIME	
#define p_info(...) 
#define p_err(...) 
#define p_dbg_track 
#define p_dbg(...) 
#define p_dbg_enter 
#define p_dbg_exit 
#define p_dbg_status 
#define p_err_miss 
#define p_err_mem 
#define p_err_fun

#endif

typedef struct // ��������֡�Ĵ���ṹ��
{
	uint8_t *pRxBuffer;
	union 
	{
		__IO uint16_t InfAll;
		struct
		{
			__IO uint16_t FramLength : 15;	// 14:0
			__IO uint16_t FramFinishFlag : 1; // 15
		} InfBit;
	};
} STRUCT_USARTx_Fram;

extern uint8_t dbg_rxdata;
extern STRUCT_USARTx_Fram dbg_Fram_Record;
extern int dbg_level;

uint32_t os_time_get(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart); // ���ڽ��ջص�����
void DebugLevelInit(iot_log_level_t level); // ������־�ȼ�
#endif
