/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name           : app_trouble_shooting.h
 * Author                :   TonyZhang
 * Date First Issued : 2015/05/27
 * Description         : This file contains the software implementation for the 
 *                           trouble shooting unit
 *******************************************************************************
 * History:DATE       | VER   | AUTOR      | Description
 * 2015/05/27 | v1.0  | TonyZhang         | initial released
 * 
 * 
 *  
 *******************************************************************************/





#ifndef __APP_TROUBLE_SHOOTING_H__
#define __APP_TROUBLE_SHOOTING_H__

#include "stm32f2xx.h"
#include "bsp.h"
#include <stdio.h>


#ifndef __APP_TROUBLE_SHOOTING_C__
#define GLOBAL extern
#else
#define GLOBAL
#endif


GLOBAL OS_EVENT *       sem_test_cancel_protect; 

/* Exported functions --------------------------------------------------------*/
void trouble_shooting_task(void);
void init_trouble_shooting_task(void);

#undef GLOBAL

#endif /* __TROUBLE_SHOOTING_H__ */
