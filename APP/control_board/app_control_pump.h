/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_pump.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/05
 * Description        : This file contains the motor error created by ARM1,ARM2,ARM3
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/05 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_MOTOR_ERROR_
#define _APP_CONTROL_MOTOR_ERROR_

#include "stdint.h"

// when send tpdo cmd, we query the rpdo status
#define QUERY_PERIOD                20
#define QUERY_COUNTS                20      /* query max time=20*20ms */

// enum where the error happened
typedef enum ERROR_MSG_SOURCE_ENUM
{
    ERR_SOURCE_UART,
    ERR_SOURCE_ARM0,
    ERR_SOURCE_CANOPEN,
    ERR_SOURCE_ARM1,
    ERR_SOURCE_ARM2,
    ERR_SOURCE_ARM3
} ERROR_MSG_SOURCE;

typedef enum ERROR_SLAVE_SOURCE_ENUM
{
    SLAVE_ERR_NONE,
    SLAVE_ERR_HARDFAULT,
    SLAVE_ERR_TASK
} ERROR_SLAVE_SOURCE;

typedef enum ERROR_SLAVE_TASK_ENUM
{
    ERR_TASK_WARING = 0x01,
    ERR_TASK_ERROR = 0x02
} ERROR_SLAVE_TASK;

// enum the motor type
typedef enum MOTOR_TYPE_ENUM
{
    TYPE_DRAW_PUMP,
    TYPE_AC_PUMP,
    TYPE_BACK_PUMP,
    TYPE_PLT_PUMP,
    TYPE_PLA_PUMP,
    TYPE_RBC_VALVE,
    TYPE_PLT_VALVE,
    TYPE_PLA_VALVE,
    TYPE_CENTRIFUGE,
    TYPE_CARTRIDGE
} MOTOR_TYPE;

// byte enum which pumps need to control
typedef __packed struct CTRL_PUMP_TYPE_STRU
{
    uint8_t ac_pump:1;
    uint8_t draw_pump:1;
    uint8_t feedBack_pump:1;
    uint8_t plt_pump:1;
    uint8_t pla_pump:1;
    uint8_t feedBack_dir:1;
    uint8_t reserved:2;
} CTRL_PUMP_TYPE;

// pump speed of control pump
typedef __packed struct CTRL_PUMP_SPEED_STRU
{
    uint16_t draw_speed;
    uint16_t ac_speed;
    uint16_t feedBack_speed;
    uint16_t pla_speed;
    uint16_t plt_speed;
} CTRL_PUMP_SPEED;

// distance pump need to turn
typedef struct CTRL_PUMP_DISTANCE_STUR
{
    uint16_t ac_distance;
    uint16_t draw_distance;
    uint16_t feedBack_distance;
    uint16_t plt_distance;
    uint16_t pla_distance;
} CTRL_PUMP_DISTANCE;

// if pump need to recount or continue count
typedef __packed struct CTRL_PUMP_COUNT_STRU
{
    uint8_t ac_count:1;
    uint8_t draw_count:1;
    uint8_t feedBack_count:1;
    uint8_t plt_count:1;
    uint8_t pla_count:1;
    uint8_t reserved:3;
} CTRL_PUMP_COUNT;

// byte enum which valves need to turn
typedef __packed struct CTRL_VALVE_TYPE_STRU
{
    uint8_t rbc_valve:1;
    uint8_t pla_valve:1;
    uint8_t plt_valve:1;
    uint8_t reserved:5;
} CTRL_VALVE_TYPE;

// valve direction
typedef __packed struct CTRL_VALVE_DIR_STRU
{
    uint8_t rbc_valve:2;
    uint8_t pla_valve:2;
    uint8_t plt_valve:2;
    uint8_t reserved:2;
} CTRL_VALVE_DIR;

// door lock
typedef enum CTRL_LOCK_TYPE_ENUM
{
    TYPE_DOOR_LOCK,
    TYPE_DOOR_UNLOCK
} CTRL_LOCK_TYPE;

// cassette rising and falling
typedef enum CTRL_CASSETTE_TYPE_ENUM
{
    TYPE_CASSETTE_RISE,
    TYPE_CASSETTE_FALL
} CTRL_CASSETTE_TYPE;

// valve turn direction
typedef enum VALVE_DIR_ENUM
{
    VALVE_DIR_LEFT,
    VALVE_DIR_MIDDLE,
    VALVE_DIR_RIGHT
} VALVE_DIR;

typedef enum CTRL_DISTANCE_TYPE_ENUM
{
    SINGLE_DISTANCE=0x00,
    TOTAL_DISTANCE=0x01,
    CLEAR_DISTANCE=0x03
} CTRL_DISTANCE_TYPE;


/*
 * Function: init_pump_param
 * Description: init all parameters of the pumps
 * Param: type,pump_type;speed,pump_speed;distance,pump_distance;count,pump_count;
 * Return: uint8_t,0-OK;1-ERROR
*/
void init_pump_param(CTRL_PUMP_TYPE *type, CTRL_PUMP_SPEED *speed, 
                        CTRL_PUMP_DISTANCE *distance, CTRL_PUMP_COUNT *count);

/*
 * Function: start_pump
 * Description: start pump or change pump speed
 * Param: type,pump_type;speed,pump_speed;distance,pump_distance;count,pump_count;
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_start_pump(CTRL_PUMP_TYPE type, CTRL_PUMP_SPEED speed, 
                        CTRL_PUMP_DISTANCE distance, CTRL_PUMP_COUNT count);

/*
 * Function: ctrl_init_pump_pos
 * Description: init the pumps position before we load the canal
 * Param: type,pump_type;
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_init_pump_pos(CTRL_PUMP_TYPE type);

/*
 * Function: wait_pump_end
 * Description: wait until all pumps run the distance
 * Param: type,pump_type;speed,pump_speed;distance,pump_distance;
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t wait_pump_end(CTRL_PUMP_TYPE type, CTRL_PUMP_SPEED speed, 
                        CTRL_PUMP_DISTANCE distance);

/*
 * Function: stop_pump
 * Description: when stop pump call this function
 * Param: type,pump_type; count,pump_count;
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_stop_pump(CTRL_PUMP_TYPE type, CTRL_PUMP_COUNT count);

/*
 * Function: init_valve_param
 * Description: init all valve params
 * Param: type,valve type; dir,valve direction;
 * Return: uint8_t,0-OK;1-ERROR
*/
void init_valve_param(CTRL_VALVE_TYPE *type, CTRL_VALVE_DIR *dir);

/*
 * Function: turn_valve
 * Description: when turn valve call this function
 * Param: type,valve type; dir,valve direction;
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_turn_valve(CTRL_VALVE_TYPE type, CTRL_VALVE_DIR dir);

/*
 * Function: start_centrifuge
 * Description: when change cetrifuge's speed call this function
 * Param: speed,turn speed;
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_start_centrifuge(uint16_t speed);

/*
 * Function: ctrl_stop_centrifuge
 * Description: when stop centrifuge call this function
 * Param: NULL
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_stop_centrifuge(void);

/*
 * Function: ctrl_door_lock
 * Description: when centrifuge lock/unlock the door
 * Param: lock,TYPE_DOOR_LOCK-lock the door, TYPE_DOOR_UNLOCK-unlock the door 
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_door_lock(CTRL_LOCK_TYPE lock);

/*
 * Function: ctrl_run_cassette
 * Description: when fall or rise cassette
 * Param: dir,TYPE_CASSETTE_RISE-rise the casstte, TYPE_CASSETTE_FALL-fall the cassette 
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t ctrl_run_cassette(CTRL_CASSETTE_TYPE dir);

/*
 * Function: pause_run_pumps
 * Description: when pause pumps
 * Param: None
 * Return: uint8_t,0-OK,1-Error
*/
uint8_t pause_run_pumps(void);

/*
 * Function: resume_run_pumps
 * Description: when resume pumps
 * Param: NONE
 * Return: uint8_t,0-OK,1-Error
*/
uint8_t resume_run_pumps(void);

/*
 * Function: pressure_abnormal_handle
 * Description: when low or high pressure happened
 * Param: NONE
 * Return: uint8_t,0-OK,1-Error
*/
uint8_t pressure_abnormal_handle(void);

/*
 * Function: pressure_abnormal_handle
 * Description: when pressure resumed happened
 * Param: NONE
 * Return: uint8_t,0-OK,1-Error
*/
uint8_t pressure_normal_handle(void);

/*
 * Function: ctrl_load_canal
 * Description: load canal
 * Param: NONE
 * Return: uint8_t,0-OK,1-Error
*/
uint8_t ctrl_load_canal(void);


/*
 * Function: ctrl_unload_canal
 * Description: unload canal
 * Param: NONE
 * Return: uint8_t,0-OK,1-Error
*/
uint8_t ctrl_unload_canal(void);

/*
 * Function: clear_pump_total_distance
 * Description: clear slave board's pump total count
 * Param: type, pump type
 * Return: None
*/
void clear_pump_total_distance(CTRL_PUMP_TYPE type);

/*
 * Function: get_pump_total_distance
 * Description: get pumps total distance of the specify pump
 * Param: type, pump type
 * Return: None
*/
CTRL_PUMP_DISTANCE get_pump_total_distance(CTRL_PUMP_TYPE type);

#endif

