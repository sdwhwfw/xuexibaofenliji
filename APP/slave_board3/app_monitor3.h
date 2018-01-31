/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_monitor3.h
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      monitor task of salve_board3
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_MONITOR3_H_
#define _APP_MONITOR3_H_

#include "ucos_ii.h"


void salve3_monitor_task(void);

/*for shell show*/
void slave3_show_parameter_table(void);
void slave3_show_sensor_table(void);
void slave3_show_error_table(void);

/*for motor direction*/
void tmr_centrifuge_motor_dir_callback(OS_TMR *ptmr, void *p_arg);
void tmr_cassette_location_callback(OS_TMR *ptmr, void *p_arg);


#endif

