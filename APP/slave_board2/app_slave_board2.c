/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_slave_board2.c
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
#ifndef _APP_SLAVE_BOARD2_C_
#define _APP_SLAVE_BOARD2_C_

#include "app_slave_board2.h"
#include "app_modbus_analyse.h"
#include "app_pump_control2.h"
#include "app_monitor2.h"
#include "app_pdo_callback2.h"
#include "app_task_err.h"
#include "app_sensor_handle2.h"
#include "app_sensor_board2.h"
#include "app_valve_control.h"
#include "app_da_output.h"
#include "trace.h"
#include "app_dc_motor.h"

#if !defined(SLAVE_BOARD_2)
	#error "Please do not include app_slave_board2.c!"
#endif /* SLAVE_BOARD_2 */

static OS_STK slave_board2_task_stk[SLAVE_BOARD2_TASK_STK_SIZE];


static p_RPDO_function p_RPDO_fun[SLAVE_RPDO_NUM] =
{
	RPDO1_callback,
	RPDO2_callback,
	RPDO3_callback,
	RPDO4_callback,
	RPDO5_callback
};
static void slave_board2_task(void *p_arg);

/*for pump timeout control */
/*
NOTE:the minmum unit is 100ms
     so if the define valve is 10 means 10*100ms=1s
*/
#define TMOEOUT_PLT_PUMP      5000
#define TMOEOUT_PLASMA_PUMP   5000

/*for pump direction*/
#define TMOEOUT_PLASMA_PUMP_DIR     1000
#define TMOEOUT_PLT_PUMP_DIR        1000

/*for valve location*/
#define TMOEOUT_PLASMA_VLAVE_LOCATION  100
#define TMOEOUT_PLT_VLAVE_LOCATION     100
#define TMOEOUT_RBC_VLAVE_LOCATION     100

#define VALVE_CONTROL_BUF_SIZE  20

static void*    valve_control_QueueTbl[20];


void init_slave_board2_task()
{
    INT8U os_err;

    init_dc_motor();

	/* slave board 2 task ***************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) slave_board2_task,
							(void		   * ) 0,
							(OS_STK 	   * )&slave_board2_task_stk[SLAVE_BOARD2_TASK_STK_SIZE - 1],
							(INT8U			 ) SLAVE_BOARD2_TASK_PRIO,
							(INT16U 		 ) SLAVE_BOARD2_TASK_PRIO,
							(OS_STK 	   * )&slave_board2_task_stk[0],
							(INT32U 		 ) SLAVE_BOARD2_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(SLAVE_BOARD2_TASK_PRIO, (INT8U *)"slave board 2", &os_err);
}


/*
 * Slave board 1 app task enter point
 *
 * Create new task:
 * 1---> mobus recv  : modbus_analyse_task
 * 2---> pump control: pump_control_task
 *
 * Note: We use this task as monitor task after all above task create
 */
static void slave_board2_task(void *p_arg)
{
	INT8U  os_err = 0;
	INT8U  tmr_err = 0;

	/* creat msg and mem event for pump control*/
	pump_mem_order = OSMemCreate(pump_valve_order,5,6,&os_err);
	assert_param(pump_mem_order);
	pump_q_send = OSQCreate(&pump_order_msggrp[0],5);
	assert_param(pump_q_send);

	/* creat OS_flag for SYNC speed(RPDO1) distance(RPDO2) and start pump order(RPDO3)*/
    start_pump_sync_flag = OSFlagCreate(0x00,&os_err);
	assert_param(start_pump_sync_flag);


	/* creat msg and mem event for TPDO tsak*/
	TPDO_mem_order = OSMemCreate(TPDO_order, 10, 8, &os_err);
	assert_param(TPDO_mem_order);
	TPDO_q_send = OSQCreate(&TPDO_order_msggrp[0], 10);
	assert_param(TPDO_q_send);

	/* creat OS_flag for  tsak_errcode ACK/NAK response*/
    task_errcode_acknak_flag = OSFlagCreate(0x00,&os_err);
	assert_param(task_errcode_acknak_flag);

	/* creat OS_flag for init order to sync sensor_handle_task and monitor_task*/
    init_order_flag = OSFlagCreate(0x00,&os_err);
	assert_param(init_order_flag);
    
    /*creat OS QUEUE for valve control*/
    valve_control_event = (OS_EVENT*)OSQCreate( valve_control_QueueTbl, VALVE_CONTROL_BUF_SIZE); 
    assert_param(valve_control_event);


	/* creat tmr for pump timeout control*/
	tmr_PLT_pump_timeout    = OSTmrCreate(TMOEOUT_PLT_PUMP,
                            	TMOEOUT_PLT_PUMP,
                            	OS_TMR_OPT_PERIODIC,
                            	(OS_TMR_CALLBACK)tmr_PLT_pump_timeout_callback,
                            	(void*)0,"TMR_PLT_PUMP",
                            	&tmr_err);
    assert_param(tmr_PLT_pump_timeout);
	tmr_plasma_pump_timeout = OSTmrCreate(TMOEOUT_PLASMA_PUMP,
                                TMOEOUT_PLASMA_PUMP,
                                OS_TMR_OPT_PERIODIC,
                                (OS_TMR_CALLBACK)tmr_plasma_pump_timeout_callback,
                                (void*)0,"TMR_PLASMA_PUMP",
                                &tmr_err);
    assert_param(tmr_plasma_pump_timeout);

    /*for pump direction*/
	tmr_plasma_pump_direction    = OSTmrCreate(TMOEOUT_PLASMA_PUMP_DIR,
                                    TMOEOUT_PLASMA_PUMP_DIR,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_plasma_pump_dir_callback,
                                    (void*)0,"TMR_PLASMA_PUMP_DIR",
                                    &tmr_err);
    assert_param(tmr_plasma_pump_direction);
	tmr_PLT_pump_direction       = OSTmrCreate(TMOEOUT_PLT_PUMP_DIR,
                                    TMOEOUT_PLT_PUMP_DIR,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_PLT_pump_dir_callback,
                                    (void*)0,"TMR_PLT_PUMP_DIR",
                                    &tmr_err);
    assert_param(tmr_PLT_pump_direction);
    /*for valve location*/
	tmr_plasma_valve_location    = OSTmrCreate(TMOEOUT_PLASMA_VLAVE_LOCATION,
                                    TMOEOUT_PLASMA_VLAVE_LOCATION,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_plasma_valve_location_callback,
                                    (void*)0,"TMR_PLASMA_VLAVE_LOCATION",
                                    &tmr_err);
    assert_param(tmr_plasma_valve_location);
	tmr_PLT_valve_location      = OSTmrCreate(TMOEOUT_PLT_VLAVE_LOCATION,
                                    TMOEOUT_PLT_VLAVE_LOCATION,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_PLT_valve_location_callback,
                                    (void*)0,"TMR_PLT_VLAVE_LOCATION",
                                    &tmr_err);
    assert_param(tmr_PLT_valve_location);
    tmr_RBC_valve_location      = OSTmrCreate(TMOEOUT_RBC_VLAVE_LOCATION,
                                    TMOEOUT_RBC_VLAVE_LOCATION,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_RBC_valve_location_callback,
                                    (void*)0,"TMR_RBC_VLAVE_LOCATION",
                                    &tmr_err);
    assert_param(tmr_RBC_valve_location);


	/* init RPDO and TPDO callback function */
	init_RPDO_call_back(p_RPDO_fun, SLAVE_RPDO_NUM);
	init_TPDO_call_back(TPDO_callback);

    init_modbus_analyse_task();

    init_pump_control_task();

    init_TPDO_task();

    init_sensor_task();
    init_sensor_handle_task();
    init_valve_control_task();
    init_da_output_task();
	//salve2_monitor_task();
}



#endif

