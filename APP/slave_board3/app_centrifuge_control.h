/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_centrifuge_control.h
 * Author             : su
 * Date First Issued  : 2013/12/09
 * Description        : This file contains the software implementation for the
 *                     centrifuge control  unit
 *******************************************************************************
 * History:
 * 2013/12/09 v1.0
 *******************************************************************************/
 #ifndef __APP_CENTRIFUGE_CONTROL_H__
#define  __APP_CENTRIFUGE_CONTROL_H__

#include "stm32f2xx.h"
#include <math.h>
#include <stdio.h>


#ifndef _APP_CENTRIFUGE_CONTROL_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

static OS_EVENT*  get_centrifuge_cmd_event;
static OS_EVENT*  cmd_finish_sem;
static OS_MEM*    get_usart2_mem;
GLOBAL u8 centrifuge_direction ;
GLOBAL s32 centrifuge_motor_current_speed;

typedef struct
{
    u16 rev_data_len;
    u8 data_recv[20];
}DataBlockTypeDef;


typedef enum
{
    GET_BAUD_RATE = 1,
    GET_AMPLIFIER_TEMP ,
    GET_AMPLIFIER_STATUS,
    GET_COMMAND_SPEED,
    SET_MOERT_SPEED,
    GET_CURRENT_SPEED,
    STOP_MOTER,
    SET_AMPLIFIER_ENABLE ,
    SET_AMPLIFIER_DISABLE,
    SET_BAUD_RATE,
    GET_COMMEND_PEAK_CURRENT,
    GET_ACTUAL_CURRENT
} CmdTypeDef;

typedef enum
{
	AMPLIFIER_DISABLE,
	AMPLIFIER_ENABLE
}StatusTypeDef;


#define IS_AMPLIFIER_CMD(CMD_AMPLIFIER) ((CMD_AMPLIFIER) == GET_BAUD_RATE  \
                                         ||(CMD_AMPLIFIER) == GET_AMPLIFIER_TEMP\
                                         ||(CMD_AMPLIFIER) == GET_AMPLIFIER_STATUS)\
                                         ||(CMD_AMPLIFIER) == GET_COMMAND_SPEED)\
                                         ||(CMD_AMPLIFIER) == SET_MOERT_SPEED)\
                                         ||(CMD_AMPLIFIER) == GET_CURRENT_SPEED)\
                                         ||(CMD_AMPLIFIER) == STOP_MOTER);

void init_centrifuge_control_task(void);
void init_centrifuge_monitor_task(void);

u8 start_centrifuge_motor(u16 speed);
void stop_centrifuge_motor(void);
void init_centrifuge_motor(void);

#undef GLOBAL

#endif /* __APP_CENTRIFUGE_CONTROL_H__*/

