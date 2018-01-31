/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_user_tmr.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/08
 * Description        : This file complete the stm32's timer
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/05 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_user_tmr.h"
#include "app_cfg.h"

#ifndef _APP_USER_TMR_C_
#define _APP_USER_TMR_C_

USER_TMR g_user_tmr_array[TIMER_MAX_COUNT];
OS_EVENT *g_user_tmr_signal;
volatile INT32U g_user_time;

static OS_STK user_tmr_task_stk[CONTROL_USER_TMR_TASK_STK_SIZE];

void user_tmr_enter_critical(void);
void user_tmr_exit_critical(void);

void user_tmr_task(void *p_arg);

static u8 user_timer_used(void);
static void user_stm_tim_init(u16 nUS);

void user_timer_init(u16 nUS)
{
    u8 i=0;
    INT8U err;

    for (i=0; i<TIMER_MAX_COUNT; i++)
    {
        g_user_tmr_array[i].TmrState = USER_TMR_STATE_UNUSED;
    }

    err = OSTaskCreateExt((void (*)(void *)) user_tmr_task,
							(void		   * )nUS,
							(OS_STK 	   * )&user_tmr_task_stk[CONTROL_USER_TMR_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_USER_TMR_TASK_PRIO,
							(INT16U 		 ) CONTROL_USER_TMR_TASK_PRIO,
							(OS_STK 	   * )&user_tmr_task_stk[0],
							(INT32U 		 ) CONTROL_USER_TMR_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == err);
	OSTaskNameSet(CONTROL_USER_TMR_TASK_PRIO, (INT8U *)"user timer", &err);
}

void user_tmr_task(void *p_arg)
{
    INT8U err;
    u8 i;
    USER_TMR *l_tmr;
    
    g_user_tmr_signal = OSSemCreate(0);
    assert_param(g_user_tmr_signal);

    user_stm_tim_init((u16)((u32)p_arg));

    for (;;)
    {
        OSSemPend(g_user_tmr_signal, 0, &err);
        user_tmr_enter_critical();
        g_user_time++;
        for (i=0; i<TIMER_MAX_COUNT; i++)
        {
            l_tmr = &g_user_tmr_array[i];
            if (l_tmr->TmrState == USER_TMR_STATE_RUNNING)
            {
                if (l_tmr->TmrMatch == g_user_time)
                {
                    if (USER_TMR_OPT_PERIODIC == l_tmr->TmrOpt)
                    {
                        l_tmr->TmrMatch = g_user_time + l_tmr->TmrPeriod;
                    }
                    else
                    {
                        l_tmr->TmrState = USER_TMR_STATE_STOPPED;
                    }
                    if (l_tmr->TmrCallback != (USER_TMR_CALLBACK)0)
                    {// excute the callback function
                        l_tmr->TmrCallback((void *)l_tmr, l_tmr->TmrCallbackArg);
                    }
                }
            }
        }
        user_tmr_exit_critical();
    }
    
}

USER_TMR* user_timer_create(INT32U dly, INT32U period, INT8U opt, 
                        void *callback_arg, USER_TMR_CALLBACK callback, INT8U *perr)
{
    u8 l_tmrPos = 0;
    USER_TMR *pTmr;

    if (USER_TMR_OPT_PERIODIC!=opt && USER_TMR_OPT_ONE_SHOT!=opt)
    {
        *perr = USER_ERR_TMR_INVALID_OPT;
        return ((USER_TMR *)0);
    }
    if (USER_TMR_OPT_PERIODIC==opt && 0==period)
    {
        *perr = USER_ERR_TMR_INVALID_PERIOD;
        return ((USER_TMR *)0);
    }
    if (USER_TMR_OPT_ONE_SHOT==opt && 0==dly)
    {
        *perr = USER_ERR_TMR_INVALID_DLY;
        return ((USER_TMR *)0);
    }
    l_tmrPos = user_timer_used();
    if (0xFF == l_tmrPos)
    {// no available
        *perr = USER_ERR_TMR_NON_AVAIL;
        return ((USER_TMR *)0);
    }

    user_tmr_enter_critical();
    pTmr = &g_user_tmr_array[l_tmrPos];
    pTmr->TmrDly = dly;
    pTmr->TmrPeriod = period;
    pTmr->TmrCallbackArg = callback_arg;
    pTmr->TmrCallback = callback;
    pTmr->TmrPos = l_tmrPos;
    pTmr->TmrState = USER_TMR_STATE_USED;
    user_tmr_exit_critical();

    *perr = USER_ERR_NONE;
    return pTmr;
}

void user_timer_del(USER_TMR *ptmr,  INT8U *perr)
{
    if (USER_TMR_STATE_UNUSED == ptmr->TmrState)
    {
        *perr = USER_ERR_TMR_INACTIVE;
        return;
    }

    user_tmr_enter_critical();
    ptmr->TmrState = USER_TMR_STATE_UNUSED;
    user_tmr_exit_critical();
}

void user_timer_start(USER_TMR *p_tmr, INT8U *perr)
{
    if (USER_TMR_STATE_UNUSED == p_tmr->TmrState)
    {
        *perr = USER_ERR_TMR_INACTIVE;
        return;
    }

    user_tmr_enter_critical();

    if (USER_TMR_OPT_PERIODIC == p_tmr->TmrOpt)
    {
        p_tmr->TmrMatch = g_user_time + p_tmr->TmrPeriod;
    }
    else
    {
        p_tmr->TmrMatch = g_user_time + p_tmr->TmrDly;
    }
    
    p_tmr->TmrState = USER_TMR_STATE_RUNNING;
    
    user_tmr_exit_critical();
    *perr = USER_ERR_NONE;
    
}

void user_timer_stop(USER_TMR *p_tmr, INT8U opt, void *callback_arg, INT8U *perr)
{
    if (USER_TMR_STATE_RUNNING != p_tmr->TmrState)
    {
        *perr = USER_ERR_TMR_NOTSTART;
        return;
    }
    
    user_tmr_enter_critical();
    p_tmr->TmrState = USER_TMR_STATE_STOPPED;
    switch (opt)
    {
        case USER_TMR_OPT_NONE:
        {
            *perr = USER_ERR_NONE;
            break;
        }
        case USER_TMR_OPT_CALLBACK:
        {
            if ((USER_TMR_CALLBACK)0 != p_tmr->TmrCallback)
            {
                p_tmr->TmrCallback((void *)p_tmr, p_tmr->TmrCallbackArg);
            }
            *perr = USER_ERR_NONE;
            break;
        }
        case USER_TMR_OPT_CALLBACK_ARG:
        {
            if ((USER_TMR_CALLBACK)0 != p_tmr->TmrCallback)
            {
                p_tmr->TmrCallback((void *)p_tmr, callback_arg);
            }
            *perr = USER_ERR_NONE;
            break;
        }
        default:
            *perr = USER_ERR_TMR_INVALID_OPT;
    }
    user_tmr_exit_critical();
}

static void user_stm_tim_init(u16 nUS)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_TMR_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USER_TMR_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Prescaler = 59;
    TIM_TimeBaseStructure.TIM_Period = nUS;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM7, ENABLE);    
}

static u8 user_timer_used(void)
{
    u8 i=0;
    
    for (i=0; i<TIMER_MAX_COUNT; i++)
    {
        if (USER_TMR_STATE_UNUSED == g_user_tmr_array[i].TmrState)
        {
            return i;
        }
    }
    return 0xFF;
}

void user_tmr_enter_critical()
{
    OSSchedLock();
}

void user_tmr_exit_critical()
{
    OSSchedUnlock();
}

void TIM7_IRQHandler(void)
{
    OSIntEnter();
    if (SET == TIM_GetFlagStatus(TIM7, TIM_FLAG_Update))
    {
        TIM_ClearFlag(TIM7, TIM_FLAG_Update);
        OSSemPost(g_user_tmr_signal);
    }
    OSIntExit();
}


#endif

