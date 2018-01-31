/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_monitor2.h
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      monitor task of salve_board2
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_MONITOR2_H_
#define _APP_MONITOR2_H_

#include "ucos_ii.h"
#include "stm32f2xx.h"
#include "struct_slave_board2.h"




void salve2_monitor_task(void);

u8 valve_location_translate(u8 valve,SensorState valve_state);


/*for shell show*/
void slave2_show_parameter_table(void);
void slave2_show_sensor_table(void);
void slave2_show_error_table(void);

/*for tmr callback*/
void tmr_plasma_pump_timeout_callback(OS_TMR *ptmr, void *p_arg);
void tmr_PLT_pump_timeout_callback(OS_TMR *ptmr, void *p_arg);


void tmr_plasma_pump_dir_callback(OS_TMR *ptmr, void *p_arg);
void tmr_PLT_pump_dir_callback(OS_TMR *ptmr, void *p_arg);

void tmr_plasma_valve_location_callback(OS_TMR *ptmr, void *p_arg);
void tmr_PLT_valve_location_callback(OS_TMR *ptmr, void *p_arg);
void tmr_RBC_valve_location_callback(OS_TMR *ptmr, void *p_arg);


#endif

