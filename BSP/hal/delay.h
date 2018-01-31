/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : delay.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/07
 * Description        : This file contains the software implementation for the
 *                      delay
 *******************************************************************************
 * History:
 * 2013/05/07 v1.0
 *******************************************************************************/
#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f2xx.h"


typedef enum
{
	DELAY_FOR_US,
	DELAY_FOR_MS
}delay_type;

void timer4_delay_config(void);
void delay_init(delay_type type);
void delay(uint32_t time);
void delay_us(uint32_t nTime);


#endif /* __DELAY_H__ */

