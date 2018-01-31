/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_poweron_test.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/12
 * Description        : This file contains the system poweron test program
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/12 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_poweron_test.h"
#include "app_control_board.h"
#include "app_cmd_send.h"
#include "struct_control_board_canopen.h"
#include "app_control_common_function.h"
#include "app_control_monitor.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"
#include "trace.h"

#define WAIT_DLY_TIME           100u /* ms */
#define WAIT_MAX_COUNT          500u  /* 50*100ms  change by Big.Man for test (50 -> 500)*/

// test centrifuge lid
uint8_t test_centrifuge_lid(void);
// test weeping detector
uint8_t test_weeping_detector(void);
// test power control
uint8_t test_power_control(void);
// test cartridge position
uint8_t test_cartridge_pos(void);
// test vavle function
uint8_t test_valve_func(void);
// init all pumps  pos
uint8_t test_all_pumps_pos(void);

uint8_t test_comm_state(void)
{
    ARM1_TPDO3 l_arm1tpdo;
    uint8_t l_armblock[8] = {0};

	#if 1	
    uint8_t l_count;
    INNER_ERROR_MSG l_errMsg;

    APP_TRACE("test_comm_state \r\n");
    while (0x01 != g_comm_sync_state.uart)
    {// wait uart communication sync
        OSTimeDly(WAIT_DLY_TIME);
        if (++l_count == WAIT_MAX_COUNT)
        {
            l_errMsg.source.all = ERR_SOURCE_UART;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_IGNORE;
            l_errMsg.errcode1 = 0x0A;
            l_errMsg.errcode2 = 0x00;
            post_error_msg(l_errMsg);
            return 1;
        }
    }
#endif

    APP_TRACE("enter <test_comm_state>\r\n");

    memset(&l_arm1tpdo, 0x00, sizeof(l_arm1tpdo));
    l_arm1tpdo.fn = get_current_fn(ARM1_DEST, FN_CREATE);
    l_arm1tpdo.module_indicate = 0xA5;

    memcpy(l_armblock, &l_arm1tpdo, sizeof(l_arm1tpdo));
    arm1_send_frame(l_armblock, 3, 1);
    arm2_send_frame(l_armblock, 7, 1);
    arm3_send_frame(l_armblock, 11, 1);
#if 1   
    while (!g_comm_sync_state.arm1 || !g_comm_sync_state.arm2 || !g_comm_sync_state.arm3)
    {// wait slave communication sync
        OSTimeDly(WAIT_DLY_TIME);
    }
#endif
    set_run_status(STATUS_COMM_OK);
    APP_TRACE("test_comm_state OK\r\n");
    return 0;
}

uint8_t test_power_on(void)
{
    u8 l_ret = 0;
    INT8U err;
    UART_FRAME l_frame;
    
    APP_TRACE("enter <test_power_on>\r\n");
    
    memset(&l_frame, 0x00, sizeof(l_frame));

    OSSemPend(g_uart_poweron_test_event, 0,&err);
    assert_param(OS_ERR_NONE == err);
    APP_TRACE("pended g_uart_poweron_test_event\r\n");

    l_ret = POWERON_NO_ERROR;
    if (test_centrifuge_lid())
    {
       // l_ret = POWERON_CENTRI_LID;//0x01 ‘› ±≤ª≤‚ ‘≤÷√≈
    }
    if (!l_ret && test_weeping_detector())
    {
        //l_ret = POWERON_WEEPING;//0x02
    }
    if (!l_ret && test_power_control())
    {
        //l_ret = POWERON_POWER_CTRL;//x0x03
    }
	/*
	if (!l_ret && test_cartridge_pos())
    {
        //l_ret = POWERON_CARTRI_POS;//0x04
    }
    */
    if (!l_ret && test_valve_func())
    {
        l_ret = POWERON_VALVE_FUN;//0x05
    }
    if (!l_ret && test_cartridge_pos())
    {
        l_ret = POWERON_CARTRI_POS;//0x04
    } 
    
    if (!l_ret && test_all_pumps_pos())
    {
        l_ret = POWERON_PUMP_INIT;//0x06
    }
    l_frame.module_flag = 0x10;
    l_frame.event_type = l_ret;
    
    APP_TRACE("l_ret = 0x%x\r\n",l_ret);
    APP_TRACE("test_power_on over\r\n");
    
    uart_send_frame(l_frame);

    return l_ret;
}


uint8_t test_centrifuge_lid(void)
{// TODO: unsure need check
	APP_TRACE("Enter test_centrifuge_lid function\r\n");

    if (!g_arm3_rpdo3_data.sensor.bit.door_hoare)
    {// door is open
       // return 1;//mask by wq because jixie
    }
    // lock the door
    if (ctrl_door_lock(TYPE_DOOR_LOCK))
    {
        return 1;
    }
    // unlock the door
    if (ctrl_door_lock(TYPE_DOOR_UNLOCK))
    {
        return 1;
    }
    
    return 0;
}

uint8_t test_weeping_detector(void)
{// TODO:unsure need check
	APP_TRACE("Enter test_weeping_detector function\r\n");

    if (g_arm3_rpdo6_data.sensor_error.bit.weeping_sensor)
    {
        return 1;
    }
    
    return 0;
}

uint8_t test_power_control(void)
{// TODO: unsure
	APP_TRACE("Enter test_power_control function\r\n");

    if (g_arm3_rpdo6_data.voltage_error.all)
    {
        return 1;
    }

    return 0;
}

uint8_t test_cartridge_pos(void)
{// TODO: unsure
	APP_TRACE("Enter test_cartridge_pos function\r\n");

    // unload the canal
    return ctrl_unload_canal();
}

uint8_t test_valve_func(void)
{// TODO: unsure
    CTRL_VALVE_TYPE l_type;
    CTRL_VALVE_DIR l_dir;

	APP_TRACE("Enter test_valve_func function\r\n");

    memset(&l_type, 0x07, sizeof(l_type));
 
    // all valve turn left
    APP_TRACE("TO LEFT\r\n");
    
    l_dir.rbc_valve = VALVE_DIR_LEFT;//00
    l_dir.pla_valve = VALVE_DIR_LEFT;
    l_dir.plt_valve = VALVE_DIR_LEFT;
    if (ctrl_turn_valve(l_type, l_dir))
    {
        APP_TRACE("TO LEFT RETURN\r\n");
        return 1;
    }

    OSTimeDlyHMSM(0, 0, 2, 0);
    // all valve turn right
    APP_TRACE("TO RIGHT\r\n");
    l_dir.rbc_valve = VALVE_DIR_RIGHT;//0x02
    l_dir.pla_valve = VALVE_DIR_RIGHT;
    l_dir.plt_valve = VALVE_DIR_RIGHT;
    if (ctrl_turn_valve(l_type, l_dir))
    {
    
        APP_TRACE("TO RIGHT RETURN\r\n");
        return 1;
    }
    
    OSTimeDlyHMSM(0, 0, 2, 0);

    
    // all valve turn mid
    APP_TRACE("TO MID\r\n");
    l_dir.rbc_valve = VALVE_DIR_MIDDLE;//0x01
    l_dir.pla_valve = VALVE_DIR_MIDDLE;
    l_dir.plt_valve = VALVE_DIR_MIDDLE;
    if (ctrl_turn_valve(l_type, l_dir))
    {
        APP_TRACE("TO MID RETURN\r\n");
        return 1;
    }
    
    return 0;
}

uint8_t test_all_pumps_pos(void)
{
    CTRL_PUMP_TYPE l_pump;

	APP_TRACE("Enter test_all_pumps_pos function\r\n");

    memset(&l_pump, 0x00, sizeof(l_pump));
    
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;

    //return 0;
    return ctrl_init_pump_pos(l_pump);
    
   // return ctrl_init_pump_pos(l_pump);//unmask by wq
}


