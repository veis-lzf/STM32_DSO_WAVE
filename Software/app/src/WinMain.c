#include "WinMain.h"
#include "app_cfg.h"
#include "debug.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
#include "bsp_uart.h"
#include "bsp_led.h"
#include "iwdg.h"

#define COMPILER_VERSION	(__DATE__##" "##__TIME__)

static void Task_1HZ(void);
static void Task_10HZ(void);
static void Task_100HZ(void);

/* 系统定时器计数变量 */
uint32_t sysTick_t = 0;
/* 任务列表 */
Task_PCB task_table[] = 
{
	{Task_1HZ, 0, 1000},
	{Task_10HZ, 0, 100},
	{Task_100HZ, 0, 10},
};

/* 任务数量 */
#define TASKNUM	sizeof(task_table)/sizeof(task_table[0])

#if 0
float32_t pSrc; 
float32_t pDst; 
q31_t pSrc1;
q31_t pDst1;
#endif

/* 1Hz调度 */
static void Task_1HZ(void)
{
//	p_dbg_track;
	led_toggle_status(LED0);
#if 0
	pSrc -= 1.23f;
	arm_abs_f32(&pSrc, &pDst, 1); 
	printf("pDst = %f\r\n", pDst);
#endif
}

/* 10Hz调度 */
static void Task_10HZ(void)
{
	FeedWDG();
//	p_dbg_track;
//	if(uhADCConverFinish)
//	{
//		uhADCConverFinish = 0;
//		
//		for(int i =0; i < 2048; i++) {
//			printf("%d\n", uhADC1ConvertedValue[i]);
//		}
//	}
}

/* 100Hz调度 */
static void Task_100HZ(void)
{
//	p_dbg_track;
	
}


static void InitSysTick(void)
{
//	SysTick_Config(SystemCoreClock / TIME_SLICE);
}

static void InitDevice(void)
{
	bsp_led_init();
	bsp_uart1_init(BAUD_115200);
	bsp_dac_init();
	bsp_InitADC();
	IWDG_Init();
}

/**
 * @brief      初始化系统的相关外设
 */
void InitSystem(void)
{
	DebugLevelInit(IOT_LOG_LEVEL_ERROR); // 默认开启日志等级=0
	/* 初始化系统用到的外设 */
	InitDevice();
	/* 初始化并启动系统定时器，定时周期1ms */
	InitSysTick();
	
	p_info("VERSION=%s", COMPILER_VERSION); // 打印编译日期
}

/**
 * @brief      系统任务调度处理函数，在对定系统定时器服务函数中调用
 */
void SysTickCallback(void)
{
	uint8_t index;
	
	for(index = 0; index < TASKNUM; index++)
	{
		if(sysTick_t == task_table[index].delay_t)
		{
			task_table[index].task();
			task_table[index].delay_t += task_table[index].period_t;
		}
	}
}

void HAL_SYSTICK_Callback(void)
{
//	p_dbg_track;
	if(sysTick_t % 10 == 0) // 10ms作为1个时间片
		SysTickCallback();
	sysTick_t++;
}
