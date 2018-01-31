/******************** (C) COPYRIGHT 2013 VINY **********************************
 * FileName             : app_control_monitor.h
 * Author               : ZhangQ
 * Date First Issued    : 2013/10/18
 * Description          : This file contains the software implementation for the
 *                        monitor task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/18 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_monitor.h"
#include "app_control_board.h"
#include "app_control_pdo_callback.h"
#include "struct_control_board_canopen.h"
#include "app_cmd_send.h"
#include "app_comX100.h"
#include "app_control_common_function.h"
#include "app_control_key_press.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"
#include "app_buzzer_ring.h"
#include "app_control_adjust_implement.h"
#include "gpio.h"
#include "trace.h"


static OS_STK control_monitor_task_stk[CONTROL_MONITOR_TASK_STK_SIZE];

static u8       g_error_msg_buf[ERROR_MSG_QUEUE_NUM][ERROR_MSG_QUEUE_SIZE];
static OS_MEM*  g_error_msg_mem;

static OS_EVENT * g_monitor_error_event;
static void *     g_monitor_error_QueueTbl[8];


static void control_monitor_task(void *p_arg);
static u8 analysis_slave_action(u8 action);

static uint8_t pause_program_implement(void);
static uint8_t stop_program_implement(void);
static uint8_t resume_program_implement(void);
static uint8_t slow_down_centrifuge(void);
static uint8_t stop_centrifuge(void);
static uint8_t low_pressure_implement(void);
static uint8_t high_pressure_implement(void);

static void uart_send_error_msg(u8 *p_err);
static void uart_send_hardfault_err(ERROR_MSG_SOURCE source);
static void canopen_answer_error(u8 *p_err);


void control_init_monitor_task(void)
{
    INT8U os_err;
	/* control monitor task *****************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_monitor_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_monitor_task_stk[CONTROL_MONITOR_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_MONITOR_TASK_PRIO,
							(INT16U 		 ) CONTROL_MONITOR_TASK_PRIO,
							(OS_STK 	   * )&control_monitor_task_stk[0],
							(INT32U 		 ) CONTROL_MONITOR_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CONTROL_MONITOR_TASK_PRIO, (INT8U *)"monitor", &os_err);
    // Init ARM0 key and led
    init_arm0_exter_gpio();
    // Init hard key func
    control_init_key_press();
}

void post_error_msg(INNER_ERROR_MSG inner_msg)
{
    INT8U err;
    u8 * l_pMemBlock;

    if (STATUS_STOP == g_run_status || STATUS_UNEXCEPTION == g_run_status)
    {// Unrecyclable error has happened
        return;
    }
    
    // get memory
  //  l_pMemBlock = (u8*)OSMemGet(g_error_msg_mem, &err);
    //assert_param(l_pMemBlock);
    //APP_TRACE("get \r\n");

   // memcpy(l_pMemBlock, &inner_msg, sizeof(inner_msg));

  //  OSQPost(g_monitor_error_event, l_pMemBlock);
  //  APP_TRACE("post g_monitor_error_event\r\n");//
}

void control_monitor_task(void* p_arg)
{
    INT8U err;
    u8 *l_error_msg;
    uint8_t l_ret;
    
    APP_TRACE("Enter control_monitor_task...\r\n");    
    g_monitor_error_event = OSQCreate(g_monitor_error_QueueTbl, 8);
    assert_param(g_monitor_error_event);

    g_error_msg_mem = OSMemCreate(g_error_msg_buf, ERROR_MSG_QUEUE_NUM, ERROR_MSG_QUEUE_SIZE, &err);
    assert_param(g_error_msg_mem);

    for (;;)
    {
        // wait until  error occur
        l_error_msg = (u8*)OSQPend(g_monitor_error_event, 0, &err);
        
        APP_TRACE("pended g_monitor_error_event l_error_msg = 0x%x \r\n",l_error_msg[0]);//add by wq
        
        /** communication error **/        
        if (ERR_SOURCE_CANOPEN == l_error_msg[0])//0x02
        {// canopen communication error
            uart_send_error_msg(l_error_msg);    
            set_run_status(STATUS_UNEXCEPTION);
            // release memory
            OSMemPut(g_error_msg_mem, (void *)l_error_msg);
            APP_TRACE("put 1 \r\n");
            
            break;
        }
        else if (ERR_SOURCE_UART == l_error_msg[0])//0x00
        {// uart communication error
            l_ret = stop_program_implement();
            set_run_status(STATUS_UNEXCEPTION);
            // release memory
            OSMemPut(g_error_msg_mem, (void *)l_error_msg);
            APP_TRACE("put 2 \r\n");
            
            break;
        }
        // no else ??????

        /* analysis slave action */
        l_ret = analysis_slave_action(l_error_msg[2]);
        if (l_ret)
        {// slave action error
            // TODO: slave can't response 
        }

        /* post to IPC */
        if (ERR_SOURCE_UART!=l_error_msg[0])
        {
            uart_send_error_msg(l_error_msg);
        }

        /* analysis slave error */
        if (SLAVE_ERR_HARDFAULT == (l_error_msg[0]&0xF0)>>4)
        {// slave error hardfault happened
            uart_send_hardfault_err((ERROR_MSG_SOURCE)(l_error_msg[0]&0x0F)); /* send to ipc */
        }
        else if(SLAVE_ERR_TASK == (l_error_msg[0]&0xF0)>>4)
        {
            canopen_answer_error(l_error_msg);
        }
        l_ret = l_ret;
        // release memory
        err = OSMemPut(g_error_msg_mem, (void *)l_error_msg);
        assert_param(OS_ERR_NONE == err);
        APP_TRACE("put 3 \r\n");
    }
}

// slave board action when exception happened
static u8 analysis_slave_action(u8 action)
{
    u8 l_ret = 0;
    
    switch (action)
    {
        case ERROR_IGNORE:
        {// ignore error
            APP_TRACE("ERROR_IGNORE! \r\n");
            break;
        }
        case ERROR_PAUSE_PROGRAM:
        {// pause
            APP_TRACE("PAUSE Error happened, all pump stop!\r\n");
            l_ret = pause_program_implement();
            break;
        }
        case ERROR_STOP_PROGRAM:
        {// stop
            APP_TRACE("STOP Error happened, all motor stop!\r\n");
            l_ret = stop_program_implement();
            break;
        }
        case ERROR_RESUME_PROGRAM:
        {// resume
            APP_TRACE("Resume program!\r\n");
            l_ret = resume_program_implement();
            break;
        }
        case ERROR_CENTRI_SLOW:
        {// slow down centrifuge
            APP_TRACE("slow down centrifuge!\r\n");
            l_ret = slow_down_centrifuge();
            break;
        }
        case ERROR_CENTRI_STOP:
        {// stop centrifuge
            APP_TRACE("stop centrifuge!\r\n");
            l_ret = stop_centrifuge();
            break;
        }
        case ERROR_PRESSURE_LOW:
        {// stop all pumps but collect pump
            APP_TRACE("draw pressure is low!\r\n");
            l_ret = low_pressure_implement();
            break;
        }
        case ERROR_PRESSURE_HIGH:
        {// stop all pumps but collect pump
            APP_TRACE("back pressure is high!\r\n");
            l_ret = high_pressure_implement();
            break;
        }
        default:
            break;
    }

    return l_ret;
}

static void uart_send_error_msg(u8 *p_err)
{
    UART_FRAME l_frame;
    
    l_frame.module_flag = 0x61;
    l_frame.event_type = 0xEE;
    l_frame.param1 = p_err[0];
    l_frame.param2 = p_err[1];
    l_frame.param3 = p_err[2];
    l_frame.param4 = p_err[3];
    l_frame.param5 = p_err[4];
    l_frame.param6 = 0;
    uart_send_frame(l_frame);
}

static void uart_send_hardfault_err(ERROR_MSG_SOURCE source)
{
    UART_FRAME l_frame;

    memset(&l_frame, 0x00, sizeof(l_frame));
    l_frame.module_flag = 0x61;
    switch(source)
    {
        case ERR_SOURCE_ARM1:
        {
            l_frame.event_type = 0xF1;
            l_frame.param1 = g_arm1_rpdo6_data.pump_errcode1.all;
            l_frame.param2 = g_arm1_rpdo6_data.pump_errcode2.all;
            l_frame.param3 = g_arm1_rpdo6_data.sensor_error.all;
            break;
        }
        case ERR_SOURCE_ARM2:
        {
            l_frame.event_type = 0xF2;
            l_frame.param1 = g_arm2_rpdo6_data.pump_errcode.all;
            l_frame.param2 = g_arm2_rpdo6_data.valve_errcode.all;
            l_frame.param3 = g_arm2_rpdo6_data.sensor_error.all;
            break;
        }
        case ERR_SOURCE_ARM3:
        {
            l_frame.event_type = 0xF3;
            l_frame.param1 = g_arm3_rpdo6_data.motor_errcode1.all;
            l_frame.param2 = g_arm3_rpdo6_data.voltage_error.all;
            l_frame.param3 = g_arm3_rpdo6_data.sensor_error.all;
            break;
        }
        default:
            break; 
    }

    uart_send_frame(l_frame);
    
}

static void canopen_answer_error(u8 *p_err)
{
    switch(p_err[0]&0x0F)
    {
        case ERR_SOURCE_ARM1:
            arm1_answer_cmd(p_err[4], ACK);
            break;
        case ERR_SOURCE_ARM2:
            arm2_answer_cmd(p_err[4], ACK);
            break;
        case ERR_SOURCE_ARM3:
            arm3_answer_cmd(p_err[4], ACK);
            break;
        default:
            break;
    }
}

static uint8_t pause_program_implement(void)
{
    INT8U err;
    uint8_t l_ret = 1;

    os_pause_flow();                // suspend task
    g_pause_status = g_run_status;  // record run status
    g_run_status = STATUS_PAUSE_IN_ONE;
    alarm_led_on();
    set_buzzer_ring_tone(BUZZER_RING_WARNING_TONE, 0);
    
    OSTmrStart(g_tmr_pause, &err);  // start an pause timer
    assert_param(OS_ERR_NONE == err);

    l_ret = pause_run_pumps();

    return l_ret;
}
static uint8_t stop_program_implement(void)
{
    uint8_t l_ret = 1;
    CTRL_PUMP_TYPE l_pumpType;
    CTRL_PUMP_COUNT l_count;

    os_stop_program();
    set_buzzer_ring_tone(BUZZER_RING_ERROR_TONE, 0);

    memset(&l_pumpType, 0x1F, sizeof(l_pumpType));
    memset(&l_count, 0x8F, sizeof(l_count));

    l_ret = ctrl_stop_pump(l_pumpType, l_count);
    if (0==l_ret)
    {
        l_ret = ctrl_stop_centrifuge();
    }
    
    return l_ret;
}

static uint8_t resume_program_implement(void)
{    
    // TODO: see if reusme is ok    
    if (STATUS_PAUSE_IN_ONE == g_run_status)
    {// resume pumps
        resume_run_pumps();   
        os_resume_flow();
    }
    else if (STATUS_PAUSE_ONE_THREE == g_run_status)
    {// resume pumps and centrifuge
        if (CENTRI_SLOW_SPEED*10 == g_centri_speed)
        {
            ctrl_start_centrifuge(CENTRI_NORMAL_SPEED*10);
        }
        resume_run_pumps();
        os_resume_flow();
    }
    else if (STATUS_PAUSE_THREE_TEN == g_run_status)
    {// feed back 3mL AC Blood
        set_adjust_event(RESUME_FEEDBACK_EVENT);
    }
    else if (STATUS_PAUSE_AFTER_TEN == g_run_status)
    {
        // TODO: what to do,can not resume?
    }

    set_buzzer_ring_tone(BUZZER_RING_STOP, 0);
    
    return 0;
}


static uint8_t slow_down_centrifuge(void)
{
    u8 l_ret = 1;

    if (g_centri_speed == CENTRI_NORMAL_SPEED*10)
    {
        l_ret = ctrl_start_centrifuge(CENTRI_SLOW_SPEED*10);
    }
    return l_ret;
}

static uint8_t stop_centrifuge(void)
{
    u8 l_ret = 1;

    os_stop_program();

    set_buzzer_ring_tone(BUZZER_RING_ERROR_TONE, 0);

    l_ret = ctrl_stop_centrifuge();
    return l_ret;
}

static uint8_t low_pressure_implement(void)
{
    OSTaskSuspend(CONTROL_FLOW_TASK_PRIO);
    OSTaskSuspend(COTNROL_CALI_PUMP_TASK_PRIO);
    set_buzzer_ring_tone(BUZZER_RING_TONE, 0);
    pressure_abnormal_handle();
    set_flow_pressure_status(FLOW_PRESSURE_LOW_HANDLED);
    return 0;
}

static uint8_t high_pressure_implement(void)
{ 
    OSTaskSuspend(CONTROL_FLOW_TASK_PRIO);
    OSTaskSuspend(COTNROL_CALI_PUMP_TASK_PRIO);
    set_buzzer_ring_tone(BUZZER_RING_TONE, 0);
    pressure_abnormal_handle();
    set_flow_pressure_status(FLOW_PRESSURE_HIGH_HANDLED);
    return 0;
}


void os_pause_flow(void)
{
    OSTaskSuspend(CONTROL_FLOW_TASK_PRIO);
    OSTaskSuspend(COTNROL_CALI_PUMP_TASK_PRIO);
    OSTaskSuspend(CONTROL_TIMING_BACK_TASK_PRIO);
}

void os_resume_flow(void)
{
    OSTaskResume(CONTROL_FLOW_TASK_PRIO);
    OSTaskResume(COTNROL_CALI_PUMP_TASK_PRIO);
    OSTaskResume(CONTROL_TIMING_BACK_TASK_PRIO);
    stop_pause_status(RESUME_PAUSE_TMR);
    set_run_status(g_pause_status);
    if (FLOW_PRESSURE_NORMAL != get_flow_pressure_status())
    {
        set_flow_pressure_status(FLOW_PRESSURE_NORMAL);
    }
}

void os_stop_program(void)
{
    OSTaskSuspend(CONTROL_FLOW_TASK_PRIO);
    OSTaskSuspend(CONTROL_TIMING_BACK_TASK_PRIO);
    OSTaskSuspend(CONTROL_ADJUST_TASK_PRIO);
    OSTaskSuspend(COTNROL_CALI_PUMP_TASK_PRIO);
    stop_pause_status(STOP_PAUSE_TMR);
    set_run_status(STATUS_STOP);
}


