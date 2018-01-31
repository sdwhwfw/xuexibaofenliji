/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_slave_board2.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the define of code struct(TPDO/RPDO)
 *                      interface
 *******************************************************************************
 * History:
 * DATE 	  | VER   | AUTOR	   | Description
 * 2013/11/19 | v1.0  | WQ		   | initial released
 *******************************************************************************/

#include "struct_slave_board2.h"
#include "app_slave_board2.h"
#include "trace.h"


#define RATIO_PUMP_SPEED 100

void write_pump_direction(pump_type pump,pump_direction dir)
{
    assert_param(IS_PUMP_TYPE(pump));
    assert_param(IS_PUMP_DIRECTION(dir));

    switch(pump)
    {
        case PLT_PUMP:
            pump_state1.pump_current_dir.bit.PLT_pump = dir;
        break;
        case PLASMA_PUMP:
            pump_state1.pump_current_dir.bit.plasma_pump = dir;
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
        case PLT_PUMP:
            pump_state1.PLT_pump_current_speed = speed * RATIO_PUMP_SPEED;
        break;
        case PLASMA_PUMP:
            pump_state1.plasma_pump_current_speed = speed * RATIO_PUMP_SPEED;
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
    //pump_state1.fn = control_order_r.fn;//updata Fn

}
void write_pump_moved_distance(pump_type pump,uint16_t distance)
{
    assert_param(IS_PUMP_TYPE(pump));
    switch(pump)
    {
        case PLT_PUMP:
            if(control_order_r.order_indicate.bit.PLT_valve_or_pump == 0x01)/*启动的时候加*/
            //if(0)
            {
                pump_state2.PLT_pump_moved_distance = distance + PLT_pump_old_single_distance;
            }
            else
            {
                pump_state2.PLT_pump_moved_distance = distance;
            }
            pump_state2.single_or_total.bit.PLT_pump = pump_state2_handle.single_or_total.bit.PLT_pump;
        break;
        case PLASMA_PUMP:
            if(control_order_r.order_indicate.bit.plasma_valve_or_pump == 0x01)/*启动的时候加*/
            //if(0)
            {
                pump_state2.plasma_pump_moved_distance = distance + plasma_pump_old_single_distance;
            }
            else
            {
                pump_state2.plasma_pump_moved_distance = distance;
            }
            pump_state2.single_or_total.bit.plasma_pump = pump_state2_handle.single_or_total.bit.plasma_pump;
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

   // pump_state2.fn = control_order_r.fn;//updata FN

}
void write_pump_distance_flag(pump_type pump,distance_totalsingle flag)
{
    assert_param(IS_PUMP_TYPE(pump));
    assert_param(IS_DISTANCE_TOTALSINGLE(flag));

    switch(pump)
    {
        case PLT_PUMP:
            control_order_r.single_or_total.bit.PLT_pump = flag;
        break;
        case PLASMA_PUMP:
            control_order_r.single_or_total.bit.plasma_pump = flag;
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
        case PLT_PUMP:
           ret = control_order_r.single_or_total.bit.PLT_pump;
        break;
        case PLASMA_PUMP:
           ret = control_order_r.single_or_total.bit.plasma_pump;
        break;
        default:
        break;
    }

    return (distance_totalsingle)ret;
}



/**************************************************************/
/***************for sensor state     **************************/
/**************************************************************/

void write_RBC_detector(RBC_detector statue)
{
    assert_param(IS_RBC_DETECTOR_STATUS(statue));
    sensor_state.RBC_detector = statue;

}


void write_valve_location(valve_type type,valve_location_type location)
{
    assert_param(IS_VALVE_TYPE(type));
    assert_param(IS_VALVE_LOCATION_TYPE(location));

    switch(type)
    {
        case RBC_VALVE:
            switch(location)
            {
                case LEFT_VALVE://0x00
                    sensor_state.valve.bit.RBC_valve_left = 0x01;
                    sensor_state.valve.bit.RBC_valve_mid = 0x00;
                    sensor_state.valve.bit.RBC_valve_right = 0x00;
                    break;
                case MID_VALVE://0x01
                    sensor_state.valve.bit.RBC_valve_left = 0x00;
                    sensor_state.valve.bit.RBC_valve_mid = 0x01;
                    sensor_state.valve.bit.RBC_valve_right = 0x00;
                    break;
                case RIGHT_VALVE://0x02
                    sensor_state.valve.bit.RBC_valve_left = 0x00;
                    sensor_state.valve.bit.RBC_valve_mid = 0x00;
                    sensor_state.valve.bit.RBC_valve_right = 0x01;
                    break;
                case UNKNOW_VALVE://0x03
                    //unknow statue
                    sensor_state.valve.bit.RBC_valve_left = 0x00;
                    sensor_state.valve.bit.RBC_valve_mid = 0x00;
                    sensor_state.valve.bit.RBC_valve_right = 0x00;
                    break;
                default:
                    break;
            }
            break;
        case PLASMA_VALVE:
            switch(location)
            {
                case LEFT_VALVE:
                    sensor_state.valve.bit.plsama_valve_left = 0x01;
                    sensor_state.valve.bit.plsama_valve_mid = 0x00;
                    sensor_state.valve.bit.plsama_valve_right = 0x00;
                    break;
                case MID_VALVE:
                    sensor_state.valve.bit.plsama_valve_left = 0x00;
                    sensor_state.valve.bit.plsama_valve_mid = 0x01;
                    sensor_state.valve.bit.plsama_valve_right = 0x00;
                    break;
                case RIGHT_VALVE:
                    sensor_state.valve.bit.plsama_valve_left = 0x00;
                    sensor_state.valve.bit.plsama_valve_mid = 0x00;
                    sensor_state.valve.bit.plsama_valve_right = 0x01;
                    break;
                case UNKNOW_VALVE:
                    //unknow statue
                    sensor_state.valve.bit.plsama_valve_left = 0x00;
                    sensor_state.valve.bit.plsama_valve_mid = 0x00;
                    sensor_state.valve.bit.plsama_valve_right = 0x00;
                    break;
                default:
                    break;
            }

            break;
        case PLT_VALVE:
            switch(location)
            {
                case LEFT_VALVE:
                    sensor_state.valve.bit.PLT_valve_left = 0x01;
                    sensor_state.valve.bit.PLT_valve_mid = 0x00;
                    sensor_state.valve.bit.PLT_valve_right = 0x00;
                    break;
                case MID_VALVE:
                    sensor_state.valve.bit.PLT_valve_left = 0x00;
                    sensor_state.valve.bit.PLT_valve_mid = 0x01;
                    sensor_state.valve.bit.PLT_valve_right = 0x00;
                    break;
                case RIGHT_VALVE:
                    sensor_state.valve.bit.PLT_valve_left = 0x00;
                    sensor_state.valve.bit.PLT_valve_mid = 0x00;
                    sensor_state.valve.bit.PLT_valve_right = 0x01;
                    break;
                case UNKNOW_VALVE:
                    //unknow statue
                    sensor_state.valve.bit.PLT_valve_left = 0x00;
                    sensor_state.valve.bit.PLT_valve_mid = 0x00;
                    sensor_state.valve.bit.PLT_valve_right = 0x00;
                    break;
                default:
                    break;
            }

            break;
        default:
            break;
    }
}


void write_PLT_pump_init(pump_init_status status)
{
    assert_param(IS_PUMP_INIT_STATUS(status));

    sensor_state.pump_init.bit.PLT_pump_init = status;
}

void write_plasma_pump_init(pump_init_status status)
{
    assert_param(IS_PUMP_INIT_STATUS(status));

    sensor_state.pump_init.bit.plasma_pump_init = status;
}

/**************************************************************/
/***************for pump_errcode     **************************/
/**************************************************************/


