/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_adjust_implement.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/23
 * Description        : This file contains the task for implement of adjust from PC
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/23 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "stm32f2xx.h"
#include "app_control_adjust_implement.h"
#include "app_control_board.h"
#include "app_control_speed_table.h"
#include "app_control_monitor.h"
#include "app_control_pump.h"
#include "app_cmd_send.h"
#include "app_control_global_state.h"
#include "app_control_common_function.h"
#include "trace.h"

static OS_EVENT*        g_adjust_event;
static void*            g_adjust_QueueTbl[ADJUST_QUEUE_SIZE];

static OS_STK control_adjust_task_stk[CONTROL_ADJUST_TASK_STK_SIZE];

static u8 param_update_func(ADJUST_EVENT event);
static u8 over_flow_func(void);
static u8 air_remove_func(void);
static u8 pour_saline_func(void);
static u8 resume_ACBlood_func(void);
static u8 air_remove_end_func(void);
static u8 over_flow_end_func(void);
static u8 low_pressure_end_func(void);
static u8 high_pressure_end_func(void);

u8 adjust_draw_speed(ADJUST_EVENT event);
u8 adjust_feedback_speed(ADJUST_EVENT event);
u8 adjust_ac_speed(ADJUST_EVENT event);
u8 adjust_agglu_rate(ADJUST_EVENT event);

u8 adjust_pump_speed(ADJUST_EVENT event, FLOW_PUMP_SPEED speed);


void control_adjust_implement_task(void *p_arg);

void control_init_adust_implement_task(void)
{
    INT8U os_err;
    /* adjust implement task *****************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_adjust_implement_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_adjust_task_stk[CONTROL_ADJUST_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_ADJUST_TASK_PRIO,
							(INT16U 		 ) CONTROL_ADJUST_TASK_PRIO,
							(OS_STK 	   * )&control_adjust_task_stk[0],
							(INT32U 		 ) CONTROL_ADJUST_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CONTROL_ADJUST_TASK_PRIO, (INT8U *)"adjust", &os_err);

    g_adjust_event = OSQCreate(g_adjust_QueueTbl, ADJUST_QUEUE_SIZE);
    assert_param(g_adjust_event);
}

void control_adjust_implement_task(void *p_arg)
{
    INT8U err;
    void *l_msg;
    ADJUST_EVENT l_event;
    UART_FRAME l_frame;

    memset(&l_frame, 0x00, sizeof(l_frame));
    
    for(;;)
    {
        l_msg = OSQPend(g_adjust_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
        
        l_event = (ADJUST_EVENT)((u32)l_msg);
        
        switch (l_event)
        {
            case DRAW_SPEED_UP_EVENT:
            case DRAW_SPEED_DOWN_EVENT:
            case BACK_SPEED_UP_EVENT:
            case BACK_SPEED_DOWN_EVENT:
            case AC_SPEED_UP_EVENT:
            case AC_SPEED_DOWN_EVENT:
            case AGGLUTINATE_UP_EVENT:
            case AGGLUTINATE_DOWN_EVENT:
            {
                l_frame.module_flag = 0x40;
                l_frame.event_type = 0x01;
                l_frame.param1 = param_update_func(l_event);
                uart_send_frame(l_frame);
                break;
            }
            case OVER_FLOW_EVENT:
            {
                l_frame.module_flag = 0x40;
                l_frame.event_type = 0x02;
                l_frame.param1 = 0x01;
                l_frame.param2 = over_flow_func();
                uart_send_frame(l_frame);
                break;
            }
            case AIR_REMOVE_EVENT:
            {
                l_frame.module_flag = 0x40;
                l_frame.event_type = 0x03;
                l_frame.param1 = 0x01;
                l_frame.param2 = air_remove_func();
                uart_send_frame(l_frame);
                break;
            }
            case POUR_SALINE_EVENT:
            {
                pour_saline_func();///??????????????????
                break;
            }
            case RESUME_FEEDBACK_EVENT:
            {
                resume_ACBlood_func();
                break;
            }
            case AIR_REMOVE_END_EVENT:
            {
                l_frame.module_flag = 0x40;
                l_frame.event_type = 0x03;
                l_frame.param1 = 0x02;
                l_frame.param2 = air_remove_end_func();
                uart_send_frame(l_frame);
                break;
            }
            case OVER_FLOW_END_EVENT:
            {
                l_frame.module_flag = 0x40;
                l_frame.event_type = 0x02;
                l_frame.param1 = 0x02;//  overflow  end
                l_frame.param2 = over_flow_end_func();
                uart_send_frame(l_frame);
                break;
            }
            case PRESSURE_LOW_END_EVENT:
            {
                low_pressure_end_func();////response_low_pressure()  deal with 
                // the low pressure   
                break;
            }
            case PRESSURE_HIGH_END_EVENT:
            {
                high_pressure_end_func();
                break;
            }
            default:
                break;
        }
    }
}

void set_adjust_event(ADJUST_EVENT event)
{
    INT8U err;

    if (PRESSURE_LOW_END_EVENT == event || PRESSURE_HIGH_END_EVENT == event)
    {
        set_flow_pressure_status(FLOW_PRESSURE_BEGIN_RESUME);
    }

    if (STATUS_COLLECT_DRAW != g_run_status && STATUS_COLLECT_BACK != g_run_status)
        return;
    
    err = OSQPost(g_adjust_event, (void *)event);
    assert_param(OS_ERR_NONE == err);
}


static u8 param_update_func(ADJUST_EVENT event)
{
    static u8 s_draw_count=0, s_back_count=0, s_ac_count=0, s_agglu_count=0;//?
    u8 l_ret = 0;//s_draw_count=0ÓÉ×Ô¼ºÀ´Éè¶¨Âðå
    
    APP_TRACE("Enter param_update_func...\r\n");

    if (0xFF == get_phase_status())////when adjust in the first phase 
    {// first phase of ten-mins
        APP_TRACE("adjust is happenning in first phase of ten-mins...\r\n");
        return 1;
    }

    switch (event)
    {
        case DRAW_SPEED_UP_EVENT:
        {
            if (!s_draw_count)//// 
            {// can't up because not down
                return 1;
            }
            else
            {
                l_ret = adjust_draw_speed(DRAW_SPEED_UP_EVENT);
                if (!l_ret)
                    s_draw_count--;
                else
                    return 1;
            }
            break;
        }
        case DRAW_SPEED_DOWN_EVENT:
        {
            s_draw_count++;
            adjust_draw_speed(DRAW_SPEED_DOWN_EVENT);
            break;
        }
        case BACK_SPEED_UP_EVENT:
        {
            if (!s_back_count)
            {// can't up because not down
                return 1;
            }
            else
            {
                l_ret = adjust_feedback_speed(BACK_SPEED_UP_EVENT);
                if (!l_ret)
                    s_back_count--;
                else
                    return 1;
            }
            break;
        }
        case BACK_SPEED_DOWN_EVENT:
        {
            s_back_count++;
            adjust_feedback_speed(BACK_SPEED_DOWN_EVENT);
            break;
        }
        case AC_SPEED_UP_EVENT:
        {
            if (!s_ac_count)
            {// can't up because not down
                return 1;
            }
            else
            {
                l_ret = adjust_ac_speed(AC_SPEED_UP_EVENT);
                if (!l_ret)
                    s_ac_count--;
                else
                    return 1;
            }
            break;
        }
        case AC_SPEED_DOWN_EVENT:
        {
            s_ac_count++;
            adjust_ac_speed(AC_SPEED_DOWN_EVENT);
            break;
        }
        case AGGLUTINATE_UP_EVENT:
        {
            if (!s_agglu_count)
            {// can't up because not down
                return 1;
            }
            else
            {
                l_ret = adjust_agglu_rate(AGGLUTINATE_UP_EVENT);
                 if (!l_ret)
                    s_agglu_count--;
                else
                    return 1;
            }
            break;
        }
        case AGGLUTINATE_DOWN_EVENT:
        {
            s_agglu_count++;
            adjust_agglu_rate(AGGLUTINATE_DOWN_EVENT);
            break;
        }
    }
    return 0;
}

static u8 over_flow_func(void)
{
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    
    APP_TRACE("Enter over_flow_func...\r\n");
    if (PLT_COLLECT_BEGIN == get_collect_status())
    {
        // Suspend timing back task
        OSTaskSuspend(CONTROL_TIMING_BACK_TASK_PRIO);
        // pause plt's total distance
        pause_plt_total_dis();
    }
    

    if (STATUS_COLLECT_DRAW!=g_run_status && STATUS_COLLECT_BACK!=g_run_status)
    {
        return 1;
    }

    set_overflow_status(OVERFLOW_ON_HANDLE);////??????????
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.plt_valve = 0x01;
    l_valve_dir.plt_valve = VALVE_DIR_RIGHT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to right
        return 1;
    }

    return 0;
}

static u8 over_flow_end_func(void)
{
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;

    APP_TRACE("Enter over_flow_end_func...\r\n");

    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.plt_valve = 0x01;
    l_valve_dir.plt_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to left
        return 1;
    }

    if (PLT_COLLECT_BEGIN == get_collect_status())
    {
        // resume timing back task
        OSTaskResume(CONTROL_TIMING_BACK_TASK_PRIO);
        // resume plt's total distance
        resume_plt_total_dis();
    }
    
    return 0;
}


static u8 air_remove_func(void)
{
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;
    
    APP_TRACE("Enter air_remove_func...\r\n");

    if (PLT_COLLECT_BEGIN == get_collect_status())
    {
        // Suspend timing back task
        OSTaskSuspend(CONTROL_TIMING_BACK_TASK_PRIO);
        // pause plt's total distance
        pause_plt_total_dis();
    }

    if (STATUS_COLLECT_DRAW!=g_run_status && STATUS_COLLECT_BACK!=g_run_status)
    {
        return 1;
    }

    set_overflow_status(REMOVE_AIR_ON_HANDLE);
    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.plt_valve = 0x01;
    l_valve_dir.plt_valve = VALVE_DIR_RIGHT;/////
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to  left
        return 1;
    }
    
    return 0;
}

static u8 air_remove_end_func(void)
{
    CTRL_VALVE_TYPE l_valve;
    CTRL_VALVE_DIR l_valve_dir;

    APP_TRACE("Enter over_flow_end_func...\r\n");

    init_valve_param(&l_valve, &l_valve_dir);
    l_valve.plt_valve = 0x01;
    l_valve_dir.plt_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_valve, l_valve_dir))
    {// plt valve turn to left
        return 1;
    }

    if (PLT_COLLECT_BEGIN == get_collect_status())
    {
        // resume timing back task
        OSTaskResume(CONTROL_TIMING_BACK_TASK_PRIO);
        // resume plt's total distance
        resume_plt_total_dis();
    }
        
    return 0;
}

static u8 pour_saline_func(void)//???????????????????????
{
    APP_TRACE("Enter pour_saline_func...\r\n");
    return 0;
}


static u8 resume_ACBlood_func(void)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;
    
    APP_TRACE("Enter feedback ACBlood\r\n");

    if (STATUS_COLLECT_DRAW==g_pause_status || STATUS_COLLECT_BACK==g_pause_status)
    {//feed back 3mL AC-Blood
        init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
        l_pump.feedBack_pump = 0x01;
        l_speed.feedBack_speed = 5000;
        l_distance.feedBack_distance = 10;
        if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
        {
            return 1;
        }
        if (wait_pump_end(l_pump, l_speed, l_distance))
        {
            return 1;
        }
        if (ctrl_stop_pump(l_pump, l_count))
        {
            return 1;
        }
    }

    // resume centrifuge
    if (CENTRI_SLOW_SPEED*10 == g_centri_speed)
    {
        ctrl_start_centrifuge(CENTRI_NORMAL_SPEED*10);
    }
    // resume pump
    resume_run_pumps();
    // resume flow task
    os_resume_flow();

    return 0;
}

u8 adjust_draw_speed(ADJUST_EVENT event)
{
    u16 l_draw_flow_change;
    FLOW_PUMP_SPEED l_cur_speed, l_set_speed;

    APP_TRACE("Enter adjust_draw_speed event=[%d]\r\n", event);
    
    l_cur_speed = get_cur_phase_speed();///????????????????how to find the 
    // current speed
    l_draw_flow_change = DRAW_PUMP_CHANGE_UNIT;
    l_draw_flow_change = l_draw_flow_change * 10 / g_ratio_parse.ratio_draw;
              ///////////////////////////////why set to 10?????
    if (DRAW_SPEED_DOWN_EVENT == event)
    {
        if (l_cur_speed.draw_speed > l_draw_flow_change)
        {
            l_set_speed.draw_speed = l_cur_speed.draw_speed - l_draw_flow_change;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        l_set_speed.draw_speed = l_cur_speed.draw_speed + l_draw_flow_change;
    }
    l_set_speed.ac_speed = (u16)((u32)l_set_speed.draw_speed*100/g_cur_program_param.rate_plt);
    l_set_speed.feedBack_speed = l_cur_speed.feedBack_speed;
    l_set_speed.pla_speed = l_cur_speed.pla_speed;
    l_set_speed.plt_speed = l_cur_speed.plt_speed; 
    set_flow_speed_table(l_set_speed);

    if (STATUS_COLLECT_DRAW == g_run_status)
    {
        adjust_pump_speed(event, l_set_speed);        
    }
    
    return 0;
}

u8 adjust_feedback_speed(ADJUST_EVENT event)
{
    u16 l_back_flow_change;
    FLOW_PUMP_SPEED l_cur_speed, l_set_speed;

    APP_TRACE("Enter adjust_feedback_speed event=[%d]\r\n", event);

    l_cur_speed = get_cur_phase_speed();
    l_back_flow_change = BACK_PUMP_CHANGE_UNIT;
    l_back_flow_change = l_back_flow_change * 10 / g_ratio_parse.ratio_feedback;
                                                            //rad/min
    if (BACK_SPEED_DOWN_EVENT == event)
    {
        if (l_cur_speed.feedBack_speed > l_back_flow_change)
        {
            l_set_speed.feedBack_speed = l_cur_speed.feedBack_speed - l_back_flow_change;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        l_set_speed.feedBack_speed = l_cur_speed.feedBack_speed + l_back_flow_change;
    }
    l_set_speed.draw_speed = l_cur_speed.draw_speed;
    l_set_speed.ac_speed = l_cur_speed.ac_speed;
    l_set_speed.pla_speed = l_cur_speed.pla_speed;
    l_set_speed.plt_speed = l_cur_speed.plt_speed;
    set_flow_speed_table(l_set_speed);
    
    if (STATUS_COLLECT_BACK == g_run_status)
    {
        APP_TRACE("adjust feedback speed=[%d], change=[%d]\r\n", g_uart_cycle_back.back_speed/g_ratio_parse.ratio_feedback,l_back_flow_change);
        adjust_pump_speed(event, l_set_speed);
    }
    
    return 0;
}

u8 adjust_ac_speed(ADJUST_EVENT event)
{
    u16 l_ac_flow_change;
    FLOW_PUMP_SPEED l_cur_speed, l_set_speed;

    APP_TRACE("Enter adjust_ac_speed event=[%d]\r\n", event);
    
    l_cur_speed = get_cur_phase_speed();
    l_ac_flow_change = AC_PUMP_CHANGE_UNIT;
    l_ac_flow_change = l_ac_flow_change * g_cur_program_param.tbv_volume / g_ratio_parse.ratio_ac / 10;
                                                                     //??????????????
    if (AC_SPEED_DOWN_EVENT == event)
    {
        if (l_cur_speed.ac_speed > l_ac_flow_change)
        {
            l_set_speed.ac_speed = l_cur_speed.ac_speed - l_ac_flow_change;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        l_set_speed.ac_speed = l_cur_speed.ac_speed + l_ac_flow_change;
    }
    l_set_speed.draw_speed = (u16)((u32)l_cur_speed.ac_speed * g_cur_program_param.rate_plt/100);
    l_set_speed.feedBack_speed = l_cur_speed.feedBack_speed;
    l_set_speed.pla_speed = l_cur_speed.pla_speed;
    l_set_speed.plt_speed = l_cur_speed.plt_speed;
    set_flow_speed_table(l_set_speed);
    
    if (STATUS_COLLECT_DRAW == g_run_status)
    {
        APP_TRACE("adjust ac speed=[%d],change speed=[%d].\r\n", g_uart_cycle_back.ac_speed/g_ratio_parse.ratio_ac,l_ac_flow_change);
        adjust_pump_speed(event, l_set_speed);
    }
    
    return 0;
}

u8 adjust_agglu_rate(ADJUST_EVENT event)
{
    FLOW_PUMP_SPEED l_cur_speed, l_set_speed;

    APP_TRACE("Enter adjust_agglu_rate event=[%d]\r\n", event);
    
    l_cur_speed = get_cur_phase_speed();

    if (AGGLUTINATE_DOWN_EVENT == event)
    {
        if (g_cur_program_param.rate_plt > DRAW_AC_RATE_CHANGE_UNIT)
        {
            g_cur_program_param.rate_plt -= DRAW_AC_RATE_CHANGE_UNIT;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        g_cur_program_param.rate_plt += DRAW_AC_RATE_CHANGE_UNIT;
    }
    l_set_speed.ac_speed = l_cur_speed.ac_speed;
    l_set_speed.draw_speed = (u16)((u32)l_cur_speed.ac_speed * g_cur_program_param.rate_plt/100);
    l_set_speed.feedBack_speed = l_cur_speed.feedBack_speed;
    l_set_speed.pla_speed = l_cur_speed.pla_speed;
    l_set_speed.plt_speed = l_cur_speed.plt_speed;
    set_flow_speed_table(l_set_speed);
    
    if (STATUS_COLLECT_DRAW == g_run_status)
    {
        adjust_pump_speed(event, l_set_speed); 
    }
    
    return 0;
}

u8 adjust_pump_speed(ADJUST_EVENT event, FLOW_PUMP_SPEED speed)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    if (DRAW_SPEED_DOWN_EVENT == event || DRAW_SPEED_UP_EVENT == event
        || AC_SPEED_DOWN_EVENT == event || AC_SPEED_UP_EVENT == event 
        || AGGLUTINATE_DOWN_EVENT == event || AGGLUTINATE_UP_EVENT == event)
    {
        l_pump.draw_pump = 0x01;
        l_pump.ac_pump = 0x01;
        l_speed.draw_speed = speed.draw_speed;
        l_speed.ac_speed = speed.ac_speed;
    }
    else if (BACK_SPEED_DOWN_EVENT == event || BACK_SPEED_UP_EVENT == event)
    {
        l_pump.feedBack_pump = 0x01;
        l_speed.feedBack_speed = speed.feedBack_speed;
    }
    
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }
    return 0;
}


static u8 low_pressure_end_func(void)
{
    APP_TRACE("Resume from low draw pressure...\r\n");
    
    pressure_normal_handle();
    // resume flow task
    OSTaskResume(CONTROL_FLOW_TASK_PRIO);
    OSTaskResume(COTNROL_CALI_PUMP_TASK_PRIO);

    set_flow_pressure_status(FLOW_PRESSURE_NORMAL);
    APP_TRACE("Resume low ok!\r\n");
    
    return 0;
}

static u8 high_pressure_end_func(void)
{
    APP_TRACE("Resume from high back pressure...\r\n");

    pressure_normal_handle();
    // resume flow task
    OSTaskResume(CONTROL_FLOW_TASK_PRIO);
    OSTaskResume(COTNROL_CALI_PUMP_TASK_PRIO);

    set_flow_pressure_status(FLOW_PRESSURE_NORMAL);
    APP_TRACE("Resume high ok!\r\n");
    
    return 0;
}

void response_low_pressure(void)
{   
    u8 i = 0, l_replaced = 0, l_count = 0;
    u32 l_cur_tick;
    static INT32U s_low_pressure_array[PRESSURE_ERR_COUNT] = {0};
    INNER_ERROR_MSG l_errMsg;

    set_flow_pressure_status(FLOW_PRESSURE_LOW);
    
    l_cur_tick = OSTimeGet();
    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (l_cur_tick - s_low_pressure_array[i] > PRESSURE_TMR_TIMEOUT
            || 0 == s_low_pressure_array[i])
        {
            if (!l_replaced)
            {
                s_low_pressure_array[i] = l_cur_tick;
                l_replaced = 1;
            }
            else
            {
                s_low_pressure_array[i] = 0;
            }
        }
    }

    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (s_low_pressure_array[i])
        {
            l_count++;
        }
    }
    
    l_errMsg.source.all = ERR_SOURCE_ARM0;
    l_errMsg.taskNo = arm0_get_cur_prio();

    if (PRESSURE_ERR_COUNT == l_count)
    {
        memset(s_low_pressure_array, 0x00, sizeof(s_low_pressure_array));
        l_errMsg.action = ERROR_PAUSE_PROGRAM;
        l_errMsg.errcode1 = 0x30;
        l_errMsg.errcode2 = 0x00; 
    }
    else
    {
        l_errMsg.action = ERROR_PRESSURE_LOW;
        l_errMsg.errcode1 = 0x28;
        l_errMsg.errcode2 = 0x00; 
    }
    post_error_msg(l_errMsg);
}

void response_high_pressure(void)
{
    u8 i = 0, l_replaced = 0, l_count = 0;
    u32 l_cur_tick;
    static INT32U s_high_pressure_array[PRESSURE_ERR_COUNT] = {0};
    INNER_ERROR_MSG l_errMsg;

    set_flow_pressure_status(FLOW_PRESSURE_HIGH);
    
    l_cur_tick = OSTimeGet();
    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (l_cur_tick - s_high_pressure_array[i] > PRESSURE_TMR_TIMEOUT
            || 0 == s_high_pressure_array[i])
        {
            if (!l_replaced)
            {
                s_high_pressure_array[i] = l_cur_tick;
                l_replaced = 1;
            }
            else
            {
                s_high_pressure_array[i] = 0;
            }
        }
    }

    for (i=0; i<PRESSURE_ERR_COUNT; i++)
    {
        if (s_high_pressure_array[i])
        {
            l_count++;
        }
    }

    memset(&l_errMsg, 0x00, sizeof(l_errMsg));
    l_errMsg.source.all = ERR_SOURCE_ARM0;
    l_errMsg.taskNo = arm0_get_cur_prio();

    if (PRESSURE_ERR_COUNT == l_count)
    {
        memset(s_high_pressure_array, 0x00, sizeof(s_high_pressure_array));
        l_errMsg.action = ERROR_PAUSE_PROGRAM;
        l_errMsg.errcode1 = 0x31;
        l_errMsg.errcode2 = 0x00;
    }
    else
    {
        l_errMsg.action = ERROR_PRESSURE_HIGH;
        l_errMsg.errcode1 = 0x29;
        l_errMsg.errcode2 = 0x00;
    }
    post_error_msg(l_errMsg);
}

