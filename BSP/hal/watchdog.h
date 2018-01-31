/**
  ******************************************************************************
  * @file    watchdog.h
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-15
  * @brief   This file contains IWDG HAL functions.
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include "stm32f2xx.h"

void init_wdg(uint32_t Tout);
uint8_t is_reset_from_wdg(void);




#endif /* __WATCHDOG_H__ */
