/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_key_press.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/5
 * Description        : This file contains the software implementation for the
 *                      pause/resume key and stop key
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/05 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_key_press.h"
#include "stm32f2xx.h"
#include "app_control_monitor.h"
#include "gpio.h"
#include "trace.h"
#include "app_control_pump.h"
#include "app_control_common_function.h"
#include "app_user_tmr.h"

USER_TMR *g_usr_tmr_keyPress;
volatile u8 g_key_press_type; /* 0x01-STOP key pressed;0x02-PAUSE/RESUME key pressed */

void tmr_pause_program_callback(OS_TMR *ptmr, void *p_arg);
void tmr_key_press_callback(USER_TMR *ptmr, void *p_arg);

void control_init_key_press(void)
{
    INT8U os_err;
    g_tmr_pause = OSTmrCreate(0,
                        PAUSE_PERIOD,
                        OS_TMR_OPT_PERIODIC,
                        (OS_TMR_CALLBACK)tmr_pause_program_callback,
                        (void*)0,"TMR_PAUSE_TIMER",
                        &os_err);
    assert_param(OS_ERR_NONE == os_err);

    g_usr_tmr_keyPress = user_timer_create(KEY_PRESS_DELAY_TIME, 
                                            0, 
                                            USER_TMR_OPT_ONE_SHOT, 
                                            (void*)0, 
                                            (OS_TMR_CALLBACK)tmr_key_press_callback,
                                            &os_err);
}

void tmr_pause_program_callback(OS_TMR *ptmr, void *p_arg)
{
    static u16 s_count = 0;
    INNER_ERROR_MSG l_errMsg;

    if (NULL != p_arg)
    {// pause status stopped
        s_count = 0;
        
        if (RESUME_PAUSE_TMR == (u32)p_arg)
        {// Resume
            alarm_led_off();
        }
        else
        {// Stop
            alarm_led_on();
        }
        return;
    }

    // timer callback
    if (0 == get_alarm_status())
    {
        alarm_led_on();
    }
    else
    {
        alarm_led_off();
    }

    switch(++s_count)
    {
        case PAUSE_ONE_MINUTE:
        {
            g_run_status = STATUS_PAUSE_ONE_THREE;
            l_errMsg.source.all = ERR_SOURCE_ARM0;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_CENTRI_SLOW;
            l_errMsg.errcode1 = 0x22;
            l_errMsg.errcode2 = 0x00;
            post_error_msg(l_errMsg);
            break;
        }
        case PAUSE_THREE_MINUTE:
        {
            g_run_status = STATUS_PAUSE_THREE_TEN;
            l_errMsg.source.all = ERR_SOURCE_ARM0;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_IGNORE;
            l_errMsg.errcode1 = 0x23;
            l_errMsg.errcode2 = 0x00;
            post_error_msg(l_errMsg);
            break;
        }
        case PAUSE_TEN_MINUTE:
        {
            g_run_status = STATUS_PAUSE_AFTER_TEN;
            l_errMsg.source.all = ERR_SOURCE_ARM0;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_CENTRI_STOP;
            l_errMsg.errcode1 = 0x24;
            l_errMsg.errcode2 = 0x00;
            post_error_msg(l_errMsg);
            break;
        }
        default:
            break;
    }
    
}


void stop_pause_status(uint32_t bResume)
{  
    INT8U err;
    
    if (STATUS_PAUSE_IN_ONE == g_run_status 
    || STATUS_PAUSE_ONE_THREE == g_run_status 
    || STATUS_PAUSE_THREE_TEN == g_run_status
    || STATUS_PAUSE_AFTER_TEN == g_run_status)
    {// stop pause-timer
        OSTmrStop(g_tmr_pause, OS_TMR_OPT_CALLBACK_ARG, (void *)bResume, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else if (STOP_PAUSE_TMR == bResume)
    {// STOP
        alarm_led_on();
    }
}

void tmr_key_press_callback(USER_TMR *ptmr, void *p_arg)
{   
    INNER_ERROR_MSG l_errMsg;
    
    if (STATUS_STOP == g_run_status || STATUS_UNEXCEPTION == g_run_status)
    {
        return;
    }
    if (KEY_PRESS_STOP == g_key_press_type)
    {// stop key
        l_errMsg.source.all = ERR_SOURCE_ARM0;
        l_errMsg.taskNo = arm0_get_cur_prio();
        l_errMsg.action = ERROR_STOP_PROGRAM;
        l_errMsg.errcode1 = 0x20;
        l_errMsg.errcode2 = 0x00;
        post_error_msg(l_errMsg);
    }
    else if (KEY_PRESS_PAUSE_RESUME == g_key_press_type)
    {// pause/resume key
        if (STATUS_PAUSE_IN_ONE == g_run_status || STATUS_PAUSE_ONE_THREE == g_run_status
            || STATUS_PAUSE_THREE_TEN == g_run_status)
        {// resume
            l_errMsg.source.all = ERR_SOURCE_ARM0;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_RESUME_PROGRAM;
            l_errMsg.errcode1 = 0x00;
            l_errMsg.errcode2 = 0x00;
            post_error_msg(l_errMsg);
        }
        else
        {// pause
            l_errMsg.source.all = ERR_SOURCE_ARM0;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_PAUSE_PROGRAM;
            l_errMsg.errcode1 = 0x21;
            l_errMsg.errcode2 = 0x00;
            post_error_msg(l_errMsg);
        }
    }
}

void EXTI15_10_IRQHandler(void)
{
    INT8U err;
    
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {// stop program
        g_key_press_type = KEY_PRESS_STOP;
        user_timer_start(g_usr_tmr_keyPress, &err);
        assert_param(USER_ERR_NONE == err);
        /* Clear the EXTI line 12 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    else if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {// pause/resume program
        g_key_press_type = KEY_PRESS_PAUSE_RESUME;
        user_timer_start(g_usr_tmr_keyPress, &err);
        assert_param(USER_ERR_NONE == err);
        /* Clear the EXTI line 13 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    OSIntExit();
}

