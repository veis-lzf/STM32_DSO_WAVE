#include "app_cfg.h"
#include "iwdg.h"

#if ST_IWDG_ENABLE

IWDG_HandleTypeDef hiwdg;


void IWDG_Init(void)
{
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
		Error_Handler();
  }
}

void FeedWDG(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}

#endif

