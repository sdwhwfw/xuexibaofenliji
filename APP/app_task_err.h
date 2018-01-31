/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_task_err.h
 * Author             : WQ
 * Date First Issued  : 2013/11/14
 * Description        : This file contains the software implementation for the
 *                      task err
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/14 | v1.0  | WQ         | initial released
 *******************************************************************************/

#ifndef _APP_TASK_ERR_H_
#define _APP_TASK_ERR_H_

#include "stm32f2xx.h"


/*
*task number for err_code
*must be 1~15
*/
typedef enum
{
	SLAVE_BOARD_TASK = 0x01,
	MODBUS_ANALYSE_TASK,
    PUMP_CONTROL_TASK,
    SENSOR_TASK,
    MONITOR_TASK,
    SHEEL_TASK,
    LED_TASK,
    WATCHDOG_TASK
}task_number;

/*
*err_code level
*/
typedef enum
{
	TASK_WAR = 0x01,
	TASK_ERR
} task_errcode_level;


#define IS_TASK_NUM(NUM)    ((NUM) >= 1 && (NUM) <= 15)
#define IS_ERR_LEVEL(LEVEL) ((LEVEL) == TASK_WAR || (LEVEL) == TASK_ERR)
#define IS_ERR_CODE(ERR_CODE) ((ERR_CODE) <= 255)


void init_TPDO_task(void);

void send_task_errcode(uint8_t task_num,task_errcode_level level ,uint8_t err_code);






#endif



