/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_board3.h
 * Author             :
 * Date First Issued  : 2013/11/24
 * Description        : This file contains the software implementation for the
 *                      led unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/24 | v1.0  |            | initial released, move code from app.c
 *******************************************************************************/
#ifndef _APP_SENSOR_BOARD3_H_
#define _APP_SENSOR_BOARD3_H_

#include "stm32f2xx.h"
#include "gpio.h"
#include "adc_dac.h"
#include "usart.h"
#include <stdio.h>
#include "trace.h"
#include "stm32f2xx_adc.h"
#include "exti_board3.h"
#include "switch_value_scan.h"
#include "app_slave_board3.h"

#define NORMAL 1
#define ABNORMAL 0

void init_sensor_task(void);

typedef struct
{
    uint16_t volage_70V ;
    uint16_t volage_24V ;
    uint16_t volage_S16 ;
    uint16_t volage_S19 ;
    uint16_t volage_positive_12V ;
    uint16_t volage_negative_12V ;
    uint16_t volage_5V ;
} Read_Volage_TypeDef;

#endif /* _APP_SENSOR_BOARD3_H_ */


