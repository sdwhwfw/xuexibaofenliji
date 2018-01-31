/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_monitor.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/18
 * Description        : This file contains the software implementation for the
 *                      monitor task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/18 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_MONITOR_H_
#define _APP_CONTROL_MONITOR_H_

#define STOP_PAUSE_TMR              0x01
#define RESUME_PAUSE_TMR            0x02

#include "stdint.h"


/********************Iner error msg******************************/
// msg queue size
#define ERROR_MSG_QUEUE_SIZE        8
#define ERROR_MSG_QUEUE_NUM         8

#define MOTOR_ERROR_BIT             0x80

// error source
__packed struct ERROR_SOURCE_STRU
{
    uint8_t board_source:4;
    uint8_t slave_error:4;
};

__packed union ERROR_SOURCE_UNION
{
    uint8_t                  all;
    struct ERROR_SOURCE_STRU bit;
};

// enum what to do when error happened
typedef enum ERROR_SLAVE_ACTION_ENUM
{
    ERROR_IGNORE,
    ERROR_PRESSURE_LOW,
    ERROR_PRESSURE_HIGH,
    ERROR_PAUSE_PROGRAM,
    ERROR_STOP_PROGRAM,
    ERROR_RESUME_PROGRAM,
    ERROR_CENTRI_SLOW,
    ERROR_CENTRI_STOP
} ERROR_SLAVE_ACTION;

// Inner error message struct
typedef __packed struct INNER_ERROR_MSG_STRU
{
    union ERROR_SOURCE_UNION source;  /* error source */
    uint8_t taskNo;                   /* task priority */
    uint8_t action;                   /* slave action */
    uint8_t errcode1;
    uint8_t errcode2;
} INNER_ERROR_MSG;


/*
 * Function: control_init_monitor_task
 * Description: Init the monitor task
 * Param: None
 * Return: None
*/
void control_init_monitor_task(void);

/*
 * Function: post_error_msg
 * Description: when error happened, call this function
 * Param: inner_msg,struct of error frame;
 * Return: None
*/
void post_error_msg(INNER_ERROR_MSG inner_msg);

/*
 * Function: os_pause_flow
 * Description: pause the flow
 * Param: None
 * Return: None
*/
void os_pause_flow(void);

/*
 * Function: os_resume_flow
 * Description: resume the flow
 * Param: None
 * Return: None
*/
void os_resume_flow(void);

/*
 * Function: os_stop_program
 * Description: stop the program
 * Param: None
 * Return: None
*/
void os_stop_program(void);

#endif /* _APP_CONTROL_MONITOR_H_ */

