/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_handle1.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the software implementation for the
 *                      sensor handle
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/19 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_SENSOR_HANDLE1_H_
#define _APP_SENSOR_HANDLE1_H_

#include "stm32f2xx.h"


void init_sensor_handle_task(void);
void tmr_AC_bubble_callback(OS_TMR *ptmr, void *p_arg);


#endif




