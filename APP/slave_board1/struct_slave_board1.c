/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_slave_board1.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the define of code struct(TPDO/RPDO)
 *                      interface
 *******************************************************************************
 * History:
 * DATE 	  | VER   | AUTOR	   | Description
 * 2013/11/19 | v1.0  | WQ		   | initial released
 *******************************************************************************/

#include "struct_slave_board1.h"
#include "app_slave_board1.h"

#define RATIO_PUMP_SPEED 100


void write_pump_direction(pump_type pump,pump_direction dir)
{
    assert_param(IS_PUMP_TYPE(pump));
    assert_param(IS_PUMP_DIRECTION(dir));

    switch(pump)
    {
        case DRAW_PUMP:
            pump_state1.pump_current_dir.bit.draw_pump = dir;
        break;
        case AC_PUMP:
            pump_state1.pump_current_dir.bit.AC_pump = dir;
        break;
        case FEEDBACK_PUMP:
            pump_state1.pump_current_dir.bit.feed_back_pump = dir;
        break;
        default:
        break;
    }

}

void write_pump_speed(pump_type pump,uint16_t speed)
{
    assert_param(IS_PUMP_TYPE(pump));

    switch(pump)
    {
        case DRAW_PUMP:
            pump_state1.draw_pump_current_speed = speed * RATIO_PUMP_SPEED;
        break;
        case AC_PUMP:
            pump_state1.AC_pump_current_speed = speed * RATIO_PUMP_SPEED;
        break;
        case FEEDBACK_PUMP:
            pump_state1.feed_back_pump_current_speed = speed * RATIO_PUMP_SPEED;
        break;
        default:
        break;
    }
    if(read_pump_distance_flag(pump) == 0x00)//single
    {
        pump_state1.fn = fn_start_pump;//updata FN
    }
    else
         pump_state1.fn = control_order_r.fn;//updata FN
   // pump_state1.fn = control_order_r.fn;//updata FN

}
void write_pump_moved_distance(pump_type pump,uint16_t distance)
{
    uint8_t tmp = 0x00;
   assert_param(IS_PUMP_TYPE(pump));
   tmp = control_order_r.order_indicate.bit.AC_pump;

    switch(pump)
    {
        case DRAW_PUMP:
            if(control_order_r.order_indicate.bit.draw_pump == 0x01)/*启动的时候加*/
            //if(0)
            {
                pump_state2.draw_pump_moved_distance = distance + draw_pump_old_single_distance;
            }
            else
            {

                pump_state2.draw_pump_moved_distance = distance;
            }

            pump_state2.single_or_total.bit.draw_pump = pump_state2_handle.single_or_total.bit.draw_pump;
        break;
        case AC_PUMP:
            if(control_order_r.order_indicate.bit.AC_pump == 0x01)/*启动的时候加*/
            //if(0)
            {
                pump_state2.AC_pump_moved_distance = distance + AC_pump_old_single_distance;
            }
            else
            {

                pump_state2.AC_pump_moved_distance = distance;
            }

            //pump_state2.AC_pump_moved_distance = distance;
            pump_state2.single_or_total.bit.AC_pump = pump_state2_handle.single_or_total.bit.AC_pump;
        break;
        case FEEDBACK_PUMP:
            if(control_order_r.order_indicate.bit.feed_back_pump == 0x01)/*启动的时候加*/
            {
                pump_state2.feed_back_pump_moved_distance = distance + feedback_pump_old_single_distance;
            }
            else
            {

                pump_state2.feed_back_pump_moved_distance = distance;
            }

            pump_state2.single_or_total.bit.feedback_pump = pump_state2_handle.single_or_total.bit.feedback_pump;
        break;
        default:
        break;
    }
    if(read_pump_distance_flag(pump) == 0x00)//single
    {
        pump_state2.fn = fn_start_pump;//updata FN
    }
    else
         pump_state2.fn = control_order_r.fn;//updata FN

  //  pump_state2.fn = control_order_r.fn;//updata FN

}
void write_pump_distance_flag(pump_type pump,distance_totalsingle flag)
{
    assert_param(IS_PUMP_TYPE(pump));
    assert_param(IS_DISTANCE_TOTALSINGLE(flag));

    switch(pump)
    {
        case DRAW_PUMP:
            pump_state2.single_or_total.bit.draw_pump = flag;
        break;
        case AC_PUMP:
            pump_state2.single_or_total.bit.AC_pump = flag;
        break;
        case FEEDBACK_PUMP:
            pump_state2.single_or_total.bit.feedback_pump = flag;
        break;
        default:
        break;
    }

}
distance_totalsingle read_pump_distance_flag(pump_type pump)
{
    uint8_t ret;
    assert_param(IS_PUMP_TYPE(pump));

    switch(pump)
    {
        case DRAW_PUMP:
           ret = control_order_r.single_or_total.bit.draw_pump;
        break;
        case AC_PUMP:
           ret = control_order_r.single_or_total.bit.AC_pump;
        break;
        case FEEDBACK_PUMP:
           ret = control_order_r.single_or_total.bit.feedback_pump;
        break;
        default:
        break;
    }

    return (distance_totalsingle)ret;
}






/**************************************************************/
/***************for sensor state     **************************/
/**************************************************************/

void write_blood_collect_preesure(u16 preesure)
{
    //transform from volatage to preesure

    sensor_state.blood_collect_preasure = preesure;
    //sensor_state.blood_collect_preasure = 0x00;
}

void write_AC_bubble(AC_bubble_status bubble)
{
    assert_param(IS_AC_BUBBLE_STATUS(bubble));

    sensor_state.liquid_level.bit.AC_bubble = bubble;

}

void write_hige_level(level_status level)
{
    assert_param(IS_LEVEL_STATUS(level));

    sensor_state.liquid_level.bit.high_level = level;

}
void write_low_level(level_status level)
{
    assert_param(IS_LEVEL_STATUS(level));

    sensor_state.liquid_level.bit.low_level = level;

}

void write_feedback_pump_state(sensor_status status)
{
    assert_param(IS_SENSOR_STATUS(status));

    sensor_state.feedback_pump_state = status;

}

void write_draw_pump_init(pump_init_status status)
{
    assert_param(IS_PUMP_INIT_STATUS(status));

    sensor_state.pump_init.bit.draw_pump_init = status;
}

void write_AC_pump_init(pump_init_status status)
{
    assert_param(IS_PUMP_INIT_STATUS(status));

    sensor_state.pump_init.bit.AC_pump_init = status;
}

void write_feedback_pump_init(pump_init_status status)
{
    assert_param(IS_PUMP_INIT_STATUS(status));

    sensor_state.pump_init.bit.feed_back_pump_init = status;
}

/**************************************************************/
/***************for pump_errcode     **************************/
/**************************************************************/








