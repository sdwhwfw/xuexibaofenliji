/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_callback.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/18
 * Description        : This file contains the software implementation for the
 *                      RPDO callback function of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/18 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_board.h"
#include "app_control_pdo_callback.h"
#include "struct_control_board_canopen.h"
#include "app_control_monitor.h"
#include "app_control_pump.h"
#include "app_cmd_send.h"
#include "app_control_common_function.h"
#include "app_control_global_state.h"
#include "app_control_adjust_implement.h"
#include "app_control_timing_back.h"
#include "trace.h"


/**************** RPDO from AMR1 ****************/
void control_RPDO1_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm1)
    {
        return;
    }
//    APP_TRACE("Enter control_board_RPDO1_callback...\r\n");
    memcpy(&g_arm1_rpdo1_data, pData, CONTROL_RPDO_SIZE);

    if (STATUS_BLOOD_PRIMING == g_run_status
        || STATUS_COLLECT_DRAW == g_run_status
        || STATUS_COLLECT_BACK == g_run_status)
    {// record the speed
        g_uart_cycle_back.ac_speed = g_arm1_rpdo1_data.AC_pump_current_speed 
                                        * g_ratio_parse.ratio_ac;
        g_uart_cycle_back.draw_speed = g_arm1_rpdo1_data.draw_pump_current_speed 
                                        * g_ratio_parse.ratio_draw;
        g_uart_cycle_back.back_speed = g_arm1_rpdo1_data.feed_back_pump_current_speed 
                                        * g_ratio_parse.ratio_feedback;
    }
}

void control_RPDO2_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm1)
    {
        return;
    }
//    APP_TRACE("Enter control_board_RPDO2_callback...\r\n");
    memcpy(&g_arm1_rpdo2_data, pData, CONTROL_RPDO_SIZE);
}

void control_RPDO3_callback(u8 *pData, u16 len)
{
    INT8U err;
    INNER_ERROR_MSG l_errMsg;
    
    if (!g_comm_sync_state.arm1)
    {
        return;
    }
   // APP_TRACE("Enter control_board_RPDO3_callback...\r\n");
    
//    APP_TRACE("pressure=%2x\r\n", g_arm1_rpdo3_data.blood_pressure.bit.pressure);
    memcpy(&g_arm1_rpdo3_data, pData, CONTROL_RPDO_SIZE);

    /* handle the ac bubble signal */
    if (g_arm1_rpdo3_data.liquid_level.bit.AC_bubble 
        && (STATUS_BLOOD_PRIMING == g_run_status 
        || STATUS_COLLECT_DRAW == g_run_status 
        || STATUS_COLLECT_BACK == g_run_status))
    {
        APP_TRACE("AC Bubble happened...\r\n");
        l_errMsg.source.all = ERR_SOURCE_ARM0;
        l_errMsg.taskNo = arm0_get_cur_prio();
        l_errMsg.action = ERROR_PAUSE_PROGRAM;
        l_errMsg.errcode1 = 0x33;
        l_errMsg.errcode2 = 0x00;
        l_errMsg = l_errMsg;
        post_error_msg(l_errMsg);
    }

    /* handle the up and low flow level signal */
    if (FLOW_WAIT_LOWER_LEVEL_SIGNAL == get_flow_wait_status() 
        && g_arm1_rpdo3_data.liquid_level.bit.low_level)
    {
        err = OSSemPost(g_canopen_lower_liquid_event);
        assert_param(OS_ERR_NONE == err);
        APP_TRACE(" 1  post  g_canopen_lower_liquid_event...\r\n");
    }
    else if (FLOW_WAIT_UPPER_LEVEL_SIGNAL == get_flow_wait_status() 
        && g_arm1_rpdo3_data.liquid_level.bit.high_level)
    {
        err = OSSemPost(g_canopen_upper_liquid_event);
        assert_param(OS_ERR_NONE == err);
        //APP_TRACE(" 2  post  g_canopen_lower_liquid_event...\r\n");
    }
    else if (FLOW_WAIT_LOWER_LEVEL_NOSIGNAL == get_flow_wait_status()//?????
        && !g_arm1_rpdo3_data.liquid_level.bit.low_level)    // debug
    {
        err = OSSemPost(g_canopen_lower_liquid_event);
        assert_param(OS_ERR_NONE == err);
        APP_TRACE(" 3  post  g_canopen_lower_liquid_event...\r\n");
    }
    // add by wenquan
    else
    {
       // APP_TRACE(" 4  post  g_canopen_lower_liquid_event...\r\n");
    }

    // reumse the plt valve for adjust air exit and overflow
    if (OVERFLOW_ON_HANDLE == get_overflow_status() 
        && !g_arm1_rpdo3_data.liquid_level.bit.low_level)
    {
        set_adjust_event(OVER_FLOW_END_EVENT);
        set_overflow_status(OVERFLOW_OVER);
    }
    else if(REMOVE_AIR_ON_HANDLE == get_overflow_status()
        && !g_arm1_rpdo3_data.liquid_level.bit.low_level)
    {
        set_adjust_event(AIR_REMOVE_END_EVENT);
        set_overflow_status(REMOVE_AIR_OVER);
    }

    /* handle the draw/feedbck pressure */
    // record the pressure
    if (STATUS_COLLECT_DRAW == g_run_status)
    {
     //   APP_TRACE("1->STATUS_COLLECT_DRAW == g_run_status\r\n");
        g_uart_cycle_back.draw_pressure = g_arm1_rpdo3_data.blood_pressure.bit.pressure;
    }
    else if (STATUS_COLLECT_BACK == g_run_status)
    {
       // APP_TRACE("2->STATUS_COLLECT_BACK == g_run_status\r\n");
        g_uart_cycle_back.back_pressure = g_arm1_rpdo3_data.blood_pressure.bit.pressure;
    }
    // handle the pressure
    if (STATUS_COLLECT_DRAW == g_run_status && g_arm1_rpdo3_data.blood_pressure.bit.sign
        && g_arm1_rpdo3_data.blood_pressure.bit.pressure < g_cur_program_param.draw_pressure_limit)
    {
        APP_TRACE("3->STATUS_COLLECT_DRAW == g_run_status\r\n");
        if (FLOW_PRESSURE_LOW_HANDLED == get_flow_pressure_status())
        {// resume from draw pressure low
            set_adjust_event(PRESSURE_LOW_END_EVENT);
        }
        //set_flow_pressure_status(FLOW_PRESSURE_NORMAL);
    }
    else if (STATUS_COLLECT_BACK == g_run_status && !g_arm1_rpdo3_data.blood_pressure.bit.sign
        && g_arm1_rpdo3_data.blood_pressure.bit.pressure < g_cur_program_param.back_pressure_limit)
    {
        APP_TRACE("4->STATUS_COLLECT_BACK == g_run_status\r\n");
        if (FLOW_PRESSURE_HIGH_HANDLED == get_flow_pressure_status())
        {// resume from back pressure high
            set_adjust_event(PRESSURE_HIGH_END_EVENT);
        }
        //set_flow_pressure_status(FLOW_PRESSURE_NORMAL);
    }
#if 0    
   else if (FLOW_PRESSURE_NORMAL==get_flow_pressure_status() && STATUS_COLLECT_DRAW==g_run_status
        && g_arm1_rpdo3_data.blood_pressure.bit.pressure > g_cur_program_param.draw_pressure_limit)
#else
     else if (FLOW_PRESSURE_NORMAL==get_flow_pressure_status() && STATUS_COLLECT_DRAW==g_run_status)
#endif
    {// draw pressure is too low
       // APP_TRACE("low draw pressure=%d\r\n", g_arm1_rpdo3_data.blood_pressure.bit.pressure);
        update_pressure_now(g_arm1_rpdo3_data.blood_pressure.bit.pressure);
        //response_low_pressure();
    }

#if 0 
    else if (FLOW_PRESSURE_NORMAL==get_flow_pressure_status() && STATUS_COLLECT_BACK==g_run_status 
        && g_arm1_rpdo3_data.blood_pressure.bit.pressure > g_cur_program_param.back_pressure_limit)
#else
    else if (FLOW_PRESSURE_NORMAL==get_flow_pressure_status() && STATUS_COLLECT_BACK==g_run_status)
#endif

    {// back pressure is too high
       // APP_TRACE("high back pressure=%d\r\n", g_arm1_rpdo3_data.blood_pressure.bit.pressure);
        update_pressure_now(g_arm1_rpdo3_data.blood_pressure.bit.pressure);
       // response_high_pressure();
    }
    else if (STATUS_SUIT_CHECKING == g_run_status)
    {
        if ((TEST_CANAL_WAIT_HIGH_PRESSURE == get_test_canal_pressure_status()
            && TEST_HIGH_PRESSURE <= g_arm1_rpdo3_data.blood_pressure.bit.pressure //310
            && !g_arm1_rpdo3_data.blood_pressure.bit.sign)//
            
            || (TEST_CANAL_WAIT_FIRST_PRESSURE == get_test_canal_pressure_status()
            && TEST_FIRST_PRESSURE >= g_arm1_rpdo3_data.blood_pressure.bit.pressure //200
            && !g_arm1_rpdo3_data.blood_pressure.bit.sign)
            
            || (TEST_CANAL_WAIT_SECOND_PRESSURE == get_test_canal_pressure_status()
            && TEST_SECOND_PRESSURE >= g_arm1_rpdo3_data.blood_pressure.bit.pressure//100 
            && !g_arm1_rpdo3_data.blood_pressure.bit.sign)
            
            || (TEST_CANAL_WAIT_LOW_PRESSURE == get_test_canal_pressure_status()
            && TEST_LOW_PRESSURE <= g_arm1_rpdo3_data.blood_pressure.bit.pressure //-250
            && g_arm1_rpdo3_data.blood_pressure.bit.sign)
            
            || (TEST_CANAL_WAIT_NORMAL_PRESSURE == get_test_canal_pressure_status()
            && TEST_NORMAL_PRESSURE <= g_arm1_rpdo3_data.blood_pressure.bit.pressure //???
            && !g_arm1_rpdo3_data.blood_pressure.bit.sign))
            
        {
        
            APP_TRACE("OSSemPost pressure_event pressure = %d state = %d\r\n",g_arm1_rpdo3_data.blood_pressure.bit.pressure,get_test_canal_pressure_status());
            err = OSSemPost(g_test_canal_pressure_event);
            assert_param(OS_ERR_NONE == err);
            
        }

    }
    else
    {
       // APP_TRACE(" un-normal branch---->g_run_status = %d\r\n", g_run_status);
    }

    /* hanle feedback pump exception */
    if (g_arm1_rpdo3_data.feedback_pump)
    {// feedback pump is error
        APP_TRACE("feedback pump error happened...\r\n");
        l_errMsg.source.all = ERR_SOURCE_ARM1;
        l_errMsg.taskNo = arm0_get_cur_prio();
        l_errMsg.action = ERROR_STOP_PROGRAM;
        l_errMsg.errcode1 = 0x34;
        l_errMsg.errcode2 = 0x00;
        l_errMsg = l_errMsg;
        post_error_msg(l_errMsg);
    }
/*
    APP_TRACE("get_flow_pressure_status() = %d\r\n",get_flow_pressure_status());
    APP_TRACE("g_run_status = %d\r\n", g_run_status);
    APP_TRACE("************************cycle   end******************************* \r\n");
 */   
 
}

void control_RPDO4_callback(u8 *pData, u16 len)
{// ARM1 ACK/NAK recv
    ARM_ANSWER_CMD l_arm1_cmd;
    INT8U err;
    
//    APP_TRACE("Enter control_board_RPDO4_callback...\r\n");
    memcpy(&l_arm1_cmd, pData, len);

    if (pData[0] != get_cmd_fn(SEND_FRAME_FN, ARM1_DEST))
    {// FN not match return
        return;
    }
    if (ACK == l_arm1_cmd.answer)
    {// ACK answer
        OSFlagPost(g_arm1_acknak_flag, FLAG_ACK_BIT, OS_FLAG_SET, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else if (NAK == l_arm1_cmd.answer)
    {// NAK answer
        OSFlagPost(g_arm1_acknak_flag, FLAG_NAK_BIT, OS_FLAG_SET, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else
    {
        APP_TRACE("RPDO4 format error!\r\n");
    }
}

void control_RPDO5_callback(u8 *pData, u16 len)
{
    INNER_ERROR_MSG l_errMsg;
    ARM_RPDO_CMD l_cmd;
    
    if (!g_comm_sync_state.arm1)
    {
        return;
    }
    
    memcpy(&l_cmd, pData, sizeof(l_cmd));
    
    APP_TRACE("Enter control_board_RPDO5_callback...\r\n");
    
    l_errMsg.source.bit.board_source = ERR_SOURCE_ARM1;
    l_errMsg.source.bit.slave_error = SLAVE_ERR_TASK;
    if (ERR_TASK_WARING == l_cmd.order)
    {
        l_errMsg.action = ERROR_IGNORE;
    }
    else if (ERR_TASK_ERROR == l_cmd.order)
    {
        l_errMsg.action = ERROR_STOP_PROGRAM;
    }
    else
    {
        APP_TRACE("RPDO5 format error!\r\n");
        return;
    }
    l_errMsg.taskNo = (l_cmd.param1&0xF0)>>4;
    l_errMsg.errcode1 = l_cmd.param2;
    l_errMsg.errcode2 = l_cmd.fn+(l_cmd.param5<<4);   /* errcode2 used to save fn */
    post_error_msg(l_errMsg);
    
}

void control_RPDO6_callback(u8 *pData, u16 len)
{
    INNER_ERROR_MSG l_errMsg;

    if (!g_comm_sync_state.arm1)
    {// ARM1 canopen communicate not init
        return;
    }

    if (judge_array_null(pData, (u8)len))
    {// ARM1 no error
        return;
    }
    
  //  APP_TRACE("Enter control_board_RPDO6_callback...\r\n");
    memcpy(&g_arm1_rpdo6_data, pData, CONTROL_RPDO_SIZE);

    l_errMsg.source.bit.board_source = ERR_SOURCE_ARM1;
    l_errMsg.source.bit.slave_error = SLAVE_ERR_HARDFAULT;
    l_errMsg.action = ERROR_STOP_PROGRAM;
    l_errMsg.errcode1 = 0x1A;
    l_errMsg.errcode2 = 0x00;
    l_errMsg = l_errMsg;
    //post_error_msg(l_errMsg);//mask by wq
}

/**************** RPDO from AMR2 ****************/
void control_RPDO7_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm2)
    {
        return;
    }
    
//    APP_TRACE("Enter control_board_RPDO7_callback...\r\n");    
    memcpy(&g_arm2_rpdo1_data, pData, CONTROL_RPDO_SIZE);
}

void control_RPDO8_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm2)
    {
        return;
    }
    
   APP_TRACE("283  Enter control_board_RPDO8_callback...\r\n");

   DumpData(pData,8);//add by Big.Man
   
    memcpy(&g_arm2_rpdo2_data, pData, CONTROL_RPDO_SIZE);
}

void control_RPDO9_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm2)
    {
        return;
    }
    
    APP_TRACE("383 Enter control_board_RPDO9_callback...\r\n");
    DumpData(pData,8);//add by Big.Man
    
    memcpy(&g_arm2_rpdo3_data, pData, CONTROL_RPDO_SIZE);

    if (0x01 == g_arm2_rpdo3_data.RBC_detector 
        && (STATUS_COLLECT_DRAW == g_run_status || STATUS_COLLECT_BACK == g_run_status) 
        && (OVERFLOW_ON_HANDLE!=get_flow_pressure_status() && REMOVE_AIR_ON_HANDLE!=get_flow_pressure_status()))
    {
        APP_TRACE("RBC detector happened...\r\n");
        
        set_adjust_event(OVER_FLOW_EVENT);
    }
}

void control_RPDO10_callback(u8 *pData, u16 len)
{    
    ARM_ANSWER_CMD l_arm2_cmd;
    INT8U err;
    
//    APP_TRACE("Enter control_board_RPDO10_callback...\r\n");
    memcpy(&l_arm2_cmd, pData, len);

    if (pData[0] != get_cmd_fn(SEND_FRAME_FN, ARM2_DEST))
    {// FN not match return
        return;
    }

    if (ACK == l_arm2_cmd.answer)
    {// ACK answer
        OSFlagPost(g_arm2_acknak_flag, FLAG_ACK_BIT, OS_FLAG_SET, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else if (NAK == l_arm2_cmd.answer)
    {// NAK answer
        OSFlagPost(g_arm2_acknak_flag, FLAG_NAK_BIT, OS_FLAG_SET, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else
    {
        APP_TRACE("RPDO10 format error!\r\n");
    }
}

void control_RPDO11_callback(u8 *pData, u16 len)
{
    INNER_ERROR_MSG l_errMsg;
    ARM_RPDO_CMD l_cmd;
    
    if (!g_comm_sync_state.arm2)
    {
        return;
    }
    
    memcpy(&l_cmd, pData, sizeof(l_cmd));
    APP_TRACE("Enter control_board_RPDO11_callback...\r\n");
    
    l_errMsg.source.bit.board_source = ERR_SOURCE_ARM2;
    l_errMsg.source.bit.slave_error = SLAVE_ERR_TASK;
    if (ERR_TASK_WARING == l_cmd.order)
    {
        l_errMsg.action = ERROR_IGNORE;
    }
    else if (ERR_TASK_ERROR == l_cmd.order)
    {
        l_errMsg.action = ERROR_STOP_PROGRAM;
    }
    else
    {
        APP_TRACE("RPDO11 format error!\r\n");
        return;
    }
    l_errMsg.taskNo = (l_cmd.param1&0xF0)>>4;
    l_errMsg.errcode1 = l_cmd.param2;
    l_errMsg.errcode2 = l_cmd.fn+(l_cmd.param5<<4);
    post_error_msg(l_errMsg);
}

void control_RPDO12_callback(u8 *pData, u16 len)
{
    INNER_ERROR_MSG l_errMsg;

    if (!g_comm_sync_state.arm2)
    {// ARM2 canopen communication not init
        return;
    }
    
    if (judge_array_null(pData, (u8)len))
    {// ARM2 no error
        return;
    }
    
    APP_TRACE("Enter control_board_RPDO12_callback...\r\n");
    memcpy(&g_arm2_rpdo6_data, pData, CONTROL_RPDO_SIZE);
    
    l_errMsg.source.bit.board_source = ERR_SOURCE_ARM2;
    l_errMsg.source.bit.slave_error = SLAVE_ERR_HARDFAULT;
    l_errMsg.action = ERROR_STOP_PROGRAM;
    l_errMsg.errcode1 = 0x1B;
    l_errMsg.errcode2 = 0x00;
    l_errMsg = l_errMsg;
    post_error_msg(l_errMsg);
}

/**************** RPDO from AMR3 ****************/
void control_RPDO13_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm3)
    {
        return;
    }
    
    APP_TRACE("Enter control_board_RPDO13_callback...\r\n");
    memcpy(&g_arm3_rpdo1_data, pData, CONTROL_RPDO_SIZE);
}

void control_RPDO14_callback(u8 *pData, u16 len)
{
    if (!g_comm_sync_state.arm3)
    {
        return;
    }
    
    APP_TRACE("Enter control_board_RPDO14_callback...\r\n");
    memcpy(&g_arm3_rpdo2_data, pData, CONTROL_RPDO_SIZE);
}
void control_RPDO15_callback(u8 *pData, u16 len)
{
    INT8U err;
    INNER_ERROR_MSG l_errMsg;
    
    if (!g_comm_sync_state.arm3)
    {
        return;
    }
    
//    APP_TRACE("Enter control_board_RPDO15_callback...\r\n");
    memcpy(&g_arm3_rpdo3_data, pData, CONTROL_RPDO_SIZE);

    /* handle the centrifuge belt pressure */
    if (FLOW_WAIT_CENTRI_PRESSURE == get_flow_wait_status()
        && g_arm3_rpdo3_data.centrifuge_motor_pressure.bit.pressure > SEPARATE_BELT_FULL)
    {// TODO: check the separate_belt_pressure
         err = OSSemPost(g_canopen_centri_full_event);
         assert_param(OS_ERR_NONE == err);
    }
    else if (FLOW_WAIT_BELT_EMPTY == get_flow_wait_status()
        && g_arm3_rpdo3_data.centrifuge_motor_pressure.bit.pressure < SEPARATE_BELT_EMPTY)
    {
         err = OSSemPost(g_canopen_centri_empty_event);
         assert_param(OS_ERR_NONE == err);
    }

    /* handle the liquid weeping */
   //if (g_arm3_rpdo3_data.weeping_detector)
   if(0)
    {// feedback pump is error
        APP_TRACE("Liquid Weeping is happened...\r\n");
        
        l_errMsg.source.all = ERR_SOURCE_ARM0;
        l_errMsg.taskNo = arm0_get_cur_prio();
        l_errMsg.action = ERROR_STOP_PROGRAM;
        l_errMsg.errcode1 = 0x35;
        l_errMsg.errcode2 = 0x00;
        l_errMsg = l_errMsg;
        post_error_msg(l_errMsg);
    }

    /* handle the centrifuge exception */
    if (g_arm3_rpdo3_data.centrifuge)
    {
        APP_TRACE("centrifuge exception is happened...\r\n");
        
        l_errMsg.source.all = ERR_SOURCE_ARM3;
        l_errMsg.taskNo = arm0_get_cur_prio();
        l_errMsg.action = ERROR_STOP_PROGRAM;
        l_errMsg.errcode1 = 0x36;
        l_errMsg.errcode2 = 0x00;
        l_errMsg = l_errMsg;
        post_error_msg(l_errMsg);
    }
    
}

void control_RPDO16_callback(u8 *pData, u16 len)
{
    ARM_ANSWER_CMD l_arm3_cmd;
    INT8U err;
    
//    APP_TRACE("Enter control_board_RPDO16_callback...\r\n");
    memcpy(&l_arm3_cmd, pData, len);

    if (pData[0] != get_cmd_fn(SEND_FRAME_FN, ARM3_DEST))
    {// FN not match return
        return;
    }

    if (ACK == l_arm3_cmd.answer)
    {// ACK answer
        OSFlagPost(g_arm3_acknak_flag, FLAG_ACK_BIT, OS_FLAG_SET, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else if (NAK == l_arm3_cmd.answer)
    {// NAK answer
        OSFlagPost(g_arm3_acknak_flag, FLAG_NAK_BIT, OS_FLAG_SET, &err);
        assert_param(OS_ERR_NONE == err);
    }
    else
    {
        APP_TRACE("RPDO16 format error!\r\n");
    }
}

void control_RPDO17_callback(u8 *pData, u16 len)
{
    INNER_ERROR_MSG l_errMsg;
    ARM_RPDO_CMD l_cmd;
    
    if (!g_comm_sync_state.arm3)
    {
        return;
    }
    
    memcpy(&l_cmd, pData, sizeof(l_cmd));
    APP_TRACE("Enter control_board_RPDO17_callback...\r\n");
    
    l_errMsg.source.bit.board_source = ERR_SOURCE_ARM3;
    l_errMsg.source.bit.slave_error = SLAVE_ERR_TASK;
    if (ERR_TASK_WARING == l_cmd.order)
    {
        l_errMsg.action = ERROR_IGNORE;
    }
    else if (ERR_TASK_ERROR == l_cmd.order)
    {
        l_errMsg.action = ERROR_STOP_PROGRAM;
    }
    else
    {
        APP_TRACE("RPDO17 format error!\r\n");
        return;
    }
    l_errMsg.taskNo = (l_cmd.param1&0xF0)>>4;
    l_errMsg.errcode1 = l_cmd.param2;
    l_errMsg.errcode2 = l_cmd.fn+(l_cmd.param5<<4);
    post_error_msg(l_errMsg);
}
void control_RPDO18_callback(u8 *pData, u16 len)
{
    INNER_ERROR_MSG l_errMsg;

    if (!g_comm_sync_state.arm3)
    {// ARM3 canopen communication not init
        return;
    }

    if (judge_array_null(pData, (u8)len))
    {// ARM3 no error
        return;
    }
    
    APP_TRACE("Enter control_board_RPDO18_callback...\r\n");
    memcpy(&g_arm3_rpdo6_data, pData, CONTROL_RPDO_SIZE);

    l_errMsg.source.bit.board_source = ERR_SOURCE_ARM3;
    l_errMsg.source.bit.slave_error = SLAVE_ERR_HARDFAULT;
    l_errMsg.action = ERROR_STOP_PROGRAM;
    l_errMsg.errcode1 = 0x1C;
    l_errMsg.errcode2 = 0x00;
    l_errMsg = l_errMsg;
    l_errMsg = l_errMsg;
    post_error_msg(l_errMsg);
}

 
