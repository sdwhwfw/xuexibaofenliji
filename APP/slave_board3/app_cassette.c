/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cassette.c
 * Author             : su
 * Date First Issued  : 2013/12/30
 * Description        : This file contains the software implementation for the
 *                      cassette task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/12/30 | v1.0  | su         | initial released
 *******************************************************************************/

#include "app_cassette.h"
#include "exti_board3.h"

#define CASSETTE_PWM_FREQUENCY   20000
#define CASSETTE_PWM_DUTY        80   //change by Big.Man from 80 to 50


OS_TMR* tmr_cassette_protect;
#define TMOEOUT_CASSETTE_PROTECT         550// 200*100ms = 20s is not enough



static OS_STK cassette_control_task_stk[CASSETTE_CONTROL_TASK_STK_SIZE];
static void cassette_control_task(void *p_arg);



void tmr_cassette_protect_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    
    stop_cassette();
	TRACE(" cassette_protect_timeout!\r\n");
    OSTmrStop(tmr_cassette_location,OS_TMR_OPT_NONE,(void*)0,&tmr_err);

}



void init_cassette_control_task()
{
    INT8U os_err;

	/* cassette control task ***************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) cassette_control_task,
							(void		   * ) 0,
							(OS_STK 	   * )& cassette_control_task_stk[CASSETTE_CONTROL_TASK_STK_SIZE - 1],
							(INT8U			 ) CASSETTE_CONTROL_TASK_PRIO,
							(INT16U 		 ) CASSETTE_CONTROL_TASK_PRIO,
							(OS_STK 	   * )& cassette_control_task_stk[0],
							(INT32U 		 ) CASSETTE_CONTROL_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CASSETTE_CONTROL_TASK_PRIO, (INT8U *)"CASSETTE CONTROL", &os_err);
}



static void cassette_control_task(void *p_arg)
{
    INT8U err,tmr_err;
    u16* cassette_flag;
    
    APP_TRACE("enter cassette control task!\r\n");
    /*init timer*/
    TIM1_8_pwm_config(PWM_TIM1,CASSETTE_PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0);
//    TIM1_8_pwm_config(PWM_TIM8,CASSETTE_PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0);

    /*for cassette protect*/

    tmr_cassette_protect    = OSTmrCreate(TMOEOUT_CASSETTE_PROTECT,
                                         TMOEOUT_CASSETTE_PROTECT,
                                         OS_TMR_OPT_PERIODIC,
                                         (OS_TMR_CALLBACK)tmr_cassette_protect_callback,
                                         (void*)0,"TMR_TMOEOUT_CASSETTE_PROTECT",
                                         &tmr_err);
	assert_param(tmr_cassette_protect);



    for( ; ; )
    {
        cassette_flag = OSQPend(cassette_control_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
         switch((u32)cassette_flag)
         {
             /* CASSETTE*/
             case CASSETTE_START_FROM_DOWN_TO_UP:
                     start_cassette(upward);
                 break;
             case CASSETTE_START_FROM_UNKNOW_TO_UP:
                     start_cassette(upward);
                 break;
             case CASSETTE_START_FROM_UP_TO_DOWN:
                     start_cassette(downward);
                 break;
             case CASSETTE_START_FROM_UNKNOW_TO_DOWN:
                     start_cassette(downward);
                 break;
             default:
                 break;
         }
     }
 }





//
void start_cassette(CassetteDirection direction)
{

    INT8U tmr_err;
    OSTmrStart(tmr_cassette_protect,&tmr_err);
	TRACE(" start tmr_cassette_protect!\r\n");
    
    if(direction==downward)
    {

		exti3_irq_operation(ENABLE,1,KASEDO_OPTICAL1_PRE_PRI,KASEDO_OPTICAL1_SUB_PRI);
	 
        pwm_change_cycle(PWM_TIM1, PWM_CHANNEL_1, CASSETTE_PWM_DUTY, 0);
        pwm_change_cycle(PWM_TIM1, PWM_CHANNEL_2, 0, 0);
    }
    else if(direction==upward)
    {
    
		exti3_irq_operation(ENABLE,2,KASEDO_OPTICAL2_PRE_PRI,KASEDO_OPTICAL2_SUB_PRI);

        pwm_change_cycle(PWM_TIM1, PWM_CHANNEL_1, 0, 0);
        pwm_change_cycle(PWM_TIM1, PWM_CHANNEL_2, CASSETTE_PWM_DUTY, 0);
    }
      
}



/*
* @brief: stop cassette motor
* @param:
* @retval: void
*/

void stop_cassette(void)
{
    INT8U tmr_err;
    pwm_change_cycle(PWM_TIM1, PWM_CHANNEL_1, 100, 0);
    pwm_change_cycle(PWM_TIM1, PWM_CHANNEL_2, 100, 0);

    OSTmrStop(tmr_cassette_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
	TRACE(" stop  tmr_cassette_protect!\r\n");
}


