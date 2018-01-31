/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_dc_motor.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/25
 * Description        : This file contains the software implementation for the
 *                      DC motor control unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/10/25 | v1.0  |            | initial released
 * 2013/11/09 | v1.1  | Bruce.zhu  | add support for ARM1 ARM2 ARM3 board
 * 2013/12/03 | v1.2  | Bruce.zhu  | fix BUG: pwm change precision
 * 2013/12/27 | v1.3  | Bruce.zhu  | update PID with RMPCNTL
 *******************************************************************************/

#ifndef _APP_DC_MOTOR_C_
#define _APP_DC_MOTOR_C_


#include "app_dc_motor_position.h"
#include "trace.h"
#include "pwm.h"
#include "queue.h"
#include "pid.h"
//#include "pid_q.h"
#include "encoder.h"
#include "basic_timer.h"
#include "protocol.h"
#include "trace.h"
#include "math.h"
#include "flash_if.h"


#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
//============================
// USB for debug
//============================
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

#endif /* USE_USB_TO_DEBUG_DC_MOTOR */



/* Private variables ---------------------------------------------------------*/
#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
static OS_STK dc_motor_task_stk[DC_MOTOR_TASK_STK_SIZE];
static OS_STK dc_motor_send_task_stk[DC_MOTOR_SEND_TASK_STK_SIZE];

#define  DC_MOTOR_QUEUE_SIZE     1024
static void*      g_dc_motor_tx_QueueTbl[DC_MOTOR_QUEUE_SIZE];
static void*      g_dc_motor_rx_QueueTbl[DC_MOTOR_QUEUE_SIZE];

#define DC_MOTOR_DEBUG_BUF_SIZE     DC_MOTOR_QUEUE_SIZE
#define DC_MOTOR_DEBUG_BUF_LEN      sizeof(dc_motor_mem_t)
static u8       g_dc_motor_debug_buf[DC_MOTOR_DEBUG_BUF_SIZE][DC_MOTOR_DEBUG_BUF_LEN];
static OS_MEM*  g_dc_motor_debug_mem;

static OS_EVENT*  g_dc_motor_tx_queue;
static OS_EVENT*  g_dc_motor_tx_sem;

OS_EVENT*  g_dc_motor_rx_queue;

#endif /* USE_USB_TO_DEBUG_DC_MOTOR */


/* Private variables ---------------------------------------------------------*/

// dc motor device instance
static dc_motor_device_t dc_motor_device[MOTOR_NUM];


// USB device
#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */


// wheel perimeter = 90mm
#define WHEEL_PERIMETER  0.09


//========================================
// PID PARAM default value
//========================================
#define		KP		                100
#define		KPP		                90
#define		KPI		                10
#define		KPD		                0

// save param Base @ of Sector 11, 128 Kbyte
#define     PID_PARAM_ADDRESS       ((uint32_t)0x080E0000)
// save buffer size byte
#define     PID_SAVE_BUFFER_SIZE    1024

// USE IQmath
//#define     USE_IQMATH_LIB

// ----------- acceleration -----------
#define MAX_SPEED           1000                                 // RPM
#define T_ACCELER_SP        1                                   // acceleration time(second)
#define K_RAMP              (MAX_SPEED*500*3.71*4/60/T_ACCELER_SP)  // k 85333.33
#define T_SAMPLE            0.02                                // 20ms

// kT^2
#define MINI_STEP           (K_RAMP*T_SAMPLE*T_SAMPLE)
#define ACCELERA_MAX_STEP   (K_RAMP*T_ACCELER_SP*T_SAMPLE)  // kT*T

// ----------- deceleration -----------
#define T_DECLERA_SP        1                                  // deceleration time(second)
#define K_DECLERA_RAMP      (MAX_SPEED*500*3.71/60/T_DECLERA_SP)  // k 85333.33
#define DECLERA_MINI_STEP   (K_DECLERA_RAMP*T_SAMPLE*T_SAMPLE)  // kT^2
#define DECLERA_MAX_STEP    (K_DECLERA_RAMP*T_DECLERA_SP*T_SAMPLE)  // kT*T



/* Private functions ---------------------------------------------------------*/
#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
static void app_dc_motor_task(void *p_arg);
static void app_dc_motor_send_task(void *p_arg);
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */


/**
  * @brief: save PID param to internal flash memory @ FLASH_Sector_11 (last sector)
  * @param: index, pid param index
  * @param: param, the pointer to pid param struct which you want to save
  * @retval: 0 ----> success
  *          1 ----> fail
  */
u8 write_pid_param_to_internal_flash(dc_motor_PID_param_t* param)
{
    u16 ret;
    u8* p_save_old_data = 0;

    // first save the old flash content
    // note: we only save PID_SAVE_BUFFER_SIZE
    p_save_old_data = (u8*)malloc(PID_SAVE_BUFFER_SIZE);
    assert_param(p_save_old_data);

    memcpy((void*)p_save_old_data, (void*)PID_PARAM_ADDRESS, PID_SAVE_BUFFER_SIZE);

    /* init internal flash */
    flash_if_init();

    ret = FLASH_If_GetWriteProtectionStatus();
    /* Some sectors inside the user flash area are write protected */
    if (ret == 0)
    {
        /* Disable the write protection */
        if (FLASH_If_DisableWriteProtection() == 2)
        {
            return 1;
        }
    }

    /* erase pid parameter application area */
    flash_if_erase(PID_PARAM_ADDRESS);

    // add CRC32
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    CRC_ResetDR();
    param->crc = CRC_CalcBlockCRC((u32*)param, sizeof(dc_motor_PID_param_t)/4);
    memcpy((void*)(p_save_old_data + param->index*(sizeof(dc_motor_PID_param_t))),
            (void*)param,
            sizeof(dc_motor_PID_param_t));

    // wirte to flash
    if (flash_if_write((vu32)PID_PARAM_ADDRESS,
                        (u32*)p_save_old_data,
                        PID_SAVE_BUFFER_SIZE/4) != 0)
    {
        APP_TRACE("flash_if_write error\r\n");
        FLASH_Lock();
        // don't forget to free memory
        free(p_save_old_data);
        return 1;
    }

    // lock flash
    FLASH_Lock();
    // free memory
    free(p_save_old_data);

    return 0;
}


void get_pid_param_from_internal_flash(dc_motor_PID_param_t* param)
{
    u32     index;
    u32     crc_value;

    index = param->index;
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));

    memcpy((void*)param,
            (void*)(PID_PARAM_ADDRESS + index*sizeof(dc_motor_PID_param_t)),
            sizeof(dc_motor_PID_param_t));

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    CRC_ResetDR();
    crc_value = CRC_CalcBlockCRC((void*)param, sizeof(dc_motor_PID_param_t)/4);
    // check CRC
    if (crc_value != param->crc)
    {
        APP_TRACE("* [%d]PID CRC error[%x, %x], use default\r\n", index + 1, crc_value, param->crc);
        param->index = index;
        param->kp  = KP;
        param->kpp = KPP;
        param->kpi = KPI;
        param->kpd = KPD;
    }
}


static void rpm_sp_to_encoder_count(dc_motor_handle v, float rpm, float *encoder_count_sp)
{
    *encoder_count_sp = rpm*v->encoder.encoder_ppr*4*v->reduction_ratio/60.0;
}


/**
  * @brief: convert speed(m/s) to encoder_count/s
  * @param: v, dc_motor_handle
  * @param: sp, the speed(m/s)
  * @param: store the convert speed value
  */
static void speed_to_encouder_count(dc_motor_handle v, float sp, float *encoder_count_sp)
{
    *encoder_count_sp = sp * v->encoder.encoder_ppr * 4 * v->reduction_ratio / WHEEL_PERIMETER;
}


void dc_motor_slope_config(dc_motor_handle v,
                                      float acceleration_slope,
                                      float deceleration_slope,
                                      float max_speed)
{
    // acceleration 1000 RPM ~ 123666.667(pulse/second^2)
    v->rmp_control.acceleration_slope.k_ramp_value = 1000.0*v->encoder.encoder_ppr*v->reduction_ratio*4/60.0;
    v->rmp_control.deceleration_slope.k_ramp_value = -1000.0*v->encoder.encoder_ppr*v->reduction_ratio*4/60.0;
    // 1000RPM ~ 123666.667(pulse/second)
    v->rmp_control.max_speed                       = 1000.0*v->encoder.encoder_ppr*v->reduction_ratio*4/60.0;
    v->rmp_control.ramp_control_state              = V_OPERATION_START;
}


/**
  * @brief: compute ramp control parameter include acceleration deceleration
  *         and constant speed stage.
  * @param: dc_motor_device instance
  * @param: position, the wanted target position
  * @NOTE:  acceleration slope and deceleration slope must be set!!!
  *
  */
void dc_motor_position_config(dc_motor_handle v, s32 position)
{
    float temp_t1, temp_t2;
    float min_erea, constant_sp_erea;

    // check ramp control state
    if (v->rmp_control.ramp_control_state == V_OPERATION_START ||
        v->rmp_control.ramp_control_state == V_OPERATION_DONE)
    {
        v->rmp_control.target_position = position;

        // compute acceleration time, unit: second
        temp_t1 = v->rmp_control.max_speed/v->rmp_control.acceleration_slope.k_ramp_value;
        // compute deceleration time, unit: second
        temp_t2 = -(v->rmp_control.max_speed/v->rmp_control.deceleration_slope.k_ramp_value);
        // compute critical point
        min_erea = (temp_t1 + temp_t2)*v->rmp_control.max_speed/2.0;

        // trapezoid or triangle motion
        if (v->rmp_control.target_position >= (s32)min_erea)
        {
            // copmute acceleration parameter
            v->rmp_control.acceleration_slope.count = (u32)(temp_t1/v->pid_compute_interval_time);
            v->rmp_control.acceleration_slope.step  = v->rmp_control.acceleration_slope.k_ramp_value *
                                                      v->pid_compute_interval_time *
                                                      v->pid_compute_interval_time;

            // compute constant speed parameter
            constant_sp_erea = v->rmp_control.target_position - min_erea;
            v->rmp_control.constant_sp.step  = v->rmp_control.max_speed*v->pid_compute_interval_time;
            v->rmp_control.constant_sp.count = (u32)(constant_sp_erea/v->rmp_control.constant_sp.step);

            // compute deceleration parameter
            v->rmp_control.deceleration_slope.count = (u32)(temp_t2/v->pid_compute_interval_time);
            v->rmp_control.deceleration_slope.step  = v->rmp_control.deceleration_slope.k_ramp_value *
                                                      v->pid_compute_interval_time *
                                                      v->pid_compute_interval_time;

            APP_TRACE("----------- parameter configuration -----------\r\n");
            APP_TRACE("- count = %d, %d, %d\r\n",
                        v->rmp_control.acceleration_slope.count,
                        v->rmp_control.constant_sp.count,
                        v->rmp_control.deceleration_slope.count);
            APP_TRACE("- step  = %d, %d, %d\r\n",
                        (s32)(v->rmp_control.acceleration_slope.step*1000),
                        (s32)(v->rmp_control.constant_sp.step*1000),
                        (s32)(v->rmp_control.deceleration_slope.step*1000));

            // set ramp control state to acceleration
            v->rmp_control.ramp_control_state = V_OPERATION_ACCELERATE;

        }
        else
        {
            // TODO: add triangle deal code
            v->rmp_control.constant_sp.count = 0;
        }
    }
    else
    {
        // error
    }
}


/**
  * @brief: init dc motor device instance
  * @param: none
  * @retval: void
  */
void dc_motor_device_init(u32 pid_interval_time)
{
    dc_motor_num i;
    dc_motor_PID_param_t pid;
    dc_motor_handle dc_motor_instance;

    APP_TRACE("------- DC motor configuration -------\r\n");

    for (i = MOTOR_NUM1; i < MOTOR_NUM; i++)
    {
        // get dc motor device instance
        dc_motor_instance = get_dc_motor_device_instance(i);

        // clear dc motor device struct
        memset(dc_motor_instance, 0x00, sizeof(dc_motor_device_t));

        // dc motor parameter configuration
        dc_motor_instance->reduction_ratio               = DC_MOTOR_REDUCTION_RATIO;
        dc_motor_instance->index                         = i;
        dc_motor_instance->motor_operational_mode        = MOTOR_MODE1;
        dc_motor_instance->pid_compute_interval_time     = pid_interval_time/1000000.0;

        // encoder device parameter init
        dc_motor_instance->encoder.encoder_ppr           = (u32)ENCODER_PPR;
        dc_motor_instance->encoder.index                 = (encoder_num)i;          // encoder index, ENCODER_NUM1 ~ ENCODER_NUM3
        dc_motor_instance->encoder.max_interval_count    = (u32)4500;
        switch (i)
        {
            case MOTOR_NUM1:
            case MOTOR_NUM2:
                QET_configuration(&dc_motor_instance->encoder);
                break;
            case MOTOR_NUM3:
#if defined(USE_ARM1_REV_0_1)
                // TODO: encoder TIM5 need to be chaned to other pins
                //QET_configuration(&dc_motor_instance->encoder);
#endif /* USE_ARM1_REV_0_1 */
                break;
        }

        // acceleration parmeter configuration
        //dc_motor_acceleration_param_config(dc_motor_instance);
        dc_motor_slope_config(dc_motor_instance, 123666.667, -123666.667, 123666.667);
        dc_motor_position_config(dc_motor_instance, 1236660);

        // get PID parameter from internal flash
        memset((void*)&pid, 0x00, sizeof pid);
        pid.index = i;
        get_pid_param_from_internal_flash(&pid);

        APP_TRACE("* motor%d PID: %d, %d, %d, %d\r\n", dc_motor_instance->index+1, pid.kp, pid.kpp, pid.kpi, pid.kpd);
    }
}

void dc_motor_sp_pid_param_config(dc_motor_handle v, dc_motor_PID_param_t* pid)
{
    float k1, k2, k3, k4;

    k1 = pid->kp/1000.0;
    k2 = pid->kpp/1000.0;
    k3 = pid->kpi/1000.0;
    k4 = pid->kpd/1000.0;

    APP_TRACE("set new speed PID parameter: kp = %d, kpp = %d, kpi = %d, kpd = %d\r\n",
                pid->kp,
                pid->kpp,
                pid->kpi,
                pid->kpd);

    PIDInit(&v->pid_sp_device);
    // A = Kp + Ki + Kd
    // B = Kp + 2Kd
    // C = Kd
    v->pid_sp_device.Proportion = k2*k1 + k3*k1 + k4*k1;
    v->pid_sp_device.Integral   = k2*k1 + 2.0*k4*k1;
    v->pid_sp_device.Derivative = k4*k1;

    memset(&v->pid_ti_speed, 0x00, sizeof(PIDREG3));
    v->pid_ti_speed.Kp = k2*k1;
    v->pid_ti_speed.Ki = k3*k1;
    v->pid_ti_speed.Kd = k4*k1;

}


void dc_motor_position_pid_param_config(dc_motor_handle v, dc_motor_PID_param_t* pid)
{
    float k1, k2, k3, k4;

    k1 = pid->kp/1000.0;
    k2 = pid->kpp/1000.0;
    k3 = pid->kpi/1000.0;
    k4 = pid->kpd/1000.0;

    PIDInit(&v->pid_position_device);
    // A = Kp + Ki + Kd
    // B = Kp + 2Kd
    // C = Kd
    v->pid_position_device.Proportion = k2*k1 + k3*k1 + k4*k1;
    v->pid_position_device.Integral   = k2*k1 + 2.0*k4*k1;
    v->pid_position_device.Derivative = k4*k1;

    APP_TRACE("position PID parameter(*1000000): A = %d, B = %d, C = %d\r\n",
                (s32)(v->pid_position_device.Proportion*1000000),
                (s32)(v->pid_position_device.Integral*1000000),
                (s32)(v->pid_position_device.Derivative*1000000));

    memset(&v->pid_ti_position, 0x00, sizeof(PIDREG3));
    v->pid_ti_position.Kp = k2*k1;
    v->pid_ti_position.Ki = k3*k1;
    v->pid_ti_position.Kd = k4*k1;

}


void set_dc_motor_mode(dc_motor_handle v, motor_mode mode)
{
    assert_param(IS_DC_MOTOR_MODE_TYPE(mode));
    assert_param(v);

    APP_TRACE("dc motor mode set to %d\r\n", mode);
    v->motor_operational_mode = mode;
}


/*
 * @brief: Set Motor speed
 * @param: dc_motor_handle, dc motor instance
 * @param: sp, speed, unit RPM
 */
void set_dc_motor_speed(dc_motor_handle v, float sp)
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();

    rpm_sp_to_encoder_count(v, sp, &v->pid_sp_device.SetPoint);
    rpm_sp_to_encoder_count(v, sp, &v->pid_ti_speed.Ref);
    //v->pid_sp_device.SetPoint = sp;
    //v->pid_ti_speed.Ref = sp;

    start_dc_motor(v);

    OS_EXIT_CRITICAL();
}


/*
 * @brief: Set Motor speed
 * @param: dc_motor_handle, dc motor instance
 * @param: sp, speed, unit RPM
 */
void set_dc_motor_position(dc_motor_handle v, s32 position)
{
    OS_CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();

    v->position_output = 0;
    v->pid_position_device.SetPoint = position;
    v->pid_ti_position.Ref = position;

    start_dc_motor(v);

    OS_EXIT_CRITICAL();
}


dc_motor_handle get_dc_motor_device_instance(dc_motor_num index)
{
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));

    return &dc_motor_device[index];
}


/**
  * @brief: get dc motor current run distance circle
  * @param: index, which dc motor
  * @retval: dc motor has run circle distance
  */
float  get_dc_motor_current_distance(dc_motor_handle v)
{
    assert_param(v);

    return v->current_motor_position / (v->reduction_ratio * v->encoder.encoder_ppr * 4);
}


void clear_dc_motor_current_distance(dc_motor_handle v)
{
    assert_param(v);

    v->current_motor_position = 0;
}


void start_dc_motor(dc_motor_handle v)
{
    assert_param(v);

    v->current_motor_position = 0;
    encoder_reset(&v->encoder);

    // start dc motor
    v->motor_run_flag = 1;
}


void stop_dc_motor(dc_motor_handle v)
{
    assert_param(v);

    // clear the value
    v->motor_run_flag       = 0;
    v->current_motor_pwm    = 0.0;

    switch (v->index)
    {
        case MOTOR_NUM1:
            pwm_change(PWM_TIM1, PWM_CHANNEL_1, 0, 1);
            pwm_change(PWM_TIM1, PWM_CHANNEL_2, 0, 1);
            break;
        case MOTOR_NUM2:
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
            /*
             * ARM2 Board use TIM1 to control 2 DC motors
             * CH3 and CH4 contorl
             */
            pwm_change(PWM_TIM1, PWM_CHANNEL_3, 0, 1);
            pwm_change(PWM_TIM1, PWM_CHANNEL_4, 0, 1);
#else
            pwm_change(PWM_TIM4, PWM_CHANNEL_1, 0, 1);
            pwm_change(PWM_TIM4, PWM_CHANNEL_2, 0, 1);
#endif /* USE_ARM2_REV_0_1 */
            break;
        case MOTOR_NUM3:
            pwm_change(PWM_TIM8, PWM_CHANNEL_1, 0, 1);
            pwm_change(PWM_TIM8, PWM_CHANNEL_2, 0, 1);
            break;
    }

}


/**
  * @brief: get pwm adjust value max and min value
  * @retval: pwm duty cycle(0%-100%)
  *
  */
s32  get_adjust_pwm_percent(dc_motor_handle v, u32* current_pwm, u32* pwm_max)
{
    u32 max_pwm;
    u32 pwm_per;
    s32 current_pwm_temp;

    assert_param(IS_DC_MOTOR_NUM_TYPE(v->index));

    switch (v->index)
    {
        case MOTOR_NUM1:
            max_pwm = get_pwm_max_value(PWM_TIM1);
            break;
        case MOTOR_NUM2:
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
            max_pwm = get_pwm_max_value(PWM_TIM1);
#else
            max_pwm = get_pwm_max_value(PWM_TIM4);
#endif
            break;
        case MOTOR_NUM3:
            max_pwm = get_pwm_max_value(PWM_TIM8);
            break;
    }

    current_pwm_temp = (s32)(v->current_motor_pwm);
    pwm_per = current_pwm_temp*100/max_pwm;
    *pwm_max = max_pwm;
    *current_pwm = current_pwm_temp;

    return pwm_per;
}


/**
  * @brief: speed in RPM
  *
  */
float  get_dc_motor_sp(dc_motor_handle v)
{
    return v->dc_motor_sp;
}


static __inline void change_motor_pwm_value(dc_motor_handle v)
{
    u32 max_pwm;
    s32 pwm_value;

    switch (v->index)
    {
        case MOTOR_NUM1:
            max_pwm = get_pwm_max_value(PWM_TIM1);
            if (v->current_motor_pwm > max_pwm)
            {
                v->current_motor_pwm = max_pwm;
            }
            else if (v->current_motor_pwm < -(s32)max_pwm)
            {
                v->current_motor_pwm = -(s32)max_pwm;
            }

            pwm_value = (s32)v->current_motor_pwm;
            if (pwm_value >= 0)
            {
                // fast decay
                //pwm_change(PWM_TIM1, PWM_CHANNEL_1, pwm_value, 0);
                //pwm_change(PWM_TIM1, PWM_CHANNEL_2, 0, 0);
                // slow decay
                pwm_change(PWM_TIM1, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_2, pwm_value, 1);
            }
            else
            {
                // fast decay
                //pwm_change(PWM_TIM1, PWM_CHANNEL_1, 0, 0);
                //pwm_change(PWM_TIM1, PWM_CHANNEL_2, -pwm_value, 0);
                // slow decay
                pwm_change(PWM_TIM1, PWM_CHANNEL_1, -pwm_value, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_2, 0, 1);
            }
            break;

        case MOTOR_NUM2:
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
            max_pwm = (s32)get_pwm_max_value(PWM_TIM1);
            if (v->current_motor_pwm > max_pwm)
            {
                v->current_motor_pwm = max_pwm;
            }
            else if (v->current_motor_pwm < -(s32)max_pwm)
            {
                v->current_motor_pwm = -(s32)max_pwm;
            }

            pwm_value = (s32)v->current_motor_pwm;

            if (pwm_value > 0)
            {
                pwm_change(PWM_TIM1, PWM_CHANNEL_3, pwm_value, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_4, 0, 1);
            }
            else
            {
                pwm_change(PWM_TIM1, PWM_CHANNEL_3, 0, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_4, -pwm_value, 1);
            }
#else
            max_pwm = (s32)get_pwm_max_value(PWM_TIM4);
            if (v->current_motor_pwm > max_pwm)
            {
                v->current_motor_pwm = max_pwm;
            }
            else if (v->current_motor_pwm < -(s32)max_pwm)
            {
                v->current_motor_pwm = -(s32)max_pwm;
            }

            pwm_value = (s32)v->current_motor_pwm;

            if (pwm_value >= 0)
            {
                pwm_change(PWM_TIM4, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM4, PWM_CHANNEL_2, pwm_value, 1);
            }
            else
            {
                pwm_change(PWM_TIM4, PWM_CHANNEL_1, -pwm_value, 1);
                pwm_change(PWM_TIM4, PWM_CHANNEL_2, 0, 1);
            }
#endif /* USE_ARM2_REV_0_1 */
            break;

        case MOTOR_NUM3:
            max_pwm = (s32)get_pwm_max_value(PWM_TIM8);
            if (v->current_motor_pwm > max_pwm)
            {
                v->current_motor_pwm = max_pwm;
            }
            else if (v->current_motor_pwm < -(s32)max_pwm)
            {
                v->current_motor_pwm = -(s32)max_pwm;
            }

            pwm_value = (s32)v->current_motor_pwm;

            if (pwm_value >= 0)
            {
                pwm_change(PWM_TIM8, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM8, PWM_CHANNEL_2, pwm_value, 1);
            }
            else
            {
                pwm_change(PWM_TIM8, PWM_CHANNEL_1, -pwm_value, 1);
                pwm_change(PWM_TIM8, PWM_CHANNEL_2, 0, 1);
            }
            break;
    }

}


#ifdef USE_USB_TO_DEBUG_DC_MOTOR

static __inline void send_debug_msg(dc_motor_handle v, u32 flag)
{
#define PID_TIME_INTERVAL   10

    INT8U           os_err;
    void*           p_data;
    dc_motor_mem_t  data;
    static u8       send_data_flag = PID_TIME_INTERVAL;

    if (flag)
    {
        data.index    = flag;
        switch (flag)
        {
            case 0xf0:
                APP_TRACE("Enter V_OPERATION_ACCELERATE\r\n");
                break;
            case 0xf1:
                APP_TRACE("Enter V_OPERATION_CONSTANT\r\n");
                break;
            case 0xf2:
                APP_TRACE("Enter V_OPERATION_DECELERATION\r\n");
                break;
            case 0xf3:
                APP_TRACE("Enter V_OPERATION_DONE\r\n");
                break;
        }

        return;
    }
    else
    {
        data.index    = v->index;
        data.pwm      = (s32)v->current_motor_pwm;
        data.speed    = (s32)v->dc_motor_sp;
        data.position = v->current_motor_position;
    }

    if (send_data_flag % PID_TIME_INTERVAL == 0)
    {
        if (send_data_flag == 0)
            send_data_flag = PID_TIME_INTERVAL;

        p_data = OSMemGet(g_dc_motor_debug_mem, &os_err);
        if (p_data)
        {
            memcpy(p_data, &data, sizeof data);
            OSQPost(g_dc_motor_tx_queue, (void*)p_data);
        }
    }

    send_data_flag--;

}

#endif /* USE_USB_TO_DEBUG_DC_MOTOR */


static __inline void compute_dc_motor_sp(dc_motor_handle v)
{
    // RPM speed
    v->dc_motor_sp = v->encoder.sp_encoder_count*60/(v->encoder.encoder_ppr*4*v->reduction_ratio*v->pid_compute_interval_time);
    // pulses count very second
    v->dc_motor_sp_count = v->encoder.sp_encoder_count/v->pid_compute_interval_time;

    // clear encoder count to reload next speed compute
    v->encoder.sp_encoder_count = 0;
}


/*
 * @brief: This function is used for other function to get current
 *         DC motor speed
 * @param: encoder number
 * @return DC motor speed, RPM
 */
s32 get_current_dc_motor_speed(dc_motor_handle v)
{
    return (s32)v->dc_motor_sp;
}


static __inline void dc_motor_speed_trapezoid_control(dc_motor_handle v)
{
    static u8 flag = 0;

    rmp_cntl_calc(&v->rmp_control);

    switch (v->rmp_control.ramp_control_state)
    {
        case V_OPERATION_ACCELERATE:                    // acceleration RAMP control
            {
                if (flag == 0)
                {
                    send_debug_msg(v, 0xf0);
                    flag = 1;
                }
                v->pid_position_device.SetPoint = v->rmp_control.SetpointValue;
                // position PID
                v->position_output += IncPIDCalc(&v->pid_position_device, v->current_motor_position);
                // speed PID
                v->pid_sp_device.SetPoint = v->position_output;
                v->current_motor_pwm += IncPIDCalc(&v->pid_sp_device, v->dc_motor_sp_count);
            }
            break;

        case V_OPERATION_CONSTANT:                      // constant velocity
            {
                if (flag == 1)
                {
                    send_debug_msg(v, 0xf1);
                    flag = 2;
                }
                v->pid_position_device.SetPoint = v->rmp_control.SetpointValue;
                // position PID
                v->position_output += IncPIDCalc(&v->pid_position_device, v->current_motor_position);
                // speed PID
                v->pid_sp_device.SetPoint = v->position_output;
                v->current_motor_pwm += IncPIDCalc(&v->pid_sp_device, v->dc_motor_sp_count);
            }
            break;

        case V_OPERATION_DECELERATION:
            {
                if (flag == 2)
                {
                    send_debug_msg(v, 0xf2);
                    flag = 3;
                }

                v->pid_position_device.SetPoint = v->rmp_control.SetpointValue;

                // position PID
                v->position_output += IncPIDCalc(&v->pid_position_device, v->current_motor_position);

                // speed PID
                v->pid_sp_device.SetPoint = v->position_output;
                v->current_motor_pwm += IncPIDCalc(&v->pid_sp_device, v->dc_motor_sp_count);
            }
            break;

        case V_OPERATION_DONE:
            {
                if (flag == 3)
                {
                    send_debug_msg(v, 0xf3);
                    flag = 0;
                }
                // finish RAMP control, all are well done, stop here
                v->pid_position_device.SetPoint = v->rmp_control.target_position;

                // position PID
                v->position_output += IncPIDCalc(&v->pid_position_device, v->current_motor_position);

                // speed PID
                v->pid_sp_device.SetPoint = v->position_output;
                v->current_motor_pwm += IncPIDCalc(&v->pid_sp_device, v->dc_motor_sp_count);
            }
            break;

        default:
            break;
    }

}

static __inline void motor_position_pid(dc_motor_handle v)
{
    // read encoder count
    encoder_get_pulse_count(&v->encoder);

    // compute dc motor speed
    compute_dc_motor_sp(v);

    // update dc motor position
    v->current_motor_position += v->encoder.interval_count;

    // dc motor motion mode control
    switch (v->motor_operational_mode)
    {
        case MOTOR_MODE0:       // speed
            v->current_motor_pwm += IncPIDCalc(&v->pid_sp_device, v->dc_motor_sp_count);
            break;

        case MOTOR_MODE1:       // position
            v->current_motor_pwm += IncPIDCalc(&v->pid_position_device, v->current_motor_position);
            break;

        case MOTOR_MODE2:       // speed + position
            v->position_output += IncPIDCalc(&v->pid_position_device, v->current_motor_position);
            v->pid_sp_device.SetPoint = 61.833*v->position_output;
            v->current_motor_pwm += IncPIDCalc(&v->pid_sp_device, v->dc_motor_sp_count);
            break;

        case MOTOR_MODE3:
            dc_motor_speed_trapezoid_control(v);
            break;
    }

    //--------------------------- PWM --------------------------
    change_motor_pwm_value(v);

    //-------------------------- DEBUG -------------------------
#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
    send_debug_msg(v, 0);
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */

}


void timer7_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    /* Time base configuration 1ms */
    TIM_TimeBaseStructure.TIM_Period    = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    TIM_SetCounter(TIM7, 0);
    TIM_Cmd(TIM7, ENABLE);
}


/*
 * @brief: init DC motor unit, include:
 *          - encoder timer interface
 *          - PWM module
 *          - compute PID interval TIM
 * @param: pwm_freq, pwm frequency
 * @param: interval_time, microsecond unit
 * @retval: void
 */
void motor_module_init(u32 pwm_freq, u32 compute_interval_time)
{
    //--------------- PWM init ---------------
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
    /* ARM2 board use TIM1 4 channels */
    TIM1_8_pwm_config(PWM_TIM1,
                      pwm_freq,
                      PWM_CHANNEL_1 | PWM_CHANNEL_2 | PWM_CHANNEL_3 | PWM_CHANNEL_4,
                      100, 100, 100, 100);
#else
    TIM1_8_pwm_config(PWM_TIM1, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
    TIM2_5_9_14_pwm_config(PWM_TIM4, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
    TIM1_8_pwm_config(PWM_TIM8, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
#endif /* USE_ARM2_REV_0_1 */

    basic_timer_init(TIMER_FOR_US, compute_interval_time);

    // dc motor PID parameter, encoder parameter, ramp control parameter init
    dc_motor_device_init(compute_interval_time);

    APP_TRACE("* pwm freq: %dKHz\r\n", pwm_freq/1000);
    APP_TRACE("* PID freq: %dHz\r\n", 1000000/(compute_interval_time));
    APP_TRACE("--------------------------------------\r\n");

    // start TIM6 to calculate PID operation
    basic_timer_start();
}



void init_dc_motor(void)
{
#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
    INT8U  os_err = 0;
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */

    /* init DC motor module */
    timer7_init();
    motor_module_init(20000, 100);

#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
    /* init USB module */
    USBD_Init( &USB_OTG_dev,
                USB_OTG_FS_CORE_ID,
                &USR_desc,
                &USBD_CDC_cb,
                &USR_cb);

    g_dc_motor_tx_queue = (OS_EVENT*)OSQCreate(g_dc_motor_tx_QueueTbl, DC_MOTOR_QUEUE_SIZE);
    assert_param(g_dc_motor_tx_queue);

    g_dc_motor_rx_queue = (OS_EVENT*)OSQCreate(g_dc_motor_rx_QueueTbl, DC_MOTOR_QUEUE_SIZE);
    assert_param(g_dc_motor_rx_queue);

    // init use debug mem
    g_dc_motor_debug_mem = OSMemCreate(g_dc_motor_debug_buf, DC_MOTOR_DEBUG_BUF_SIZE, DC_MOTOR_DEBUG_BUF_LEN, &os_err);
    assert_param(OS_ERR_NONE == os_err);

    // USB send data sem
    g_dc_motor_tx_sem = OSSemCreate(1);
    assert_param(g_dc_motor_tx_sem);

    os_err = OSTaskCreateExt((void (*)(void *)) app_dc_motor_task,
                            (void 		 * ) 0,
                            (OS_STK		 * )&dc_motor_task_stk[DC_MOTOR_TASK_STK_SIZE - 1],
                            (INT8U		   ) DC_MOTOR_TASK_PRIO,
                            (INT16U		   ) DC_MOTOR_TASK_PRIO,
                            (OS_STK		 * )&dc_motor_task_stk[0],
                            (INT32U		   ) DC_MOTOR_TASK_STK_SIZE,
                            (void 		 * ) 0,
                            (INT16U		   )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(DC_MOTOR_TASK_PRIO, (INT8U *)"DC motor", &os_err);

    os_err = OSTaskCreateExt((void (*)(void *)) app_dc_motor_send_task,
                            (void 		 * ) 0,
                            (OS_STK		 * )&dc_motor_send_task_stk[DC_MOTOR_SEND_TASK_STK_SIZE - 1],
                            (INT8U		   ) DC_MOTOR_SEND_TASK_PRIO,
                            (INT16U		   ) DC_MOTOR_SEND_TASK_PRIO,
                            (OS_STK		 * )&dc_motor_send_task_stk[0],
                            (INT32U		   ) DC_MOTOR_SEND_TASK_STK_SIZE,
                            (void 		 * ) 0,
                            (INT16U		   )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(DC_MOTOR_SEND_TASK_PRIO, (INT8U *)"DC motor send", &os_err);
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */
}


#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
static void app_dc_motor_task(void *p_arg)
{
    INT8U err;
    void* data;

    APP_TRACE("DC motor task enter...\r\n");

    for (;;)
    {
        // recv data from PC
        data = OSQPend(g_dc_motor_rx_queue, 0, &err);
        assert_param(err == OS_ERR_NONE);
        do_cmd_data((u8)(u32)data);
    }
}


static void app_dc_motor_send_task(void *p_arg)
{
    void* value;
    dc_motor_mem_t data;
    u8 vl_buf[64];
    INT8U err;

    APP_TRACE("DC motor send usb debug message task start...\r\n");

    for (;;)
    {
        // send data to PC
        value = OSQPend(g_dc_motor_tx_queue, 0, &err);
        assert_param(err == OS_ERR_NONE);

        memcpy(&data, value, sizeof(dc_motor_mem_t));
        if (data.index == 0xff)
        {
            APP_TRACE("");
        }
        else
        {
            snprintf((char*)vl_buf, sizeof(vl_buf), "R%d %d %d %d;",
                        data.index + 1,
                        data.speed,
                        data.pwm,
                        data.position);
            OSSemPend(g_dc_motor_tx_sem, 0, &err);
            VCP_DataTx(vl_buf, strlen((const char*)vl_buf));
            OSSemPost(g_dc_motor_tx_sem);
        }

        OSMemPut(g_dc_motor_debug_mem, value);
    }
}


//
// get DC motor param: MAX pwm value, encoder number, reduction ratio
// A1 3000 512 1006;
//
void get_dc_motor_param(dc_motor_num index)
{
    u32 max_pwm;
    INT8U err;
    u8 vl_buf[64];

    switch (index)
    {
        case MOTOR_NUM1:
            max_pwm = get_pwm_max_value(PWM_TIM1);
            break;
        case MOTOR_NUM2:
            max_pwm = get_pwm_max_value(PWM_TIM1);
            break;
        case MOTOR_NUM3:
            max_pwm = get_pwm_max_value(PWM_TIM1);
            break;
        default:
            return;
    }

    snprintf((char*)vl_buf, sizeof(vl_buf), "A%d %d %d %d;",
                        index + 1,
                        max_pwm,
                        ENCODER_PPR,
                        (u32)(DC_MOTOR_REDUCTION_RATIO*100));

    OSSemPend(g_dc_motor_tx_sem, 0, &err);
    VCP_DataTx(vl_buf, strlen((const char*)vl_buf));
    OSSemPost(g_dc_motor_tx_sem);
}

#endif /* USE_USB_TO_DEBUG_DC_MOTOR */



/*
 * Handle TIMER6 interrupt
 *
 */
void TIM6_DAC_IRQHandler(void)
{
    dc_motor_handle v;
    dc_motor_num    i;
    u32             operation_time;

    OSIntEnter();

    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
    {
        /* Clear the interrupt pending flag */
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);

        for (i = MOTOR_NUM1; i < MOTOR_NUM; i++)
        {
            // dc motor number 1
            operation_time = TIM7->CNT;
            v = get_dc_motor_device_instance(i);
            if (v->motor_run_flag)
                motor_position_pid(v);
            v->pid_operation_time = TIM7->CNT - operation_time;
        }
    }

    OSIntExit();
}

#if defined(USE_USB_TO_DEBUG_DC_MOTOR)

/*------------------------------ USB ------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"

extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif



/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_WKUP_IRQHandler(void)
{
	if(USB_OTG_dev.cfg.low_power)
	{
		*(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9;
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_WKUP_IRQHandler(void)
{
	if(USB_OTG_dev.cfg.low_power)
	{
		*(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
	USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
	USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}


/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
	USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif



#endif /* USE_USB_TO_DEBUG_DC_MOTOR */

#endif /* _APP_DC_MOTOR_C_ */


