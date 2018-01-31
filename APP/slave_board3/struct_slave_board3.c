/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_slave_board3.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the define of code struct(TPDO/RPDO)
 *                      interface
 *******************************************************************************
 * History:
 * DATE 	  | VER   | AUTOR	   | Description
 * 2013/11/19 | v1.0  | WQ		   | initial released
 *******************************************************************************/

#include "struct_slave_board3.h"
#include "app_slave_board3.h"


void write_motor_direction(motor_direction dir)
{
    assert_param(IS_MOTOR_DIRECTION(dir));
    pump_state1.motor_current_dir.bit.centrifuge_motor = dir;

}

void write_motor_speed(uint16_t speed)
{
    pump_state1.centrifuge_motor_current_speed = speed;
}




/**************************************************************/
/***************for sensor state     **************************/
/**************************************************************/

void write_centrifuge_motor_preesure(u16 preesure)
{
    //transform from volatage to preesure

    sensor_state.centrifuge_motor_preasure = preesure;

}



void write_centrifuge_motor_state(sensor_status status)
{
    assert_param(IS_SENSOR_STATUS(status));

    sensor_state.centrifuge_motor_state = status;

}

void write_weeping_detector(sensor_status status)
{
    assert_param(IS_SENSOR_STATUS(status));
    sensor_state.weeping_detector = status;
}

void write_door_electromagnet(door_electromagne_status status)
{
    assert_param(IS_DOOR_ELECTROMAGNE_STATUS(status));
    if(status == LOCK)
    {
        sensor_state.sensor.bit.door_switch1 = 0x01;
        sensor_state.sensor.bit.door_switch2 = 0x00;
    }
    else
    {
        sensor_state.sensor.bit.door_switch1 = 0x00;
        sensor_state.sensor.bit.door_switch2 = 0x01;
    }
}

void write_door_hall(door_hall_status status)
{
    assert_param(IS_DOOR_HALL_STATUS(status));
    sensor_state.sensor.bit.door_hoare = status;
}


void write_cassette_location(cassette_location_status status)
{
    assert_param(IS_CASSETTE_LACATION_STATUS(status));
    sensor_state.sensor.bit.cassette_location = status;
}

void write_voltage_status(voltage_type voltage,sensor_status status)
{
    assert_param(IS_VOLTAGE_TYPE(voltage));
    assert_param(IS_SENSOR_STATUS(status));
    switch(voltage)
    {
        case STATUS_70V :
            motor_errcode.errcode_voltage.bit.volage_70V = status;
            break;
        case STATUS_24V :
            motor_errcode.errcode_voltage.bit.volage_24V = status;
            break;
        case STATUS_S16 :
            motor_errcode.errcode_voltage.bit.volage_S16 = status;
            break;
        case STATUS_S19 :
            motor_errcode.errcode_voltage.bit.volage_S19 = status;
            break;
        case STATUS_P_12 :
            motor_errcode.errcode_voltage.bit.volage_positive_12V = status;
            break;
        case STATUS_N_12 :
            motor_errcode.errcode_voltage.bit.volage_negative_12V = status;
            break;
        case STATUS_5V :
            motor_errcode.errcode_voltage.bit.volage_5V = status;
            break;

    }

}

void write_fan_state(fan_type fan,sensor_status status)
{
    assert_param(IS_FAN_TYPE(fan));
    assert_param(IS_SENSOR_STATUS(status));

    switch(fan)
    {
        case FAN1:
            motor_errcode.errcode_fan.bit.fan1 = status;
            break;
        case FAN2:
            motor_errcode.errcode_fan.bit.fan2 = status;
            break;
        case FAN3:
            motor_errcode.errcode_fan.bit.fan3 = status;
            break;
        case FAN4:
            motor_errcode.errcode_fan.bit.fan4 = status;
            break;
        default:
            break;
    }
}

/**************************************************************/
/***************for pump_errcode     **************************/
/**************************************************************/




