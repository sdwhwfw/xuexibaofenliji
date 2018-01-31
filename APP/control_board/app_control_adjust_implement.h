/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_adjust_implement.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/23
 * Description        : This file contains the task for implement of adjust from PC
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/23 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_ADJUST_IMPLEMENT_H_
#define _APP_CONTROL_ADJUST_IMPLEMENT_H_

#define DRAW_PUMP_CHANGE_UNIT               50     /* 0.1mL/min */
#define BACK_PUMP_CHANGE_UNIT               300    /* 0.1mL/min */
#define AC_PUMP_CHANGE_UNIT                 6      /* 0.01mL/min/L TBV */
#define DRAW_AC_RATE_CHANGE_UNIT            77     /* 0.01 */

#define ADJUST_QUEUE_SIZE                   5

#define PRESSURE_TMR_TIMEOUT                3*60*100     /* unit:1ms */
#define PRESSURE_ERR_COUNT                  3


// adjust event
typedef enum ADJUST_EVENT_ENUM
{
    DRAW_SPEED_UP_EVENT,
    DRAW_SPEED_DOWN_EVENT,
    BACK_SPEED_UP_EVENT,
    BACK_SPEED_DOWN_EVENT,
    AC_SPEED_UP_EVENT,
    AC_SPEED_DOWN_EVENT,
    AGGLUTINATE_UP_EVENT,
    AGGLUTINATE_DOWN_EVENT,
    OVER_FLOW_EVENT,
    AIR_REMOVE_EVENT,
    POUR_SALINE_EVENT,
    RESUME_FEEDBACK_EVENT,    
    AIR_REMOVE_END_EVENT,
    OVER_FLOW_END_EVENT,
    PRESSURE_LOW_END_EVENT,
    PRESSURE_HIGH_END_EVENT
} ADJUST_EVENT;


/*
 * Function: control_init_adust_implement_task
 * Description: Init the adjust implement task of arm0
 * Param: None
 * Return: None
*/
void control_init_adust_implement_task(void);


/*
 * Function: set_adjust_event
 * Description: post event to triggle the adjust program
 * Param: event, the program type
 * Return: None
*/
void set_adjust_event(ADJUST_EVENT event);

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

#endif /* _APP_CONTROL_ADJUST_IMPLEMENT_H_ */

