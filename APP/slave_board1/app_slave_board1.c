/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_slave_board1.c
 * Author             : WQ
 * Date First Issued  : 2013/08/02
 * Description        : This file contains the software implementation for the
 *                      slave board 1 task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/08/02 | v1.0  | WQ         | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | merge source code to NEW framework
 *******************************************************************************/
#ifndef _APP_SLAVE_BOARD1_C_
#define _APP_SLAVE_BOARD1_C_

#include "app_slave_board1.h"
#include "app_modbus_analyse.h"
#include "app_pump_control1.h"
#include "app_pdo_callback1.h"
#include "app_task_err.h"
#include "app_sensor_handle1.h"
#include "app_sensor_board1.h"
#include "app_monitor1.h"
#include "app_dc_motor.h"
#include "trace.h"



#if !defined(SLAVE_BOARD_1)
	#error "Please do not include app_slave_board1.c!"
#endif /* SLAVE_BOARD_1 */


static OS_STK slave_board1_task_stk[SLAVE_BOARD1_TASK_STK_SIZE];


static p_RPDO_function p_RPDO_fun[SLAVE_RPDO_NUM] =
{
	RPDO1_callback,
	RPDO2_callback,
	RPDO3_callback,
	RPDO4_callback,
	RPDO5_callback
};

/*for pump timeout control */
/*
NOTE:the minmum unit is 100ms
     so if the define valve is 10 means 10*100ms=10s
*/
#define TMOEOUT_DRAW_PUMP      5000
#define TMOEOUT_AC_PUMP        5000
#define TMOEOUT_FEEDBACK_PUMP  5000
/*for pump direction*/
#define TMOEOUT_DRAW_PUMP_DIR      10
#define TMOEOUT_AC_PUMP_DIR        10
#define TMOEOUT_FEEDBACK_PUMP_DIR  10

#define TMOEOUT_AC_BUBBLE           1


static void slave_board1_task(void *p_arg);


void init_slave_board1_task()
{
    INT8U os_err;

	/* slave board 1 task ***************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) slave_board1_task,
							(void		   * ) 0,
							(OS_STK 	   * )&slave_board1_task_stk[SLAVE_BOARD1_TASK_STK_SIZE - 1],
							(INT8U			 ) SLAVE_BOARD1_TASK_PRIO,
							(INT16U 		 ) SLAVE_BOARD1_TASK_PRIO,
							(OS_STK 	   * )&slave_board1_task_stk[0],
							(INT32U 		 ) SLAVE_BOARD1_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(SLAVE_BOARD1_TASK_PRIO, (INT8U *)"slave board 1", &os_err);
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
static void slave_board1_task(void *p_arg)
{
	INT8U  os_err = 0;
    
	INT8U  tmr_err = 0;

	/* creat msg and mem event for pump control*/
	pump_mem_order = OSMemCreate(pump_valve_order, 5, 6, &os_err);
	assert_param(pump_mem_order);
	pump_q_send = OSQCreate(&pump_order_msggrp[0], 5);
	assert_param(pump_q_send);

	/* creat OS_flag for SYNC speed(RPDO1) distance(RPDO2) and start pump order(RPDO3)*/
    start_pump_sync_flag = OSFlagCreate(0x00,&os_err);
	assert_param(start_pump_sync_flag);

	/* creat msg and mem event for TPDO tsak*/
	TPDO_mem_order = OSMemCreate(TPDO_order, 10, 8, &os_err);
	assert_param(TPDO_mem_order);
	TPDO_q_send = OSQCreate(&TPDO_order_msggrp[0], 10);
	assert_param(TPDO_q_send);

	/* creat OS_flag for  task_errcode ACK/NAK response*/
    task_errcode_acknak_flag = OSFlagCreate(0x00,&os_err);
	assert_param(task_errcode_acknak_flag);

	/* creat OS_flag for init order to sync sensor_handle_task and monitor_task*/
    init_order_flag = OSFlagCreate(0x00,&os_err);
	assert_param(init_order_flag);


	/* creat tmr for pump timeout control*/
	tmr_draw_pump_timeout      = OSTmrCreate(TMOEOUT_DRAW_PUMP,
                                	TMOEOUT_DRAW_PUMP,
                                	OS_TMR_OPT_PERIODIC,
                                	(OS_TMR_CALLBACK)tmr_draw_pump_timeout_callback,
                                	(void*)0,"TMR_DRAW_PUMP_TIMEOUT",
                                	&tmr_err);
	assert_param(tmr_draw_pump_timeout);
	tmr_AC_pump_timeout        = OSTmrCreate(TMOEOUT_AC_PUMP,
                                    TMOEOUT_AC_PUMP,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_AC_pump_timeout_callback,
                                    (void*)0,"TMR_AC_PUMP_TIMEOUT",
                                    &tmr_err);
    assert_param(tmr_AC_pump_timeout);
	tmr_feedback_pump_timeout  = OSTmrCreate(TMOEOUT_FEEDBACK_PUMP,
                                    TMOEOUT_FEEDBACK_PUMP,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_feedback_pump_timeout_callback,
                                    (void*)0,"TMR_FEEDBACK_PUMP_TIMEOUT",
                                    &tmr_err);
    assert_param(tmr_feedback_pump_timeout);
    /*for pump direction*/
	tmr_draw_pump_direction    = OSTmrCreate(TMOEOUT_DRAW_PUMP_DIR,
                                    TMOEOUT_DRAW_PUMP_DIR,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_draw_pump_dir_callback,
                                    (void*)0,"TMR_DRAW_PUMP_DIR",
                                    &tmr_err);
    assert_param(tmr_draw_pump_direction);
	tmr_AC_pump_direction      = OSTmrCreate(TMOEOUT_AC_PUMP_DIR,
                                    TMOEOUT_AC_PUMP_DIR,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_AC_pump_dir_callback,
                                    (void*)0,"TMR_AC_PUMP_DIR",
                                    &tmr_err);
    assert_param(tmr_AC_pump_direction);
	tmr_feedback_pump_direction = OSTmrCreate(TMOEOUT_FEEDBACK_PUMP_DIR,
                                    TMOEOUT_FEEDBACK_PUMP_DIR,
                                    OS_TMR_OPT_PERIODIC,
                                    (OS_TMR_CALLBACK)tmr_feedback_pump_dir_callback,
                                    (void*)0,"TMR_FEEDBACK_PUMP_DIR",
                                    &tmr_err);
    assert_param(tmr_feedback_pump_direction);

    /*for AC_bubble detect*/
    tmr_AC_bubble    = OSTmrCreate(TMOEOUT_AC_BUBBLE,
                                        TMOEOUT_AC_BUBBLE,
                                        OS_TMR_OPT_PERIODIC,
                                        (OS_TMR_CALLBACK)tmr_AC_bubble_callback,
                                        (void*)0,"TMR_TMOEOUT_AC_BUBBLE",
                                        &tmr_err);
	assert_param(tmr_AC_bubble);

    init_dc_motor();

	/* init RPDO and TPDO callback function */
	init_RPDO_call_back(p_RPDO_fun, SLAVE_RPDO_NUM);
	init_TPDO_call_back(TPDO_callback);

   // init_RPDO();
   // init_TPDO();

    init_modbus_analyse_task();////?????

    init_pump_control_task();

    init_TPDO_task();
    init_sensor_task();
    init_sensor_handle_task();
    //need delay some time wait other task running to judge
	//salve1_monitor_task();
   
}







#endif


