/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_da_output.h
 * Author             : SU
 * Date First Issued  : 2013/12/02
 * Description        : This file contains the software implementation for the
 *                      da unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/12/02 | v1.0  |   SU       | initial released, move code from app.c
 *******************************************************************************/
#ifndef _APP_DA_OUTPUT_H_
#define _APP_DA_OUTPUT_H_

#include "stm32f2xx.h"
#include "gpio.h"
#include "adc_dac.h"
#include "usart.h"
#include <stdio.h>
#include "trace.h"
#include "basic_timer.h"
#include "stm32f2xx_adc.h"

#ifndef _APP_DA_OUTPUT_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif
GLOBAL u8  red_or_green_flag;//red 0,green 1




void init_da_output_task(void);

#undef GLOBAL

#endif /* _APP_DA_OUTPUT_H_ */

