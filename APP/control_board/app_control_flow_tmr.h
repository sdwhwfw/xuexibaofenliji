/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_flow_tmr.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/26
 * Description        : This file contains the timer used in flow task to detect
 *                      the sensor error.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/26 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_FLOW_TMR_H_
#define _APP_CONTROL_FLOW_TMR_H_

#define PRESSURE_TMR_TIMEOUT            1*60*1000     /* unit:1ms */
#define PRESSURE_ERR_COUNT              3

typedef enum FLOW_TMR_TYPE_ENUM
{
    FLOW_TMR_STOP,
    FLOW_TMR_PRIMING,
    FLOW_TMR_DRAW,
    FLOW_TMR_BACK
} FLOW_TMR_TYPE;

/*
 * Function: init_flow_tmr
 * Description: Init the flow tmr
 * Param: None
 * Return: None
*/
void init_flow_tmr(void);

/*
 * Function: start_flow_tmr
 * Description: start the tmr when priming,draw and feedback in flow task
 * Param: time, one-shot timer time,unit:100ms; type, detect the flow
 * Return: None
*/
void start_flow_tmr(uint32_t time, FLOW_TMR_TYPE type);

/*
 * Function: pause_flow_tmr
 * Description: when flow suspend, call this func
 * Param: NONE
 * Return: None
*/
void pause_flow_tmr(void);

/*
 * Function: restart_flow_tmr
 * Description: when flow resume, call this func
 * Param: NONE
 * Return: None
*/
void restart_flow_tmr(void);

/*
 * Function: stop_flow_tmr
 * Description: when the right event occur, call this func
 * Param: NONE
 * Return: None
*/
void stop_flow_tmr(void);

/*
 * Function: response_low_pressure
 * Description: when draw low pressure event occur, call this func
 * Param: None
 * Return: None
*/
void response_low_pressure(void);

/*
 * Function: response_high_pressure
 * Description: when back high pressure event occur, call this func
 * Param: None
 * Return: None
*/
void response_high_pressure(void);

#endif

