/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : exti_board2.h
 * Author             : su
 * Date First Issued  : 2013/11/25
 * Description        : This file contains the software implementation for the
 *                      external interrupt of board2 unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/25 | v1.0  | su         | initial released
 *******************************************************************************/

#ifndef _EXTI_BOARD2_H_
#define _EXTI_BOARD2_H_


/* Includes ------------------------------------------------------------------*/
#include "app_user_tmr.h"
#include "stm32f2xx.h"
#include "gpio.h"
#include "trace.h"
#include "stdint.h"
#include "app_valve_control.h"
#include "app_pump_control2.h"
#include "struct_slave_board2.h"
#include "app_cfg.h"

#ifndef _EXTI_BOARD2_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

#define PLASMAPUMP_HALL_TRIGGER         0x01
#define PLTPUMP_HALL_TRIGGER            0x02
#define PLTVALVE_OPTICAL2_TRIGGER       0x03
#define PLAVALVE_OPTICAL1_TRIGGER       0x04
#define PLAVALVE_OPTICAL2_TRIGGER       0x05
#define PLAVALVE_OPTICAL3_TRIGGER       0x06
#define RBCVALVE_OPTICAL1_TRIGGER       0x07
#define RBCVALVE_OPTICAL2_TRIGGER       0x08
#define RBCVALVE_OPTICAL3_TRIGGER       0x09
#define PLTVALVE_OPTICAL3_TRIGGER       0x10
#define PLTVALVE_OPTICAL1_TRIGGER       0x11


#define PLASMAPUMP_HALL_DELAY_TIME        1 /* unit: 1ms */
#define PLTPUMP_HALL_DELAY_TIME           1 /* unit: 1ms */
#define PLTVALVE_OPTICAL2_DELAY_TIME      1 /* unit: 1ms */
#define PLAVALVE_OPTICAL1_DELAY_TIME      1 /* unit: 1ms */
#define PLAVALVE_OPTICAL2_DELAY_TIME      1 /* unit: 1ms */
#define PLAVALVE_OPTICAL3_DELAY_TIME      1 /* unit: 1ms */
#define RBCVALVE_OPTICAL1_DELAY_TIME      1 /* unit: 1ms */
#define RBCVALVE_OPTICAL2_DELAY_TIME      1 /* unit: 1ms */
#define RBCVALVE_OPTICAL3_DELAY_TIME      1 /* unit: 1ms */
#define PLTVALVE_OPTICAL3_DELAY_TIME      1 /* unit: 1ms */
#define PLTVALVE_OPTICAL1_DELAY_TIME      1 /* unit: 1ms */

GLOBAL SensorState arm2_sensor_status;

typedef enum
{
    PLASMAPUMP_HALL = 0,
    PLTPUMP_HALL = 1,
    PLTVALVE_OPTICAL2 = 2,
    PLAVALVE_OPTICAL1 = 3,
    PLAVALVE_OPTICAL2 = 4,
    PLAVALVE_OPTICAL3 = 5,
    RBCVALVE_OPTICAL1 = 6,
    RBCVALVE_OPTICAL2 = 7,
    RBCVALVE_OPTICAL3 = 8,
    PLTVALVE_OPTICAL3 = 9,
    PLTVALVE_OPTICAL1 = 10
} EXTI_Board2_TypeDef ;

/**
  * @brief  Init the  external interrupt GPIO of arm2
  *         arm2_exti_gpio_init
  * @param
  * @param
  * @note
  * @retval None
  */

void arm2_exti_gpio_init(void);

/**
  * @brief  This function can init user timers of arm2
  *         init_arm2sensor_tmr()
  * @param  None
  * @retval None
*/
void init_arm2sensor_tmr(void);

/**
  * @brief  This function can read the hall and optical sensor status
  *
  * @param  None
  * @retval None
*/
void read_optical_hall_status_arm2(void);

#undef GLOBAL

#endif

