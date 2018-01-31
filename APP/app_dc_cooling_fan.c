/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_dc_cooling_fan.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/11/13
 * Description        : This file contains the software implementation for the
 *                      DC cooling fan unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/11/13 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#include "app_dc_cooling_fan.h"
#include "input_capture.h"
#include "trace.h"


static OS_STK dc_cooling_fan_task_stk[DC_COOLIING_FAN_TASK_STK_SIZE];

static vu32 g_dc_cooling_fan_speeed[DC_COOLING_FAN_NUM];


static void app_dc_cooling_fan_task(void *p_arg);


void init_dc_cooling_fan(void)
{
    INT8U os_err;

    init_input_capture(PWM_CAPTURE_TIM2);
    os_err = OSTaskCreateExt((void (*)(void *)) app_dc_cooling_fan_task,
                            (void 		 * ) 0,
                            (OS_STK		 * )&dc_cooling_fan_task_stk[DC_COOLIING_FAN_TASK_STK_SIZE - 1],
                            (INT8U		   ) DC_COOLIING_FAN_TASK_PRIO,
                            (INT16U		   ) DC_COOLIING_FAN_TASK_PRIO,
                            (OS_STK		 * )&dc_cooling_fan_task_stk[0],
                            (INT32U		   ) DC_COOLIING_FAN_TASK_STK_SIZE,
                            (void 		 * ) 0,
                            (INT16U		   )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(DC_COOLIING_FAN_TASK_PRIO, (INT8U *)"fan", &os_err);
}


static void app_dc_cooling_fan_task(void *p_arg)
{
    u32 count;

    APP_TRACE("DC cooling fan task start...\r\n");

    for (;;)
    {
        OSTimeDlyHMSM(0, 0, 1, 0);
        count = get_input_capture_value(PWM_CAPTURE_TIM2);

        // calculate the DC cooling fan speed (RPM)
        g_dc_cooling_fan_speeed[DC_COOLING_FAN_1] = count * 60 / 4;
        // TODO: add code to check fan speed
        //APP_TRACE("fan speed = [%4d]RPM\r\n", g_dc_cooling_fan_speeed[DC_COOLING_FAN_1]);
    }
}


u32 get_dc_cooling_fan_speed(dc_cooling_fan_type index)
{
    assert_param(IS_DC_COOLING_FAN_NUM_TYPE(index));

    return g_dc_cooling_fan_speeed[index];
}


