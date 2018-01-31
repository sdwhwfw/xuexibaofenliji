/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_flow_tmr.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/26
 * Description        : This file contains the timer used in flow task to detect
 *                      the sensor error.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/26 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_flow_tmr.h"
#include "app_control_monitor.h"
#include "app_control_common_function.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"

static OS_TMR *         g_flow_tmr;
static INT32U           g_flow_remain_time;
static FLOW_TMR_TYPE    g_type_flow_tmr;

static void tmr_flow_callback(OS_TMR *ptmr, void *p_arg);

void init_flow_tmr(void)
{
    g_flow_tmr = NULL;
    g_flow_remain_time = 0;
}


void start_flow_tmr(uint32_t time, FLOW_TMR_TYPE type)
{
    INT8U err;
    
    if (g_flow_tmr)
    {
        OSTmrDel(g_flow_tmr, &err);
        assert_param(OS_ERR_NONE == err);
        g_flow_tmr = NULL;
    }
    g_type_flow_tmr = type;
    g_flow_tmr = OSTmrCreate(time, 0, OS_TMR_OPT_ONE_SHOT, 
        (OS_TMR_CALLBACK)tmr_flow_callback,
        (void*)0, "FLOW_TMR", &err);
    assert_param(OS_ERR_NONE == err);
    
    OSTmrStart(g_flow_tmr, &err);
    assert_param(OS_ERR_NONE == err);
}

void pause_flow_tmr(void)
{
    INT8U l_state;
    INT8U err;

    if (!g_flow_tmr)
    {
        return;
    }
    l_state = OSTmrStateGet(g_flow_tmr, &err);
    assert_param(OS_ERR_NONE == err);
    if (OS_TMR_STATE_RUNNING != l_state)
    {
        return;
    }
    g_flow_remain_time = OSTmrRemainGet(g_flow_tmr, &err);
    OSTmrStop(g_flow_tmr, OS_TMR_OPT_NONE, (void*)0, &err);
    assert_param(OS_ERR_NONE == err);
    OSTmrDel(g_flow_tmr, &err);
    assert_param(OS_ERR_NONE == err);
    g_flow_tmr = NULL;
}

void restart_flow_tmr(void)
{
    INT8U err;
    
    if (0==g_flow_remain_time)
    {
        return;
    }
    g_flow_tmr = OSTmrCreate(g_flow_remain_time, 0, OS_TMR_OPT_ONE_SHOT, 
        (OS_TMR_CALLBACK)tmr_flow_callback,
        (void*)0, "TMR_FLOW", &err);
    assert_param(OS_ERR_NONE == err);
    
    OSTmrStart(g_flow_tmr, &err);
    assert_param(OS_ERR_NONE == err);
}

void stop_flow_tmr(void)
{
    INT8U err;
    
    OSTmrStop(g_flow_tmr, OS_TMR_OPT_NONE, (void*)0, &err);
    assert_param(OS_ERR_NONE == err);
    OSTmrDel(g_flow_tmr, &err);
    assert_param(OS_ERR_NONE == err);
    g_flow_tmr = NULL;
    g_flow_remain_time = 0;
}


static void tmr_flow_callback(OS_TMR *ptmr, void *p_arg)
{
    INT8U err;
    INNER_ERROR_MSG l_errMsg;

    g_flow_remain_time = 0;
    OSTmrDel(g_flow_tmr, &err);
    assert_param(OS_ERR_NONE == err);
    g_flow_tmr = NULL;

    l_errMsg.source.all = ERR_SOURCE_ARM0;
    l_errMsg.taskNo = arm0_get_cur_prio();
    l_errMsg.action = ERROR_PAUSE_PROGRAM;  
    l_errMsg.errcode2 = 0x00;

    switch (g_type_flow_tmr)
    {
        case FLOW_TMR_PRIMING:
            l_errMsg.errcode1 = 0x25;
            break;
        case FLOW_TMR_DRAW:
            l_errMsg.errcode1 = 0x26;
            break;
        case FLOW_TMR_BACK:
            l_errMsg.errcode1 = 0x27;
            break;
        case FLOW_TMR_STOP:
            return;
        default:
            break;
    }
    l_errMsg = l_errMsg;
  //  post_error_msg(l_errMsg);
}

void response_low_pressure(void)
{   
    u8 i = 0, l_replaced = 0, l_count = 0;
    u32 l_cur_tick;
    static INT32U s_low_pressure_array[PRESSURE_ERR_COUNT] = {0};
    INNER_ERROR_MSG l_errMsg;

    set_flow_pressure_status(FLOW_PRESSURE_LOW);

    l_cur_tick = OSTimeGet();
    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (l_cur_tick - s_low_pressure_array[i] > PRESSURE_TMR_TIMEOUT
            || 0 == s_low_pressure_array[i])
        {
            if (!l_replaced)
            {
                s_low_pressure_array[i] = l_cur_tick;
                l_replaced = 1;
            }
            else
            {
                s_low_pressure_array[i] = 0;
            }
        }
    }

    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (s_low_pressure_array[i])
        {
            l_count++;
        }
    }

    l_errMsg.source.all = ERR_SOURCE_ARM0;
    l_errMsg.taskNo = arm0_get_cur_prio();

    if (PRESSURE_ERR_COUNT == l_count)
    {
        memset(s_low_pressure_array, 0x00, sizeof(s_low_pressure_array));
        l_errMsg.action = ERROR_PAUSE_PROGRAM;
        l_errMsg.errcode1 = 0x30;
        l_errMsg.errcode2 = 0x00; 
    }
    else
    {
        l_errMsg.action = ERROR_PRESSURE_LOW;
        l_errMsg.errcode1 = 0x28;
        l_errMsg.errcode2 = 0x00; 
    }
    post_error_msg(l_errMsg);
}

void response_high_pressure(void)
{
    u8 i = 0, l_replaced = 0, l_count = 0;
    u32 l_cur_tick;
    static INT32U s_high_pressure_array[PRESSURE_ERR_COUNT] = {0};
    INNER_ERROR_MSG l_errMsg;

    set_flow_pressure_status(FLOW_PRESSURE_HIGH);

    l_cur_tick = OSTimeGet();
    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (l_cur_tick - s_high_pressure_array[i] > PRESSURE_TMR_TIMEOUT
            || 0 == s_high_pressure_array[i])
        {
            if (!l_replaced)
            {
                s_high_pressure_array[i] = l_cur_tick;
                l_replaced = 1;
            }
            else
            {
                s_high_pressure_array[i] = 0;
            }
        }
    }

    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (s_high_pressure_array[i])
        {
            l_count++;
        }
    }

    memset(&l_errMsg, 0x00, sizeof(l_errMsg));
    l_errMsg.source.all = ERR_SOURCE_ARM0;
    l_errMsg.taskNo = arm0_get_cur_prio();

    if (PRESSURE_ERR_COUNT == l_count)
    {
        memset(s_high_pressure_array, 0x00, sizeof(s_high_pressure_array));
        l_errMsg.action = ERROR_PAUSE_PROGRAM;
        l_errMsg.errcode1 = 0x31;
        l_errMsg.errcode2 = 0x00;
    }
    else
    {
        l_errMsg.action = ERROR_PRESSURE_HIGH;
        l_errMsg.errcode1 = 0x29;
        l_errMsg.errcode2 = 0x00; 
    }
    post_error_msg(l_errMsg);
}


