/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_set_canal.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/14
 * Description        : This file contains the load suit & remove air 
 *                      & canal test & AC priming
 *                      program.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/14 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_set_canal.h"
#include "app_cmd_send.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"
#include "trace.h"
#include "app_trouble_shooting.h"


static uint8_t load_canal(void);
static uint8_t remove_air(void);
static uint8_t test_canal(void);
static uint8_t ac_priming(void);

OS_TMR* tmr_test_cancal_protect;



void tmr_test_cancal_protect_callback(OS_TMR *ptmr, void *p_arg)
{
    u8 tmr_err = 0;

    tmr_err = OSSemPost(sem_test_cancel_protect);/*test cancel protect event*/
    assert_param(tmr_err == OS_ERR_NONE);
//    APP_TRACE(...);
	TRACE(" tmr_test_cancal_protect_callback timeout!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    OSTmrStop(tmr_test_cancal_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);

}






uint8_t set_canal(void)
{
    UART_FRAME l_frame;
    INT8U err;

    g_test_canal_pressure_event = OSSemCreate(0);
    assert_param(g_test_canal_pressure_event);

    set_test_canal_pressure_status(TEST_CANAL_WAIT_NONE);

    /* 1. recv begin suit load event */
    
    APP_TRACE("pending g_uart_suit_  load cancal event!!\r\n");
    OSSemPend(g_uart_suit_load_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended g_uart_suit_  load cancal event!!\r\n");
    
    memset(&l_frame, 0x00, sizeof(l_frame));
    l_frame.module_flag = 0x20;
    l_frame.event_type = 0x01;
    
    if (load_canal())//装载管路+排空气
    {// load canal fail
        l_frame.param1 = 0x01;
        uart_send_frame(l_frame);
        APP_TRACE("load_canal faild\r\n");
        return 1;
    }
    
    APP_TRACE("<load_canal> over \r\n");
    
    uart_send_frame(l_frame); // load canal ok
    
    /* 2. recv beign suit test event */
    OSSemPend(g_uart_begin_suit_test_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    
    l_frame.event_type = 0x02;
	
    if (test_canal())//测试管路
    {// test canal fail
        //l_frame.param1 = 0x01;
        //uart_send_frame(l_frame);
        APP_TRACE("test_canal fail\r\n");
       // return 1;
    }
	
    uart_send_frame(l_frame); // test canal ok

    /* 3. recv ac priming event */
    APP_TRACE("waiting ac_priming  \r\n");
    OSSemPend(g_uart_ac_priming_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    l_frame.event_type = 0x03;
    if (ac_priming())//灌注抗凝剂
    {// ac priming fail
        l_frame.param1 = 0x01;
        uart_send_frame(l_frame);
        APP_TRACE("ac priming fail\r\n");
        return 1;
    }
    uart_send_frame(l_frame); // ac priming ok
    
    return 0;
}

uint8_t load_canal(void)
{// TODO: unsure

    APP_TRACE("enter <load_canal>\r\n");
    
    /* mask by wq 
    if (ctrl_door_lock(TYPE_DOOR_LOCK))
    {// lock the cetrifuge door
        return 1;
    }
    */
    if (ctrl_load_canal())//装载管路
    {// load canal
        return 1;
    }
    
    if (remove_air())//排空气
    {// remove air
        return 1;
    }
    return 0;
}

uint8_t remove_air()
{// TODO: what's the process?
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_dir;
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    APP_TRACE("enter <remove_air>\r\n");

    init_valve_param(&l_valve, &l_dir);
    
    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_dir.rbc_valve = VALVE_DIR_RIGHT;
    l_dir.pla_valve = VALVE_DIR_MIDDLE;
    l_dir.plt_valve = VALVE_DIR_MIDDLE;
    
    if (ctrl_turn_valve(l_valve, l_dir))
    {
        return 1;
    }

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_speed.feedBack_speed = 3330;
   // l_distance.feedBack_distance = 10*DISTANCE_REMOVE_AIR;
   // l_distance.feedBack_distance = 20*DISTANCE_REMOVE_AIR;//need to modify to 
   // 40圈!!!!!!!!!!!!!!!!精度0.1圈   
   l_distance.feedBack_distance = 400;
    
    APP_TRACE("remove_air  feedBack_distance = %d \n",  l_distance.feedBack_distance);
    
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start feedback pump
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {// wait pump turn ok
        return 1;
    }
    APP_TRACE("remove_air wart pump end\r\n");
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop pump
        return 1;
    }
    
    APP_TRACE("<remove_air> over \r\n");
    
    return 0;
}

uint8_t test_canal()
{// TODO: check the pressure value

    #if  1
    INT8U err,tmr_err = 0,i;
    
    CTRL_PUMP_TYPE     l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED    l_speed;
    CTRL_PUMP_COUNT    l_count;
    CTRL_VALVE_TYPE    l_valve;
    CTRL_VALVE_DIR     l_dir;
    
    tmr_test_cancal_protect  = OSTmrCreate(100,//80*100ms = 8s
                                             100,
                                             OS_TMR_OPT_PERIODIC,
                                             (OS_TMR_CALLBACK)tmr_test_cancal_protect_callback,
                                             (void*)0,"TMR_TMOEOUT_TEST_CANCAL_PROTECT",
                                             &err);

    APP_TRACE("test_canal err = %d\r\n",err);
	assert_param(tmr_test_cancal_protect);


    APP_TRACE("enter <test_canal>\r\n");


    init_valve_param(&l_valve, &l_dir);
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_dir.pla_valve = VALVE_DIR_RIGHT;
    l_dir.plt_valve = VALVE_DIR_RIGHT;
    if (ctrl_turn_valve(l_valve, l_dir))
    {
        return 1;
    }
    
    /**************310*********************/
    set_test_canal_pressure_status(TEST_CANAL_WAIT_HIGH_PRESSURE);//高压力310
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_speed.ac_speed = 19600;
    
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start ac pump
        return 1;
    }

    OSTmr_SetDly(tmr_test_cancal_protect,300,&tmr_err);//300*100ms= 30s  
    OSTmrStart(tmr_test_cancal_protect,&tmr_err);  
    
    APP_TRACE("OSTmrStart tmr_err   = %d OSLockNesting = %d\r\n",tmr_err,OSLockNesting);
    
    OSSemSet(g_test_canal_pressure_event,0,&err);

	
    if(TEST_CANAL_WAIT_HIGH_PRESSURE == get_test_canal_pressure_status())
        {

            APP_TRACE("Pending 1 TEST_CANAL_WAIT_HIGH_PRESSURE +310  OSLockNesting = %d \r\n",OSLockNesting);
            OSSemPend(g_test_canal_pressure_event, 10000, &err);//from 0 => 10s
            APP_TRACE("g_test_canal_pressure_event err = %d\r\n",err);
            
           // assert_param(OS_ERR_NONE == err);
            APP_TRACE("Pended 1 TEST_CANAL_WAIT_HIGH_PRESSURE +310\r\n");
            
            OSTmrStop(tmr_test_cancal_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
            if (ctrl_stop_pump(l_pump, l_count))///////?????????????????spare
            {// stop ac pump
                return 1;
            }
            
        }
    OSTimeDlyHMSM(0, 0, 2, 0);


#if 0
    /***************200**********************/
    set_test_canal_pressure_status(TEST_CANAL_WAIT_FIRST_PRESSURE);//200
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    
    l_pump.feedBack_dir = 0x01;//dir 顺时针

    
    l_speed.ac_speed = 1500;
    l_speed.draw_speed = 1500;
    l_speed.feedBack_speed = 1500;



    OSTmr_SetDly(tmr_test_cancal_protect,300,&tmr_err);//300*100ms= 30s
    OSTmrStart(tmr_test_cancal_protect,&tmr_err);  
    
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start ac/draw/feedback pump
        return 1;
    }
    OSSemSet(g_test_canal_pressure_event,0,&err);
    if(TEST_CANAL_WAIT_FIRST_PRESSURE == get_test_canal_pressure_status())
        {
            APP_TRACE("Pending 2 TEST_CANAL_WAIT_FIRST_PRESSURE +200\r\n");
            OSSemPend(g_test_canal_pressure_event, 0, &err);
            assert_param(OS_ERR_NONE == err);
            APP_TRACE("Pended 2 TEST_CANAL_WAIT_FIRST_PRESSURE +200\r\n");
            
            OSTmrStop(tmr_test_cancal_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
            if (ctrl_stop_pump(l_pump, l_count))
            {// stop ac/draw/feedback pump
                return 1;
            }
        }
    /***************100***********************/
    OSTimeDlyHMSM(0, 0, 2, 0);
    
    set_test_canal_pressure_status(TEST_CANAL_WAIT_SECOND_PRESSURE);//100

    OSTmr_SetDly(tmr_test_cancal_protect,30,&tmr_err);//30*100ms= 3s
    OSTmrStart(tmr_test_cancal_protect,&tmr_err);  
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start ac/draw/feedback pump
        return 1;
    }
    OSSemSet(g_test_canal_pressure_event,0,&err);
    if(TEST_CANAL_WAIT_SECOND_PRESSURE == get_test_canal_pressure_status())
        {
            APP_TRACE("Pending 3 TEST_CANAL_WAIT_SECOND_PRESSURE 110\r\n");
            OSSemPend(g_test_canal_pressure_event, 0, &err);
            assert_param(OS_ERR_NONE == err);
            APP_TRACE("Pended 3 TEST_CANAL_WAIT_SECOND_PRESSURE 110\r\n");

            OSTmrStop(tmr_test_cancal_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
            if (ctrl_stop_pump(l_pump, l_count))
            {// stop ac/draw/feedback pump
                return 1;
            }
        }

    /******************-250*******************/
    set_test_canal_pressure_status(TEST_CANAL_WAIT_LOW_PRESSURE);//-250
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    l_speed.draw_speed = 16700;

    OSTmr_SetDly(tmr_test_cancal_protect,250,&tmr_err);//250*100ms= 25s
    OSTmrStart(tmr_test_cancal_protect,&tmr_err);      
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start draw pump
        return 1;
    }
    OSSemSet(g_test_canal_pressure_event,0,&err);
    if(TEST_CANAL_WAIT_LOW_PRESSURE == get_test_canal_pressure_status())
        {
            APP_TRACE("Pending 4 TEST_CANAL_WAIT_LOW_PRESSURE -250\r\n");
            OSSemPend(g_test_canal_pressure_event, 0, &err);
            assert_param(OS_ERR_NONE == err);
            APP_TRACE("Pended 4 TEST_CANAL_WAIT_LOW_PRESSURE -250\r\n");

            OSTmrStop(tmr_test_cancal_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
            if (ctrl_stop_pump(l_pump, l_count))
            {// stop draw pump
                return 1;
            }
        }

    /*******************0******************/
    set_test_canal_pressure_status(TEST_CANAL_WAIT_NORMAL_PRESSURE);//0
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_speed.ac_speed = 18700;
    l_distance.ac_distance = 280;

    OSTmr_SetDly(tmr_test_cancal_protect,100,&tmr_err);//100*100ms= 10s
    OSTmrStart(tmr_test_cancal_protect,&tmr_err);    
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start ac pump
        return 1;
    }
    OSSemSet(g_test_canal_pressure_event,0,&err);
    if(TEST_CANAL_WAIT_NORMAL_PRESSURE == get_test_canal_pressure_status())
        {
            APP_TRACE("Pending 5 TEST_CANAL_WAIT_NORMAL_PRESSURE 0\r\n");
            OSSemPend(g_test_canal_pressure_event, 0, &err);
            assert_param(OS_ERR_NONE == err);
            APP_TRACE("Pended 5 TEST_CANAL_WAIT_NORMAL_PRESSURE 0\r\n");

            OSTmrStop(tmr_test_cancal_protect,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
            if (ctrl_stop_pump(l_pump, l_count))
            {// stop ac pump
                return 1;
            }
        }

#endif
    APP_TRACE("test_canal() over \r\n");

#endif

    return 0;
}

uint8_t ac_priming(void)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    APP_TRACE("enter <ac_priming> \r\n");
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    clear_pump_total_distance(l_pump);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_speed.ac_speed = 400;//from 6000=>4000
    l_speed.draw_speed = 400;
    
    l_distance.ac_distance = 243;
    
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start ac/draw pump
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {// wait ac/draw pump run end
        return 1;
    }
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop ac/draw pump
        return 1;
    }

    // TODO: check the distance is ok?
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_speed.feedBack_speed = 300;//from 2100 =>800
    l_distance.feedBack_distance = 3;//from 17 => 3
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start feedback pump
       return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {// wait feedback pump run end
        return 1;
    }
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
       return 1;
    }
    
    APP_TRACE(" <ac_priming> over\r\n");
    return 0;
}


