/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_feedback.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/29
 * Description        : This file contains the feedback program when collect end.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/29 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_FEEDBACK_H_
#define _APP_CONTROL_FEEDBACK_H_

#include "stm32f2xx.h"

void control_init_feedback_task(void);

u8 unload_canal(void);//for test by shell add by wq

#endif

