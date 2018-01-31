/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cmd_analysis.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/16
 * Description        : This file contains the software implementation for the
 *                      cmd analysis task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/16 | v1.0  | Bruce.zhu  | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | This task is only to analysis cmd packet, 
 *                                   not using to receive usart dma data
 * 2013/10/21 | v1.1  | ZhangQ     | Receive and parse cmd command from PC
                                     add UART and TPDO FIFO task
 *******************************************************************************/
#include "app_cmd_analysis.h"
#include "app_control_board.h"
#include "app_control_common_function.h"
#include "app_control_monitor.h"
#include "app_cmd_send.h"
#include "app_usart_dma_route.h"
#include "app_control_speed_table.h"
#include "app_control_global_state.h"
#include "app_buzzer_ring.h"
#include "app_control_adjust_implement.h"
#include <string.h>
#include "trace.h"

#define UART_RECV_EVENT_TYPE        0x00
#define UART_RECV_DATA_TARNSE       0X01
#define UART_RECV_DATA_REQUEST      0X02
#define UART_RECV_INIT              0X05

static u8 uart_data_recv(u8* p_data, u16 len);
static u8 uart_data_parse(u8* p_data);

static void init_speed_table(void);

void control_uart_cmd_analysis_init(void)
{
    APP_TRACE("ENTER cmd analysis task...\r\n");
	// first init recv callback function
	init_usart_dma_route_task();
    init_usart_with_recv_callback(COM3, uart_data_recv, 115200, PARITY_NONE);

    /* define sem we use */    
    g_uart_poweron_test_event = OSSemCreate(0);
    assert_param(g_uart_poweron_test_event);
    g_uart_suit_load_event = OSSemCreate(0);
    assert_param(g_uart_suit_load_event);
    g_uart_begin_suit_test_event = OSSemCreate(0);
    assert_param(g_uart_begin_suit_test_event);
    g_uart_ac_priming_event = OSSemCreate(0);
    assert_param(g_uart_ac_priming_event);
    g_uart_start_collect_event = OSSemCreate(0);
    assert_param(g_uart_start_collect_event);
    g_uart_reset_machine_event = OSSemCreate(0);
    assert_param(g_uart_reset_machine_event);
    g_uart_stop_run_event = OSSemCreate(0);
    assert_param(g_uart_stop_run_event);
    g_uart_pause_run_event = OSSemCreate(0);
    assert_param(g_uart_pause_run_event);
    g_uart_continue_run_event = OSSemCreate(0);
    assert_param(g_uart_continue_run_event);
    g_uart_mute_event = OSSemCreate(0);
    assert_param(g_uart_mute_event);
    g_uart_unload_event = OSSemCreate(0);
    assert_param(g_uart_unload_event);
    g_uart_show_result = OSSemCreate(0);
    assert_param(g_uart_show_result);
}

static u8 uart_data_recv(u8* p_data, u16 len)
{
    INT8U err;
    u8 l_pFrame[UART_FRAME_TEXT_SIZE] = {0};
    
   // APP_TRACE("Enter uart_data_recv...\r\n");
    DumpData(p_data, len);  /* for debug */

    if (1==len)
    {
        if (ACK == p_data[0])
        {
            OSFlagPost(g_uart_acknak_flag, FLAG_ACK_BIT, OS_FLAG_SET, &err);
            return 0;
        }
        else if (NAK == p_data[0])
        {
           // APP_TRACE("Uart Recv NAK!\r\n");
            OSFlagPost(g_uart_acknak_flag, FLAG_NAK_BIT, OS_FLAG_SET, &err);
            return 1;
        }
        else
        {
            APP_TRACE("Uart Recv data length error!\r\n");
            uart_answer_cmd(NAK);
            return 1;
        }      
    }
    else if (UART_FRAME_ALL_SIZE != len)
    {
        APP_TRACE("Uart Recv data length error!\r\n");
        uart_answer_cmd(NAK);
        return 1;
    }

    if (STX != p_data[0] || ETX != p_data[11] || LF != p_data[12])
    {
        APP_TRACE("Uart Recv data format error!\r\n");
        uart_answer_cmd(NAK);
        return 1;
    }
    if (!uart_crc_check_func(p_data, UART_FRAME_ALL_SIZE))
    {
        APP_TRACE("Uart Recv data CRC chek error!\r\n");
        uart_answer_cmd(NAK);
        return 1;
    }
    // recv data format ok
    uart_answer_cmd(ACK);
    
    memcpy(l_pFrame, &p_data[2], sizeof(l_pFrame));/////
    // begin parse uart data
    return uart_data_parse(l_pFrame);
}


static u8 uart_data_parse(u8* p_data)
{
    uint8_t l_module_flag, l_event_type;
    uint8_t l_param1, l_param2, l_param3, l_param4, l_param5, l_param6;
    INT8U   l_error;
    ADJUST_EVENT l_adjust_event;
    
    // uart recv data analysis
    l_module_flag = p_data[0]  & 0xF0;
    l_event_type = p_data[1];
    
    switch (p_data[0] & 0x0F)
    {
        case UART_RECV_EVENT_TYPE://0x00
        {// recv event
            if (0x10 == l_module_flag)
            {// start power on test
                set_run_status(STATUS_POWERON);
                APP_TRACE("post g_uart_poweron_test_event\r\n");
                l_error = OSSemPost(g_uart_poweron_test_event); /* triggle poweron test */
                assert_param(OS_ERR_NONE == l_error);
            }
            else if (0x20 == l_module_flag)
            {// load suit and suit test
                if (0X01 == l_event_type)
                {// load suit                    
                    set_run_status(STATUS_SUIT_LOADING);
                    l_error = OSSemPost(g_uart_suit_load_event); /* triggle load suit */
                    assert_param(OS_ERR_NONE == l_error);
                    APP_TRACE("post g_uart_suit_load_event \r\n");
                    
                }
                else if (0x02 == l_event_type)
                {// suit test
                    set_run_status(STATUS_SUIT_CHECKING);
                    l_error = OSSemPost(g_uart_begin_suit_test_event); /* triggle test suit */
                    assert_param(OS_ERR_NONE == l_error);
                }
                else if (0x03 == l_event_type)
                {// AC priming
                    set_run_status(STATUS_AC_PRIMING);
                    l_error = OSSemPost(g_uart_ac_priming_event); /* triggle test suit */
                    assert_param(OS_ERR_NONE == l_error);
                }
            }
            else if (0x40 == l_module_flag)
            {// update param
                if (0x01 == l_event_type)
                {// param adjust
                    switch (l_param1)
                    {
                        case 0x01:
                        {// draw speed
                            if (0x01 == l_param2)
                            {// up
                                l_adjust_event = DRAW_SPEED_UP_EVENT;
                            }
                            else
                            {// down
                                l_adjust_event = DRAW_SPEED_DOWN_EVENT;
                            }
                            break;
                        }
                        case 0x02:
                        {// back speed
                            if (0x01 == l_param2)
                            {// up
                                l_adjust_event = BACK_SPEED_UP_EVENT;
                            }
                            else
                            {// down
                                l_adjust_event = BACK_SPEED_DOWN_EVENT;
                            }
                            break;
                        }
                        case 0x03:
                        {// AC speed
                            if (0x01 == l_param2)
                            {// up
                                l_adjust_event = AC_SPEED_UP_EVENT;
                            }
                            else
                            {// down
                                l_adjust_event = AC_SPEED_DOWN_EVENT;
                            }
                            break;
                        }
                        case 0x04:
                        {// Agglutinate rate
                            if (0x01 == l_param2)
                            {// up
                                l_adjust_event = AGGLUTINATE_UP_EVENT;
                            }
                            else
                            {// down
                                l_adjust_event = AGGLUTINATE_DOWN_EVENT;
                            }
                            break;
                        }
                    }
                    set_adjust_event(l_adjust_event);/////???????????
                }
                if (0x02 == l_event_type)
                {// rbc overflow adjust
                     /* triggle overflow program */
                    set_adjust_event(OVER_FLOW_EVENT);
                }
                else if (0x03 == l_event_type)
                {// air remove adjust
                    /* triggle remove air program */
                    set_adjust_event(AIR_REMOVE_EVENT);
                }
                else if (0x04 == l_event_type)
                {// pour saline
                    /* triggle pour saline program */
                    set_adjust_event(POUR_SALINE_EVENT);
                }
            }
            else if (0x50 == l_module_flag)
            {// Collect finish
                if (0x01 == l_event_type)
                {
                    l_error = OSSemPost(g_uart_unload_event); /* triggle unload canal */
                    assert_param(OS_ERR_NONE == l_error);
                }
                else if (0x03 == l_event_type)
                {
                    l_error = OSSemPost(g_uart_reset_machine_event); /* triggle reset machine program */
                    assert_param(OS_ERR_NONE == l_error);
                }
            }
            else if (0x60 == l_module_flag)
            {// system information
                if (0x01 == l_event_type)
                {// stop
                    l_error = OSSemPost(g_uart_stop_run_event); /* triggle stop run program */
                    assert_param(OS_ERR_NONE == l_error);
                }
                else if (0x02 == l_event_type)
                {// pause
                    l_error = OSSemPost(g_uart_pause_run_event); /* triggle pause run program */
                    assert_param(OS_ERR_NONE == l_error);
                }
                else if (0x03 == l_event_type)
                {// resume       
                    l_error = OSSemPost(g_uart_continue_run_event); /* triggle continue run program */
                    assert_param(OS_ERR_NONE == l_error);
                }
                else if (0x04 == l_event_type)
                {// feedback
                    l_error = OSSemPost(g_feedback_event);
                    assert_param(OS_ERR_NONE == l_error);
                   
                }
                else if (0x06 == l_event_type)
                {// mute
                    set_buzzer_ring_tone(BUZZER_RING_STOP, 0);
                }
            }
            break;
        }
        
        case UART_RECV_DATA_TARNSE://0x01
        {// recv data
            l_param1 = p_data[2];
            l_param2 = p_data[3];
            l_param3 = p_data[4];
            l_param4 = p_data[5];
            l_param5 = p_data[6];
            l_param6 = p_data[7];
            if (0x30 == l_module_flag)
            {
                if (0x01 == l_event_type)
                {// volume need to collect
                    APP_TRACE("0x03 0x01 ......");
                    DUMP_DATA(p_data,8);
                    
                   
                    g_cur_program_param.plt_pre_volume = l_param2*255 + l_param1;       /* need collect plt volume */
                    g_cur_program_param.plasma_pre_volume = l_param4*255 + l_param3;    /* need collect plasma volume */
                    g_cur_program_param.rbc_pre_volume = l_param6*255 + l_param5;       /* need collect rbc volume */                    
                }
                else if (0x02 == l_event_type)
                {// draw max speed & back max speed
                    g_cur_program_param.draw_max_speed= l_param2*255 + l_param1;        /* draw max speed */
                    g_cur_program_param.back_max_speed = l_param4*255 + l_param3;       /* feedback max speed */
                    g_cur_program_param.tbv_volume = l_param5;
                }
                else if (0x04 == l_event_type)
                {
                    g_cur_program_param.ac_max_speed = l_param1;
                    g_cur_program_param.rate_plt = l_param2 + l_param3*255;
                    g_cur_program_param.rate_rbc = l_param4 + l_param5*255;
                }
                else if (0x05 == l_event_type)
                {
                    g_cur_program_param.draw_pressure_limit = l_param2*255 + l_param1;
                    g_cur_program_param.back_pressure_limit = l_param4*255 + l_param3;
                }
                else if (0x06 == l_event_type)
                {
                    g_cur_program_param.runtime_max = l_param1;
                    g_cur_program_param.bool_back_switch = l_param2;
                    g_cur_program_param.bool_speedup_switch = l_param3;

                    // Init flow speed table
                    init_speed_table();
                    APP_TRACE("init_speed_table!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
                }
            }
            break;
        }
        case UART_RECV_DATA_REQUEST://0x02
        {
            if (0x30 == l_module_flag && 0x09 == l_event_type)
            {
                l_error = OSSemPost(g_uart_start_collect_event); /* triggle start collect program */
                assert_param(OS_ERR_NONE == l_error);
            }
            else if(0x50 == l_module_flag && 0x02 == l_event_type)
            {
                l_error = OSSemPost(g_uart_show_result); /* triggle feedback program result */
                assert_param(OS_ERR_NONE == l_error);
            }
            break;
        }
        case UART_RECV_INIT://0x05
        {
            
            if (COMM_MODULE_FLAG == p_data[0])//0xA5
            {
                g_comm_sync_state.uart = 0x01;
                APP_TRACE("g_comm_sync_state.uart = 0x01;\r\n");
            }
        }
        
        default:
            break;
    }
    	
	return 0;
}

static void init_speed_table(void)
{
    u16 l_draw_speed, l_feedback_speed, l_pla_speed, l_plt_speed;
    FLOW_PUMP_SPEED l_flow_speed;

    // TODO: the init speed need to check
    l_draw_speed = g_cur_program_param.draw_max_speed > g_cur_program_param.ac_max_speed*11 ? 
        g_cur_program_param.ac_max_speed*11 : g_cur_program_param.draw_max_speed;
    l_draw_speed = l_draw_speed*10 / g_ratio_parse.ratio_draw;
    l_feedback_speed = g_cur_program_param.back_max_speed*10/g_ratio_parse.ratio_feedback;
    l_pla_speed = 700;
    l_plt_speed = 1400;

    l_flow_speed.draw_speed = l_draw_speed;
    l_flow_speed.feedBack_speed = l_feedback_speed;
    l_flow_speed.pla_speed = l_pla_speed;
    l_flow_speed.plt_speed = l_plt_speed;

    init_flow_speed_table(l_flow_speed);
}


