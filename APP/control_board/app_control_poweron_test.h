/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_poweron_test.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/12
 * Description        : This file contains the system poweron test program
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/12 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_POWERON_TEST_H_
#define _APP_CONTROL_POWERON_TEST_H_

#include "stdint.h"

typedef enum POWER_ON_ERROR_STRU
{
    POWERON_NO_ERROR,       /* Power-on test OK! */
    POWERON_CENTRI_LID,     /* centrifuge test error! */
    POWERON_WEEPING,        /* weeping detect error! */    
    POWERON_POWER_CTRL,     /* power cotnrol test error! */
    POWERON_CARTRI_POS,     /* cartridge pos test error! */
    POWERON_VALVE_FUN,      /* valve function test error! */
    POWERON_PUMP_INIT       /* pump init position error! */
} POWER_ON_ERROR;

/*
 * Function: test_comm_state
 * Description: synchronize comminication with IPC&SlaveBoard
 * Param: NONE
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t test_comm_state(void);

/*
 * Function: test_power_on
 * Description: excute the power-on test
 * Param: NONE
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t test_power_on(void);

#endif

