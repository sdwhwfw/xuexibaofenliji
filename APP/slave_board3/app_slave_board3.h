/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_slave_board3.h
 * Author             : WQ
 * Date First Issued  : 2013/08/02
 * Description        : This file contains the software implementation for the
 *                      slave board 3 task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/08/02 | v1.0  | WQ         | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | merge source code to NEW framework
 *******************************************************************************/
#ifndef _APP_SLAVE_BOARD3_H
#define _APP_SLAVE_BOARD3_H

#include "stm32f2xx.h"
#include "struct_slave_board3.h"
#include "string.h"

#ifndef _APP_SLAVE_BOARD3_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif


/**********************SENSOR TABLE FOR HANDLE********************************/
/*read  by sensor_handle_task*/
/*write by sensor_task*/
GLOBAL SensorState sensor_state_handle;
GLOBAL PumpState1 pump_state1_handle;

GLOBAL MotorErrCode motor_errcode_handle;


/**********************SETTING PARAMETER TABLE********************************/
GLOBAL PumpSet1             pump_set1;                  //RPDO1
GLOBAL PumpSet2             pump_set2;                  //RPDO2
GLOBAL ControlOrder         control_order_r;            //RPDO3
GLOBAL ControlOrderSync     control_order_sync_r;       //RPDO4

/**********************STATE PARAMETER TABLE********************************/
GLOBAL PumpState1           pump_state1;                //TPDO1
GLOBAL PumpState2           pump_state2;                //TPDO2
GLOBAL SensorState          sensor_state;               //TPDO3
GLOBAL ControlOrderSync     control_order_sync_t;       //TPDO4
GLOBAL ControlOrder_t       task_errcode;               //TPDO5
GLOBAL MotorErrCode         motor_errcode;              //TPDO6


/***************PUMP CONTROL EVENT AND MEMORY ***************/
GLOBAL OS_EVENT *           pump_q_send;                //Pump Order Event
GLOBAL void*                pump_order_msggrp [5];      //


GLOBAL OS_MEM *             pump_mem_order;             //Pump order Mem
GLOBAL u8                   pump_valve_order[5][4];     //Pump Memory Block
GLOBAL u8*                  pump_mem_ptr;               // Pump Mem ptr

/*for SYNC speed(RPDO1)  and start motor order(RPDO3)*/
GLOBAL OS_FLAG_GRP *  start_pump_sync_flag;


/*for pump direction*/
GLOBAL OS_TMR* tmr_centrifuge_motor_direction;
GLOBAL OS_TMR* tmr_cassette_location;

GLOBAL volatile u8 flag_tmr_centrifuge_motor_dir;
GLOBAL volatile u8 flag_tmr_cassette_location;

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

/**********magnet task flag ***************/
GLOBAL OS_FLAG_GRP *  magnet_flag;


/**********start centrifuge monitor task ***************/
GLOBAL  OS_EVENT*  start_monitor_sem;

GLOBAL OS_EVENT *  cassette_control_event;

void init_slave_board3_task(void);




#undef GLOBAL

#endif /* _APP_SLAVE_BOARD3_H */

