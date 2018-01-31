/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pump_control1.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/26
 * Description        : This file contains the software implementation for the
 *                      pump control task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/26 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#include "app_pump_control1.h"
#include "trace.h"
#include "app_slave_board1.h"
#include "app_dc_motor.h"

/*for pump*/
#define STOP 	0x00
#define START 	0x01
#define INIT 	0x02
#define IGNORE 	0x03

static OS_STK pump_control_task_stk[PUMP_CONTROL_TASK_STK_SIZE];


static void pump_control_task(void *p_arg);


static void start_pump(pump_type pump, u8 dir, u16 speed, u16 distance);
static void stop_pump(pump_type pump);

void init_pump_control_task()
{
    INT8U os_err;

	/* pump control task ***************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) pump_control_task,
							(void		   * ) 0,
							(OS_STK 	   * )& pump_control_task_stk[PUMP_CONTROL_TASK_STK_SIZE - 1],
							(INT8U			 ) PUMP_CONTROL_TASK_PRIO,
							(INT16U 		 ) PUMP_CONTROL_TASK_PRIO,
							(OS_STK 	   * )& pump_control_task_stk[0],
							(INT32U 		 ) PUMP_CONTROL_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(PUMP_CONTROL_TASK_PRIO, (INT8U *)"PUMP CONTROL", &os_err);

}



static void pump_control_task(void *p_arg)
{
	u8 err = 0,OSflag;
    u8 tmr_err = 0;
	PumpValveOrder pump_order;
	u8* pump_order_ptr;

    /*for pump control (speed and diatance)*/
    u16 speed_draw_pump        = 0;//
    u16 speed_AC_pump          = 0;//
    u16 speed_feedback_pump    = 0;//

    u8 dir_draw_pump = 0;
    u8 dir_AC_pump = 0;
    u8 dir_feedback_pump = 0;
    /*
    u16 distance_draw_pump     = 0;//
    u16 distance_AC_pump       = 0;//
    u16 distance_feedback_pump = 0;//
    */

	APP_TRACE("pump control task ready...\r\n");
    OSflag = OSFlagPend(init_order_flag,0x04,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);//   
    // ???????????????
	assert_param(OSflag);
    APP_TRACE("pump_control_task OSFlagPend \r\n");

	while (1)
	{

    /*    pump_order_ptr = (u8*)OSQPend(pump_q_send,0,&err);
        assert_param(pump_order_ptr);

       // APP_TRACE("pend start pump_Q\r\n\r\n");
		memcpy(&pump_order,pump_order_ptr,sizeof(PumpValveOrder));

		OSMemPut(pump_mem_order,(void*)pump_mem_ptr);

		//APP_TRACE("draw_pump     = 0x%x\r\n",pump_order.pump.bit.draw_pump);
		//APP_TRACE("AC_pump       = 0x%x\r\n",pump_order.pump.bit.AC_pump);
		//APP_TRACE("feedback_pump = 0x%x\r\n",pump_order.pump.bit.feedback_pump);

        if(((pump_order.pump.bit.draw_pump == START)&&(pump_order.go_on == 0x00))\
            ||((pump_order.pump.bit.AC_pump == START)&&(pump_order.go_on == 0x00))\
            ||((pump_order.pump.bit.feedback_pump == START)&&(pump_order.go_on == 0x00)))//有任意一个泵为启动状态(非继续)
        {
            OSflag = OSFlagPend(start_pump_sync_flag,0x03,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0x00,&err);
            assert_param(OSflag);
       //     APP_TRACE("pend start_pump_sync_flag\r\n");
        }
        */

        OSflag = OSFlagPend(start_pump_sync_flag,0x07,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
        assert_param(OSflag);
       // APP_TRACE("pended start_pump_sync_flag \r\n");


        switch(control_order_r.order_indicate.bit.draw_pump)
		{
			case STOP:
                stop_pump(DRAW_PUMP);

                fn_start_pump = control_order_r.fn;
                //如果继续行程记录 需先记录行程，在清零
                if(control_order_r.para1.bit.draw_pump == 0x00)
                {
                   // draw_pump_old_single_distance = pump_state2.draw_pump_moved_distance;
                   draw_pump_old_single_distance = get_dc_motor_current_distance(MOTOR_NUM3);
                    APP_TRACE("draw_pump_old_single_distance = %d\r\n",draw_pump_old_single_distance);
                }
                else
                {
                    draw_pump_old_single_distance = 0x00;
                }
                clear_dc_motor_current_distance(MOTOR_NUM3);/*clear*/
                APP_TRACE("stop draw pump \r\n");
                OSTmrStop(tmr_draw_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
				break;
			case START:
                //OS_ENTER_CRITICAL();
                //OSSchedLock();
                fn_start_pump = control_order_r.fn;

                speed_draw_pump    = pump_set1.draw_pump_speed_limit;
                //distance_draw_pump = pump_set2.draw_pump_need_distance;
                dir_draw_pump = pump_set1.pump_direction.bit.draw_pump;
                //OSSchedUnlock();
                APP_TRACE("start draw pump \r\n");

              //  APP_TRACE("speed_draw_pump = 0x%x \r\n",speed_draw_pump);
               // APP_TRACE("dir_draw_pump   = 0x%x \r\n",dir_draw_pump);

                start_pump(DRAW_PUMP,dir_draw_pump,speed_draw_pump,0x00);

                OSTmrStart(tmr_draw_pump_timeout,&tmr_err);
                flag_tmr_draw_pump_timeout = 0;
				break;
			case INIT:
                // TODO:  pump init
				break;
			case IGNORE:
				break;
			default:
				APP_TRACE("Unknow draw_pump Order!\r\n");
				break;
		}

		switch(control_order_r.order_indicate.bit.AC_pump)
		{
			case STOP:
                stop_pump(AC_PUMP);/**/

                fn_start_pump = control_order_r.fn;
                //如果继续行程记录 需先记录行程，在清零
                if(control_order_r.para1.bit.AC_pump == 0x00)
                {
                    //AC_pump_old_single_distance = pump_state2.AC_pump_moved_distance;
                    AC_pump_old_single_distance = get_dc_motor_current_distance(MOTOR_NUM2);
                    APP_TRACE("AC_pump_old_single_distance = %d\r\n",AC_pump_old_single_distance);
                }
                else
                {
                    AC_pump_old_single_distance = 0x00;
                }
                clear_dc_motor_current_distance(MOTOR_NUM2);/*clear*/
                OSTmrStop(tmr_AC_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
                
                APP_TRACE("stop AC pump \r\n");
				break;
			case START:
                fn_start_pump = control_order_r.fn;

                speed_AC_pump = pump_set1.AC_pump_speed_limit;
                //distance_AC_pump = pump_set2.AC_pump_need_distance;
                dir_AC_pump = pump_set1.pump_direction.bit.AC_pump;
                start_pump(AC_PUMP,dir_AC_pump,speed_AC_pump,0x00);

                OSTmrStart(tmr_AC_pump_timeout,&tmr_err);
                flag_tmr_AC_pump_timeout = 0;

                APP_TRACE("start AC pump \r\n");
				break;
			case INIT:
                // TODO:pump init
				break;
			case IGNORE:
				break;
			default:
				APP_TRACE("Unknow AC_pump Order!\r\n");
				break;
		}

		switch(control_order_r.order_indicate.bit.feed_back_pump)
		{
			case STOP:
                stop_pump(FEEDBACK_PUMP);

                fn_start_pump = control_order_r.fn;
                //如果继续行程记录 需先记录行程，在清零
                if(control_order_r.para1.bit.feed_back_pump == 0x00)
                {
                    //feedback_pump_old_single_distance = pump_state2.feed_back_pump_moved_distance;
                    feedback_pump_old_single_distance = get_dc_motor_current_distance(MOTOR_NUM1);
                    APP_TRACE("feedback_pump_old_single_distance = %d\r\n",feedback_pump_old_single_distance);
                }
                else
                {
                    feedback_pump_old_single_distance = 0x00;
                }
                clear_dc_motor_current_distance(MOTOR_NUM1);/*clear*/
                OSTmrStop(tmr_feedback_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
                
                APP_TRACE("stop feed_back_pump \r\n");
				break;
			case START:

                fn_start_pump = control_order_r.fn;

                speed_feedback_pump = pump_set1.feed_back_pump_speed_limit;
                //distance_feedback_pump = pump_set2.feed_back_pump_need_distance;
                dir_feedback_pump = pump_set1.pump_direction.bit.feed_back_pump;

               // APP_TRACE("start feedback_pump \r\n");
                start_pump(FEEDBACK_PUMP,dir_feedback_pump,speed_feedback_pump,0x00);
                OSTmrStart(tmr_feedback_pump_timeout,&tmr_err);
                flag_tmr_feedback_pump_timeout = 0;
                
                 APP_TRACE("start feedback_pump \r\n");
				break;
			case INIT:
                // TODO:pump init
				break;
			case IGNORE:
				break;
			default:
				APP_TRACE("Unknow feedback_pump Order!\r\n");
				break;
		}

		//OSTimeDlyHMSM(0, 0, 0, 50);
	}
}





static void start_pump(pump_type pump, u8 dir, u16 speed, u16 distance)
{
    MotorType pump_tmp;
    float tmp_speed;
    assert_param(IS_PUMP_TYPE(pump));

    switch(pump)
    {
        case DRAW_PUMP:
            pump_tmp = MOTOR_NUM3;
        break;
        case AC_PUMP:
            pump_tmp = MOTOR_NUM2;
        break;
        case FEEDBACK_PUMP:
            pump_tmp = MOTOR_NUM1;
        break;
        default:
        break;
    }
    tmp_speed = speed/100.0;

    //APP_TRACE("pump_tmp = 0x%x\r\n",pump_tmp);
   // APP_TRACE("speed    = %d rpm\r\n",tmp_speed);
   // APP_TRACE("distance = 0x%x\r\n",distance);


    if(dir == 0x01)//clockwise
    {
        set_motor_speed(pump_tmp, tmp_speed, distance);
    }
    else  //un clockwise
    {
        set_motor_speed(pump_tmp,-tmp_speed, distance);
    }
}


static void stop_pump(pump_type pump)
{
    MotorType pump_tmp;
    assert_param(IS_PUMP_TYPE(pump));

    switch(pump)
    {
        case DRAW_PUMP:
            pump_tmp = MOTOR_NUM3;
        break;
        case AC_PUMP:
            pump_tmp = MOTOR_NUM2;
        break;
        case FEEDBACK_PUMP:
            pump_tmp = MOTOR_NUM1;
        break;
        default:
        break;
    }
    stop_dc_motor(pump_tmp);
}





