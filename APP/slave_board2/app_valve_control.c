/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_valve_control.c
 * Author             : su
 * Date First Issued  : 2013/11/26
 * Description        : This file contains the software implementation for the
 *                      valve control task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/26 | v1.0  | su         | initial released
 *******************************************************************************/
#ifndef _APP_VALVE_CONTROL_C_
#define _APP_VALVE_CONTROL_C_

#include "app_valve_control.h"
#include "trace.h"

#define PWM_FREQUENCY   20000
#define PWM_DUTY        100


static OS_STK valve_control_task_stk[VALVE_CONTROL_TASK_STK_SIZE];
static void valve_control_task(void *p_arg);
void start_valve(ValveType x_valve,ValveDirection direction);

void init_valve_control_task()
{
    INT8U os_err;

	/* valve control task ***************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) valve_control_task,
							(void		   * ) 0,
							(OS_STK 	   * )& valve_control_task_stk[VALVE_CONTROL_TASK_STK_SIZE - 1],
							(INT8U			 ) VALVE_CONTROL_TASK_PRIO,
							(INT16U 		 ) VALVE_CONTROL_TASK_PRIO,
							(OS_STK 	   * )& valve_control_task_stk[0],
							(INT32U 		 ) VALVE_CONTROL_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(VALVE_CONTROL_TASK_PRIO, (INT8U *)"VALVE CONTROL", &os_err);
}

static void valve_control_task(void *p_arg)
{
    INT8U err;
    u16* valve_flag;
//    valve_control_grp=OSFlagCreate(0x0000,&err);
//    assert_param(valve_control_grp);
    APP_TRACE("enter valve control task!\r\n");
    /*init timer*/
    TIM1_8_pwm_config(PWM_TIM1,PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0);
    TIM1_8_pwm_config(PWM_TIM8,PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0);
    TIM2_5_9_14_pwm_config( PWM_TIM4,PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0) ;
    TIM2_5_9_14_pwm_config( PWM_TIM9,PWM_FREQUENCY,PWM_CHANNEL_1|PWM_CHANNEL_2,100,100,0,0) ;
	
    for( ; ; )
    {
        valve_flag = OSQPend(valve_control_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
         switch((u32)valve_flag)
         {
             /*PLT VALVE*/
             case PLT_START_FROM_LEFT_TO_MIDDLE:
                    APP_TRACE("VALVE CONTROL PLT_START_FROM_LEFT_TO_MIDDLE!!!!\r\n");
                    start_valve(PLTVALVE,forward);
                 break;
             case PLT_START_FROM_LEFT_TO_RIGHT :
                    APP_TRACE("VALVE CONTROL PLT_START_FROM_LEFT_TO_RIGHT!!!!\r\n");
                    start_valve(PLTVALVE,forward);
                 break;
             case PLT_START_FROM_MIDDLE_TO_LEFT:
                    APP_TRACE("VALVE CONTROL PLT_START_FROM_MIDDLE_TO_LEFT!!!!\r\n");
                    start_valve(PLTVALVE,reverse);
                 break;
             case PLT_START_FROM_MIDDLE_TO_RIGHT:
                    APP_TRACE("VALVE CONTROL PLT_START_FROM_MIDDLE_TO_RIGHT!!!!\r\n");
                    start_valve(PLTVALVE,forward);
                 break;
             case PLT_START_FROM_RIGHT_TO_MIDDLE:
                    APP_TRACE("VALVE CONTROL PLT_START_FROM_RIGHT_TO_MIDDLE!!!!\r\n");
                     start_valve(PLTVALVE,reverse);
                 break;
             case PLT_START_FROM_RIGHT_TO_LEFT:
                     APP_TRACE("VALVE CONTROL PLT_START_FROM_RIGHT_TO_LEFT!!!!\r\n");
                     start_valve(PLTVALVE,reverse);
                 break;
             case PLT_START_FROM_UNKNOW_TO_LEFT:
                 APP_TRACE("VALVE CONTROL PLT_START_FROM_UNKNOW_TO_LEFT!!!!\r\n");
                     start_valve(PLTVALVE,forward);
                 break;
             case PLT_START_FROM_UNKNOW_TO_MIDDLE:
                 APP_TRACE("VALVE CONTROL PLT_START_FROM_UNKNOW_TO_MIDDLE!!!!\r\n");
                     start_valve(PLTVALVE,forward);
                 break;
             case PLT_START_FROM_UNKNOW_TO_RIGHT:
                 APP_TRACE("VALVE CONTROL PLT_START_FROM_UNKNOW_TO_RIGHT!!!!\r\n");
                     start_valve(PLTVALVE,forward);
                 break;

             /*PLA VALVE*/

             case PLA_START_FROM_LEFT_TO_MIDDLE:
                    start_valve(PLAVALVE,forward);
                    APP_TRACE("VALVE CONTROL PLA_START_FROM_LEFT_TO_MIDDLE!!!!\r\n");
                 break;
             case PLA_START_FROM_LEFT_TO_RIGHT :
                     APP_TRACE("VALVE CONTROL PLA_START_FROM_LEFT_TO_RIGHT!!!!\r\n");
                     start_valve(PLAVALVE,forward);
                 break;
             case PLA_START_FROM_MIDDLE_TO_LEFT:
                     start_valve(PLAVALVE,reverse);
                     APP_TRACE("VALVE CONTROL PLA_START_FROM_MIDDLE_TO_LEFT!!!!\r\n");
                 break;
             case PLA_START_FROM_MIDDLE_TO_RIGHT:
                     APP_TRACE("VALVE CONTROL PLA_START_FROM_MIDDLE_TO_RIGHT!!!!\r\n");
                     start_valve(PLAVALVE,forward);
                 break;

             case PLA_START_FROM_RIGHT_TO_MIDDLE:
                 APP_TRACE("VALVE CONTROL PLA_START_FROM_RIGHT_TO_MIDDLE!!!!\r\n");
                     start_valve(PLAVALVE,reverse);
                 break;
             case PLA_START_FROM_RIGHT_TO_LEFT:
                 APP_TRACE("VALVE CONTROL PLA_START_FROM_RIGHT_TO_LEFT!!!!\r\n");
                     start_valve(PLAVALVE,reverse);
                 break;
             case PLA_START_FROM_UNKNOW_TO_LEFT:
                 APP_TRACE("VALVE CONTROL PLA_START_FROM_UNKNOW_TO_LEFT!!!!\r\n");
                     start_valve(PLAVALVE,forward);
                 break;
             case PLA_START_FROM_UNKNOW_TO_MIDDLE:
                 APP_TRACE("VALVE CONTROL PLA_START_FROM_UNKNOW_TO_MIDDLE!!!!\r\n");
                     start_valve(PLAVALVE,forward);
                 break;
             case PLA_START_FROM_UNKNOW_TO_RIGHT:
                 APP_TRACE("VALVE CONTROL PLA_START_FROM_UNKNOW_TO_RIGHT!!!!\r\n");
                     start_valve(PLAVALVE,forward);
                 break;

             /*  RBC VALVE*/
             case RBC_START_FROM_LEFT_TO_MIDDLE:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_LEFT_TO_MIDDLE!!!!\r\n");
                     start_valve(RBCVALVE,forward);
                 break;
             case RBC_START_FROM_LEFT_TO_RIGHT :
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_LEFT_TO_RIGHT!!!!\r\n");
                     start_valve(RBCVALVE,forward);
                 break;
             case RBC_START_FROM_MIDDLE_TO_LEFT:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_MIDDLE_TO_LEFT!!!!\r\n");
                     start_valve(RBCVALVE,reverse);
                 break;
             case RBC_START_FROM_MIDDLE_TO_RIGHT:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_MIDDLE_TO_RIGHT!!!!\r\n");
                     start_valve(RBCVALVE,forward);
                 break;
             case RBC_START_FROM_RIGHT_TO_MIDDLE:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_RIGHT_TO_MIDDLE!!!!\r\n");
                     start_valve(RBCVALVE,reverse);
                 break;

             case RBC_START_FROM_RIGHT_TO_LEFT:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_RIGHT_TO_LEFT!!!!\r\n");
                     start_valve(RBCVALVE,reverse);
                 break;
             case RBC_START_FROM_UNKNOW_TO_LEFT:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_UNKNOW_TO_LEFT!!!!\r\n");
                     start_valve(RBCVALVE,forward);
                 break;
             case RBC_START_FROM_UNKNOW_TO_MIDDLE:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_UNKNOW_TO_MIDDLE!!!!\r\n");
                     start_valve(RBCVALVE,forward);
                 break;
             case RBC_START_FROM_UNKNOW_TO_RIGHT:
                 APP_TRACE("VALVE CONTROL RBC_START_FROM_UNKNOW_TO_RIGHT!!!!\r\n");
                     start_valve(RBCVALVE,forward);
                 break;
             default:
                 break;
         }
         OSTimeDlyHMSM(0,0,0,10);
     }
 }

 void start_valve(ValveType x_valve,ValveDirection direction)
 {

     assert_param(IS_VALVE_AND_CASSETTE_TYPE(x_valve));
     switch(x_valve)
     {
         case PLAVALVE:
             {
                if(direction == forward)
                {
                    pwm_change_cycle(PWM_TIM4, PWM_CHANNEL_1, PWM_DUTY, 0);
                    pwm_change_cycle(PWM_TIM4, PWM_CHANNEL_2, 0, 0);
                }
                 else if(direction==reverse)
                 {
                    pwm_change_cycle(PWM_TIM4, PWM_CHANNEL_1, 0, 0);
                    pwm_change_cycle(PWM_TIM4, PWM_CHANNEL_2, PWM_DUTY, 0);
                 }
             }
             break;
        case PLTVALVE:
             {
                 if(direction==forward)
                 {
                    pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, PWM_DUTY, 0);
                    pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 0, 0);
                 }
                 else if(direction==reverse)
                 {
                    pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, 0, 0);
                    pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, PWM_DUTY, 0);
                 }
             }
             break;
        case RBCVALVE:
             {
                 if(direction==forward)
                 {
                    pwm_change_cycle(PWM_TIM9, PWM_CHANNEL_1, PWM_DUTY, 0);
                    pwm_change_cycle(PWM_TIM9, PWM_CHANNEL_2, 0, 0);
                 }
                 else if(direction==reverse)
                 {
                    pwm_change_cycle(PWM_TIM9, PWM_CHANNEL_1, 0, 0);
                    pwm_change_cycle(PWM_TIM9, PWM_CHANNEL_2, PWM_DUTY, 0);
                 }
             }
             break;
         default:
             break;
     }

 }


 /*
  * @brief: stop valves
  * @param: valve, select which valve to stop
  *                  PLAVALVE
  *                  PLTVALVE
  *                  RBCVALVE
  *                  CASSETTE
  * @retval: void
  */

 void stop_valve(ValveType valve)
 {
     assert_param(IS_VALVE_AND_CASSETTE_TYPE(valve));
     switch(valve)
     {
         case PLAVALVE:
            pwm_change_cycle(PWM_TIM4, PWM_CHANNEL_1, 100, 0);
            pwm_change_cycle(PWM_TIM4, PWM_CHANNEL_2, 100, 0);
             break;

        case PLTVALVE:
            pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_1, 100, 0);
            pwm_change_cycle(PWM_TIM8, PWM_CHANNEL_2, 100, 0);

             break;

        case RBCVALVE:
            pwm_change_cycle(PWM_TIM9, PWM_CHANNEL_1, 100, 0);
            pwm_change_cycle(PWM_TIM9, PWM_CHANNEL_2, 100, 0);
             break;
        default:
             break;
     }
 }

#endif/*_APP_VALVE_CONTROL_C_*/





