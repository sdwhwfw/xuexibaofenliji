/**
  ******************************************************************************
  * @file    watchdog.c
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-15
  * @brief   IWDG HAL functions.
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "watchdog.h"


/**
  * @brief  Configures IWDG
  * @param  time unit: ms
  * @retval none
  * @NOTE   This setting are given for a 32 kHz clock but the microcontroller¡¯s internal RC frequency can vary from 30 
  * to 60 kHz. Moreover, given an exact RC oscillator frequency, the exact timings still depend on the phasing 
  * of the APB interface clock versus the LSI clock so that there is always a full RC period of uncertainty.
  */
void init_wdg(uint32_t Tout)
{
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	IWDG_SetPrescaler(IWDG_Prescaler_256);

	IWDG_SetReload(Tout*32/256);
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	/* Enable IWDG */
	IWDG_Enable();
}

/**
  * @brief  Check if the system has resumed from IWDG reset 
  * @param  none
  * @retval uint8: 1 is reset from IWDG and 0 is normal reset.
  */
uint8_t is_reset_from_wdg()
{
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		/* Clear reset flags */
		RCC_ClearFlag();
		return 1;
	}
	else
	{
		/* IWDGRST flag is not set */
		return 0;
	}
}



