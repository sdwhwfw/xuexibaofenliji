/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_speed_table.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/20
 * Description        : This file contains the global status of ARM0
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/20 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_SPEED_TABLE_H_
#define _APP_CONTROL_SPEED_TABLE_H_

#include "stdint.h"

#define FLOW_SPEED_TABLE_SIZE           9

typedef __packed struct FLOW_PUMP_SPEED_STRU
{
    uint16_t draw_speed;
    uint16_t ac_speed;
    uint16_t feedBack_speed;
    uint16_t pla_speed;
    uint16_t plt_speed;
} FLOW_PUMP_SPEED;

extern FLOW_PUMP_SPEED g_flow_speedTable[FLOW_SPEED_TABLE_SIZE];

/*
 * Function: init_flow_speed_table
 * Description: when ipc set the program, we init the g_flow_speedTable
 * Param: maxSpeed, the max-speed limit of program
 * Return: void
*/
void init_flow_speed_table(FLOW_PUMP_SPEED maxSpeed);

/*
 * Function: set_flow_speed_table
 * Description: when ipc adjust the program parameter, we set the new speed table
 * Param: flowSpeed, the new flow speed of program
 * Return: void
*/
void set_flow_speed_table(FLOW_PUMP_SPEED flowSpeed);

/*
 * Function: get_flow_speed_table
 * Description: flow task get current pumps speed in current g_flow_speedTable
 * Param: offset, point out the pos of g_flow_speedTable
 * Return: FLOW_PUMP_SPEED, the current pumps speed in current phase
*/
FLOW_PUMP_SPEED get_flow_speed_table(uint8_t offset);

/*
 * Function: get_phase_status
 * Description: get current phase
 * Param: NONE
 * Return: uint8_t,g_flow_phase
*/
uint8_t get_phase_status(void);

/*
 * Function: get_cur_phase_speed
 * Description: get current flow speed
 * Param: NONE
 * Return: FLOW_PUMP_SPEED, current phase speeds struct
*/
FLOW_PUMP_SPEED get_cur_phase_speed(void);


#endif

