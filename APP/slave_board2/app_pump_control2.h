/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pump_control2.h
 * Author             : WQ
 * Date First Issued  : 2013/10/10
 * Description        : This file contains the software implementation for the
 *                      pump control task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/10 | v1.0  | WQ         | initial released
 *******************************************************************************/


#ifndef _APP_PUMP_CONTROL2_H_
#define _APP_PUMP_CONTROL2_H_

#include "stm32f2xx.h"

#ifndef _APP_PUMP_CONTROL2_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

//void pump_control_task(void *p_arg);

void init_pump_control_task(void);

/*The flag for which optical will be trigger*/
GLOBAL INT8U  PLTVALVE_LEFT_OPTICAL_FLAG;
GLOBAL INT8U  PLTVALVE_RIGHT_OPTICAL_FLAG;
GLOBAL INT8U  PLTVALVE_MIDDLE_OPTICAL_FLAG;
GLOBAL INT8U  PLAVALVE_LEFT_OPTICAL_FLAG;
GLOBAL INT8U  PLAVALVE_RIGHT_OPTICAL_FLAG;
GLOBAL INT8U  PLAVALVE_MIDDLE_OPTICAL_FLAG;
GLOBAL INT8U  RBCVALVE_LEFT_OPTICAL_FLAG;
GLOBAL INT8U  RBCVALVE_RIGHT_OPTICAL_FLAG;
GLOBAL INT8U  RBCVALVE_MIDDLE_OPTICAL_FLAG;

#undef GLOBAL

#endif /* _APP_PUMP_CONTROL2_H_*/

