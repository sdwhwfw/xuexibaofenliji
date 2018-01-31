/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : protocol.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/21
 * Description        : This file contains the software implementation for the
 *                      protocol unit
 *******************************************************************************
 * History:
 * 2013/05/21 v1.0
 *******************************************************************************/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "stm32f2xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_dc_motor.h"


typedef enum
{
	SET_CMD_UNKNOWM,
	SET_PID_TYPE,
	START_MOTOR,
	STOP_MOTOR,
	SET_MOTOR_SPEED,
	GET_DC_MOTOR_PARAM
}cmd_type;



void do_cmd_data(uint8_t ch);






#endif /* __PROTOCOL_H__ */

