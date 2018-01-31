/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_electromagnet.h
 * Author             : su
 * Date First Issued  : 2013/11/26
 * Description        : This file contains the software implementation for the
 *                      electromagnet control unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/26 | v1.0  | su         | initial released
 *******************************************************************************/

#ifndef _APP_ELECTROMAGNET_H_
#define _APP_ELECTROMAGNET_H_

#include "stm32f2xx.h"
#include "trace.h"
#include "pwm.h"
#include "string.h"


void init_magnet_control_task(void);
/*lock magnet */
void lock_magnet(void);

/*unlock magnet */
void unlock_magnet(void);


#endif

