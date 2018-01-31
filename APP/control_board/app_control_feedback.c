/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_feedback.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/29
 * Description        : This file contains the feedback program when collect end.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/29 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_feedback.h"
#include "app_control_board.h"
#include "app_control_monitor.h"
#include "app_cmd_send.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"
#include "app_buzzer_ring.h"
#include "trace.h"

static OS_STK control_feedback_task_stk[CONTROL_FEEDBACK_TASK_STK_SIZE];

// no plasma assist to feedback
static uint8_t feedback_direct(void);
// plasma assist to feedback---<incomplete>
static uint8_t feedback_with_pla(void);
// unload canal
//static uint8_t unload_canal(void);
uint8_t unload_canal(void);

// get program result
static void get_final_result(void);


// commit the result of the program
static uint8_t commit_result(void);

void control_feedback_task(void *p_arg);

void control_init_feedback_task(void)
{
    INT8U os_err;
    /* feedback task *****************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_feedback_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_feedback_task_stk[CONTROL_FEEDBACK_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_FEEDBACK_TASK_PRIO,
							(INT16U 		 ) CONTROL_FEEDBACK_TASK_PRIO,
							(OS_STK 	   * )&control_feedback_task_stk[0],
							(INT32U 		 ) CONTROL_FEEDBACK_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CONTROL_FEEDBACK_TASK_PRIO, (INT8U *)"feedback", &os_err);

    g_feedback_event = OSSemCreate(0);
    assert_param(g_feedback_event);

}

void control_feedback_task(void *p_arg)
{
    INT8U err;
    u8 l_ret;
    UART_FRAME l_frame;

    APP_TRACE("Enter control_feedback_task...\r\n");

    OSSemPend(g_feedback_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("OSSemPended g_feedback_event \r\n");
    
    set_collect_status(PLT_COLLECT_END);

    /* 1. uart send collect over */
    memset(&l_frame, 0x00,sizeof(l_frame));    
    l_frame.module_flag = 0x50;
    l_frame.event_type = 0x01;
    l_frame.param1 = 0x01;
    
    uart_send_frame(l_frame);

    /* 2. feedback over */
    l_frame.module_flag = 0x50;
    l_frame.event_type = 0x02;
    if (!g_cur_program_param.bool_back_switch)//////»¹ÐèÒªÉÏÎ»µÄÃüÁîå
    {
        l_ret = feedback_direct();
    }
    else
    {
        l_ret = feedback_with_pla();
    }
    l_frame.param1 = l_ret;
    uart_send_frame(l_frame);

    APP_TRACE("FeedBack over!\r\n");

    /* 3. unload canal */
    OSSemPend(g_uart_unload_event, 0, &err);
    assert_param(OS_ERR_NONE == err); 
    l_frame.module_flag = 0x50;
    l_frame.event_type = 0x03;
    l_frame.param1 = unload_canal();//Ð¶ÔØ¿¨Ï»
    uart_send_frame(l_frame);

    /* 3. show final result */
    OSSemPend(g_uart_show_result, 0, &err);
    assert_param(OS_ERR_NONE == err);
    commit_result();
}

static uint8_t feedback_direct(void)
{
    INT8U err;
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    APP_TRACE("Enter feedback_direct...\r\n");
    
    os_pause_flow();
    OSTaskSuspend(CONTROL_ADJUST_TASK_PRIO);
    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

    set_buzzer_ring_tone(BUZZER_RING_TONE, 0);

    /* 1. ac pump stop */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {
        return 1;
    }
    
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_RIGHT;
    l_valve_dir.pla_valve = VALVE_DIR_RIGHT;
    l_valve_dir.plt_valve = VALVE_DIR_RIGHT;    
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {
        return 1;
    }
#if 0
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_speed.feedBack_speed = 2333;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    
    // TODO: how to set the waitout time ?, is this stop condition right??
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
#endif

    /* 2. draw/feedback/plt/pla pump start */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.draw_speed = 12000;
    l_speed.feedBack_speed = 4875;
    l_speed.pla_speed = 9775;
    l_speed.plt_speed = 10598;
    l_distance.draw_distance = 680;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start draw/feedback/pla/plt pumps
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }

    /* 3. draw pump stop */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.draw_pump = 0x01;
    l_count.draw_count = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop draw pump
        return 1;
    }
    if (ctrl_stop_centrifuge())
    {// stop centrifuge
        return 1;
    }

    // TODO: think of the low level is nosignal
    /* 4. plt/pla/feedback pump turn until low level triggle */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump= 0x01;
    l_speed.feedBack_speed = 2250;
    l_distance.feedBack_distance = 337;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start feedback pump
        return 1;
    }
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.plt_pump = 0x01;
    l_speed.plt_speed = 10598;
    l_distance.plt_distance = 1060;
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {// wait about 1min
        return 1;
    }

    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.rbc_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {
        return 1;
    }

    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    /* 5. feedback stop,pla/plt turn until centri pressure triggle */
    set_flow_wait_status(FLOW_WAIT_BELT_EMPTY);
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {// stop feedback pump
        return 1;
    }
    
    OSSemPend(g_canopen_centri_empty_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    /* 6. feedback turn */    
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_speed.feedBack_speed = 2250;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {// start feedback pump
        return 1;
    }
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    /* 7. program over, stop all pumps */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {
        return 1;
    }

    /* save final result */
    get_final_result();
    
    return 0;
}

static uint8_t feedback_with_pla(void)
{
    INT8U err;
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    APP_TRACE("Enter feedback_with_pla...");

    os_pause_flow();
    set_flow_wait_status(FLOW_WAIT_LOWER_LEVEL_NOSIGNAL);

    // TODO: beep prompt
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_RIGHT;
    l_valve_dir.pla_valve = VALVE_DIR_RIGHT;
    l_valve_dir.plt_valve = VALVE_DIR_RIGHT;    
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {
       return 1;
    }

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.feedBack_speed = 2100;
    l_speed.plt_speed = 2400;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    
    // TODO: how to set the waitout time 
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.rbc_valve = 0x01;///??????????
    l_valve_dir.rbc_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {
       return 1;
    }
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.feedBack_speed = 3231;
    l_speed.pla_speed = 4468;
    l_speed.plt_speed = 7021;
    l_distance.feedBack_distance = 200;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    if (wait_pump_end(l_pump, l_speed, l_distance))
    {
        return 1;
    }
    
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_speed.feedBack_speed = 2667;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    // TODO: need to check
    OSSemPend(g_canopen_lower_liquid_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_RIGHT;
    l_valve_dir.pla_valve = VALVE_DIR_MIDDLE;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {
       return 1;
    }
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {
        return 1;
    }
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.plt_pump = 0x01;
    l_speed.plt_speed = 11727;
    
    // TODO: flow to check unsure now, incomplete
    
    return 0;
}

//static uint8_t unload_canal(void) //mask by wq

uint8_t unload_canal(void)
{
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    
    /*1. unload canal */
    /*
    if (ctrl_unload_canal())
    {
        return 1;
    }
*/
    /* 2. all valves turn to middle */
    l_valve.rbc_valve = 0x01;
    l_valve.pla_valve = 0x01;
    l_valve.plt_valve = 0x01;
    l_valve_dir.rbc_valve = VALVE_DIR_MIDDLE;
    l_valve_dir.pla_valve = VALVE_DIR_MIDDLE;
    l_valve_dir.plt_valve = VALVE_DIR_MIDDLE;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {
        return 1;
    }
    
    /*1. unload canal */
    if (ctrl_unload_canal())
    {
        return 1;
    }
    

    /* 3. unlock the centrifuge lock */
    if (ctrl_door_lock(TYPE_DOOR_UNLOCK))
    {
        return 1;
    }
    
    return 0;
}

static uint8_t commit_result(void)
{ 
    UART_FRAME l_frame;
   
    memset(&l_frame, 0x00, sizeof(l_frame));
        
    l_frame.module_flag = 0x51;
    l_frame.event_type = 0x03;
    l_frame.param1 = g_uart_final_back.ac_volume & 0x00FF;
    l_frame.param2 = (g_uart_final_back.ac_volume & 0xFF00) >> 8;
    uart_send_frame(l_frame);

    l_frame.module_flag = 0x51;
    l_frame.event_type = 0x04;
    l_frame.param1 = g_uart_final_back.draw_volume & 0x00FF;
    l_frame.param2 = (g_uart_final_back.draw_volume & 0xFF00) >> 8;
    uart_send_frame(l_frame);

    l_frame.module_flag = 0x51;
    l_frame.event_type = 0x05;
    l_frame.param1 = g_uart_final_back.plt_volume & 0x00FF;
    l_frame.param2 = (g_uart_final_back.plt_volume & 0xFF00) >> 8;
    uart_send_frame(l_frame);
    
    return 0;
}

static void get_final_result(void)
{
    CTRL_PUMP_TYPE l_type;
    CTRL_PUMP_DISTANCE l_distance;

    memset(&l_type, 0x00, sizeof(l_type));
    memset(&l_distance, 0x00, sizeof(l_distance));
    
    l_type.ac_pump = 0x01;
    l_type.plt_pump = 0x01;
    l_type.draw_pump = 0x01;
    
    l_distance = get_pump_total_distance(l_type);
    g_uart_final_back.ac_volume = l_distance.ac_distance/10*g_ratio_parse.ratio_ac;
    g_uart_final_back.plt_volume = g_uart_cycle_back.plt_volume;
    g_uart_final_back.draw_volume = l_distance.plt_distance/10*g_ratio_parse.ratio_draw;
}

