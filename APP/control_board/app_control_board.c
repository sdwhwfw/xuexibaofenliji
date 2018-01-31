/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_board.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/26
 * Description        : This file contains the software implementation for the
 *                      control board task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/26 | v1.0  | Bruce.zhu  | initial released
 * 2013/10/22 | V1.1  | ZhangQ     | modify control code
 *******************************************************************************/
#ifndef _APP_CONTROL_BOARD_C_
#define _APP_CONTROL_BOARD_C_

#include "app_control_board.h"

#include "app_comX100.h"
#include "app_fs.h"
#include "app_usart_dma_route.h"
#include "struct_control_board_usart.h"
#include "app_cmd_analysis.h"
#include "app_control_monitor.h"
#include "app_control_pdo_callback.h"
#include "app_control_flow.h"
#include "app_control_adjust_implement.h"
#include "app_control_timing_back.h"
#include "app_control_feedback.h"
#include "app_cmd_send.h"
#include "app_user_tmr.h"
#include "app_trouble_shooting.h"


#if !defined(CONTROL_MAIN_BOARD)
	#error "Please do not include app_control_board.c!"
#endif /* CONTROL_MAIN_BOARD */

static OS_STK control_board_task_stk[CONTROL_BOARD_TASK_STK_SIZE];


/* function array register for comX100 */
static control_pRPDO_function control_pRPDO_fun[CONTROL_RPDO_NUM] =
{
    control_RPDO1_callback,
    control_RPDO2_callback,
    control_RPDO3_callback,
    control_RPDO4_callback,
    control_RPDO5_callback,
    control_RPDO6_callback,
    control_RPDO7_callback,
    control_RPDO8_callback,
    control_RPDO9_callback,
    control_RPDO10_callback,
    control_RPDO11_callback,
    control_RPDO12_callback,
    control_RPDO13_callback,
    control_RPDO14_callback,
    control_RPDO15_callback,
    control_RPDO16_callback,
    control_RPDO17_callback,
    control_RPDO18_callback
};


void init_control_board_task(void)
{
    INT8U os_err;

	/* control board task ***************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_board_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_board_task_stk[CONTROL_BOARD_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_BOARD_TASK_PRIO,
							(INT16U 		 ) CONTROL_BOARD_TASK_PRIO,
							(OS_STK 	   * )&control_board_task_stk[0],
							(INT32U 		 ) CONTROL_BOARD_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CONTROL_BOARD_TASK_PRIO, (INT8U *)"control board", &os_err);
}


void control_board_task(void *p_arg)
{
#if defined(_COMX100_MODULE_FTR_)
    init_comX100_module_task(control_pRPDO_fun, CONTROL_RPDO_NUM);
#endif /* _COMX100_MODULE_FTR_ */
    // Init user timer
    user_timer_init(1000);
    // monitor task
   // control_init_monitor_task(); 
    // adjust task
    control_init_adust_implement_task();
    // timing back task
    control_init_timing_back_task();

#if defined(USE_FATFS_FTR)
    init_fs_task();
#endif /* USE_FATFS_FTR */

#if defined(USE_STM32_CAN_DEBUG)
    init_can_app_task();
#endif /* USE_STM32_CAN_DEBUG */

    // recv and parse PC cmd
    control_uart_cmd_analysis_init();
    // send cmd task
    control_init_send_task();

    // init feedback task
    control_init_feedback_task();

    // enter flow process
    control_init_flow_task();
    
    init_trouble_shooting_task();

}


#endif /* _APP_CONTROL_BOARD_C_ */


