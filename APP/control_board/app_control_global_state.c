/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_global_state.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/29
 * Description        : This file contains the global state in the program.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/29 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_global_state.h"

#include "trace.h"
static uint16_t get_plt_total_distance(void);

static u8   g_cmd_frame_fn[3];
static u8   g_start_pump_fn[3];	
static u8   g_get_distance_fn[3];

static FLOW_WAIT_EVENT              g_flow_wait_event;
static COLLECT_STATUS               g_collect_status;
static FLOW_PRESSURE_STATUS         g_flow_pressure_status;
static OVERFLOW_HANDLE_STATUS       g_overflow_status;
static volatile TEST_CANAL_PRESSURE_STATUS   g_test_canal_pressure_status;
static CTRL_PUMP_TYPE               g_status_pump_type;
static CTRL_PUMP_TYPE               g_status_pressure_abnoraml;

static u16  g_plt_pause_dis = 0;
static u16  g_plt_resume_dis = 0;
static u16  g_plt_miss_dis = 0;

void set_run_status(RUN_STATUS status)
{
    OSSchedLock();
    g_run_status = status;
    OSSchedUnlock();
}

/* set/get flow wait status */
void set_flow_wait_status(FLOW_WAIT_EVENT event)
{
    OSSchedLock();
    g_flow_wait_event = event;
    OSSchedUnlock();
}
FLOW_WAIT_EVENT get_flow_wait_status(void)
{
    return g_flow_wait_event;
}
/******************************/

/* set/get flow pressure status */
void set_flow_pressure_status(FLOW_PRESSURE_STATUS status)
{
    OSSchedLock();
    g_flow_pressure_status = status;
    OSSchedUnlock();
}
FLOW_PRESSURE_STATUS get_flow_pressure_status(void)
{
    return g_flow_pressure_status;
}
/*******************************/

/* set/get overflow status */
void set_overflow_status(OVERFLOW_HANDLE_STATUS status)
{
    OSSchedLock();
    g_overflow_status = status;
    OSSchedUnlock();
}
OVERFLOW_HANDLE_STATUS get_overflow_status(void)
{
    return g_overflow_status;
}
/**************************/

/* set/get test_canal_pressure status */
void set_test_canal_pressure_status(TEST_CANAL_PRESSURE_STATUS status)
{
    OSSchedLock();
    g_test_canal_pressure_status = status;
    OSSchedUnlock();
}
TEST_CANAL_PRESSURE_STATUS get_test_canal_pressure_status(void)
{
    return g_test_canal_pressure_status;
}
/*************************************/

/* set/get cmd fn */
void set_cmd_fn(CMD_FN_TYPE type, FN_SOURCE source, uint8_t fn)
{
    switch (type)
    {
        case SEND_FRAME_FN:
        {
            if (ARM1_DEST == source)
                g_cmd_frame_fn[0] = fn;
            else if (ARM2_DEST == source)
                g_cmd_frame_fn[1] = fn;
            else if (ARM3_DEST == source)
                g_cmd_frame_fn[2] = fn;
            break;
        }
        case START_PUMP_FN:
        {
            if (ARM1_DEST == source)
                g_start_pump_fn[0] = fn;
            else if (ARM2_DEST == source)
                g_start_pump_fn[1] = fn;
            else if (ARM3_DEST == source)
                g_start_pump_fn[2] = fn;
            break;
        }
        case GET_DISTANCE_FN:
        {
            if (ARM1_DEST == source)
                g_get_distance_fn[0] = fn;
            else if (ARM2_DEST == source)
                g_get_distance_fn[1] = fn;
            else if (ARM3_DEST == source)
                g_get_distance_fn[2] = fn;
            break;
        }
        default:
            break;
    }
}

uint8_t get_cmd_fn(CMD_FN_TYPE type, FN_SOURCE source)
{
    u8 l_ret = 0;
    
    switch (type)
    {
        case SEND_FRAME_FN:
        {
            if (ARM1_DEST == source)
                l_ret = g_cmd_frame_fn[0];
            else if (ARM2_DEST == source)
                l_ret = g_cmd_frame_fn[1];
            else if (ARM3_DEST == source)
                l_ret = g_cmd_frame_fn[2];
            break;
        }
        case START_PUMP_FN:
        {
            if (ARM1_DEST == source)
                l_ret = g_start_pump_fn[0];
            else if (ARM2_DEST == source)
                l_ret = g_start_pump_fn[1];
            else if (ARM3_DEST == source)
                l_ret = g_start_pump_fn[2];
            break;
        }
        case GET_DISTANCE_FN:
        {
            if (ARM1_DEST == source)
                l_ret = g_get_distance_fn[0];
            else if (ARM2_DEST == source)
                l_ret = g_get_distance_fn[1];
            else if (ARM3_DEST == source)
                l_ret = g_get_distance_fn[2];
            break;
        }
        default:
            break;
    }
    return l_ret;
}
/********************************/

/* set/get collect status */
void set_collect_status(COLLECT_STATUS status)
{
    g_collect_status = status;
}

COLLECT_STATUS get_collect_status(void)
{
    return g_collect_status;
}
/**************************/

void set_pause_state(CTRL_PUMP_TYPE type)
{
    g_status_pump_type = type;
}

CTRL_PUMP_TYPE get_pause_state(void)
{
    return g_status_pump_type;
}

void set_pressure_abnormal_state(CTRL_PUMP_TYPE type)
{
    OSSchedLock();
    g_status_pressure_abnoraml = type;
    OSSchedUnlock();
}

CTRL_PUMP_TYPE get_pressure_abnormal_state(void)
{
    return g_status_pressure_abnoraml;
}

void pause_plt_total_dis(void)
{
    APP_TRACE("pause_plt_total_dis!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\r\n");
    g_plt_pause_dis = get_plt_total_distance();
}

void resume_plt_total_dis(void)
{
    APP_TRACE("resume_plt_total_dis!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\r\n");

    g_plt_resume_dis = get_plt_total_distance();
    g_plt_miss_dis = g_plt_resume_dis - g_plt_pause_dis;
}

uint16_t get_plt_collect_dis(void)
{
    APP_TRACE("g_plt_miss_dis = %d\r\n",g_plt_miss_dis);//for debug
    return get_plt_total_distance() - g_plt_miss_dis;////??????????
}

static uint16_t get_plt_total_distance(void)
{
    CTRL_PUMP_TYPE l_type;
    CTRL_PUMP_DISTANCE l_distance;
    
    memset(&l_type, 0x00, sizeof(l_type));
    memset(&l_distance, 0x00, sizeof(l_distance));
    
    l_type.plt_pump = 0x01;
    l_distance = get_pump_total_distance(l_type);
    
    APP_TRACE("l_distance.plt_distance from arm2 283 283 283 = %d\r\n",l_distance.plt_distance);
    
    return l_distance.plt_distance;
}


