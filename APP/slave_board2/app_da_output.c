/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_da_output.c
 * Author             : su
 * Date First Issued  : 2013/12/02
 * Description        : This file contains the software implementation for da
                        output unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/12/02 | v1.0  |            | initial released, move code from app.c
 *******************************************************************************/
#ifndef _APP_DA_OUTPUT_C_
#define _APP_DA_OUTPUT_C_

#include "app_da_output.h"

/* Private variables ---------------------------------------------------------*/

static OS_STK da_output_task_stk[DA_OUTPUT_TASK_STK_SIZE];

static void da_output_task(void *p_arg);

void init_da_output_task(void)
{
    INT8U os_err;
	/* DA output task ************************************************************/

    os_err = OSTaskCreateExt((void (*)(void *)) da_output_task,
                        (void          * ) 0,
                        (OS_STK        * )&da_output_task_stk[DA_OUTPUT_TASK_STK_SIZE - 1],
                        (INT8U           ) DA_OUTPUT_TASK_PRIO,
                        (INT16U          ) DA_OUTPUT_TASK_PRIO,
                        (OS_STK        * )&da_output_task_stk[0],
                        (INT32U          ) DA_OUTPUT_TASK_STK_SIZE,
                        (void          * )0,
                        (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(DA_OUTPUT_TASK_PRIO, (INT8U *)"sensor", &os_err);

}

static void da_output_task(void *p_arg)
{
    /* Configure all DAC Channels as analog output,all DAC Channels means PA4,PA5**/
    DAC_GPIO_ALLCONFIG();
    APP_TRACE("enter DAC output task!\r\n");
	basic_timer_5_init(TIMER_FOR_US,100);
    for (;;)
    {
        /* Configure DAC1(PA4) output DAC_Data1/4096*Vref,DAC2(PA5) output DAC_Data2/4096*Vref ******/
		basic_timer_5_start();
		//APP_TRACE("AD_result[55]:%d,[56]:%d,[57]:%d,[58]:%d,[59]:%d,[60]:%d,[61]:%d\r\n",ADC1_convert_values[55],ADC1_convert_values[56],ADC1_convert_values[57],ADC1_convert_values[58],ADC1_convert_values[59],ADC1_convert_values[60]);
		//决定红绿灯闪烁的周期，200即周期为0.2s
		OSTimeDlyHMSM(0, 0, 0,100);
    }
}


/*
 * Handle TIMER6 interrupt
 *
 */
void TIM5_IRQHandler(void)
{
	static u16 count = 0;
    OSIntEnter();
	/* Clear the interrupt pending flag */
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
    {
    	count++;
		//红绿灯切换，中断时间*count = 切换周期。如果是0.1s切换一次红绿灯，红绿灯各亮50%,中断时间100us，则count = 1000。
		if(count >= 990)
		{
			count = 0;
			basic_timer_5_stop();
			DAC_DUAL_Config(1,1);
			ADC_start();
			red_or_green_flag = !red_or_green_flag;
		}
		else
		{
			//设置亮灯时方波的占空比，count%2 != 0  50%占空比，count%5 != 0  80%占空比
			if(count%5 != 0)
			{
				if(red_or_green_flag)
				{
					DAC_DUAL_Config(4095,1);
				}
				else
				{
					DAC_DUAL_Config(1,4095);
				}
			}
			else
			{
				DAC_DUAL_Config(1,1);
			}
		}
        TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    }
    OSIntExit();
}


#endif/*_APP_DA_OUTPUT_C_*/

