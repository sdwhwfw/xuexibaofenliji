/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_slave_board3.c
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
#ifndef _APP_SLAVE_BOARD3_C_
#define _APP_SLAVE_BOARD3_C_

#include "app_slave_board3.h"
#include "app_modbus_analyse.h"
#include "app_pump_control3.h"
#include "app_sensor_board3.h"
#include "app_monitor3.h"
#include "app_pdo_callback3.h"
#include "app_task_err.h"
#include "app_sensor_handle3.h"
#include "app_centrifuge_control.h"
#include "app_dc_cooling_fan.h"
#include "app_cassette.h"
#include "app_electromagnet.h"
#include "trace.h"
#include "gpio.h"

/*for pump timeout control */
/*
NOTE:the minmum unit is 100ms
     so if the define valve is 10 means 10*100ms=1s
*/
/*for pump direction*/
#define TMOEOUT_CENTRIFUGE_MOTOR_DIR      1000
#define TMOEOUT_CASSETTE_LOCATION         1000

#define CASSETTE_CONTROL_BUF_SIZE  20

static void*    cassette_control_QueueTbl[20];


#if !defined(SLAVE_BOARD_3)
	#error "Please do not include app_slave_board3.c!"
#endif /* SLAVE_BOARD_3 */

static OS_STK slave_board3_task_stk[SLAVE_BOARD3_TASK_STK_SIZE];



static p_RPDO_function p_RPDO_fun[SLAVE_RPDO_NUM] =
{
	RPDO1_callback,
	RPDO2_callback,
	RPDO3_callback,
	RPDO4_callback,
	RPDO5_callback
};


static void slave_board3_task(void *p_arg);


void init_slave_board3_task(void)
{
    INT8U os_err;

    power_gate_init();
    enable_power_gate(GATE1);
    enable_power_gate(GATE2);




	/* slave board 3 task ***************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) slave_board3_task,
							(void		   * ) 0,
							(OS_STK 	   * )&slave_board3_task_stk[SLAVE_BOARD3_TASK_STK_SIZE - 1],
							(INT8U			 ) SLAVE_BOARD3_TASK_PRIO,
							(INT16U 		 ) SLAVE_BOARD3_TASK_PRIO,
							(OS_STK 	   * )&slave_board3_task_stk[0],
							(INT32U 		 ) SLAVE_BOARD3_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(SLAVE_BOARD3_TASK_PRIO, (INT8U *)"slave board 3", &os_err);
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
static void slave_board3_task(void *p_arg)
{
	INT8U  os_err = 0;
    INT8U  tmr_err = 0;

	/* creat msg and mem event for pump control*/
	pump_mem_order = OSMemCreate(pump_valve_order, 5, 4, &os_err);
	assert_param(pump_mem_order);
	pump_q_send = OSQCreate(&pump_order_msggrp[0], 5);
	assert_param(pump_q_send);

	/* creat OS_flag for SYNC speed(RPDO1)  and start motor order(RPDO3)*/
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

    /* creat OS_flag for magnet task*/
    magnet_flag = OSFlagCreate(0x00,&os_err);
	assert_param(magnet_flag);
    
    /*creat OS QUEUE for cassette control*/
    cassette_control_event = (OS_EVENT*)OSQCreate( cassette_control_QueueTbl, CASSETTE_CONTROL_BUF_SIZE); 
    assert_param(cassette_control_event);
    
    /* creat OS_sem for centrifuge*/
    start_monitor_sem = OSSemCreate(0);  
	assert_param(start_monitor_sem);
    /*for motor direction*/
	tmr_centrifuge_motor_direction  = OSTmrCreate(TMOEOUT_CENTRIFUGE_MOTOR_DIR,
                                        TMOEOUT_CENTRIFUGE_MOTOR_DIR,
                                        OS_TMR_OPT_PERIODIC,
                                        (OS_TMR_CALLBACK)tmr_centrifuge_motor_dir_callback,
                                        (void*)0,"TMR_TMOEOUT_CENTRIFUGE_MOTOR_DIR_DIR",
                                        &tmr_err);
	assert_param(tmr_centrifuge_motor_direction);

    /*for cassette location*/
    tmr_cassette_location    = OSTmrCreate(TMOEOUT_CASSETTE_LOCATION,
                                        TMOEOUT_CASSETTE_LOCATION,
                                        OS_TMR_OPT_PERIODIC,
                                        (OS_TMR_CALLBACK)tmr_cassette_location_callback,
                                        (void*)0,"TMR_TMOEOUT_CASSETTE_LOCATION",
                                        &tmr_err);
	assert_param(tmr_cassette_location);

    // init usart dma route task
		
 //   init_usart_dma_route_task();

	/* init RPDO and TPDO callback function */
	init_RPDO_call_back(p_RPDO_fun, SLAVE_RPDO_NUM);
	init_TPDO_call_back(TPDO_callback);

    init_modbus_analyse_task();

    init_pump_control_task();

    init_TPDO_task();
    init_sensor_handle_task();   
    init_sensor_task();
    init_centrifuge_control_task();
  //  init_centrifuge_monitor_task();  
    init_dc_cooling_fan();
    init_cassette_control_task();
    init_magnet_control_task();    
	//salve3_monitor_task();
}



#endif /* _APP_SLAVE_BOARD3_C_ */

