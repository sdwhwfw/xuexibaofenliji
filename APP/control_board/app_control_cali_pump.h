/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_cali_pump.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/12/19
 * Description        : This file contains the implement of calibration pump k value
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/12/19 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_CALI_PUMP_H_
#define _APP_CONTROL_CALI_PUMP_H_

#define TOTAL_DIS_QUEUE_SIZE        3

typedef enum TOTAL_DISTANCE_EVENT_ENUM
{
    CLEAR_DRAW_DIS,             /* clear draw pump's total distance */
    CLEAR_FEEDBACK_DIS,         /* clear feedback pump's total distance */
    GET_DRAW_TOTAL_DIS,         /* get draw pump's total distance */
    GET_DRAW_BACK_TOTAL_DIS     /* get draw pump and back pump total distance */
} TOTAL_DISTANCE_EVENT;

/*
 * Function: control_init_cali_pump_task
 * Description: Init the calibration pump task
 * Param: None
 * Return: None
*/
void control_init_cali_pump_task(void);

/*
 * Function: set_total_distance_event
 * Description: point the event to set/get the total distance
 * Param: event, event type
 * Return: None
*/
void set_total_distance_event(TOTAL_DISTANCE_EVENT event);


#endif /* _APP_CONTROL_CALI_PUMP_H_ */

