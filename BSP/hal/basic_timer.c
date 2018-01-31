/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : basic_timer.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/04/15
 * Description        : This file contains the software implementation for the
 *                      basic timer(TIM6) HAL control unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/04/15 | v1.0  | Bruce.zhu  | initial released
 * 2013/11/07 | v1.1  | Bruce.zhu  | change file name from timer.c to basic_timer.c
 *******************************************************************************/

#include "basic_timer.h"



void basic_timer_5_init(timer_type ty, uint32_t time)
{
    NVIC_InitTypeDef        NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    /* Time base configuration 1 */
    TIM_TimeBaseStructure.TIM_Period    = time - 1;

    switch (ty)
    {
        case TIMER_FOR_US:
            TIM_TimeBaseStructure.TIM_Prescaler = 59;
            break;
        case TIMER_FOR_MS:
            TIM_TimeBaseStructure.TIM_Prescaler = 59999;
            break;
        default:
            return;
    }

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    /* Enable the TIM6 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



void basic_timer_5_start()
{
    TIM_Cmd(TIM5, ENABLE);
}


void basic_timer_5_stop()
{
    TIM_Cmd(TIM5, DISABLE);
}



/*
 * TIM6定时函数，中断函数名为
 * void TIM6_DAC_IRQHandler(void);
 * 在调用TIM6的地方实现该函数
 *
 */
void basic_timer_init(timer_type ty, uint32_t time)
{
    NVIC_InitTypeDef        NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    /* Time base configuration 1 */
    TIM_TimeBaseStructure.TIM_Period    = time - 1;

    switch (ty)
    {
        case TIMER_FOR_US:
            TIM_TimeBaseStructure.TIM_Prescaler = 59;
            break;
        case TIMER_FOR_MS:
            TIM_TimeBaseStructure.TIM_Prescaler = 59999;
            break;
        default:
            return;
    }

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    /* Enable the TIM6 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}


void basic_timer_start()
{
    TIM_Cmd(TIM6, ENABLE);
}


void basic_timer_stop()
{
    TIM_Cmd(TIM6, DISABLE);
}




