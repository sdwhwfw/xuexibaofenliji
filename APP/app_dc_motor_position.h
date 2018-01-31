/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_dc_motor_position.h
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
#include "rmp_cntl.h"
#include "pid.h"
#include "encoder.h"
#include "pid_reg3.h"

typedef enum {
	MOTOR_NUM1,
	MOTOR_NUM2,
	MOTOR_NUM3,
	MOTOR_NUM
} dc_motor_num;

#define IS_DC_MOTOR_NUM_TYPE(TYPE)    (((TYPE) == MOTOR_NUM1) || \
                                        ((TYPE) == MOTOR_NUM2) || \
                                        ((TYPE) == MOTOR_NUM3))


typedef enum
{
    MOTOR_MODE0,    // speed closed loop control
    MOTOR_MODE1,    // position closed loop control
    MOTOR_MODE2,    // speed + position closed loop control
    MOTOR_MODE3,    // speed + position closed loop control with a limit mini speed
    MOTOR_MODE_NUM
} motor_mode;

#define IS_DC_MOTOR_MODE_TYPE(TYPE)    (((TYPE) == MOTOR_MODE0) || \
                                        ((TYPE) == MOTOR_MODE1) || \
                                        ((TYPE) == MOTOR_MODE2) || \
                                        ((TYPE) == MOTOR_MODE3))


typedef struct
{
    u32 index;
    s32 speed;
    s32 pwm;
    s32 position;
}dc_motor_mem_t;


typedef struct
{
    // PID parameter
    u32 index;
    u32 kp;
    u32 kpp;
    u32 kpi;
    u32 kpd;

    // private value
    u32 crc;
}dc_motor_PID_param_t;


typedef struct
{
    // dc motor parameter
    float                   reduction_ratio;
    motor_mode              motor_operational_mode;
    // encoder device instance
    encoder_device          encoder;
    // PID device instance
    PID                     pid_position_device;
    PID                     pid_sp_device;
    // TI PID
    PIDREG3                 pid_ti_position;
    PIDREG3                 pid_ti_speed;
    // RAMP control instance
    RMPCNTL                 rmp_control;                    // PID RAMP control
    // variables
    dc_motor_num            index;
    float                   current_motor_pwm;              // PID calculation results, float
    vu8                     motor_run_flag;                 // motor run flag, 1 ---> run, 0 ---> stop
    vs32                    pid_operation_time;             // for record PID operation time, debug only
    float                   pid_compute_interval_time;      // interval time of PID compute, unit: second
    float                   position_output;                // PID position closed loop output

    // compute dc motor speed
    float                   dc_motor_sp;                    // dc motor speed in RPM
    float                   dc_motor_sp_count;              // dc motor speed in pulses count

    // output
    vs32                    current_motor_position;         // DC motor position

}dc_motor_device_t;


typedef dc_motor_device_t *dc_motor_handle;


// dc motor reduction ratio
#define DC_MOTOR_REDUCTION_RATIO    (3.71)   //(10.06)

// enocder parameter: number of pulses per revolution
#define ENCODER_PPR                 500 //(512)

// PI
#define PI                          (3.1415926)


// DC motor module init
void    init_dc_motor(void);
void    motor_module_init(u32 pwm_freq, u32 interval_time);
void    dc_motor_device_init(u32 pid_interval_time);
void    dc_motor_sp_pid_param_config(dc_motor_handle v, dc_motor_PID_param_t* pid);
void    dc_motor_position_pid_param_config(dc_motor_handle v, dc_motor_PID_param_t* pid);

// DC motor PID parameter read and set
void    get_pid_param_from_internal_flash(dc_motor_PID_param_t* param);
u8      write_pid_param_to_internal_flash(dc_motor_PID_param_t* param);

// DC motor operation
void    set_dc_motor_mode(dc_motor_handle v, motor_mode mode);
dc_motor_handle get_dc_motor_device_instance(dc_motor_num index);
void    set_dc_motor_speed(dc_motor_handle v, float sp);
void    set_dc_motor_position(dc_motor_handle v, s32 position);
void    start_dc_motor(dc_motor_handle);
void    stop_dc_motor(dc_motor_handle);

// get parameter of DC motor
float   get_dc_motor_current_distance(dc_motor_handle);
void    clear_dc_motor_current_distance(dc_motor_handle);
s32     get_adjust_pwm_percent(dc_motor_handle v, u32* current_min, u32* pwm_max);
float   get_dc_motor_sp(dc_motor_handle);
s32     get_current_dc_motor_speed(dc_motor_handle);


#ifdef USE_USB_TO_DEBUG_DC_MOTOR
void    get_dc_motor_param(dc_motor_num index);
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */


#endif /* _APP_DC_MOTOR_H_ */

