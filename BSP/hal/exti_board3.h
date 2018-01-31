/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : exti_board3.h
 * Author             : su
 * Date First Issued  : 2013/11/25
 * Description        : This file contains the software implementation for the
 *                      external interrupt of board3 unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/25 | v1.0  | su         | initial released
 *******************************************************************************/

#ifndef _EXTI_BOARD3_H_
#define _EXTI_BOARD3_H_

/* Includes ------------------------------------------------------------------*/
#include "app_user_tmr.h"
#include "stm32f2xx.h"
#include "gpio.h"
#include "trace.h"
#include "stdint.h"
#include "app_cassette.h"
#include "app_pump_control3.h"
#include "struct_slave_board3.h"
#include "app_cfg.h"

#ifndef _EXTI_BOARD3_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

#define KASEDO_OPTICAL1_ACT              0x01
#define KASEDO_OPTICAL2_ACT              0x02

#define KASEDO_OPTICAL1_DELAY_TIME      1 /* unit: 1ms */
#define KASEDO_OPTICAL2_DELAY_TIME      1 /* unit: 1ms  */
#define KASEDO_OPTICAL_STATUS_DELAY_TIME      1 /* unit: 1ms  */


GLOBAL SensorState arm3_sensor_status;

typedef enum
{
    KASEDO_OPTICAL1 = 0,
    KASEDO_OPTICAL2 = 1,
} EXTI_Board3_TypeDef;

/**
  * @brief  Init the  external interrupt GPIO of arm3
  *         arm3_exti_gpio_init
  * @param
  * @param
  * @note
  * @retval None
  */

void arm3_exti_gpio_init(void);


/**
  * @brief  Init  timers of sensors connect to arm3
  *         init_arm3sensor_tmr
  * @param
  * @param
  * @note
  * @retval None
  */

void init_arm3sensor_tmr(void);


void read_optical_status_arm3(void);

void exti3_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority);


#undef GLOBAL
#endif

