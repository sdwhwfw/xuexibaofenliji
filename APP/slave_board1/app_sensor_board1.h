/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_board1.h
 * Author             : 
 * Date First Issued  : 2013/11/24
 * Description        : This file contains the software implementation for the
 *                      sensor unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/24 | v1.0  |            | initial released, move code from app.c
 *******************************************************************************/
#ifndef _APP_SENSOR_BOARD1_H_
#define _APP_SENSOR_BOARD1_H_

#include <math.h>
#include "stm32f2xx.h"
#include "gpio.h"
#include "adc_dac.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#include "exti_board1.h"
#include "stm32f2xx_adc.h"
#include "switch_value_scan.h"
#include "app_slave_board1.h"
#include "struct_slave_board1.h"
#include "app_dc_motor.h"
#include "trace.h"


void init_sensor_task(void);


#endif /* _APP_SENSOR_BOARD1_H_ */


