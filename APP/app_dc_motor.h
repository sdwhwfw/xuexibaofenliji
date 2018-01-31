/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_dc_motor.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/25
 * Description        : This file contains the software implementation for the
 *                      DC motor control unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/25 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/
#ifndef _APP_DC_MOTOR_H_
#define _APP_DC_MOTOR_H_

#include "stm32f2xx.h"


typedef enum {
	MOTOR_NUM1,
	MOTOR_NUM2,
	MOTOR_NUM3,
	MOTOR_NUM
} MotorType;


#define IS_DC_MOTOR_NUM_TYPE(TYPE)    (((TYPE) == MOTOR_NUM1) || \
                                        ((TYPE) == MOTOR_NUM2) || \
                                        ((TYPE) == MOTOR_NUM3))


typedef struct
{
    u32 index;
    s32 speed;
    s32 pwm;
}dc_motor_mem_t;


typedef struct
{
    u32 index;
    u32 kp;
    u32 kpp;
    u32 kpi;
    u32 kpd;
    u32 crc;    // private value
}dc_motor_PID_param_t;


#define DC_MOTOR_REDUCTION_RATIO    10.06
#define DC_MOTOR_DIS_PRECISION      10
#define DC_MOTOR_SPEED_PRECISION    1


// DC motor module init
void init_dc_motor(void);
void motor_module_init(uint32_t pwm_freq, uint32_t interval_time);
void dc_motor_pid_param_config(MotorType index, uint32_t kp, uint32_t kpp, uint32_t kpi, uint32_t kpd);

// DC motor PID parameter read and set
void get_pid_param_from_internal_flash(dc_motor_PID_param_t* param);
u8   write_pid_param_to_internal_flash(dc_motor_PID_param_t* param);

// DC motor operation
void set_motor_speed(MotorType index, float sp, u32 distance);
void start_dc_motor(MotorType index);
void stop_dc_motor(MotorType index);

// get parameter of DC motor
u32  get_dc_motor_current_distance(MotorType index);
void clear_dc_motor_current_distance(MotorType index);
u32  get_adjust_pwm_percent(MotorType index, u32* current_min, u32* pwm_max);
float  get_dc_motor_sp(MotorType index);




#endif /* _APP_DC_MOTOR_H_ */

