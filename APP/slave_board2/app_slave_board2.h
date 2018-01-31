/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_slave_board2.h
 * Author             : WQ
 * Date First Issued  : 2013/08/02
 * Description        : This file contains the software implementation for the
 *                      slave board 2 task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/08/02 | v1.0  | WQ         | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | merge source code to NEW framework
 *******************************************************************************/
#ifndef _APP_SLAVE_BOARD2_H
#define _APP_SLAVE_BOARD2_H

#include "stm32f2xx.h"
#include "struct_slave_board2.h"
#include "string.h"



#ifndef _APP_SLAVE_BOARD2_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

/**********************SENSOR TABLE FOR HANDLE********************************/
/*read  by sensor_handle_task*/
/*write by sensor_task*/
GLOBAL SensorState sensor_state_handle;
GLOBAL PumpState1 pump_state1_handle;
GLOBAL PumpState2 pump_state2_handle;

/*total moved distance*/
GLOBAL volatile uint16_t PLT_pump_total_distance;
GLOBAL volatile uint16_t plasma_pump_total_distance;

/*old single moved distance*/
GLOBAL volatile uint16_t PLT_pump_old_single_distance;
GLOBAL volatile uint16_t plasma_pump_old_single_distance;

GLOBAL volatile uint8_t fn_start_pump;


/**********************SETTING PARAMETER TABLE********************************/
GLOBAL PumpSet1             pump_set1;              //RPDO1
GLOBAL PumpSet2             pump_set2;              //RPDO2
GLOBAL ControlOrder         control_order_r;        //RPDO3
GLOBAL ControlOrderSync     control_order_sync_r;   //RPDO4


/**********************STATE PARAMETER TABLE********************************/
GLOBAL PumpState1           pump_state1;            //TPDO1
GLOBAL PumpState2           pump_state2;            //TPDO2
GLOBAL SensorState          sensor_state;           //TPDO3
GLOBAL ControlOrderSync     control_order_sync_t;   //TPDO4
GLOBAL ControlOrder_t       task_errcode;           //TPDO5
GLOBAL PumpErrCode          pump_errcode;           //TPDO6

/***************PUMP CONTROL EVENT AND MEMORY ***************/
GLOBAL OS_EVENT *           pump_q_send;            //Pump Order Event
GLOBAL void*                pump_order_msggrp [5];  //

GLOBAL OS_MEM *             pump_mem_order;         //Pump order Mem
GLOBAL u8                   pump_valve_order[5][6]; //Pump Memory Block
GLOBAL u8*                  pump_mem_ptr;           // Pump Mem ptr

/*for SYNC speed(RPDO1) distance(RPDO2) and start pump order(RPDO3)*/
GLOBAL OS_FLAG_GRP *  start_pump_sync_flag;


/***************PUMP CONTROL TIMEOUT TMR ***************/
GLOBAL OS_TMR* tmr_plasma_pump_timeout;
GLOBAL OS_TMR* tmr_PLT_pump_timeout;

GLOBAL volatile u8 flag_tmr_plasma_pump_timeout;
GLOBAL volatile u8 flag_tmr_PLT_pump_timeout;




/*for pump direction*/
GLOBAL OS_TMR* tmr_plasma_pump_direction;
GLOBAL OS_TMR* tmr_PLT_pump_direction;

GLOBAL volatile u8 flag_tmr_plasma_pump_dir;
GLOBAL volatile u8 flag_tmr_PLT_pump_dir;

/*for valve location*/
GLOBAL OS_TMR* tmr_plasma_valve_location;
GLOBAL OS_TMR* tmr_PLT_valve_location;
GLOBAL OS_TMR* tmr_RBC_valve_location;

GLOBAL volatile u8 flag_tmr_plasma_valve_location;
GLOBAL volatile u8 flag_tmr_PLT_valve_location;
GLOBAL volatile u8 flag_tmr_RBC_valve_location;

/***************TPDO TASK EVENT AND MEMORY ***************/
GLOBAL OS_EVENT * 	        TPDO_q_send;            //TPDO Event
GLOBAL void* 		        TPDO_order_msggrp [10];  //

GLOBAL OS_MEM * 	        TPDO_mem_order;         //TPDO Mem
GLOBAL u8 			        TPDO_order[10][8];       //TPDO Memory Block
GLOBAL u8* 			        TPDO_mem_ptr;           // TPDO Mem ptr

/***************TASK ERRCODE RESPONSE (ACK/NAK) ***************/
GLOBAL OS_FLAG_GRP *    task_errcode_acknak_flag;

/**********init sensor_handle_task and monitor_task ***************/
GLOBAL OS_FLAG_GRP *  init_order_flag;

GLOBAL OS_EVENT *  valve_control_event;

void init_slave_board2_task(void);


#undef GLOBAL


#endif /* _APP_SLAVE_BOARD2_H */

