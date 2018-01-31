/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_timing_back.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/24
 * Description        : This file contains the software implementation for the
 *                      timing feedback task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/24 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_TIMING_BACK_H_
#define _APP_CONTROL_TIMING_BACK_H_

/*
 * Function: control_init_timing_back_task
 * Description: Init the timing back task
 * Param: None
 * Return: None
*/
void control_init_timing_back_task(void);

/*
 * Function: update_pressure_now
 * Description: Send the pressure immediately
 * Param: pressure, the high or low pressure
 * Return: None
*/
void update_pressure_now(u16 pressure);

#endif /* _APP_CONTROL_TIMING_BACK_H_ */

