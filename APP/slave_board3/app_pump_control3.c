/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pump_control3.c
 * Author             : WQ
 * Date First Issued  : 2013/10/10
 * Description        : This file contains the software implementation for the
 *                      pump control task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/10 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_PUMP_CONTROL3_C_
#define _APP_PUMP_CONTROL3_C_

#include "app_pump_control3.h"
#include "trace.h"
#include "app_slave_board3.h"
#include "app_cassette.h"
#include "app_centrifuge_control.h"

/**** for centrifuge_motor*******/
#define STOP 	0x00
#define START 	0x01
#define INIT 	0x02
#define IGNORE 	0x03

/**** for cassette location*******/
#define UP       0x01
#define DOWN     0x02

/**** for magnet*******/
#define UNLOCK 	0x00
#define LOCK 	0x01

static OS_STK pump_control_task_stk[PUMP_CONTROL_TASK_STK_SIZE];

static void pump_control_task(void *p_arg);

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
    u8 cassette_current_location=0;   
    u16 cassette_cmd;
    PumpValveOrder pump_order;
    SensorState sensor_state_compare;

	u8* pump_order_ptr;

    /*for motor speed and distance*/


	APP_TRACE("pump control task ready...\r\n");
    
    //for debug
//    CASSETTE_UP_OPTICAL_FLAG=1;
//    CASSETTE_DOWN_OPTICAL_FLAG=1;  
//    pump_order.pump_or_valve = 0x02;
//    pump_order.valve.bit.magnet = LOCK;
    //for debug

	while (1)
	{
		//pump_order_ptr = (u8*)OSQPend(pump_q_send,0,&err);
       // assert_param(pump_order_ptr);

        OSflag = OSFlagPend(start_pump_sync_flag,0x07,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
        assert_param(OSflag);
        APP_TRACE("Pended start_pump_sync_flag OSFlag = %d err = %d \r\n\r\n",OSflag,err);
        
        
		//memcpy(&pump_order,pump_order_ptr,sizeof(PumpValveOrder));
		
		//memcpy(&pump_order,control_order_r,sizeof(ControlOrder));
		memcpy(&sensor_state_compare,&sensor_state,sizeof(SensorState));

		//OSMemPut(pump_mem_order,(void*)pump_mem_ptr);

		//APP_TRACE("centrifuge_motor  = 0x%x\r\n",pump_order.pump.bit.centrifuge_motor);
		//APP_TRACE("cassette_motor    = 0x%x\r\n",pump_order.pump.bit.cassette_motor);
		//APP_TRACE("magnet            = 0x%x\r\n",pump_order.valve.bit.magnet);

		switch(control_order_r.module_indicate)
        {
            case 0x01://centrifuge or cassette
				switch(control_order_r.order_indicate.bit.cassette_motor)
                {   
                    case 0x01://up
                        APP_TRACE("cassette to up\r\n");
                        cassette_current_location = sensor_state_compare.sensor.bit.cassette_location;

                        if( cassette_current_location!= UP)
                        {
                            if(cassette_current_location==DOWN)
                            {
                                cassette_cmd = CASSETTE_START_FROM_DOWN_TO_UP;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);    
                                APP_TRACE("CASSETTE_START_FROM_DOWN_TO_UP!\r\n");                                
                            }
                            else
                            {
                                cassette_cmd = CASSETTE_START_FROM_UNKNOW_TO_UP;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);    
                                APP_TRACE("CASSETTE_START_FROM_UNKNOW_TO_UP!\r\n");                                
                            }  
                            /*cassette UP optical will be trigger*/
                            CASSETTE_UP_OPTICAL_FLAG=1;
                            
                        }
                        else
                        {
                            APP_TRACE("cassette_location is already UP!\r\n");
                        }
                        break;
                    
                    case 0x02://down
                        APP_TRACE("cassette to down\r\n");
                        cassette_current_location=sensor_state_compare.sensor.bit.cassette_location;

                        if(cassette_current_location != DOWN)
                        {
                            if(cassette_current_location==UP)
                            {
                                cassette_cmd = CASSETTE_START_FROM_UP_TO_DOWN;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);    
                                APP_TRACE("CASSETTE_START_FROM_UP_TO_DOWN!\r\n");                                
                            }
                            else
                            {
                                cassette_cmd = CASSETTE_START_FROM_UNKNOW_TO_DOWN;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);                                    
                                APP_TRACE("CASSETTE_START_FROM_UNKNOW_TO_DOWN!\r\n");   
                                
                            }  
                            /*cassette DOWN optical will be trigger*/
                            CASSETTE_DOWN_OPTICAL_FLAG=1;  
                        }
                        else
                        {
                            APP_TRACE("cassette_location is already DOWN!\r\n");
                        }
                        break;
                    case 0x03://ignore
                    
                        break;
                    default:
                        APP_TRACE("Unknow order_indicate.cassette_motor !\r\n");
                        break;
                }
				switch(control_order_r.order_indicate.bit.centrifugeo_or_electromagnet)
                {   
                    case 0x00://stop
                        stop_centrifuge_motor();
                        break;
                    
                    case 0x01://start
                        // start_centrifuge_motor();    
                        break;
                    case 0x02://init
                        init_centrifuge_motor(); 
                        break;
                    case 0x03://ignore
                    
                        break;
                    default:
                        APP_TRACE("Unknow order_indicate.centrifugeo !\r\n");
                        break;
                }                
                break;
                
            case 0x02://magnet
                switch(control_order_r.order_indicate.bit.centrifugeo_or_electromagnet)
                {   
                    case 0x00://unlock
                        OSFlagPost(magnet_flag,0x02,OS_FLAG_SET,&err);
                        assert_param(OS_ERR_NONE == err);
                        APP_TRACE("UNLOCK magnet!\r\n");
                        break;
                    
                    case 0x01://lock
                        OSFlagPost(magnet_flag,0x01,OS_FLAG_SET,&err);
                        assert_param(OS_ERR_NONE == err);
                        APP_TRACE("LOCK magnet!\r\n");
                        break;
                    case 0x02://init
                    
                        break;
                    case 0x03://ignore
                    
                        break;
                    default:
                        APP_TRACE("Unknow order_indicate.magnet !\r\n");
                        break;
                }
                break;
                
            default:
                APP_TRACE("Unknow module_indicate !\r\n");
                break;
        }      
/////////////


#if 0
		switch(pump_order.pump_or_valve)
		{
			case 0x01://motor
				switch(pump_order.pump.bit.centrifuge_motor)
				{
					case STOP:
                        stop_centrifuge_motor();
						break;
					case START:
                        // start_centrifuge_motor();    
						break;
					case INIT:
                        init_centrifuge_motor(); 
                        // TODO: init
						break;
					case IGNORE:
						break;
					default:
						APP_TRACE("Unknow centrifuge_motor Order!\r\n");
						break;
				}
				switch(pump_order.pump.bit.cassette_motor)
				{

					case UP:
                        APP_TRACE("cassette to up\r\n");
                        cassette_current_location = sensor_state_compare.sensor.bit.cassette_location;

                        if( cassette_current_location!= UP)
                        {
                            if(cassette_current_location==DOWN)
                            {
                                cassette_cmd = CASSETTE_START_FROM_DOWN_TO_UP;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);    
                                APP_TRACE("CASSETTE_START_FROM_DOWN_TO_UP!\r\n");                                
                            }
                            else
                            {
                                cassette_cmd = CASSETTE_START_FROM_UNKNOW_TO_UP;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);    
                                APP_TRACE("CASSETTE_START_FROM_UNKNOW_TO_UP!\r\n");                                
                            }  
                            /*cassette UP optical will be trigger*/
                            CASSETTE_UP_OPTICAL_FLAG=1;
                            
                        }
                        else
                        {
                            APP_TRACE("cassette_location is already UP!\r\n");
                        }
						break;
					case DOWN:
                        APP_TRACE("cassette to down\r\n");
                        cassette_current_location=sensor_state_compare.sensor.bit.cassette_location;

                        if(cassette_current_location != DOWN)
                        {
                            if(cassette_current_location==UP)
                            {
                                cassette_cmd = CASSETTE_START_FROM_UP_TO_DOWN;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);    
                                APP_TRACE("CASSETTE_START_FROM_UP_TO_DOWN!\r\n");                                
                            }
                            else
                            {
                                cassette_cmd = CASSETTE_START_FROM_UNKNOW_TO_DOWN;
                                err = OSQPost(cassette_control_event, (void*)cassette_cmd); 
                                assert_param(OS_ERR_NONE == err);                                    
                                APP_TRACE("CASSETTE_START_FROM_UNKNOW_TO_DOWN!\r\n");   
                                
                            }  
                            /*cassette DOWN optical will be trigger*/
                            CASSETTE_DOWN_OPTICAL_FLAG=1;  
                        }
                        else
                        {
                            APP_TRACE("cassette_location is already DOWN!\r\n");
                        }
						break;

					case IGNORE:
						break;
					default:
						APP_TRACE("Unknow cassette location Order!\r\n");
						break;
				}

				break;
			case 0x02://magnet
				switch(pump_order.valve.bit.magnet)
				{
					case UNLOCK:
                        OSFlagPost(magnet_flag,0x02,OS_FLAG_SET,&err);
                        assert_param(OS_ERR_NONE == err);
//                        APP_TRACE("UNLOCK magnet!\r\n");
						break;
					case LOCK:
                        
                         OSFlagPost(magnet_flag,0x01,OS_FLAG_SET,&err);
                         assert_param(OS_ERR_NONE == err);
//                         APP_TRACE("LOCK magnet!\r\n");
                         
						break;
					case INIT:

						break;
					case IGNORE:

						break;
					default:
						APP_TRACE("Unknow magnet Order!\r\n");
						break;
				}
				break;
			default:
				APP_TRACE("Unknow pump_or_valve !\r\n");
				break;
		}
#endif
		//OSTimeDlyHMSM(0, 0, 0, 50);
	}
}
#endif

