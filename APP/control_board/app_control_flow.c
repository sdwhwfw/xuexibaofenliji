/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_flow.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/23
 * Description        : This file contains the software implementation for the
 *                      flow process task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/23 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_flow.h"
#include "app_control_board.h"
#include "app_comX100.h"
#include "app_control_poweron_test.h"
#include "app_control_set_canal.h"
#include "app_control_blood_collect.h"
#include "app_control_cali_pump.h"
#include "trace.h"
#include "gpio.h"

#include "app_control_global_state.h"
#include "app_control_common_function.h"
#include "app_cmd_send.h"
#include "app_control_pump.h"

static OS_STK control_flow_task_stk[CONTROL_FLOW_TASK_STK_SIZE];

static void control_flow_task(void *p_arg);

void control_init_flow_task(void)
{
    INT8U os_err;
	/* control flow task *****************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_flow_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_flow_task_stk[CONTROL_FLOW_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_FLOW_TASK_PRIO,
							(INT16U 		 ) CONTROL_FLOW_TASK_PRIO,
							(OS_STK 	   * )&control_flow_task_stk[0],
							(INT32U 		 ) CONTROL_FLOW_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CONTROL_FLOW_TASK_PRIO, (INT8U *)"flow", &os_err);
}

static void control_flow_task(void *p_arg)
{
    APP_TRACE("Enter control_flow_process_task...\r\n");

    g_canopen_centri_full_event = OSSemCreate(0);   /* blood is full of centrifuge belt event */
    g_canopen_centri_empty_event = OSSemCreate(0);  /* blood is empty of centrifuge belt event */
    g_canopen_lower_liquid_event = OSSemCreate(0);  /* lower liquid level is triggled event */
    g_canopen_upper_liquid_event = OSSemCreate(0);  /* upper liquid level is triggled event */

    // init cali pump task
    control_init_cali_pump_task();
    
    set_flow_pressure_status(FLOW_PRESSURE_NORMAL);//add by Big.Man

    
    OSTimeDlyHMSM(0, 0, 5, 0);

    /* 1. Init Communication state */
    if (test_comm_state())
    {
        APP_TRACE("test_comm_state() return \r\n");
        //return;
    }
    
#if 1
    // 2. Power-on Test 
    if (test_power_on())//通电测试
    {
        APP_TRACE("test_power_on() return \r\n");
        //return;
    }



    // 3. set canal suit 
    if (set_canal())//装载管路+排空气+管路测试+ac灌注
    {
    
        APP_TRACE("set_canal() return \r\n");
       // return;
    }
#endif

    /* 4. blood collect */
    if (blood_collect())//
    {
        APP_TRACE("blood_collect() return \r\n");
        return;
    }
}

