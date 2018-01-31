/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_watchdog.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/19
 * Description        : This file contains the software implementation for the
 *                      led unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/19 | v1.0  | Bruce.zhu  | initial released, move code from app.c
 *******************************************************************************/

#include "app_watchdog.h"
#include "gpio.h"
#include "watchdog.h"
#include "trace.h"


static OS_STK wdg_task_stk[WDG_TASK_STK_SIZE];

static void watchdog_task(void *p_arg);


void init_watchdog_task()
{
    INT8U os_err;

	/* WATCHDOG task *******************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) watchdog_task,
							(void		   * ) 0,
							(OS_STK 	   * )&wdg_task_stk[WDG_TASK_STK_SIZE - 1],
							(INT8U			 ) WDG_TASK_PRIO,
							(INT16U 		 ) WDG_TASK_PRIO,
							(OS_STK 	   * )&wdg_task_stk[0],
							(INT32U 		 ) WDG_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(WDG_TASK_PRIO, (INT8U *)"watchdog", &os_err);
}

static void watchdog_task(void *p_arg)
{
	(void)p_arg;

	//init_led_gpio(LED3);
	init_wdg(3000);
	//led_trun_off(LED3);

	if (is_reset_from_wdg())
	{
	//	led_trun_on(LED3);
		APP_TRACE("Warning: reset from watchdog!\r\n");
	}

	while (1)
	{
		IWDG_ReloadCounter();
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
}



















