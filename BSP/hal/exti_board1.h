/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : exti_board1.h
 * Author             : su
 * Date First Issued  : 2013/11/25
 * Description        : This file contains the software implementation for the
 *                      external interrupt of board1 unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/25 | v1.0  | su         | initial released
 *******************************************************************************/
#ifndef _EXTI_BOARD1_H_
#define _EXTI_BOARD1_H_

/* Includes ------------------------------------------------------------------*/
#include "app_user_tmr.h"
#include "stm32f2xx.h"
#include "gpio.h"
#include "trace.h"
#include "stdint.h"
#include "struct_slave_board1.h"
#include "app_cfg.h"

#ifndef _EXTI_BOARD1_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif


#define DRAWBLOODPUMP_HALL_TRIGGER             0x01
#define ACPUMP_HALL_TRIGGER                    0x02
#define RETRANSFUSIONPUMP_HALL_TRIGGER         0x03
#define AC_BUBBLE_TRIGGER                      0x04
#define LIQUIDLEVEL_HIGH_TRIGGER               0x05
#define LIQUIDLEVEL_LOW_TRIGGER                0x06

#define DRAWBLOODPUMP_HALL_DELAY_TIME       10 /* unit: 1ms */
#define ACPUMP_HALL_DELAY_TIME              10 /* unit: 1ms */
#define RETRANSFUSIONPUMP_HALL_DELAY_TIME   10 /* unit: 1ms */
#define AC_BUBBLE_DELAY_TIME                5 /* unit: 1ms */
#define LIQUIDLEVEL_HIGH_DELAY_TIME         10 /* unit: 1ms */
#define LIQUIDLEVEL_LOW_DELAY_TIME          10 /* unit: 1ms */

#define AIR_BUBBLE_PASS  0X01
#define NONE_AIR_BUBBLE  0X00
#define HIGH_LEVEL_TRIGGER     0X01
#define HIGH_LEVEL_NOT_TRIGGER 0X00
#define LOW_LEVEL_TRIGGER      0X01
#define LOW_LEVEL_NOT_TRIGGER  0X00
#define INIT_FINISH      0X01
#define INIT_NOT_FINISH  0X00

GLOBAL SensorState arm1_sensor_status;

typedef enum
{
    DRAWBLOODPUMP_HALL = 0,
    ACPUMP_HALL= 1,
    RETRANSFUSIONPUMP_HALL= 2,
    AC_BUBBLE = 3,
    LIQUIDLEVEL_HIGH = 4,
    LIQUIDLEVEL_LOW = 5
} EXTI_Board1_TypeDef;

/**
  * @brief  This function can init all external interrupt sensors of arm1.
  *         arm1_exti_gpio_init()
  * @param  None
  * @retval None
*/


void arm1_exti_gpio_init(void);

/**
  * @brief  This function can init user timers of arm1
  *         init_arm1sensor_tmr()
  * @param  None
  * @retval None
*/

void arm1sensor_tmr_init(void);

//void read_hall_liquid_level_status(void);
/*clear ac bubble status*/
void clear_ac_bubble_status(void);

#undef GLOBAL

#endif

