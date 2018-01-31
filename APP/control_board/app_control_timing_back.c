/******************** (C) COPYRIGHT 2013 VINY **********************************
 * FileName             : app_control_timing_back.c
 * Author               : ZhangQ
 * Date First Issued    : 2013/10/24
 * Description          : This file contains the software implementation for the
 *                        timing feedback task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/24 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_timing_back.h"
#include "app_cmd_send.h"
#include "struct_control_board_canopen.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"
#include "trace.h"

#define PRESSURE_TBL_SIZE           10

static OS_EVENT*        g_update_pressure_event;
static void*            g_pressure_QueueTbl[PRESSURE_TBL_SIZE];

static OS_STK control_timing_back_task_stk[CONTROL_TIMING_BACK_TASK_STK_SIZE];

static void send_pressure_frame(RUN_STATUS status);
static void send_collect_volume(void);
static void send_pressure_now(u16 pressure);
//static void send_current_speed(void);
static void control_timing_back_task(void *p_arg);

void control_init_timing_back_task(void)
{
    INT8U os_err;
    
    /* timing back task *****************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_timing_back_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_timing_back_task_stk[CONTROL_TIMING_BACK_TASK_STK_SIZE - 1],
							(INT8U			 ) CONTROL_TIMING_BACK_TASK_PRIO,
							(INT16U 		 ) CONTROL_TIMING_BACK_TASK_PRIO,
							(OS_STK 	   * )&control_timing_back_task_stk[0],
							(INT32U 		 ) CONTROL_TIMING_BACK_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(CONTROL_TIMING_BACK_TASK_PRIO, (INT8U *)"timingback", &os_err);

    memset(&g_uart_cycle_back, 0x00, sizeof(g_uart_cycle_back));

    g_update_pressure_event = OSQCreate(g_pressure_QueueTbl, PRESSURE_TBL_SIZE);
    assert_param(g_update_pressure_event);
}

static void control_timing_back_task(void *p_arg)
{
    INT8U err;
    void * l_msg;
    u16 l_pressure;
    
    APP_TRACE("Enter control_timing_back_task...\r\n");

    for (;;)
    {
        l_msg = OSQPend(g_update_pressure_event, 1000,  &err);////waiting for 
        // post ,1s later waiting timeout ,then send the pressure and blood 
        // amount
      //  APP_TRACE("OSQPended g_update_pressure_event...\r\n");

        if(err == OS_ERR_NONE)
            {
                if (l_msg)
                {
                    l_pressure = (u16)((u32)l_msg);
                   
                    send_pressure_now(l_pressure);
                    //APP_TRACE("send_pressure_now(l_pressure);\r\n");
                  //  continue;
                } 
            }
        else if(err == OS_ERR_TIMEOUT)
            {
               // APP_TRACE("err == OS_ERR_TIMEOUT)\r\n");
                switch (g_run_status)
                {
                    case STATUS_BLOOD_PRIMING://10
                    {
                        send_pressure_frame(g_run_status);
                        
                       // send_collect_volume();//add by Big.Man
                        
                        break;
                    }
                    case STATUS_COLLECT_DRAW://11
                    {
                        send_pressure_frame(g_run_status);
                        send_collect_volume();
                        break;
                    }
                    case STATUS_COLLECT_BACK://12
                    {
                        send_pressure_frame(g_run_status);
                        send_collect_volume();
                        break;
                    }
                    default:
                        break;
                }
          

            }
        else
            {
              APP_TRACE("else g_update_pressure_event \r\n");
            }
      //  APP_TRACE("g_run_status = %d\r\n",g_run_status);
        


        if (STATUS_BLOOD_FEEDBACK == g_run_status)
        {
            break;
        }
    }
}

void update_pressure_now(u16 pressure)
{
    INT8U err;
    
    err = OSQPost(g_update_pressure_event, (void *)pressure);
   // APP_TRACE("OSQPost(g_update_pressure_event),err = %d\r\n",err);
    
    assert_param(OS_ERR_NONE == err);
}

static void send_pressure_frame(RUN_STATUS status)
{
    UART_FRAME l_pressure_frame;

    memset(&l_pressure_frame, 0x00, sizeof(l_pressure_frame));
    
    l_pressure_frame.module_flag = 0x31;
    l_pressure_frame.event_type = 0x0C;
    if (STATUS_BLOOD_PRIMING == status || STATUS_COLLECT_DRAW == status)
    {// send draw pressure
        l_pressure_frame.param1 = 0X01;
        if (g_arm1_rpdo3_data.blood_pressure.bit.sign)
        {// negative
            l_pressure_frame.param1 = 0X05;
        }
        l_pressure_frame.param2 = g_uart_cycle_back.draw_pressure & 0x00FF;
        l_pressure_frame.param3 = (g_uart_cycle_back.draw_pressure & 0xFF00) >> 8; 
    }
    else if (STATUS_COLLECT_BACK == status)
    {// send feedback pressure
        l_pressure_frame.param1 = 0X02;
        l_pressure_frame.param2 = g_uart_cycle_back.back_pressure & 0x00FF;
        l_pressure_frame.param3 = (g_uart_cycle_back.back_pressure & 0xFF00) >> 8; 
    }

    uart_send_frame(l_pressure_frame);
}

static void send_pressure_now(u16 pressure)
{
     UART_FRAME l_pressure_frame;

    memset(&l_pressure_frame, 0x00, sizeof(l_pressure_frame));
    
    l_pressure_frame.module_flag = 0x31;
    l_pressure_frame.event_type = 0x0C;

    if (STATUS_COLLECT_DRAW == g_run_status)
    {// send draw pressure
        l_pressure_frame.param1 = 0X05;//???????????????????????????????????????
        l_pressure_frame.param2 = pressure & 0x00FF;
        l_pressure_frame.param3 = (pressure & 0xFF00) >> 8; 
    }
    else if (STATUS_COLLECT_BACK == g_run_status)
    {// send feedback pressure
        l_pressure_frame.param1 = 0X02;//??????????????????????????????????????
        l_pressure_frame.param2 = pressure & 0x00FF;
        l_pressure_frame.param3 = (pressure & 0xFF00) >> 8; 
    }

    uart_send_frame(l_pressure_frame);
}


static void send_collect_volume(void)
{// TODO: rbc volume how to get
    INT8U err;
    UART_FRAME l_collect_frame;
    COLLECT_STATUS l_status;

    static uint16_t last_plt_valume = 0;
    static uint16_t ret_base = 0;//add by wq

    APP_TRACE("1 send_collect_volume()\r\n");
    
    memset(&l_collect_frame, 0x00, sizeof(l_collect_frame));

    l_status = get_collect_status();
    APP_TRACE("l_status = %d\r\n",l_status);
    
    if (NO_COLLECT == l_status)
    {
        APP_TRACE("return l_status = get_collect_status();\r\n");
        return;
    }
    
    if (PLT_COLLECT_BEGIN == l_status)
    {
        APP_TRACE("g_ratio_parse.ratio_plt * 1000 =%d\r\n",(uint32_t)(g_ratio_parse.ratio_plt*1000));

        g_total_distance_calc.plt_distance = (uint16_t)(get_plt_collect_dis()*g_ratio_parse.ratio_plt/10.0);

        
       // g_uart_cycle_back.plt_volume = (uint16_t)(get_plt_collect_dis()*g_ratio_parse.ratio_plt/10.0);

       g_uart_cycle_back.plt_volume =  g_total_distance_return.plt_distance + g_total_distance_calc.plt_distance;


      APP_TRACE("1111111  g_total_distance_calc.plt_plt_distancedistance = %d\r\n",  g_total_distance_calc.plt_distance);
      APP_TRACE("1111111  g_total_distance_return. = %d\r\n",  g_total_distance_return.plt_distance);

/*
      //add by wq for calc total plt valume  
        if(g_uart_cycle_back.plt_volume == 0)
        {
            ret_base = ret_base + last_plt_valume;
            g_uart_cycle_back.plt_volume = g_uart_cycle_back.plt_volume + ret_base;
            
            APP_TRACE("g_uart_cycle_back.plt_volume == 0 ret_base   = %d\r\n",ret_base);
            APP_TRACE("g_uart_cycle_back.plt_volume == 0 plt_volume = %d\r\n",g_uart_cycle_back.plt_volume);
        }
        else
        {
            last_plt_valume =  g_uart_cycle_back.plt_volume;
            g_uart_cycle_back.plt_volume = g_uart_cycle_back.plt_volume + ret_base;
            APP_TRACE("g_uart_cycle_back.plt_volume != 0 plt_volume = %d\r\n",g_uart_cycle_back.plt_volume);
        }
*/

        //last_plt_valume =  g_uart_cycle_back.plt_volume;
      
        APP_TRACE("2222222 g_uart_cycle_back.plt_volume = %d\r\n", g_uart_cycle_back.plt_volume);

    }
    
    l_collect_frame.module_flag = 0x31;
    l_collect_frame.event_type  = 0x0D;
    l_collect_frame.param1 = g_uart_cycle_back.plt_volume & 0x00FF;
    l_collect_frame.param2 = (g_uart_cycle_back.plt_volume & 0xFF00) >> 8;
    l_collect_frame.param3 = g_uart_cycle_back.plasma_volume & 0x00FF;
    l_collect_frame.param4 = (g_uart_cycle_back.plasma_volume & 0xFF00) >> 8;
    l_collect_frame.param5 = g_uart_cycle_back.rbc_volume & 0x00FF;
    l_collect_frame.param6 = (g_uart_cycle_back.rbc_volume & 0xFF00) >> 8;
    
    APP_TRACE("2 send_collect_volume()\r\n");
    uart_send_frame(l_collect_frame);
    
    APP_TRACE("3 send_collect_volume()\r\n");
    // check if the collect voume is reach the target voume


    APP_TRACE("g_cur_program_param.plt_pre_volume = %d\r\n\r\n",g_cur_program_param.plt_pre_volume);   
     
  //  if (g_uart_cycle_back.plt_volume >= 50)//g_cur_program_param.plt_pre_volume) // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     
    if (g_uart_cycle_back.plt_volume >= g_cur_program_param.plt_pre_volume) // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {// triggle feedback flow  

        APP_TRACE("g_uart_cycle_back.plt_volume >= g_cur_program_param.plt_pre_volume!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
        set_run_status(STATUS_BLOOD_FEEDBACK);
        err = OSSemPost(g_feedback_event);
        assert_param(OS_ERR_NONE == err);
    }
}

#if 0                             
/////？？？？？？？？？？？？？？？？？？不反馈底控板发回来的数据
static void send_current_speed(void)
{
    UART_FRAME l_speed_frame;

    l_speed_frame.module_flag = 0x31;
    l_speed_frame.event_type = 0x0D;
    l_speed_frame.param1 = g_uart_cycle_back.ac_speed & 0x00FF;
    l_speed_frame.param2 = (g_uart_cycle_back.ac_speed & 0xFF00) >> 8;
    l_speed_frame.param3 = g_uart_cycle_back.draw_speed & 0x00FF;
    l_speed_frame.param4 = (g_uart_cycle_back.draw_speed & 0xFF00) >> 8;
    l_speed_frame.param5 = g_uart_cycle_back.back_speed & 0x00FF;
    l_speed_frame.param6 = (g_uart_cycle_back.back_speed & 0xFF00) >> 8;

    uart_send_frame(l_speed_frame);
}
#endif

