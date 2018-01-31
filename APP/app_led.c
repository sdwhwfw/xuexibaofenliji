/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_led.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/19
 * Description        : This file contains the software implementation for the
 *                      led unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/19 | v1.0  | Bruce.zhu  | initial released, move code from app.c
 *******************************************************************************/

#include "app_led.h"
#include "gpio.h"
#include "app_task_err.h"

static OS_STK led_task_stk[LED_TASK_STK_SIZE];

static void led_task(void *p_arg);


void init_led_task(void)
{
    INT8U os_err;

    init_led_gpio(LED1);

	/* LED task ************************************************************/
    os_err = OSTaskCreateExt((void (*)(void *)) led_task,
                            (void          * ) 0,
                            (OS_STK        * )&led_task_stk[LED_TASK_STK_SIZE - 1],
                            (INT8U           ) LED_TASK_PRIO,
                            (INT16U          ) LED_TASK_PRIO,
                            (OS_STK        * )&led_task_stk[0],
                            (INT32U          ) LED_TASK_STK_SIZE,
                            (void          * )0,
                            (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(LED_TASK_PRIO, (INT8U *)"led", &os_err);
}


static void led_task(void *p_arg)
{


    

    for (;;)
    {
        led_trun_on(LED1);
        OSTimeDlyHMSM(0, 0, 0, 50);
        led_trun_off(LED1);
        OSTimeDlyHMSM(0, 0, 0, 50);
    }
}


