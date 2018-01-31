/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pump_control3.h
 * Author             : WQ
 * Date First Issued  : 2013/10/10
 * Description        : This file contains the software implementation for the
 *                      pump control task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/10 | v1.0  | WQ         | initial released
 *******************************************************************************/


#ifndef _APP_PUMP_CONTROL3_H_
#define _APP_PUMP_CONTROL3_H_

#include "stm32f2xx.h"

#ifndef _APP_PUMP_CONTROL3_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

GLOBAL INT8U  CASSETTE_UP_OPTICAL_FLAG; 
GLOBAL INT8U  CASSETTE_DOWN_OPTICAL_FLAG ;

//void pump_control_task(void *p_arg);

void init_pump_control_task(void);
 
#undef GLOBAL
#endif /* _APP_PUMP_CONTROL_H_ */

