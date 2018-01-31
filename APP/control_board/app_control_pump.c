/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_pump.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/05
 * Description        : This file contains the motor error created by ARM1,ARM2,ARM3
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/05 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_pump.h"
#include "stm32f2xx.h"
#include "app_control_board.h"
#include "app_control_monitor.h"
#include "app_control_common_function.h"
#include "app_control_global_state.h"
#include "app_cmd_send.h"
#include "trace.h"

#define LOAD_CANAL_PUMP_SPEED           50    /* 50rpm */
#define UNLOAD_CANAL_PUMP_SPEED         50    /* 50rpm */

static u8 wait_pump_stopped(CTRL_PUMP_TYPE type);
static u8 wait_valve_end(CTRL_VALVE_TYPE type, CTRL_VALVE_DIR dir);
#if 0
static u8 wait_centrifuge_end(u16 speed);
static u8 wait_centrifuge_stopped(void);
#endif
static u8 wait_lock_end(CTRL_LOCK_TYPE lock);
static u8 wait_cassette_end(CTRL_CASSETTE_TYPE dir);

static uint8_t set_pump_distance_type(CTRL_PUMP_TYPE type, CTRL_DISTANCE_TYPE distance);


void init_pump_param(CTRL_PUMP_TYPE *type, CTRL_PUMP_SPEED *speed, 
                        CTRL_PUMP_DISTANCE *distance, CTRL_PUMP_COUNT *count)
{
    memset(type, 0x00, sizeof(CTRL_PUMP_TYPE));
    memset(speed, 0x00, sizeof(CTRL_PUMP_SPEED));
    memset(distance, 0x00, sizeof(CTRL_PUMP_DISTANCE));
    memset(count, 0x1F, sizeof(CTRL_PUMP_COUNT));
}

void init_valve_param(CTRL_VALVE_TYPE *type, CTRL_VALVE_DIR *dir)
{
    memset(type, 0x00, sizeof(CTRL_VALVE_TYPE));
    memset(dir, 0x00, sizeof(CTRL_VALVE_DIR));
}


uint8_t ctrl_start_pump(CTRL_PUMP_TYPE type, CTRL_PUMP_SPEED speed, 
                        CTRL_PUMP_DISTANCE distance, CTRL_PUMP_COUNT count)
{
    ARM1_TPDO1 l_arm1tpdo1;
    ARM1_TPDO2 l_arm1tpdo2;
    ARM1_TPDO3 l_arm1tpdo3;
    ARM2_TPDO1 l_arm2tpdo1;
    ARM2_TPDO2 l_arm2tpdo2;
    ARM2_TPDO3 l_arm2tpdo3;
    uint8_t l_arm1block[24] = {0};
    uint8_t l_arm2block[24] = {0};
    

    memset(&l_arm1tpdo1, 0x00, sizeof(l_arm1tpdo1));
    memset(&l_arm1tpdo2, 0x00, sizeof(l_arm1tpdo2));
    memset(&l_arm1tpdo3, 0x00, sizeof(l_arm1tpdo3));
    memset(&l_arm2tpdo1, 0x00, sizeof(l_arm2tpdo1));
    memset(&l_arm2tpdo2, 0x00, sizeof(l_arm2tpdo2));
    memset(&l_arm2tpdo3, 0x00, sizeof(l_arm2tpdo3));
/*
    if (type.ac_pump || type.draw_pump || type.feedBack_pump)
    {// ARM1 pump start
        l_arm1tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        set_cmd_fn(START_PUMP_FN, ARM1_DEST, l_arm1tpdo3.fn);
        l_arm1tpdo3.module_indicate = 0x01;
        l_arm1tpdo3.order_indicate.all = 0xff;
        
        if (type.ac_pump)
        {
            l_arm1tpdo1.AC_pump_speed_limit = speed.ac_speed;
            l_arm1tpdo2.AC_pump_need_distance = distance.ac_distance;
            l_arm1tpdo3.order_indicate.bit.AC_pump = 0x01;
            l_arm1tpdo3.param1.bit.AC_pump = count.ac_count;
        }
        if (type.draw_pump)
        {
            l_arm1tpdo1.draw_pump_speed_limit = speed.draw_speed;
            l_arm1tpdo2.draw_pump_need_distance = distance.draw_distance;
            l_arm1tpdo3.order_indicate.bit.draw_pump = 0x01;
            l_arm1tpdo3.param1.bit.draw_pump = count.draw_count;
        }
        if (type.feedBack_pump)
        {
            l_arm1tpdo1.feed_back_pump_speed_limit = speed.feedBack_speed;
            l_arm1tpdo2.feed_back_pump_need_distance = distance.feedBack_distance;
            l_arm1tpdo1.pump_direction.bit.feedBack_pump = type.feedBack_dir;
            l_arm1tpdo3.order_indicate.bit.feedBack_pump = 0x01;
            l_arm1tpdo3.param1.bit.feedBack_pump = count.feedBack_count;
        }
        memcpy(l_arm1block, &l_arm1tpdo1, sizeof(l_arm1tpdo1));
        memcpy(l_arm1block+8, &l_arm1tpdo2, sizeof(l_arm1tpdo2));
        memcpy(l_arm1block+16, &l_arm1tpdo3, sizeof(l_arm1tpdo3));

        
        DUMP_DATA(l_arm1block,8);
        DUMP_DATA(l_arm1block+16,8);
        // send arm1 tpdo
      //  arm1_send_frame(l_arm1block, 1, 3);//mask by wq
        
        arm1_send_frame(l_arm1block, 1, 1);//tpdo1
       // arm1_send_frame(l_arm1block, 2, 1);//tpdo2
        arm1_send_frame(l_arm1block+16, 3, 1);//tpdo3
    }
*/
    if (type.pla_pump || type.plt_pump)
    {// ARM2 pump start
        l_arm2tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
        set_cmd_fn(START_PUMP_FN, ARM2_DEST, l_arm2tpdo3.fn);
        l_arm2tpdo3.module_indicate = 0x01;
        l_arm2tpdo3.order_indicate.all = 0xff;

        if (type.pla_pump)
        {
            l_arm2tpdo1.plasma_pump_speed_limit = speed.pla_speed;
            l_arm2tpdo2.plasma_pump_need_distance = distance.pla_distance;
            l_arm2tpdo3.order_indicate.bit.plasma_valve_or_pump = 0x01;
            l_arm2tpdo3.param1.bit.plasma_pump = count.pla_count;
        }
        if (type.plt_pump)
        {
            l_arm2tpdo1.PLT_pump_speed_limit = speed.plt_speed;
            l_arm2tpdo2.plasma_pump_need_distance = distance.plt_distance;
            l_arm2tpdo3.order_indicate.bit.PLT_valve_or_pump = 0x01;
            l_arm2tpdo3.param1.bit.PLT_pump = count.plt_count;
        }
        memcpy(l_arm2block, &l_arm2tpdo1, sizeof(l_arm2tpdo1));
        memcpy(l_arm2block+8, &l_arm2tpdo2, sizeof(l_arm2tpdo2));
        memcpy(l_arm2block+16, &l_arm2tpdo3, sizeof(l_arm2tpdo3));
        // send arm2 tpdo
       // arm2_send_frame(l_arm2block, 5, 3);//mask by wq
        
      //  arm2_send_frame(l_arm2block, 5, 1);//tpdo1
      //  arm2_send_frame(l_arm2block+8, 6, 1);//tpdo2
      //  arm2_send_frame(l_arm2block+16, 7, 1);//tpdo3
    }


    
    if (type.ac_pump || type.draw_pump || type.feedBack_pump)
    {// ARM1 pump start
        l_arm1tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        set_cmd_fn(START_PUMP_FN, ARM1_DEST, l_arm1tpdo3.fn);
        l_arm1tpdo3.module_indicate = 0x01;
        l_arm1tpdo3.order_indicate.all = 0xff;
        
        if (type.ac_pump)
        {
            l_arm1tpdo1.AC_pump_speed_limit = speed.ac_speed;
            l_arm1tpdo2.AC_pump_need_distance = distance.ac_distance;
            l_arm1tpdo3.order_indicate.bit.AC_pump = 0x01;
            l_arm1tpdo3.param1.bit.AC_pump = count.ac_count;
        }
        if (type.draw_pump)
        {
            l_arm1tpdo1.draw_pump_speed_limit = speed.draw_speed;
            l_arm1tpdo2.draw_pump_need_distance = distance.draw_distance;
            l_arm1tpdo3.order_indicate.bit.draw_pump = 0x01;
            l_arm1tpdo3.param1.bit.draw_pump = count.draw_count;
        }
        if (type.feedBack_pump)
        {
            l_arm1tpdo1.feed_back_pump_speed_limit = speed.feedBack_speed;
            l_arm1tpdo2.feed_back_pump_need_distance = distance.feedBack_distance;
            l_arm1tpdo1.pump_direction.bit.feedBack_pump = type.feedBack_dir;
            l_arm1tpdo3.order_indicate.bit.feedBack_pump = 0x01;
            l_arm1tpdo3.param1.bit.feedBack_pump = count.feedBack_count;
        }
        memcpy(l_arm1block, &l_arm1tpdo1, sizeof(l_arm1tpdo1));
        memcpy(l_arm1block+8, &l_arm1tpdo2, sizeof(l_arm1tpdo2));
        memcpy(l_arm1block+16, &l_arm1tpdo3, sizeof(l_arm1tpdo3));

        
       // DUMP_DATA(l_arm1block,8);
       // DUMP_DATA(l_arm1block+16,8);
        // send arm1 tpdo
      //  arm1_send_frame(l_arm1block, 1, 3);//mask by wq
        
       // arm1_send_frame(l_arm1block, 1, 1);//tpdo1
       // arm1_send_frame(l_arm1block+8, 2, 1);//tpdo2
      //  arm1_send_frame(l_arm1block+16, 3, 1);//tpdo3
    }
    
    arm2_send_frame(l_arm2block, 5, 1);//tpdo1
    arm2_send_frame(l_arm2block+8, 6, 1);//tpdo2

    arm1_send_frame(l_arm1block, 1, 1);//tpdo1
    // arm1_send_frame(l_arm1block+8, 2, 1);//tpdo2
    
    OSTimeDlyHMSM(0,0,0,5);///// why to delay to send the 
    // message?????????????????????
    
    arm2_send_frame(l_arm2block+16, 7, 1);//tpdo3
    arm1_send_frame(l_arm1block+16, 3, 1);//tpdo3
    return 0;
}

uint8_t ctrl_stop_pump(CTRL_PUMP_TYPE type, CTRL_PUMP_COUNT count)
{
    ARM1_TPDO3 l_arm1tpdo3;
    ARM2_TPDO3 l_arm2tpdo3;
    uint8_t l_arm1block[8] = {0};
    uint8_t l_arm2block[8] = {0};

    memset(&l_arm1tpdo3, 0x00, sizeof(l_arm1tpdo3));
    memset(&l_arm2tpdo3, 0x00, sizeof(l_arm2tpdo3));

    if (type.ac_pump || type.draw_pump || type.feedBack_pump)
    {
        l_arm1tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        l_arm1tpdo3.module_indicate = 0x01;
        l_arm1tpdo3.order_indicate.all = 0xff;

        if (type.ac_pump)
        {
            l_arm1tpdo3.order_indicate.bit.AC_pump = 0x00;
            l_arm1tpdo3.param1.bit.AC_pump = count.ac_count;
        }
        if (type.draw_pump)
        {
            l_arm1tpdo3.order_indicate.bit.draw_pump = 0x00;
            l_arm1tpdo3.param1.bit.draw_pump = count.draw_count;
        }
        if (type.feedBack_pump)
        {
            l_arm1tpdo3.order_indicate.bit.feedBack_pump = 0x00;
            l_arm1tpdo3.param1.bit.feedBack_pump = count.feedBack_count;
        }
        memcpy(l_arm1block, &l_arm1tpdo3, sizeof(l_arm1tpdo3));

        // send arm1 tpdo
        arm1_send_frame(l_arm1block, 3, 1);
    }

    if (type.pla_pump || type.plt_pump)
    {
        l_arm2tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
        l_arm2tpdo3.module_indicate = 0x01;
        l_arm2tpdo3.order_indicate.all = 0xff;

        if (type.pla_pump)
        {
            l_arm2tpdo3.order_indicate.bit.plasma_valve_or_pump = 0x00;
            l_arm2tpdo3.param1.bit.plasma_pump = count.pla_count;
        }
        if (type.plt_pump)
        {
            l_arm2tpdo3.order_indicate.bit.PLT_valve_or_pump = 0x00;
            l_arm2tpdo3.param1.bit.PLT_pump = count.plt_count;
        }
        memcpy(l_arm2block, &l_arm2tpdo3, sizeof(l_arm2tpdo3));

        // send arm2 tpdo
        arm2_send_frame(l_arm2block, 7, 1);
    }

    return wait_pump_stopped(type);
}

uint8_t ctrl_init_pump_pos(CTRL_PUMP_TYPE type)
{    
    ARM1_TPDO3 l_arm1tpdo3;
    ARM2_TPDO3 l_arm2tpdo3;
    uint8_t l_arm1block[8] = {0};
    uint8_t l_arm2block[8] = {0};
    u8 l_preTimeCount = QUERY_COUNTS*5;
    
    APP_TRACE("TO <ctrl_init_pump_pos>\r\n");

    memset(&l_arm1tpdo3, 0x00, sizeof(l_arm1tpdo3));
    memset(&l_arm2tpdo3, 0x00, sizeof(l_arm2tpdo3));
    
    if (type.ac_pump || type.draw_pump || type.feedBack_pump)
    {
        l_arm1tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        l_arm1tpdo3.module_indicate = 0x01;
        l_arm1tpdo3.order_indicate.all = 0xff;
        if (type.ac_pump)
        {
            l_arm1tpdo3.order_indicate.bit.AC_pump = 0x10;
            l_arm1tpdo3.param1.bit.AC_pump = 0x01;
        }
        if (type.draw_pump)
        {
            l_arm1tpdo3.order_indicate.bit.draw_pump = 0x10;
            l_arm1tpdo3.param1.bit.draw_pump = 0x01;
        }
        if (type.feedBack_pump)
        {
            l_arm1tpdo3.order_indicate.bit.feedBack_pump = 0x10;
            l_arm1tpdo3.param1.bit.feedBack_pump = 0x01;
        }
        
        memcpy(l_arm1block, &l_arm1tpdo3, sizeof(l_arm1tpdo3));
        // send init pump
        arm1_send_frame(l_arm1block, 3, 1);
    }
    if (type.pla_pump || type.plt_pump)
    {
        l_arm2tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
        l_arm2tpdo3.module_indicate = 0x01;
        l_arm2tpdo3.order_indicate.all = 0xff;
        if (type.pla_pump)
        {
            l_arm2tpdo3.order_indicate.bit.plasma_valve_or_pump = 0x10;
            l_arm2tpdo3.param1.bit.plasma_pump = 0x01;
        }
        if (type.plt_pump)
        {
            l_arm2tpdo3.order_indicate.bit.PLT_valve_or_pump = 0x10;
            l_arm2tpdo3.param1.bit.PLT_pump = 0x01;//清除行程
        }

        memcpy(l_arm2block, &l_arm2tpdo3, sizeof(l_arm2tpdo3));
        // send init pump
        arm2_send_frame(l_arm2block, 7, 1);
    }

    while ((type.ac_pump && !g_arm1_rpdo3_data.pump_init.bit.AC_pump_init)
        || (type.draw_pump && !g_arm1_rpdo3_data.pump_init.bit.draw_pump_init)
        || (type.feedBack_pump && !g_arm1_rpdo3_data.pump_init.bit.feedBack_pump_init)
        || (type.pla_pump && !g_arm2_rpdo3_data.pump_init.bit.plasma_pump_init)
        || (type.plt_pump && !g_arm2_rpdo3_data.pump_init.bit.PLT_pump_init))
    {
        --l_preTimeCount;
        OSTimeDly(QUERY_PERIOD);
        if (0==l_preTimeCount)
        {
            APP_TRACE("return <ctrl_init_pump_pos>\r\n");
            return 1;
        }
    }
    
    return 0;
}

uint8_t ctrl_turn_valve(CTRL_VALVE_TYPE type, CTRL_VALVE_DIR dir)
{
    ARM2_TPDO3 l_arm2tpdo3;
    uint8_t l_arm2block[8] = {0};

    memset(&l_arm2tpdo3, 0x00, sizeof(l_arm2tpdo3));

    l_arm2tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
    l_arm2tpdo3.module_indicate = 0x02;
    l_arm2tpdo3.order_indicate.all = 0xff;

    if (type.pla_valve)
    {
        l_arm2tpdo3.order_indicate.bit.plasma_valve_or_pump = dir.pla_valve;//
    }
    if (type.plt_valve)
    {
        l_arm2tpdo3.order_indicate.bit.PLT_valve_or_pump = dir.plt_valve;
    }
    if (type.rbc_valve)
    {
        l_arm2tpdo3.order_indicate.bit.RBC_valve = dir.rbc_valve;
    }
    memcpy(l_arm2block, &l_arm2tpdo3, sizeof(l_arm2tpdo3));

    // send arm2 tpdo
    arm2_send_frame(l_arm2block, 7, 1);

    return wait_valve_end(type, dir);
}

uint8_t ctrl_start_centrifuge(uint16_t speed)
{// TODO: centrifuge direction
    ARM3_TPDO1 l_arm3tpdo1;
    ARM3_TPDO2 l_arm3tpdo2;
    ARM3_TPDO3 l_arm3tpdo3;
    uint8_t l_arm3block[24] = {0};

    memset(&l_arm3tpdo1, 0x00, sizeof(l_arm3tpdo3));
    memset(&l_arm3tpdo2, 0x00, sizeof(l_arm3tpdo3));
    memset(&l_arm3tpdo3, 0x00, sizeof(l_arm3tpdo3));

    l_arm3tpdo3.fn = get_current_fn(ARM3_DEST, FN_CREATE);
    l_arm3tpdo3.module_indicate = 0x01;
    l_arm3tpdo3.order_indicate.all = 0xff;
    
    l_arm3tpdo1.centrifuge_motor_speed_limit = speed;
    l_arm3tpdo3.order_indicate.bit.centrifuge_or_electromagnet = 0x01;
    
    memcpy(l_arm3block, &l_arm3tpdo1, sizeof(l_arm3tpdo1));
    memcpy(l_arm3block+8, &l_arm3tpdo2, sizeof(l_arm3tpdo2));
    memcpy(l_arm3block+16, &l_arm3tpdo3, sizeof(l_arm3tpdo3));
    // send arm3 tpdo
    arm3_send_frame(l_arm3block, 9, 3);
    g_centri_speed = speed;

    return 0;//wait_centrifuge_end(speed);
}

uint8_t ctrl_stop_centrifuge(void)
{
    ARM3_TPDO3 l_arm3tpdo3;
    uint8_t l_arm3block[8] = {0};

    memset(&l_arm3tpdo3, 0x00, sizeof(l_arm3tpdo3));

    l_arm3tpdo3.fn = get_current_fn(ARM3_DEST, FN_CREATE);
    l_arm3tpdo3.module_indicate = 0x01;
    l_arm3tpdo3.order_indicate.all = 0xff;
    
    l_arm3tpdo3.order_indicate.bit.centrifuge_or_electromagnet = 0x00;

    memcpy(l_arm3block, &l_arm3tpdo3, sizeof(l_arm3tpdo3));
    // send arm3 tpdo
    arm3_send_frame(l_arm3block, 11, 1);

    return 0;//wait_centrifuge_stopped();
}

uint8_t ctrl_door_lock(CTRL_LOCK_TYPE lock)
{
    ARM3_TPDO3 l_arm3tpdo3;
    uint8_t l_arm3block[8] = {0};

    memset(&l_arm3tpdo3, 0x00, sizeof(l_arm3tpdo3));
    l_arm3tpdo3.fn = get_current_fn(ARM3_DEST, FN_CREATE);
    l_arm3tpdo3.module_indicate = 0x02;
    l_arm3tpdo3.order_indicate.all = 0xff;

    if (TYPE_DOOR_LOCK == lock)
    {
        l_arm3tpdo3.order_indicate.bit.centrifuge_or_electromagnet = 0x01;
    }
    else if (TYPE_DOOR_UNLOCK == lock)
    {
        l_arm3tpdo3.order_indicate.bit.centrifuge_or_electromagnet = 0x00;
    }

    memcpy(l_arm3block, &l_arm3tpdo3, sizeof(l_arm3tpdo3));
    // send arm3 tpdo
    arm3_send_frame(l_arm3block, 11, 1);

    return wait_lock_end(lock);
}

uint8_t ctrl_run_cassette(CTRL_CASSETTE_TYPE dir)
{
    ARM3_TPDO3 l_arm3tpdo3;
    uint8_t l_arm3block[8] = {0};
    
    memset(&l_arm3tpdo3, 0x00, sizeof(l_arm3tpdo3));
    l_arm3tpdo3.fn = get_current_fn(ARM3_DEST, FN_CREATE);
    l_arm3tpdo3.module_indicate = 0x01;
    l_arm3tpdo3.order_indicate.all = 0xff;

    if (TYPE_CASSETTE_RISE == dir)
    {
        l_arm3tpdo3.order_indicate.bit.cassette_motor = 0x01;
    }
    else if (TYPE_CASSETTE_FALL== dir)
    {
        l_arm3tpdo3.order_indicate.bit.cassette_motor = 0x02;
    }

    memcpy(l_arm3block, &l_arm3tpdo3, sizeof(l_arm3tpdo3));
    // send arm3 tpdo
    arm3_send_frame(l_arm3block, 11, 1);

    return wait_cassette_end(dir);
}


uint8_t wait_pump_end(CTRL_PUMP_TYPE type, CTRL_PUMP_SPEED speed, CTRL_PUMP_DISTANCE distance)
{// TODO: do arm0 need to judge the timeover event
    uint32_t l_preTime[5] = {0};
    uint32_t l_maxCount = 0;

    uint32_t dis_feed = distance.feedBack_distance;



    if (type.ac_pump && distance.ac_distance)
    {
        l_preTime[0] = (u32)((u32)distance.ac_distance*60*10000/speed.ac_speed);
    }
    if (type.draw_pump && distance.draw_distance)
    {

        l_preTime[1] = (u32)((u32)distance.draw_distance*60*10000/speed.draw_speed);
    }
    if (type.feedBack_pump && distance.feedBack_distance)
    {
        APP_TRACE("distance.feedBack_distance = %d\r\n",distance.feedBack_distance);
        APP_TRACE("speed.feedBack_speed       = %d\r\n",speed.feedBack_speed);
    
        l_preTime[2] = (u32)((u32)dis_feed*60*10000/speed.feedBack_speed);
    }
    if (type.pla_pump && distance.pla_distance)
    {
    
        l_preTime[3] = (u32)((u32)distance.pla_distance*60*10000/speed.pla_speed);
    }
    if (type.plt_pump && distance.plt_distance)
    {
        l_preTime[4] = (u32)((u32)distance.plt_distance*60*10000/speed.plt_speed);
    }

    l_maxCount = get_array_max_value(l_preTime, 5)/QUERY_PERIOD;//+ QUERY_COUNTS;

    APP_TRACE("distance.feedBack_distance = %d\r\n",distance.feedBack_distance);
    APP_TRACE("speed.feedBack_speed       = %d\r\n",speed.feedBack_speed);
       
    
    if (distance.ac_distance>0 || distance.draw_distance>0 || distance.feedBack_distance>0
        || distance.pla_distance>0 || distance.plt_distance>0)
    {
        while ((distance.ac_distance>0 
                && (g_arm1_rpdo2_data.AC_pump_moved_distance<distance.ac_distance 
                || g_arm1_rpdo2_data.fn != get_cmd_fn(START_PUMP_FN, ARM1_DEST)))
            || (distance.draw_distance>0 
                && (g_arm1_rpdo2_data.draw_pump_moved_distance<distance.draw_distance
               || g_arm1_rpdo2_data.fn != get_cmd_fn(START_PUMP_FN, ARM1_DEST)))
           || (distance.feedBack_distance>0 
               && (g_arm1_rpdo2_data.feed_back_pump_moved_distance<distance.feedBack_distance
               || g_arm1_rpdo2_data.fn != get_cmd_fn(START_PUMP_FN, ARM1_DEST)))
            || (distance.pla_distance>0
                && (g_arm2_rpdo2_data.plasma_pump_moved_distance<distance.pla_distance
                || g_arm2_rpdo2_data.fn != get_cmd_fn(START_PUMP_FN, ARM2_DEST)))
            || (distance.plt_distance>0
                && (g_arm2_rpdo2_data.PLT_pump_moved_distance<distance.plt_distance
                || g_arm2_rpdo2_data.fn != get_cmd_fn(START_PUMP_FN, ARM2_DEST))))
        {
            --l_maxCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_maxCount)
            {
              //  APP_TRACE("wait_pump_end Timeout!\r\n");
                return 0;
            }
        }
    }

    return 0;
}

static u8 wait_pump_stopped(CTRL_PUMP_TYPE type)
{
  //  u8 l_preTimeCount = QUERY_COUNTS*2;
    u8 l_preTimeCount = QUERY_COUNTS*4;//change by wq
    
    while ((type.ac_pump>0 && 0x00!=g_arm1_rpdo1_data.pump_current_dir.bit.AC_pump)
        || (type.draw_pump>0 && 0x00!=g_arm1_rpdo1_data.pump_current_dir.bit.draw_pump)
        || (type.feedBack_pump>0 && 0x00!=g_arm1_rpdo1_data.pump_current_dir.bit.feedBack_pump)
        || (type.pla_pump>0 && 0x00!=g_arm2_rpdo1_data.pump_current_dir.bit.plasma_pump)
        || (type.plt_pump>0 && 0x00!=g_arm2_rpdo1_data.pump_current_dir.bit.PLT_pump))
    {
        --l_preTimeCount;
        OSTimeDly(QUERY_PERIOD);
        if (0x00 == l_preTimeCount)
        {
            return 1;
        }
    }
    return 0;
}

static u8 wait_valve_end(CTRL_VALVE_TYPE type, CTRL_VALVE_DIR dir)
{
    uint16_t l_dir = 0x0000;
   // uint16_t l_preTimeCount = QUERY_COUNTS*2;
   
    uint16_t l_preTimeCount = QUERY_COUNTS*20;//change by wq
    if (type.rbc_valve>0)
    {
        switch(dir.rbc_valve)
        {
            case 0x00:
                l_dir |= 0x0001;
                break;
            case 0x01:
                l_dir |= 0x0002;
                break;
            case 0x02:
                l_dir |= 0x0004;
                break;
        }
    }
    if (type.plt_valve>0)
    {
        switch(dir.plt_valve)
        {
            case 0x00:
                l_dir |= 0x0008;
                break;
            case 0x01:
                l_dir |= 0x0010;
                break;
            case 0x02:
                l_dir |= 0x0020;
                break;
        }
    }
    if (type.pla_valve>0)
    {
        switch(dir.pla_valve)
        {
            case 0x00:
                l_dir |= 0x0040;
                break;
            case 0x01:
                l_dir |= 0x0080;
                break;
            case 0x02:
                l_dir |= 0x0100;
                break;
        }
    }
    
    while ((type.rbc_valve>0 && (g_arm2_rpdo3_data.valve.all&0x0007)!=(l_dir&0x0007))
        || (type.pla_valve>0 && (g_arm2_rpdo3_data.valve.all&0x0038)!=(l_dir&0x0038))
        || (type.plt_valve>0 && (g_arm2_rpdo3_data.valve.all&0x0380)!=(l_dir&0x0380)))
    {
        --l_preTimeCount;
        OSTimeDly(QUERY_PERIOD);
        if (0x00 == l_preTimeCount)
        {
           // return 1;//mask by wq
           return 0;
        }
    }
    return 0;
}

#if 0
static u8 wait_centrifuge_end(u16 speed)
{
    u8 l_preTimeCount = QUERY_COUNTS;
    
    while (g_arm3_rpdo1_data.centrifuge_motor_current_speed != speed)
    {
        --l_preTimeCount;
        OSTimeDly(QUERY_PERIOD);
        if (0x00 == l_preTimeCount)
        {
            return 1;
        }
    }
    return 0;
}

static u8 wait_centrifuge_stopped(void)
{
    u8 l_preTimeCount = QUERY_COUNTS;
    
    while (0x00 != g_arm3_rpdo1_data.motor_current_dir.bit.centrifuge_motor)
    {
        --l_preTimeCount;
        OSTimeDly(QUERY_PERIOD);
        if (0x00 == l_preTimeCount)
        {
            return 1;
        }
    }
    return 0;
}
#endif

static u8 wait_lock_end(CTRL_LOCK_TYPE lock)
{
    u8 l_preTimeCount = QUERY_COUNTS;

    if (TYPE_DOOR_LOCK == lock)
    {
        while (0x01 != g_arm3_rpdo3_data.sensor.bit.door_switch2)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                return 1;
            }
        }
    }
    else if (TYPE_DOOR_UNLOCK == lock)
    {
        while (0x01 != g_arm3_rpdo3_data.sensor.bit.door_switch1)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                return 1;
            }
        }
    }
    
    return 0;
}

static u8 wait_cassette_end(CTRL_CASSETTE_TYPE dir)
{
    u8 l_preTimeCount = QUERY_COUNTS*10;//200

    if (TYPE_CASSETTE_RISE == dir)
    {
        while (0x01 != g_arm3_rpdo3_data.sensor.bit.cassette_up_location)
        {
            --l_preTimeCount;
            OSTimeDly(80);//change by wq QUERY_PERIOD 

			APP_TRACE("wait_cassette_end() GPIOD GPIO_Pin_2 is %d\r\n",GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2));
			APP_TRACE("wait_cassette_end() GPIOD GPIO_Pin_1 is %d\r\n",GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1));


			
			if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2) == 0x00)//卡匣在上限位
			{
				
				APP_TRACE("wait_cassette_end() GPIOD GPIO_Pin_2 is %d\r\n",GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2));
                return 1;
				
			}

			
            if (0x00 == l_preTimeCount)
            {
                return 1;
            }
        }
    }
    else if (TYPE_CASSETTE_FALL == dir)
    {
        while (0x01 != g_arm3_rpdo3_data.sensor.bit.cassette_down_location)
        {
            --l_preTimeCount;
            OSTimeDly(1000);
            if (0x00 == l_preTimeCount)
            {
                return 1;
            }
        }
    }
    
    return 0;
}

uint8_t ctrl_load_canal(void)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    /* 1. all pump start turn */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    l_speed.ac_speed = 100*100;
    l_speed.draw_speed = 100*100;
    l_speed.feedBack_speed = 60*100;
    l_speed.pla_speed = 75*100;
    l_speed.plt_speed = 75*100;
    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        return 1;
    }

    /* 2. cassette fall down */
    
    // can't move mask by wq
    if (ctrl_run_cassette(TYPE_CASSETTE_FALL))
    {
        return 1;
    }

    // OSTimeDlyHMSM(0,0,1,0);//add by wq 

    /* 3. all pumps stop */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {
        return 1;
    }

    return 0;
}

uint8_t ctrl_unload_canal(void)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_DISTANCE l_distance;
    CTRL_PUMP_SPEED l_speed;
    CTRL_PUMP_COUNT l_count;

    /* 1. all pump start turn */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    
    l_speed.ac_speed = 100*100;
    l_speed.draw_speed = 100*100;
    l_speed.feedBack_speed = 100*100;
    l_speed.pla_speed = 100*100;
    l_speed.plt_speed = 100*100;

    if (ctrl_start_pump(l_pump, l_speed, l_distance, l_count))
    {
        APP_TRACE("ctrl_start_pump\r\n");
        return 1;
    }

    /* 2. cassette fall down */

    APP_TRACE("GPIOD GPIO_Pin_2 is %d\r\n",GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2));
    
	APP_TRACE("GPIOD GPIO_Pin_1 is %d\r\n",GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1));
	
    if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2) != 0x00)//卡匣不在上限位
    {
        if (ctrl_run_cassette(TYPE_CASSETTE_RISE))
         {
             APP_TRACE("cassette test is failed\r\n");
            // return 1;
         }
    }
    else
    {
        APP_TRACE("cassette is already up can't move!!!\r\n");

    }


    //OSTimeDlyHMSM(0,0,3,0);//add by wq

    
    /* 3. all pumps stop */
    init_pump_param(&l_pump, &l_speed, &l_distance, &l_count);
    l_pump.ac_pump = 0x01;
    l_pump.draw_pump = 0x01;
    l_pump.feedBack_pump = 0x01;
    l_pump.pla_pump = 0x01;
    l_pump.plt_pump = 0x01;
    if (ctrl_stop_pump(l_pump, l_count))
    {
        APP_TRACE("ctrl_stop_pump\r\n");
        return 1;
    }

    return 0;
}

uint8_t pause_run_pumps(void)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_COUNT l_count;

    memset(&l_pump, 0x00, sizeof(l_pump));
    // record the pump state    
    l_pump.draw_pump = g_arm1_rpdo1_data.pump_current_dir.bit.draw_pump>0 ? 0x01 : 0x00;
    l_pump.ac_pump = g_arm1_rpdo1_data.pump_current_dir.bit.AC_pump>0 ? 0x01 : 0x00;
    l_pump.feedBack_pump = g_arm1_rpdo1_data.pump_current_dir.bit.feedBack_pump>0 ? 0x01 : 0x00;
    l_pump.pla_pump= g_arm2_rpdo1_data.pump_current_dir.bit.plasma_pump>0 ? 0x01 : 0x00;
    l_pump.plt_pump= g_arm2_rpdo1_data.pump_current_dir.bit.PLT_pump>0 ? 0x01 : 0x00;
    set_pause_state(l_pump);

    // stop the run pump
    memset(&l_count, 0x00, sizeof(l_count));
    
    return ctrl_stop_pump(l_pump, l_count);
}


uint8_t resume_run_pumps(void)
{    
    ARM1_TPDO3 l_arm1tpdo3;
    ARM2_TPDO3 l_arm2tpdo3;
    uint8_t l_arm1block[8] = {0};
    uint8_t l_arm2block[8] = {0};
    CTRL_PUMP_TYPE l_pumpType;

    memset(&l_arm1tpdo3, 0x00, sizeof(l_arm1tpdo3));
    memset(&l_arm2tpdo3, 0x00, sizeof(l_arm2tpdo3));

    l_pumpType = get_pause_state();

    if (l_pumpType.ac_pump || l_pumpType.draw_pump || l_pumpType.feedBack_pump)
    {
        l_arm1tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        l_arm1tpdo3.module_indicate = 0x01;
        l_arm1tpdo3.order_indicate.all = 0xff;

        if (l_pumpType.ac_pump)
        {
            l_arm1tpdo3.order_indicate.bit.AC_pump = 0x01;
            l_arm1tpdo3.param1.bit.AC_pump = 0x00;
        }
        if (l_pumpType.draw_pump)
        {
            l_arm1tpdo3.order_indicate.bit.draw_pump = 0x01;
            l_arm1tpdo3.param1.bit.draw_pump = 0x00;
        }
        if (l_pumpType.feedBack_pump)
        {
            l_arm1tpdo3.order_indicate.bit.feedBack_pump = 0x01;
            l_arm1tpdo3.param1.bit.feedBack_pump = 0x00;
        }
        memcpy(l_arm1block, &l_arm1tpdo3, sizeof(l_arm1tpdo3));

        // send arm1 tpdo
        arm1_send_frame(l_arm1block, 3, 1);
    }

    if (l_pumpType.pla_pump || l_pumpType.plt_pump)
    {
        l_arm2tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
        l_arm2tpdo3.module_indicate = 0x01;
        l_arm2tpdo3.order_indicate.all = 0xff;

        if (l_pumpType.pla_pump)
        {
            l_arm2tpdo3.order_indicate.bit.plasma_valve_or_pump = 0x01;
            l_arm2tpdo3.param1.bit.plasma_pump = 0x00;
        }
        if (l_pumpType.plt_pump)
        {
            l_arm2tpdo3.order_indicate.bit.PLT_valve_or_pump = 0x01;
            l_arm2tpdo3.param1.bit.PLT_pump = 0x00;
        }
        memcpy(l_arm2block, &l_arm2tpdo3, sizeof(l_arm2tpdo3));

        // send arm2 tpdo
        arm2_send_frame(l_arm2block, 7, 1);
    }

    return 0;
}

uint8_t pressure_abnormal_handle(void)
{
    CTRL_PUMP_TYPE l_pump;
    CTRL_PUMP_COUNT l_count;
    
    l_pump.draw_pump = g_arm1_rpdo1_data.pump_current_dir.bit.draw_pump>0 ? 0x01 : 0x00;
    l_pump.ac_pump = g_arm1_rpdo1_data.pump_current_dir.bit.AC_pump>0 ? 0x01 : 0x00;
    l_pump.feedBack_pump = g_arm1_rpdo1_data.pump_current_dir.bit.feedBack_pump>0 ? 0x01 : 0x00;
    l_pump.pla_pump= g_arm2_rpdo1_data.pump_current_dir.bit.plasma_pump>0 ? 0x01 : 0x00;
    l_pump.plt_pump= g_arm2_rpdo1_data.pump_current_dir.bit.PLT_pump>0 ? 0x01 : 0x00;
    set_pressure_abnormal_state(l_pump);

    memset(&l_pump, 0x1F, sizeof(l_pump));
    memset(&l_count, 0x00, sizeof(l_count));

    if (PLT_COLLECT_BEGIN == get_collect_status())
    {
        l_pump.plt_pump = 0x00;
    }
    
    return ctrl_stop_pump(l_pump, l_count);
}

uint8_t pressure_normal_handle(void)
{
    ARM1_TPDO3 l_arm1tpdo3;
    ARM2_TPDO3 l_arm2tpdo3;
    uint8_t l_arm1block[8] = {0};
    uint8_t l_arm2block[8] = {0};
    CTRL_PUMP_TYPE l_pumpType;

    memset(&l_arm1tpdo3, 0x00, sizeof(l_arm1tpdo3));
    memset(&l_arm2tpdo3, 0x00, sizeof(l_arm2tpdo3));

    l_pumpType = get_pressure_abnormal_state();

    if (l_pumpType.ac_pump || l_pumpType.draw_pump || l_pumpType.feedBack_pump)
    {
        l_arm1tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        l_arm1tpdo3.module_indicate = 0x01;
        l_arm1tpdo3.order_indicate.all = 0xff;

        if (l_pumpType.ac_pump)
        {
            l_arm1tpdo3.order_indicate.bit.AC_pump = 0x01;
            l_arm1tpdo3.param1.bit.AC_pump = 0x00;
        }
        if (l_pumpType.draw_pump)
        {
            l_arm1tpdo3.order_indicate.bit.draw_pump = 0x01;
            l_arm1tpdo3.param1.bit.draw_pump = 0x00;
        }
        if (l_pumpType.feedBack_pump)
        {
            l_arm1tpdo3.order_indicate.bit.feedBack_pump = 0x01;
            l_arm1tpdo3.param1.bit.feedBack_pump = 0x00;
        }
        memcpy(l_arm1block, &l_arm1tpdo3, sizeof(l_arm1tpdo3));

        // send arm1 tpdo
        arm1_send_frame(l_arm1block, 3, 1);
    }

    if (l_pumpType.pla_pump || l_pumpType.plt_pump)
    {
        l_arm2tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
        l_arm2tpdo3.module_indicate = 0x01;
        l_arm2tpdo3.order_indicate.all = 0xff;

        if (l_pumpType.pla_pump)
        {
            l_arm2tpdo3.order_indicate.bit.plasma_valve_or_pump = 0x01;
            l_arm2tpdo3.param1.bit.plasma_pump = 0x00;
        }
        if (l_pumpType.plt_pump)
        {
            l_arm2tpdo3.order_indicate.bit.PLT_valve_or_pump = 0x01;
            l_arm2tpdo3.param1.bit.PLT_pump = 0x00;
        }
        memcpy(l_arm2block, &l_arm2tpdo3, sizeof(l_arm2tpdo3));

        // send arm2 tpdo
        arm2_send_frame(l_arm2block, 7, 1);
    }

    return 0;
}


uint8_t set_pump_distance_type(CTRL_PUMP_TYPE type, CTRL_DISTANCE_TYPE distance)
{
    ARM1_TPDO3 l_arm1_tpdo3;
    ARM2_TPDO3 l_arm2_tpdo3;
    uint8_t l_arm1block[8] = {0};
    uint8_t l_arm2block[8] = {0};

    memset(&l_arm1_tpdo3, 0x00, sizeof(l_arm1_tpdo3));
    memset(&l_arm2_tpdo3, 0x00, sizeof(l_arm2_tpdo3));
    
    if (type.ac_pump || type.draw_pump || type.feedBack_pump)
    {
        l_arm1_tpdo3.fn = get_current_fn(ARM1_DEST, FN_CREATE);
        l_arm1_tpdo3.module_indicate = 0xA5;
        l_arm1_tpdo3.param2 = 0x01;
        if (type.draw_pump)
        {
            l_arm1_tpdo3.param3.bit.draw_pump = distance;
        }
        if (type.ac_pump)
        {
            l_arm1_tpdo3.param3.bit.AC_pump = distance;
        }
        if (type.feedBack_pump)
        {
            l_arm1_tpdo3.param3.bit.feedback_pump = distance;
        }

        memcpy(l_arm1block, &l_arm1_tpdo3, sizeof(l_arm1_tpdo3));
        // send arm1 tpdo
        arm1_send_frame(l_arm1block, 3, 1);
    }

    if (type.plt_pump || type.pla_pump)
    {
        l_arm2_tpdo3.fn = get_current_fn(ARM2_DEST, FN_CREATE);
        l_arm2_tpdo3.module_indicate = 0xA5;
        l_arm2_tpdo3.param2 = 0x01;
        if (type.pla_pump)
        {
            l_arm2_tpdo3.param3.bit.pla_pump = distance;
        }
        if (type.plt_pump)
        {
            l_arm2_tpdo3.param3.bit.plt_pump = distance;
        }
        
        memcpy(l_arm2block, &l_arm2_tpdo3, sizeof(l_arm2_tpdo3));
        // send arm2 tpdo
        arm2_send_frame(l_arm2block, 7, 1);
    }

    return 0;
}

void clear_pump_total_distance(CTRL_PUMP_TYPE type)
{
    // clear pump's total distance
    set_pump_distance_type(type, CLEAR_DISTANCE);
}

CTRL_PUMP_DISTANCE get_pump_total_distance(CTRL_PUMP_TYPE type)
{
    u8 l_errFlag = 0;
    u8 l_preTimeCount = QUERY_COUNTS;
    CTRL_PUMP_DISTANCE l_distance;//
    
    INNER_ERROR_MSG l_errMsg;

    memset(&l_distance, 0x00, sizeof(l_distance));//add by Big.Man

   // set_pump_distance_type(type, TOTAL_DISTANCE);

    if (type.ac_pump)
    {
        l_preTimeCount = QUERY_COUNTS;
        while (!g_arm1_rpdo2_data.distance_type.bit.AC_pump)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                l_errFlag = 1;
                break;
            }
        }
        l_distance.ac_distance = g_arm1_rpdo2_data.AC_pump_moved_distance;
        APP_TRACE("g_arm2_rpdo2_data.AC_pump_moved_distance = %d \r\n",g_arm1_rpdo2_data.AC_pump_moved_distance);
    }
    if (type.draw_pump)
    {
        l_preTimeCount = QUERY_COUNTS;
        while (!g_arm1_rpdo2_data.distance_type.bit.draw_pump)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                l_errFlag = 1;
                break;
            }
        }
        l_distance.draw_distance = g_arm1_rpdo2_data.draw_pump_moved_distance;
        APP_TRACE("g_arm2_rpdo2_data.draw_pump_moved_distance = %d \r\n",g_arm1_rpdo2_data.draw_pump_moved_distance);
    }
    if (type.feedBack_pump)
    {
        l_preTimeCount = QUERY_COUNTS;
        while (!g_arm1_rpdo2_data.distance_type.bit.feedback_pump)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                l_errFlag = 1;
                break;
            }
        }
        l_distance.feedBack_distance = g_arm1_rpdo2_data.feed_back_pump_moved_distance;
        APP_TRACE("g_arm2_rpdo2_data.feed_back_pump_moved_distance = %d \r\n",g_arm1_rpdo2_data.feed_back_pump_moved_distance);
    }
    if (type.pla_pump)
    {
        l_preTimeCount = QUERY_COUNTS;
        while (!g_arm2_rpdo2_data.distance_type.bit.pla_pump)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                l_errFlag = 1;
                break;
            }
        }
        l_distance.pla_distance = g_arm2_rpdo2_data.plasma_pump_moved_distance;
        APP_TRACE("g_arm2_rpdo2_data.plasma_pump_moved_distance = %d \r\n",g_arm2_rpdo2_data.plasma_pump_moved_distance);
    }
    if (type.plt_pump)
    {
        l_preTimeCount = QUERY_COUNTS;
        while (!g_arm2_rpdo2_data.distance_type.bit.plt_pump)
        {
            --l_preTimeCount;
            OSTimeDly(QUERY_PERIOD);
            if (0x00 == l_preTimeCount)
            {
                l_errFlag = 1;
                break;
            }
        }  

        
        l_distance.plt_distance = g_arm2_rpdo2_data.PLT_pump_moved_distance;


        
        APP_TRACE("g_arm2_rpdo2_data.PLT_pump_moved_distance = %d \r\n",g_arm2_rpdo2_data.PLT_pump_moved_distance);
    }

    set_pump_distance_type(type, SINGLE_DISTANCE);

    if (l_errFlag)
    {
      //  APP_TRACE("get_pump_total_distance ERROR!\r\n");
        l_errMsg.source.all = ERR_SOURCE_CANOPEN;
        l_errMsg.taskNo = arm0_get_cur_prio();
        l_errMsg.action = ERROR_STOP_PROGRAM;
        l_errMsg.errcode1 = 0x32;
        l_errMsg.errcode2 = 0x00;
        l_errMsg = l_errMsg;
        post_error_msg(l_errMsg);
    }
    
    return l_distance;
}


