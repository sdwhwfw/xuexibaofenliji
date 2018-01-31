/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_monitor1.h
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      monitor task of salve_board1
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_MONITOR1_H_
#define _APP_MONITOR1_H_

#include "ucos_ii.h"


void salve1_monitor_task(void);

/*for shell show*/
void slave1_show_parameter_table(void);
void slave1_show_sensor_table(void);
void slave1_show_error_table(void);

/*for tmr callback*/
void tmr_draw_pump_timeout_callback(OS_TMR *ptmr, void *p_arg);
void tmr_AC_pump_timeout_callback(OS_TMR *ptmr, void *p_arg);
void tmr_feedback_pump_timeout_callback(OS_TMR *ptmr, void *p_arg);

void tmr_draw_pump_dir_callback(OS_TMR *ptmr, void *p_arg);
void tmr_AC_pump_dir_callback(OS_TMR *ptmr, void *p_arg);
void tmr_feedback_pump_dir_callback(OS_TMR *ptmr, void *p_arg);




#endif

