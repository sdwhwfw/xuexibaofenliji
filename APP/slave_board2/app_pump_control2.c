/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pump_control2.c
 * Author             : WQ
 * Date First Issued  : 2013/10/10
 * Description        : This file contains the software implementation for the
 *                      pump control task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/10 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_PUMP_CONTROL2_C_
#define _APP_PUMP_CONTROL2_C_

#include "app_pump_control2.h"
#include "trace.h"
#include "app_slave_board2.h"
#include "app_monitor2.h"
#include "app_valve_control.h"
#include "app_dc_motor.h"

/*for pump*/
#define STOP 	0x00
#define START 	0x01
#define INIT 	0x02
#define IGNORE 	0x03

/*for valve lacotion*/
#define LEFT 	0x00
#define MID 	0x01
#define RIGHT 	0x02

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
    u8 current_location=0;
	PumpValveOrder pump_order;
	SensorState  sensor_state_compare;
    u16 valve_cmd;
	u8* pump_order_ptr;
    
    /*for pump control (speed and diatance)*/
    u16 speed_PLT_pump        = 0;//
    u16 speed_plasma_pump     = 0;//

    u8 dir_PLT_pump = 0;
    u8 dir_plasma_pump = 0;
    /*
    u16 distance_PLT_pump     = 0;//
    u16 distance_plasma_pump  = 0;//
    */

	APP_TRACE("pump control task ready...\r\n");
    //for debug
    /*
    PLTVALVE_LEFT_OPTICAL_FLAG=1;
    PLTVALVE_RIGHT_OPTICAL_FLAG=1;
    PLTVALVE_MIDDLE_OPTICAL_FLAG=1;
    PLAVALVE_LEFT_OPTICAL_FLAG=1;
    PLAVALVE_RIGHT_OPTICAL_FLAG=1;
    PLAVALVE_MIDDLE_OPTICAL_FLAG=1;
    RBCVALVE_LEFT_OPTICAL_FLAG=1;
    RBCVALVE_RIGHT_OPTICAL_FLAG=1;
    RBCVALVE_MIDDLE_OPTICAL_FLAG=1;
    
    control_order_r.module_indicate = 0x02;
    control_order_r.order_indicate.bit.plasma_valve_or_pump = RIGHT;
    control_order_r.order_indicate.bit.PLT_valve_or_pump = RIGHT;
    control_order_r.order_indicate.bit.RBC_valve = RIGHT;
    current_location = LEFT;
    */
    //for debug

	while (1)
	{

//		pump_order_ptr = (u8*)OSQPend(pump_q_send,0,&err);
//        assert_param(pump_order_ptr);
//		memcpy(&pump_order,pump_order_ptr,sizeof(PumpValveOrder));
//		memcpy(&sensor_state_compare,&sensor_state,sizeof(SensorState));//unmask by wq

//		OSMemPut(pump_mem_order,(void*)pump_mem_ptr);

      if(((pump_order.pump.bit.plasma_pump == START)&&(pump_order.go_on == 0x00))\
            ||((pump_order.pump.bit.PLT_pump == START)&&(pump_order.go_on == 0x00)))//有任意一个泵为启动状态
        {
            OSflag = OSFlagPend(start_pump_sync_flag,0x03,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0x00,&err);
            assert_param(OSflag);
            APP_TRACE("pend start_pump_sync_flag\r\n");
        }
        
        OSflag = OSFlagPend(start_pump_sync_flag,0x0f,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
        assert_param(OSflag);
        APP_TRACE("pended start_pump_sync_flag \r\n");
        
		memcpy(&sensor_state_compare,&sensor_state,sizeof(SensorState));//read after pended 
		
		switch(control_order_r.module_indicate)
		{
			case 0x01://pump
				switch(control_order_r.order_indicate.bit.plasma_valve_or_pump)
				{
					case STOP:
                        stop_pump(PLASMA_PUMP);

                        fn_start_pump = control_order_r.fn;
                        //如果继续行程记录 需先记录行程，在清零
                        if(control_order_r.para1.bit.plasma_pump == 0x00)
                        {
                           // plasma_pump_old_single_distance = pump_state2.plasma_pump_moved_distance;

                            plasma_pump_old_single_distance = get_dc_motor_current_distance(MOTOR_NUM2);
                            APP_TRACE("plasma_pump_old_single_distance = %d\r\n",plasma_pump_old_single_distance);
                        }
                        else
                        {
                            plasma_pump_old_single_distance = 0x00;
                        }
                        clear_dc_motor_current_distance(MOTOR_NUM2);/*clear*/

                        APP_TRACE("stop plasma pump\r\n");
                        OSTmrStop(tmr_plasma_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
						break;
					case START:
                        fn_start_pump = control_order_r.fn;
                        speed_plasma_pump    = pump_set1.plasma_pump_speed_limit;
                        //distance_plasma_pump = pump_set2.plasma_pump_need_distance;
                        dir_plasma_pump = pump_set1.pump_direction.bit.plasma_pump;
                        start_pump(PLASMA_PUMP,dir_plasma_pump,speed_plasma_pump,0x00);
                        OSTmrStart(tmr_plasma_pump_timeout,&tmr_err);
                        flag_tmr_plasma_pump_timeout = 0;
                        
                        APP_TRACE("start plasma pump..................................................\r\n");
						break;
					case INIT:
						break;
					case IGNORE:
						break;
					default:
						APP_TRACE("Unknow plasma_pump Order!\r\n");
						break;
				}
				switch(control_order_r.order_indicate.bit.PLT_valve_or_pump)
				{
					case STOP:
                        stop_pump(PLT_PUMP);

                        fn_start_pump = control_order_r.fn;
                        //如果继续行程记录 需先记录行程，在清零
                        if(control_order_r.para1.bit.PLT_pump == 0x00)
                        {
                           // PLT_pump_old_single_distance = pump_state2.PLT_pump_moved_distance;

                            PLT_pump_old_single_distance = get_dc_motor_current_distance(MOTOR_NUM1);
                            APP_TRACE("PLT_pump_old_single_distance = %d\r\n",PLT_pump_old_single_distance);
                        }
                        else
                        {
                            PLT_pump_old_single_distance = 0x00;
                        }
                        clear_dc_motor_current_distance(MOTOR_NUM1);/*clear*/

                        OSTmrStop(tmr_PLT_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
                        
                        APP_TRACE("stop pltpump\r\n");
						break;
					case START:
                        fn_start_pump = control_order_r.fn;
                        speed_PLT_pump    = pump_set1.PLT_pump_speed_limit;
                        // distance_PLT_pump = pump_set2.PLT_pump_need_distance;
                        dir_PLT_pump = pump_set1.pump_direction.bit.PLT_pump;
                        start_pump(PLT_PUMP,dir_PLT_pump,speed_PLT_pump,0x00);
                        OSTmrStart(tmr_PLT_pump_timeout,&tmr_err);
                        flag_tmr_PLT_pump_timeout = 0;
                        
                        APP_TRACE("start plt pump........................................................\r\n");
						break;
					case INIT:
						break;
					case IGNORE:
						break;
					default:
						APP_TRACE("Unknow PLT_pump Order!\r\n");
						break;
				}

				break;
			case 0x02://valve

				switch(control_order_r.order_indicate.bit.plasma_valve_or_pump)
				{
                    case LEFT:
                        current_location = valve_location_translate(0x01,sensor_state_compare);
                        APP_TRACE("PLA current_location = %d\r\n",current_location);
                        
                        if( current_location!= LEFT)
                        {
                            if(current_location==MID)
                            {
                                valve_cmd = PLA_START_FROM_MIDDLE_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err);                               
                            }
                            else if(current_location==RIGHT)
                            {

                                valve_cmd = PLA_START_FROM_RIGHT_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err);   
                            }
                            else
                            {
                                valve_cmd = PLA_START_FROM_UNKNOW_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err);                                  
                            }    
                            /*PLA valve left optical will be trigger*/
                            PLAVALVE_LEFT_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("plasma_valve is already LEFT!\r\n");
                        }
						break;
					case MID:
                        current_location = valve_location_translate(0x01,sensor_state_compare);
                        
                        APP_TRACE("PLA current_location = %d\r\n",current_location);
                        if(current_location != MID)
                        {
                            if(current_location==LEFT)
                            {
                                valve_cmd = PLA_START_FROM_LEFT_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err);                               
                            }
                            else if(current_location==RIGHT)
                            {
                                valve_cmd = PLA_START_FROM_RIGHT_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = PLA_START_FROM_UNKNOW_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*PLA valve middle optical will be trigger*/
                            PLAVALVE_MIDDLE_OPTICAL_FLAG=1;

                        }
                        else
                        {
                            APP_TRACE("plasma_valve is already MID!\r\n");
                        }

						break;
                        
					case RIGHT:
                        current_location=valve_location_translate(0x01,sensor_state_compare);
                        
                        APP_TRACE("PLA current_location = %d\r\n",current_location);
                        if(current_location != RIGHT)
                        {
                            if(current_location==MID)
                            {
                               valve_cmd = PLA_START_FROM_MIDDLE_TO_RIGHT;
                               err = OSQPost(valve_control_event, (void*)valve_cmd); 
                               assert_param(OS_ERR_NONE == err); 
                            }
                            else if(current_location==LEFT)
                            {
                                valve_cmd = PLA_START_FROM_LEFT_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = PLA_START_FROM_UNKNOW_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*PLA valve right optical will be trigger*/
                            PLAVALVE_RIGHT_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("plasma_valve is already RIGHT!\r\n");
                        }
						break;
					case IGNORE:

						break;
					default:
						APP_TRACE("Unknow plasma_valve Order!\r\n");
						break;
				}

				switch(control_order_r.order_indicate.bit.PLT_valve_or_pump)
				{
					case LEFT:                        
                        current_location = valve_location_translate(0x02,sensor_state_compare);
                        
                        APP_TRACE("PLT current_location = %d\r\n",current_location);
                        if(current_location!= LEFT)
                        {
                            if(current_location==MID)
                            {
                               valve_cmd = PLT_START_FROM_MIDDLE_TO_LEFT;
                               err = OSQPost(valve_control_event, (void*)valve_cmd); 
                               APP_TRACE("err = %d\r\n",err);
                               assert_param(OS_ERR_NONE == err); 
                            }
                            else if(current_location==RIGHT)
                            {
                                valve_cmd = PLT_START_FROM_RIGHT_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = PLT_START_FROM_UNKNOW_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*PLT valve left optical will be trigger*/
                            PLTVALVE_LEFT_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("PLT_valve is already LEFT!\r\n");
                        }
						break;
					case MID:
                        current_location = valve_location_translate(0x02,sensor_state_compare);
                        
                        APP_TRACE("PLT current_location = %d\r\n",current_location);
                        if(current_location!= MID)
                        {
                            if(current_location==LEFT)
                            {
                               valve_cmd = PLT_START_FROM_LEFT_TO_MIDDLE;
                               err = OSQPost(valve_control_event, (void*)valve_cmd); 
                               assert_param(OS_ERR_NONE == err);                                
                            }
                            else if(current_location==RIGHT)
                            {
                                valve_cmd = PLT_START_FROM_RIGHT_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = PLT_START_FROM_UNKNOW_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*PLT valve middle optical will be trigger*/
                            PLTVALVE_MIDDLE_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("PLT_valve is already MID!\r\n");
                        }
						break;
					case RIGHT:
                        current_location = valve_location_translate(0x02,sensor_state_compare);
                        
                        APP_TRACE("PLT current_location = %d\r\n",current_location);
                        if(current_location!= RIGHT)
                        {
                            if(current_location==MID)
                            {
                                valve_cmd = PLT_START_FROM_MIDDLE_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else if(current_location==LEFT)
                            {
                                valve_cmd = PLT_START_FROM_LEFT_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = PLT_START_FROM_UNKNOW_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*PLT valve right optical will be trigger*/
                            PLTVALVE_RIGHT_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("PLT_valve is already RIGHT!\r\n");
                        }
						break;
					case IGNORE:
						break;
					default:
						APP_TRACE("Unknow PLT_valve Order!\r\n");
						break;
				}

				switch(control_order_r.order_indicate.bit.RBC_valve)
				{
                    case LEFT:                        
                        //APP_TRACE("RBC to LEFT!\r\n");
                        current_location =valve_location_translate(0x03,sensor_state_compare);
                        
                        APP_TRACE("RBC current_location = %d\r\n",current_location);
                        if(current_location != LEFT)
                        {
                            if(current_location==MID)
                            {
                                valve_cmd = RBC_START_FROM_MIDDLE_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else if(current_location==RIGHT)
                            {
                                valve_cmd = RBC_START_FROM_RIGHT_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = RBC_START_FROM_UNKNOW_TO_LEFT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*RBC valve left optical will be trigger*/
                            RBCVALVE_LEFT_OPTICAL_FLAG=1;

                        }
                        else
                        {
                            APP_TRACE("RBC_valve is already LEFT!\r\n");
                        }
						break;
					case MID:
                        current_location =valve_location_translate(0x03,sensor_state_compare);
                        
                        APP_TRACE("RBC current_location = %d\r\n",current_location);
                        if(current_location != MID)
                        {
                            if(current_location==LEFT)
                            {
                                valve_cmd = RBC_START_FROM_LEFT_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else if(current_location==RIGHT)
                            {
                                valve_cmd = RBC_START_FROM_RIGHT_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = RBC_START_FROM_UNKNOW_TO_MIDDLE;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*RBC valve middle optical will be trigger*/
                            RBCVALVE_MIDDLE_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("RBC_valve is already MID!\r\n");
                        }
						break;
					case RIGHT:
                        current_location =valve_location_translate(0x03,sensor_state_compare);
                        
                        APP_TRACE("RBC current_location = %d\r\n",current_location);
                        if(current_location != RIGHT)
                        {
                            if(current_location==MID)
                            {
                               valve_cmd = RBC_START_FROM_MIDDLE_TO_RIGHT;
                               err = OSQPost(valve_control_event, (void*)valve_cmd); 
                               assert_param(OS_ERR_NONE == err); 
                            }
                            else if(current_location==LEFT)
                            {
                                valve_cmd = RBC_START_FROM_LEFT_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            else
                            {
                                valve_cmd = RBC_START_FROM_UNKNOW_TO_RIGHT;
                                err = OSQPost(valve_control_event, (void*)valve_cmd); 
                                assert_param(OS_ERR_NONE == err); 
                            }
                            /*RBC valve right optical will be trigger*/
                            RBCVALVE_RIGHT_OPTICAL_FLAG=1;
                        }
                        else
                        {
                            APP_TRACE("RBC_valve is already RIGHT!\r\n");
                        }

						break;
					case IGNORE:

						break;
					default:
						APP_TRACE("Unknow RBC_valve Order!\r\n");
						break;
				}
				break;
			default:
				APP_TRACE("Unknow pump_or_valve !\r\n");
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
        case PLT_PUMP:
            pump_tmp = MOTOR_NUM1;
        break;
        case PLASMA_PUMP:
            pump_tmp = MOTOR_NUM2;
        break;

        default:
        break;
    }
    tmp_speed = speed/100.0;

   // APP_TRACE("pump_tmp = 0x%x\r\n",pump_tmp);
    APP_TRACE("speed    = %d rpm\r\n",tmp_speed);

    if(dir == 0x01)//clockwise
    {
        set_motor_speed(pump_tmp, (tmp_speed), distance);
    }
    else  //un clockwise
    {
        set_motor_speed(pump_tmp,-(tmp_speed), distance);
    }
}


static void stop_pump(pump_type pump)
{
    MotorType pump_tmp;
    assert_param(IS_PUMP_TYPE(pump));

    switch(pump)
    {
        case PLT_PUMP:
            pump_tmp = MOTOR_NUM1;
        break;
        case PLASMA_PUMP:
            pump_tmp = MOTOR_NUM2;
        break;
        default:
        break;
    }
    stop_dc_motor(pump_tmp);
}
#endif

