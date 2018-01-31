/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_global_state.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/29
 * Description        : This file contains the global state in the program.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/29 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_GLOBAL_STATE_H_
#define _APP_CONTROL_GLOBAL_STATE_H_

#include "app_control_board.h"
#include "app_control_common_function.h"
#include "app_control_pump.h"

typedef enum FLOW_WAIT_EVENT_ENUM
{
    FLOW_WAIT_NONE,                         /* wait none */
    FLOW_WAIT_CENTRI_PRESSURE,              /* centrifuge belt is full  */
    FLOW_WAIT_LOWER_LEVEL_SIGNAL,           /* lower level is signal */
    FLOW_WAIT_LOWER_LEVEL_NOSIGNAL,         /* lower level is no signal */
    FLOW_WAIT_UPPER_LEVEL_SIGNAL,           /* upper level is signal */
    FLOW_WAIT_BELT_EMPTY                    /* centrifuge belt is empty */
} FLOW_WAIT_EVENT;

typedef enum FLOW_PRESSURE_STATUS_ENUM
{
    FLOW_PRESSURE_LOW,                      /* draw pressure is too low 0 */
    FLOW_PRESSURE_HIGH,                     /* back pressure is too high 1 */
    FLOW_PRESSURE_LOW_HANDLED,              /* high pressure is hanled  2 */
    FLOW_PRESSURE_HIGH_HANDLED,             /* low pressure is hanled 3 */
    FLOW_PRESSURE_BEGIN_RESUME,             /* pressure begin to resume normal 4*/
    FLOW_PRESSURE_NORMAL                    /* pressure is normal 5*/
} FLOW_PRESSURE_STATUS;

typedef enum OVERFLOW_HANDLE_STATUS_ENUM
{
    OVERFLOW_OVER,                          /* rbc over flow handled */
    REMOVE_AIR_OVER,                        /* remove air handled */
    OVERFLOW_ON_HANDLE,                     /* in overflow process */
    REMOVE_AIR_ON_HANDLE                    /* in remove air process */
} OVERFLOW_HANDLE_STATUS;

typedef enum TEST_CANAL_PRESSURE_STATUS_ENUM
{
    TEST_CANAL_WAIT_NONE,
    TEST_CANAL_WAIT_HIGH_PRESSURE,
    TEST_CANAL_WAIT_FIRST_PRESSURE,
    TEST_CANAL_WAIT_SECOND_PRESSURE,
    TEST_CANAL_WAIT_LOW_PRESSURE,
    TEST_CANAL_WAIT_NORMAL_PRESSURE
} TEST_CANAL_PRESSURE_STATUS;


typedef enum CMD_FN_TYPE_ENUM
{
    SEND_FRAME_FN,
    START_PUMP_FN,
    GET_DISTANCE_FN
} CMD_FN_TYPE;


typedef enum COLLECT_STATUS_ENUM
{
    NO_COLLECT,
    PLA_COLLECT_BEGIN,
    PLA_COLLECT_END,
    PLT_COLLECT_BEGIN,
    PLT_COLLECT_END,
    RBC_COLLECT_BEGIN,
    RBC_COLLECT_END
} COLLECT_STATUS;


/*
 * Function: set_run_status
 * Description: set g_run_status
 * Param: status, current run status
 * Return: None
*/
void set_run_status(RUN_STATUS status);

/*
 * Function: set_run_status
 * Description: set g_flow_wait_event
 * Param: event, the flow's wait event
 * Return: None
*/
void set_flow_wait_status(FLOW_WAIT_EVENT event);
FLOW_WAIT_EVENT get_flow_wait_status(void);

/*
 * Function: set_flow_pressure_status
 * Description: set g_flow_pressure_status
 * Param: status, the current pressure status
 * Return: None
*/
void set_flow_pressure_status(FLOW_PRESSURE_STATUS status);
FLOW_PRESSURE_STATUS get_flow_pressure_status(void);


/*
 * Function: set_overflow_status
 * Description: set g_overflow_status
 * Param: status, if the overflow happened
 * Return: None
*/
void set_overflow_status(OVERFLOW_HANDLE_STATUS status);
OVERFLOW_HANDLE_STATUS get_overflow_status(void);


/*
 * Function: set_test_pressure_status
 * Description: set g_test_canal_pressure_status
 * Param: status, which pressure is test canal waiting
 * Return: None
*/
void set_test_canal_pressure_status(TEST_CANAL_PRESSURE_STATUS status);
TEST_CANAL_PRESSURE_STATUS get_test_canal_pressure_status(void);


/*
 * Function: set_cmd_fn
 * Description: set the cmd's fn
 * Param: type,the cmd type; source,the slave board; fn, frame no;
 * Return: None
*/
void set_cmd_fn(CMD_FN_TYPE type, FN_SOURCE source, uint8_t fn);

/*
 * Function: get_cmd_fn
 * Description: get the cmd's fn
 * Param: type,the cmd type; source,the slave board;
 * Return: uint8_t,fn
*/
uint8_t get_cmd_fn(CMD_FN_TYPE type, FN_SOURCE source);

/*
 * Function: set_collect_status
 * Description: set the now collect status
 * Param: status,collect status
 * Return: None
*/
void set_collect_status(COLLECT_STATUS status);

/*
 * Function: get_collect_status
 * Description: get the now collect status
 * Param: None
 * Return: COLLECT_STATUS, now collect status
*/
COLLECT_STATUS get_collect_status(void);

void set_pause_state(CTRL_PUMP_TYPE type);
CTRL_PUMP_TYPE get_pause_state(void);

void set_pressure_abnormal_state(CTRL_PUMP_TYPE type);
CTRL_PUMP_TYPE get_pressure_abnormal_state(void);

void pause_plt_total_dis(void);
void resume_plt_total_dis(void);
uint16_t get_plt_collect_dis(void);

#endif

