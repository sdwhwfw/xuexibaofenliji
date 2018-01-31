/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : bell.h
 * Author             : WQ
 * Date First Issued  : 2013/10/29
 * Description        : This file contains the software implementation for the
 *                      buzzer control unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/10/29 | v1.0  | WQ         | initial released
 * 2013/11/09 | v1.1  | Bruce.zhu  | add support for ARM0 ARM1 ARM2 ARM3 board
 *******************************************************************************/
#ifndef __BELL_H__
#define __BELL_H__

#include "stm32f2xx.h"


void bell_init(void);
void bell_on(void);
void bell_off(void);


#endif /* __BELL_H__ */


