/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : delay.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/07
 * Description        : This file contains the software implementation for the
 *                      delay
 *******************************************************************************
 * History:
 * 2013/05/07 v1.0
 *******************************************************************************/

#include "delay.h"

static volatile uint8_t g_delay_flag;


/*
 * init TIM7 for delay
 *
 */
void delay_init(delay_type type)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	/* Time base configuration */
	switch (type)
	{
		case DELAY_FOR_US:
			TIM_TimeBaseStructure.TIM_Prescaler = 59;
			break;
		case DELAY_FOR_MS:
			TIM_TimeBaseStructure.TIM_Prescaler = 59999;
			break;
		default:
			return;
	}
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	/* Enable the TIM7 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/**
 * TIM7 is a 16 bit timer
 * DELAY TIME: [min delay: 0x0001], [max delay: 0xffff]
 * NOTE: this function should NOT be disable interrupt when call
 */
void delay(uint32_t time)
{
	TIM_SetAutoreload(TIM7, time - 1);
	TIM_Cmd(TIM7, ENABLE);

	g_delay_flag = 0;
	while (g_delay_flag == 0)
	{
	}
}

#if 0

void TIM7_IRQHandler(void)
{
	FlagStatus status;

	status = TIM_GetFlagStatus(TIM7, TIM_FLAG_Update);
	if (status == SET)
	{
		g_delay_flag = 1;
		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	}
	TIM_Cmd(TIM7, DISABLE);
}

#endif


/**
 * init TIM4 for delay.
 * precision: microsecond
 * NOTE: this function should be disable interrupt when call
 */
void timer4_delay_config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 59;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
}


/**
 * TIM4 is a 16 bit timer
 * DELAY TIME: [min delay: 0x0001 us], [max delay: 0xffff us]
 * Note: This function will disable interrupt when called it.
 *
 */
void delay_us(uint32_t nTime)
{
	uint32_t tim_count = nTime;

	TIM_Cmd(TIM4, ENABLE);
	TIM_SetCounter(TIM4, nTime - 1);

	while (tim_count > 1)
	{
		tim_count = TIM_GetCounter(TIM4);
	}

	TIM_Cmd(TIM4, DISABLE);
}






