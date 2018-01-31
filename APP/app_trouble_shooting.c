/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_trouble_shooting.h
 * Author             :   TonyZhang
 * Date First Issued  : 2015/05/27
 * Description        : This file contains the software implementation for the 
 *                           trouble shooting unit
 *******************************************************************************
 * History:DATE       | VER   | AUTOR      | Description
 *2015/05/27 | v1.0   | TonyZhang         | initial released
 * 
 *  
 *******************************************************************************/




#ifndef __APP_TROUBLE_SHOOTING_C__
#define __APP_TROUBLE_SHOOTING_C__

#include "app_trouble_shooting.h"
#include "trace.h"
#include "app_control_pump.h"


static OS_STK    trouble_shooting_task_stk[TROUBLE_SHOOTING_TASK_STK_SIZE];



void init_trouble_shooting_task(void)
{
    INT8U  os_err = 0;
    os_err = OSTaskCreateExt((void (*)(void *))trouble_shooting_task,
                            (void 		 * ) 0,
                            (OS_STK		 * )&trouble_shooting_task_stk[TROUBLE_SHOOTING_TASK_STK_SIZE-1],
                            (INT8U		   ) TROUBLE_SHOOTING_TASK_PRIO,
                            (INT16U		   ) TROUBLE_SHOOTING_TASK_PRIO,
                            (OS_STK		 * )&trouble_shooting_task_stk[0],
                            (INT32U		   ) TROUBLE_SHOOTING_TASK_STK_SIZE,
                            (void 		 * ) 0,
                            (INT16U		   )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(TROUBLE_SHOOTING_TASK_PRIO, (INT8U *)"trouble_shooting task", &os_err);
}




void  trouble_shooting_task(void)
{
    INT8U err;
	INT8U l_error;

    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;


    
    sem_test_cancel_protect = OSSemCreate(0);  /*test cancel protect*/
	assert_param(sem_test_cancel_protect);
    
    APP_TRACE("trouble_shooting_task\r\n");
    
    while(1)
    {
        OSSemPend(sem_test_cancel_protect,0,&err);/*test cancel protect event*/
        assert_param(err == OS_ERR_NONE);
                
        init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
        
        l_pump.ac_pump       = 0x01;
        l_pump.draw_pump     = 0x01;
        l_pump.feedBack_pump = 0x01;

        ctrl_stop_pump(l_pump, l_count);    

        APP_TRACE("trouble_shooting_task stop pump\r\n");

        
    }


}



#endif


