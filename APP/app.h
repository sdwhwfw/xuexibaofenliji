/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/20
 * Description        : This file contains the software implementation for the
 *                      app unit
 *******************************************************************************
 * History:
 * 2013/07/20 v1.0
 *******************************************************************************/
#ifndef __APP_H__
#define __APP_H__

#include "stm32f2xx.h"
#include "bsp.h"
#include <stdio.h>


#ifndef __APP_C__
#define GLOBAL extern
#else
#define GLOBAL
#endif



/* Exported functions --------------------------------------------------------*/
void init_app_start_task(void);


#undef GLOBAL

#endif /* __APP_H__ */

