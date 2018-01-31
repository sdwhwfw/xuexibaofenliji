/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : basic_timer.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/04/15
 * Description        : This file contains the software implementation for the
 *                      basic timer(TIM6 and TIM7) HAL control unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/04/15 | v1.0  | Bruce.zhu  | initial released
 * 2013/11/07 | v1.1  | Bruce.zhu  | change file name from timer.h to basic_timer.h
 *******************************************************************************/

#ifndef __BASIC_TIMER_H__
#define __BASIC_TIMER_H__

#include "stm32f2xx.h"

typedef enum
{
	TIMER_FOR_US,
	TIMER_FOR_MS
}timer_type;


void basic_timer_init(timer_type ty, uint32_t time);
void basic_timer_start(void);
void basic_timer_stop(void);
void basic_timer_5_init(timer_type ty, uint32_t time);
void basic_timer_5_start(void);
void basic_timer_5_stop(void);



#endif /* __BASIC_TIMER_H__ */

