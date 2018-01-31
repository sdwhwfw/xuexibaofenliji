/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_blood_collect.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/18
 * Description        : This file contains the Blood priming & product collection
 *                      program.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/18 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_blood_collect.h"
#include "app_control_pump.h"
#include "app_cmd_send.h"
#include "app_control_common_function.h"
#include "app_control_speed_table.h"
#include "app_control_global_state.h"
#include "app_control_cali_pump.h"
#include "app_buzzer_ring.h"
#include "trace.h"

#define BLOOD_FULL_CENTRI_TIMEOUT              40*10 /* unit:100ms */
#define PRIMING_LOW_LEVEL_TIMEOUT              50*10 /* unit:100ms */
#define FIRST_LOW_LEVEL_TIMEOUT                35*10 /* unit:100ms */

uint8_t blood_priming(void);
uint8_t first_phase_ten_mins(void);
uint8_t second_phase_ten_mins(void);
uint8_t stable_blood_collect(void);

uint8_t blood_collect(void)
{
    UART_FRAME l_frame;
    INT8U err;

    APP_TRACE("enter <blood_collect>\r\n");
    
    /* 1. blood priming */
    OSSemPend(g_uart_start_collect_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    // set collect status
    set_collect_status(NO_COLLECT);////???????? why  to set the status,as a 
    // signal 
    // set_collect_status(PLT_COLLECT_BEGIN);

    
    // clear draw pump's total distance
    set_total_distance_event(CLEAR_DRAW_DIS);

    memset(&l_frame, 0x00, sizeof(l_frame));
    l_frame.module_flag = 0x30;
    l_frame.event_type = 0x01;////??????????????


   
    if (blood_priming())//血液灌注
    {
        l_frame.param1 = 0x01;
        uart_send_frame(l_frame);
        return 1;
    }
    
    l_frame.param1 = 0x00;
    uart_send_frame(l_frame);

    APP_TRACE("blood priming end\r\n");
    set_flow_pressure_status(FLOW_PRESSURE_NORMAL);


#if 1
    /* 2. Fisrt phase of ten-mins collect */
    if (first_phase_ten_mins())//前十分钟梯度变化
    {
        return 1;
    }

    APP_TRACE("first_phase_ten_mins end\r\n");

    /* 3. Second phase of ten-mins collect */
    if (second_phase_ten_mins())//
    {
        return 1;
    }

#endif

    APP_TRACE("second_phase_ten_mins end\r\n");

  //  set_collect_status(PLT_COLLECT_BEGIN);//add by wq for test

    /* 4. Stable phase of collect blood */
    if (stable_blood_collect())//稳定采血回输
    {
        return 1;
    }

    return 0;
    
    
}




uint8_t blood_priming(void)
{
    INT8U err;    
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;
    
    APP_TRACE("enter <blood_priming>\r\n");

#if 1
    
    set_run_status(STATUS_BLOOD_PRIMING);
    // set the flow's next wait event
  //  set_flow_wait_status(FLOW_WAIT_CENTRI_PRESSURE);



    // TODO: check the speed of centrifuge
    if (ctrl_start_centrifuge(CENTRI_SPEED_ONE*10))
    {
        return 1;
    }




    // start ac/draw pump
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    l_pump.ac_pump = 0x01;
    
    l_speed.draw_speed = 5217;
    l_speed.ac_speed = 1000;

    l_distance.ac_distance = 30;
    l_distance.draw_distance = 300;

    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// AC/draw  pumps start
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }



    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_SIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop draw pump
        return 1;
    }
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    
    l_pump.feedBack_dir = 0x01;//dir 顺时针???
    
    l_speed.ac_speed = 365;
   // l_speed.feedBack_speed = 1500;
    l_speed.feedBack_speed = 3500;//change by wq
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// AC/feedback pump turn,feedbck clockwise
        return 1;
    }


    // wait until low level is reach
    APP_TRACE("pending g_canopen_lower_liquid_event......\r\n");
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended  g_canopen_lower_liquid_event \r\n");
    
    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedBack pump
        return 1;
    }
#else
 
     set_run_status(STATUS_BLOOD_PRIMING);
     // set the flow's next wait event
     set_flow_wait_status(FLOW_WAIT_CENTRI_PRESSURE);

     // TODO: check the speed of centrifuge
     if (ctrl_start_centrifuge(CENTRI_SPEED_ONE*10))
     {
         return 1;
     }

     // start ac/draw pump
     init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
     l_pump.draw_pump = 0x01;
     l_pump.ac_pump = 0x01;
     l_speed.draw_speed = 5217;
     l_speed.ac_speed = 1000;

     
     if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
     {// AC/draw pump turn
         return 1;
     }


     // wait until blood is full of the centrifuge belt
     OSSemPend(g_canopen_centri_full_event, 0, &err);
     assert_param(OS_ERR_NONE == err);
     APP_TRACE("pend pressure == 100\r\n");



     set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_SIGNAL);

     init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
     l_pump.draw_pump = 0x01;
     if (ctrl_stop_pump(l_pump, l_count))
     {// stop draw pump
         return 1;
     }
     
     init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
     l_pump.ac_pump = 0x01;
     l_pump.feedBack_pump = 0x01;
     l_pump.feedBack_dir = 0x01;
     l_speed.ac_speed = 365;
    // l_speed.feedBack_speed = 1500;
     l_speed.feedBack_speed = 3500;//change by wq
     if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
     {// AC/feedback pump turn,feedbck clockwise
         return 1;
     }


     // wait until low level is reach
     APP_TRACE("pending g_canopen_lower_liquid_event......\r\n");
     OSSemPend(g_canopen_lower_liquid_event, 0, &err);
     assert_param(OS_ERR_NONE == err);
     APP_TRACE("pended  g_canopen_lower_liquid_event \r\n");
     
     set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);
     
     init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
     l_pump.feedBack_pump = 0x01;
     if (ctrl_stop_pump(l_pump, l_count))
     {// stop feedBack pump
         return 1;
     }




#endif

    APP_TRACE("exit  <blood_priming>\r\n");
    return 0;
}

uint8_t first_phase_ten_mins(void)
{
    INT8U err;
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;
    
    APP_TRACE("enter <first_phase_ten_mins>\r\n");
    
    set_run_status(STATUS_COLLECT_DRAW);

    /** 1.First Draw blood phase one **/
    APP_TRACE("1.First Draw blood phase 1\r\n");

    
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_MIDDLE;
    l_valve_dir.pla_valve = VALVE_DIR_MIDDLE;
    l_valve_dir.plt_valve = VALVE_DIR_MIDDLE;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// all valves turn to middle
        return 1;
    }
    
    OSTimeDlyHMSM(0, 0, 3, 0);

    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_RIGHT;
    l_valve_dir.pla_valve = VALVE_DIR_RIGHT;
    l_valve_dir.plt_valve = VALVE_DIR_RIGHT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// all valves turn to right
        return 1;
    }

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.ac_speed = 850;
    l_speed.draw_speed = 4500;
    l_speed.pla_speed = 570;
    l_speed.plt_speed = 3750;
    l_distance.ac_distance = 110;
    l_distance.draw_distance = 600;
    l_distance.pla_distance = 76;
    l_distance.plt_distance = 500;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// AC/draw/plt/pla pumps start
        return 1;
    }
    // TODO: cetrifuge speed up??
    if (ctrl_start_centrifuge(CENTRI_SPEED_TWO*10))
    {
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }

    /** 2.First Draw blood phase two **/
    APP_TRACE("2.First Draw blood phase 2\r\n");
    l_speed.ac_speed = 150;
    l_speed.draw_speed = 1600;
    l_speed.pla_speed = 231;
    l_speed.plt_speed = 1321;
    l_distance.ac_distance = 10;
    l_distance.draw_distance = 117;
    l_distance.pla_distance = 17;
    l_distance.plt_distance = 97;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// AC/draw/plt/pla pumps start
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop pla/plt pumps
        return 1;
    }
    
    /** 3.First Draw blood phase three **/
    APP_TRACE("3.First Draw blood phase 3\r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_speed.ac_speed = 400;
    l_speed.draw_speed = 4528;
    l_distance.ac_distance = 36;
    l_distance.draw_distance = 408;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// AC/draw pumps start
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }

    /** 4.First Draw blood phase four **/
    APP_TRACE("4.First Draw blood phase 4\r\n");
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.ac_speed = 528;
    l_speed.draw_speed = 6000;
    l_speed.pla_speed = 1724;
    l_speed.plt_speed = 1807;
    l_distance.ac_distance = 20;
    l_distance.draw_distance = 240;
    l_distance.pla_distance = 68;
    l_distance.plt_distance = 72;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// AC/draw/pla/plt pumps start
        return 1;
    }
    l_distance.ac_distance = 10;
    l_distance.draw_distance = 120;
    l_distance.pla_distance = 34;
    l_distance.plt_distance = 36;
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }
    // TODO: why plt valve turn to left for about 10s
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.plt_valve = 0x01;
    l_valve_dir.plt_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to left
        return 1;
    }

    l_distance.ac_distance = 20;
    l_distance.draw_distance = 240;
    l_distance.pla_distance = 68;
    l_distance.plt_distance = 72;
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }

    l_valve_dir.plt_valve = VALVE_DIR_RIGHT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to left
        return 1;
    }
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.plt_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop plt pump
        return 1;
    }

    /** 5.First Draw blood phase five **/
    APP_TRACE("5.First Draw blood phase 5\r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_speed.ac_speed = 528;
    l_speed.draw_speed = 6000;
    l_speed.feedBack_speed = 7000;// change by Big.Man
    l_speed.pla_speed = 1380;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);// add by Big.Man

    APP_TRACE("pending 111 g_canopen_upper_liquid_event......\r\n");
    OSSemSet (g_canopen_upper_liquid_event,0, &err);
    OSSemPend(g_canopen_upper_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended 111 g_canopen_upper_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);
    
    // beep prompt first back is start
    set_buzzer_ring_tone(BUZZER_RING_TONE, 0);    

    
    // TODO: Here cetrifuge speed up??
    if (ctrl_start_centrifuge(CENTRI_NORMAL_SPEED*10))
    {
        return 1;
    }

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop ac pump
        return 1;
    }
        
    set_run_status(STATUS_COLLECT_BACK);
    // clear feedback total distance
    set_total_distance_event(CLEAR_FEEDBACK_DIS);
    // get draw total distance
    set_total_distance_event(GET_DRAW_TOTAL_DIS);
    
    /** 6.First FeedBack blood **/
    APP_TRACE("6.First FeedBack blood\r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.draw_speed = 5240;
    
    //l_speed.feedBack_speed = 6000;
    l_speed.feedBack_speed = 7000;// change by Big.Man
    
    l_speed.pla_speed = 570;
    l_speed.plt_speed = 1200;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// draw/feedback/pla/plt pumps turn
        return 1;
    }

    
    APP_TRACE("pending 222 g_canopen_lower_liquid_event......\r\n");
    OSSemSet (g_canopen_lower_liquid_event,0, &err);
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    APP_TRACE("pended 222 g_canopen_lower_liquid_event\r\n");
    
    assert_param(OS_ERR_NONE == err);
    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
        return 1;
    }

    // clear total draw/feedback distance
    set_total_distance_event(GET_DRAW_BACK_TOTAL_DIS);
    
    set_run_status(STATUS_COLLECT_DRAW);

    /** 7.Second Draw blood **/
    APP_TRACE("7.Second Draw blood \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_speed.ac_speed = 458;
    l_speed.draw_speed = 5286;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    
    APP_TRACE("pending 333 g_canopen_upper_liquid_event......\r\n");
    OSSemSet (g_canopen_upper_liquid_event,0, &err);
    OSSemPend(g_canopen_upper_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended 333 g_canopen_upper_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop ac pump
        return 1;
    }

    set_total_distance_event(GET_DRAW_TOTAL_DIS);
    set_run_status(STATUS_COLLECT_BACK);
    
    /** 8.Second Feedback blood **/
    APP_TRACE("8.Second Feedback blood \r\n");
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_speed.draw_speed = 4316;
  // l_speed.feedBack_speed = 4724;
    l_speed.feedBack_speed = 6000;// change by Big.Man
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    APP_TRACE("pending 444 g_canopen_lower_liquid_event......\r\n");
    OSSemSet (g_canopen_lower_liquid_event,0, &err);
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended 444 g_canopen_lower_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
        return 1;
    }

    set_total_distance_event(GET_DRAW_BACK_TOTAL_DIS);
    
    APP_TRACE("exit  <first_phase_ten_mins>\r\n");
    
    return 0;
}

uint8_t second_phase_ten_mins(void)
{
    INT8U err;
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;
    FLOW_PUMP_SPEED l_flow_speed;

    APP_TRACE("enter <second_phase_ten_mins>\r\n");
    
    set_run_status(STATUS_COLLECT_DRAW);

    /** 1.Third Draw blood **/
    APP_TRACE("1.Third Draw blood \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    
   // l_flow_speed = get_flow_speed_table(0);//mask by Big.Man
    l_flow_speed.ac_speed = 458;
    l_flow_speed.draw_speed = 5286;
    
    l_flow_speed.pla_speed = 231;
    l_flow_speed.plt_speed = 1321;
    
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    
    APP_TRACE("pending 555 g_canopen_upper_liquid_event......\r\n");
    OSSemPend(g_canopen_upper_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended  555 g_canopen_upper_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop ac pump
        return 1;
    }

    set_total_distance_event(GET_DRAW_TOTAL_DIS);

    set_run_status(STATUS_COLLECT_BACK);
    
    /** 2.Third Feedback blood **/

    APP_TRACE("2.Third Feedback blood \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_flow_speed = get_flow_speed_table(1);
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    
    APP_TRACE("pending 666 g_canopen_lower_liquid_event......\r\n");
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended  666 g_canopen_lower_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
        return 1;
    }
    
    set_total_distance_event(GET_DRAW_BACK_TOTAL_DIS);

    set_run_status(STATUS_COLLECT_DRAW);

    /** 3.Fourth Draw blood **/
    APP_TRACE("3.Fourth Draw blood  \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    //l_flow_speed = get_flow_speed_table(2);
    
    l_flow_speed.ac_speed = 458;
    l_flow_speed.draw_speed = 5286;
    
    l_flow_speed.pla_speed = 231;
    l_flow_speed.plt_speed = 1321;

    
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    APP_TRACE("pending 777 g_canopen_upper_liquid_event......\r\n");
    OSSemPend(g_canopen_upper_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended  777 g_canopen_upper_liquid_event\r\n");

    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop ac pump
        return 1;
    }

    set_total_distance_event(GET_DRAW_TOTAL_DIS);

    set_run_status(STATUS_COLLECT_BACK);

    /** 4.Fourth Feedback blood **/
    
    APP_TRACE("4.Fourth Feedback blood  \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_flow_speed = get_flow_speed_table(3);
    
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    
    l_flow_speed.draw_speed = 5286;//
    
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    APP_TRACE("pending 888 g_canopen_lower_liquid_event......\r\n");
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended 888 g_canopen_lower_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
        return 1;
    }

    set_run_status(STATUS_COLLECT_DRAW);

    /** 5.Fifth Draw blood **/
    APP_TRACE(" 5.Fifth Draw blood  \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_flow_speed = get_flow_speed_table(4);

    l_flow_speed.ac_speed = 458;
    l_flow_speed.draw_speed = 5286;
    
    l_flow_speed.pla_speed = 231;
    l_flow_speed.plt_speed = 1321;
    
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    APP_TRACE("pending 999 g_canopen_upper_liquid_event......\r\n");
    OSSemPend(g_canopen_upper_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended 999 g_canopen_upper_liquid_event\r\n");

    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop ac pump
        return 1;
    }
    
    set_run_status(STATUS_COLLECT_BACK);

    /** 6.Fifth Feedback blood **/
    APP_TRACE(" 6.Fifth Feedback blood \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_flow_speed = get_flow_speed_table(5);

    l_flow_speed.draw_speed = 5286;//
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    APP_TRACE("pending aaa g_canopen_lower_liquid_event......\r\n");
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended aaa g_canopen_lower_liquid_event\r\n");
    
    set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
        return 1;
    }
    
    set_run_status(STATUS_COLLECT_DRAW);

    /** 7.Sixth-one Draw blood **/
    APP_TRACE(" 7.Sixth-one Draw blood \r\n");
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
   // l_flow_speed = get_flow_speed_table(6);
    
    l_flow_speed.ac_speed   = 658;
    l_flow_speed.draw_speed = 5286;
    
    l_flow_speed.pla_speed = 231;
    l_flow_speed.plt_speed = 1321;
    
    l_pump.ac_pump   = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.pla_pump  = 0x01;
    l_pump.plt_pump  = 0x01;
    
    memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
    l_distance.ac_distance = 28;
    l_distance.draw_distance = 318;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.plt_valve = 0x01;
    l_valve_dir.plt_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to left
        return 1;
    }

    // clear plt pump total distance
    memset(&l_pump, 0x00, sizeof(l_pump));
    l_pump.plt_pump = 0x01;
    clear_pump_total_distance(l_pump);

    APP_TRACE("exit <second_phase_ten_mins>\r\n");

    
    // First Ten minutes Process is End!
    set_collect_status(PLT_COLLECT_BEGIN);
    APP_TRACE("Begin to collect plt!\r\n");
    
    return 0;
}

uint8_t stable_blood_collect(void)
{
    INT8U err;
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;
    FLOW_PUMP_SPEED l_flow_speed;
    
    APP_TRACE("enter <stable_blood_collect>\r\n");
    
    for (;;)
    {
        /* Draw flow */
        set_run_status(STATUS_COLLECT_DRAW);
        
        init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
        //l_flow_speed = get_flow_speed_table(7);

        l_flow_speed.ac_speed = 658;
        l_flow_speed.draw_speed = 5286;
        
        l_flow_speed.pla_speed = 231;
        l_flow_speed.plt_speed = 1321;
        
        l_pump.ac_pump = 0x01;
        l_pump.draw_pump = 0x01;
        l_pump.pla_pump = 0x01;
        l_pump.plt_pump = 0x01;

        g_total_distance_return.plt_distance += g_total_distance_calc.plt_distance;
                                                      ////return the total 
                                                      // distance
        
        memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
        if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
        {
            return 1;
        }
        
        APP_TRACE("pending g_canopen_upper_liquid_event......\r\n");
        OSSemPend(g_canopen_upper_liquid_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
        APP_TRACE("pended g_canopen_upper_liquid_event\r\n");
        
        set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

        init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
        l_pump.ac_pump = 0x01;
        if (ctrl_stop_pump(l_pump, l_count))
        {// stop ac pump
            return 1;
        }
        
        /* FeedBack flow */
        set_run_status(STATUS_COLLECT_BACK);

        init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
       
        l_flow_speed = get_flow_speed_table(8);

        l_flow_speed.draw_speed = 1600;
        
        l_pump.draw_pump = 0x01;
        l_pump.feedBack_pump = 0x01;
        l_pump.pla_pump = 0x01;
        l_pump.plt_pump = 0x01;



        g_total_distance_return.plt_distance += 
        g_total_distance_calc.plt_distance;////the tatal distance has been 
        // clear in the ground floor

        
        memcpy(&l_speed, &l_flow_speed, sizeof(l_flow_speed));
        if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
        {
            return 1;
        }

        APP_TRACE("pending g_canopen_lower_liquid_event......\r\n");
        OSSemPend(g_canopen_lower_liquid_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
        APP_TRACE("pended g_canopen_lower_liquid_event\r\n");
        
        set_flow_wait_status(FLOW_WAIT_UPPER_LEVEL_SIGNAL);

        init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
        l_pump.feedBack_pump = 0x01;
        if (ctrl_stop_pump(l_pump, l_count))
        {// stop feedback pump
            return 1;
        }
    }
//    return 0;
}

