/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_electromagnet.c
 * Author             : su
 * Date First Issued  : 2013/12/30
 * Description        : This file contains the software implementation for the
 *                      electromagnet task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/12/30 | v1.0  | su         | initial released
 *******************************************************************************/

#include "app_electromagnet.h"
#include "app_slave_board3.h"

#define MAGNET_PWM_FREQUENCY   100
#define LOCK_DUTY    80
#define HOLD_DUTY    20

#define LOCK_MAGNET   0x01
#define UNLOCK_MAGNET 0X02
static OS_STK magnet_control_task_stk[MAGNET_CONTROL_TASK_STK_SIZE];
static void  magnet_control_task(void *p_arg);

void init_magnet_control_task()
{
    INT8U os_err;
	/* cassette control task ***************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) magnet_control_task,
							(void		   * ) 0,
							(OS_STK 	   * )& magnet_control_task_stk[MAGNET_CONTROL_TASK_STK_SIZE - 1],
							(INT8U			 ) MAGNET_CONTROL_TASK_PRIO,
							(INT16U 		 ) MAGNET_CONTROL_TASK_PRIO,
							(OS_STK 	   * )& magnet_control_task_stk[0],
							(INT32U 		 ) MAGNET_CONTROL_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(MAGNET_CONTROL_TASK_PRIO, (INT8U *)"MAGNET CONTROL", &os_err);
}

static void magnet_control_task(void *p_arg)
{
    INT8U err;
    u8 electromagnet_flag;
    APP_TRACE("enter magnet control task!\r\n");
    /*init timer*/
    TIM1_8_pwm_config(PWM_TIM8,MAGNET_PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0);
    for( ; ; )
    {
        electromagnet_flag = OSFlagPend(magnet_flag,0x03,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
        assert_param(OS_ERR_NONE == err);
        APP_TRACE("electromagnet_flag = %d\r\n",electromagnet_flag);
        switch(electromagnet_flag)
        {
         /* CASSETTE*/
         case LOCK_MAGNET:
                pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, LOCK_DUTY, 0);
                pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 0, 0);
                OSTimeDlyHMSM(0, 0, 2, 0);
                pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, HOLD_DUTY, 0);
                APP_TRACE("APP LOCK_MAGNET!\r\n");
             break;
         case UNLOCK_MAGNET:
                pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, 0, 0);
                pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 0, 0);
                APP_TRACE("APP UNLOCK_MAGNET!\r\n");
             break;
         default:
             break;
        }

     }
 }

 /*unlock magnet */
 void unlock_magnet(void)
 {
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, LOCK_DUTY, 0);
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 0, 0);
     OSTimeDlyHMSM(0, 0, 2, 0);
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, 0, 0);
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 0, 0);
     APP_TRACE("UNLOCK_MAGNET!\r\n");
 }

 /*lock magnet */
 void lock_magnet(void)
 {
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, 0, 0);
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, LOCK_DUTY, 0);
     OSTimeDlyHMSM(0, 0, 2, 0);
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, 0, 0);
     pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 0, 0);

     APP_TRACE("LOCK_MAGNET!\r\n");
 }



