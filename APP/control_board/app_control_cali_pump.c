/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_cali_pump.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/12/19
 * Description        : This file contains the implement of calibration pump k value
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/12/19 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_cali_pump.h"
#include "app_control_pump.h"
#include "app_control_common_function.h"
#include "app_cmd_send.h"
#include "trace.h"

static OS_EVENT*        g_cali_pump_event;
static void*            g_cali_pump_QueueTbl[TOTAL_DIS_QUEUE_SIZE];
static OS_STK           control_cali_pump_task_stk[CONTROL_CALI_PUMP_TASK_STK_SIZE];

static void control_cali_pump_task(void *p_arg);
static void calculate_pump_k_value(u16 *p_draw_distance, u16 *p_back_distance);

void control_init_cali_pump_task(void)
{
    INT8U os_err;
    
    /* total distance task *****************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) control_cali_pump_task,
							(void		   * ) 0,
							(OS_STK 	   * )&control_cali_pump_task_stk[CONTROL_CALI_PUMP_TASK_STK_SIZE - 1],
							(INT8U			 ) COTNROL_CALI_PUMP_TASK_PRIO,
							(INT16U 		 ) COTNROL_CALI_PUMP_TASK_PRIO,
							(OS_STK 	   * )&control_cali_pump_task_stk[0],
							(INT32U 		 ) CONTROL_CALI_PUMP_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(COTNROL_CALI_PUMP_TASK_PRIO, (INT8U *)"cali pump", &os_err);

    g_cali_pump_event = OSQCreate(g_cali_pump_QueueTbl, TOTAL_DIS_QUEUE_SIZE);
    assert_param(g_cali_pump_event);
}

static void control_cali_pump_task(void *p_arg)
{
    INT8U err;
    void *l_msg;
    TOTAL_DISTANCE_EVENT l_event;
    CTRL_PUMP_TYPE l_type;
    CTRL_PUMP_DISTANCE l_distance;
    u16 l_total_draw_distance[7] = {0};
    u16 l_total_back_distance[3] = {0};
    u8 l_draw_cursor = 0, l_back_cursor = 0;
    u8 l_exit_task = 0;
    
    for(;;)
    {
        l_msg = OSQPend(g_cali_pump_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
        l_event = (TOTAL_DISTANCE_EVENT)((u32)l_msg);

        memset(&l_type, 0x00, sizeof(l_type));
        memset(&l_distance, 0x00, sizeof(l_distance));

        switch (l_event)
        {
            case GET_DRAW_TOTAL_DIS:
            {
                l_type.draw_pump = 0x01;
                l_distance = get_pump_total_distance(l_type);

                APP_TRACE("GET_DRAW_TOTAL_DIS get_pump_total_distance \r\n");
                
                l_total_draw_distance[l_draw_cursor++] = l_distance.draw_distance;

                APP_TRACE("l_total_draw_distance[0] = %d\r\n",l_total_draw_distance[0]);
                APP_TRACE("l_total_draw_distance[1] = %d\r\n",l_total_draw_distance[1]);
                APP_TRACE("l_total_draw_distance[2] = %d\r\n",l_total_draw_distance[2]);
                APP_TRACE("l_total_draw_distance[3] = %d\r\n",l_total_draw_distance[3]);
                APP_TRACE("l_total_draw_distance[4] = %d\r\n",l_total_draw_distance[4]);
                APP_TRACE("l_total_draw_distance[5] = %d\r\n",l_total_draw_distance[5]);
                APP_TRACE("l_total_draw_distance[6] = %d\r\n",l_total_draw_distance[6]);
                APP_TRACE("l_draw_cursor = %d\r\n\r\n",l_draw_cursor);

                
                if (7 == l_draw_cursor)
                {
                    APP_TRACE("GET_DRAW_TOTAL_DIS calculate_pump_k_value 7777777777\r\n");
                    calculate_pump_k_value(l_total_draw_distance, l_total_back_distance);
                    APP_TRACE("draw ratio*1000=%d back ratio*1000=%d\r\n", 
                    (uint16_t)(g_ratio_parse.ratio_draw*1000), 
                    (uint16_t)(g_ratio_parse.ratio_feedback*1000));
                    l_exit_task = 1;
                }
                break;

            }
            case GET_DRAW_BACK_TOTAL_DIS:
            {
                l_type.draw_pump = 0x01;
                l_type.feedBack_pump = 0x01;
                l_distance = get_pump_total_distance(l_type);
                
                APP_TRACE("GET_DRAW_BACK_TOTAL_DIS get_pump_total_distance \r\n");
                
                l_total_draw_distance[l_draw_cursor++] = l_distance.draw_distance;
                l_total_back_distance[l_back_cursor++] = l_distance.feedBack_distance;
                break;
            }
            default:
                break;
        }
        if (l_exit_task)
            break;
    }

}


void set_total_distance_event(TOTAL_DISTANCE_EVENT event)
{
    INT8U err;
    CTRL_PUMP_TYPE l_type;

    memset(&l_type, 0x00, sizeof(l_type));

    if (CLEAR_DRAW_DIS == event)
    {// clear draw distance
        l_type.draw_pump = 0x01;
        clear_pump_total_distance(l_type);
    }
    else if (CLEAR_FEEDBACK_DIS == event)
    {// clear feedback distance
        l_type.feedBack_pump = 0x01;
        clear_pump_total_distance(l_type);
    }
    else
    {
        err = OSQPost(g_cali_pump_event, (void *)event);
        assert_param(OS_ERR_NONE == err);
    }
}
static void calculate_pump_k_value(u16 *p_draw_distance, u16 *p_back_distance)
{
    float l_contain_size = FEEDBACK_CONTAINER_VOLUME; // 50 mL
    float l_avg_draw_rounds, l_avg_back_rounds, l_avg_back_draw_rounds;
    u16 l_draw_full_dis[3] = {0};   /* draw flow draw pump's distance */
    u16 l_back_full_dis[3] = {0};   /* back flow back pump's distance */
    u16 l_back_draw_dis[3] = {0};   /* back flow draw pump's distance */
    
    UART_FRAME l_frame;
    
    l_back_full_dis[0] = p_back_distance[0];
    l_back_full_dis[1] = p_back_distance[1] - p_back_distance[0];
    l_back_full_dis[2] = p_back_distance[2] - p_back_distance[1];
    l_back_draw_dis[0] = p_draw_distance[1] - p_draw_distance[0];
    l_back_draw_dis[1] = p_draw_distance[3] - p_draw_distance[2];
    l_back_draw_dis[2] = p_draw_distance[5] - p_draw_distance[4];

    l_draw_full_dis[0] = p_draw_distance[2] - p_draw_distance[1];
    l_draw_full_dis[1] = p_draw_distance[4] - p_draw_distance[3];
    l_draw_full_dis[2] = p_draw_distance[6] - p_draw_distance[5];
    
    // calculate the draw pump's k value
    l_avg_draw_rounds = get_average_value(l_draw_full_dis, 3);
    
    l_avg_back_rounds = get_average_value(l_back_full_dis, 3);//add by wq


    
    APP_TRACE("l_avg_draw_rounds*1000 = %d\r\n",(uint32_t)(l_avg_draw_rounds*1000));
    
    if (l_avg_back_rounds == 0 || l_avg_draw_rounds==0)
        {
            APP_TRACE("  if (l_avg_back_rounds == 0 || l_avg_draw_rounds==0)\r\n");
          //  return;
        }
    
    g_ratio_parse.ratio_draw = l_contain_size*10/l_avg_draw_rounds;//50*10/l_avg_draw_rounds

    APP_TRACE(" g_ratio_parse.ratio_draw *1000 = %d\r\n\r\n",(uint32_t)(g_ratio_parse.ratio_draw*1000));
    //开始ac泵，pla泵,plt泵,采血泵动，回输泵的运动不一样，所以另外计算它的K值
    g_ratio_parse.ratio_ac = g_ratio_parse.ratio_draw;
    g_ratio_parse.ratio_pla = g_ratio_parse.ratio_draw;
    g_ratio_parse.ratio_plt = g_ratio_parse.ratio_draw;

    //calculate the feedback pump's k value
    l_avg_back_draw_rounds = get_average_value(l_back_draw_dis, 3);
    l_avg_back_rounds = get_average_value(l_back_full_dis, 3);
    g_ratio_parse.ratio_feedback = (l_contain_size*10+l_avg_back_draw_rounds*g_ratio_parse.ratio_draw)
                                    /l_avg_back_rounds;

    // send cali pump's k value to IPC
    memset(&l_frame, 0x00, sizeof(l_frame));
    l_frame.module_flag = 0x31;
    l_frame.event_type = 0x0E;
    l_frame.param1 = (u8)(g_ratio_parse.ratio_draw*10);
    l_frame.param2 = (u8)(g_ratio_parse.ratio_ac*10);
    l_frame.param3 = (u8)(g_ratio_parse.ratio_feedback*10);
    l_frame.param4 = (u8)(g_ratio_parse.ratio_pla*10);
    l_frame.param5 = (u8)(g_ratio_parse.ratio_plt*10);
    uart_send_frame(l_frame);
}

