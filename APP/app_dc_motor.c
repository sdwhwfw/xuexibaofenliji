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

#include "app_dc_motor.h"
#include "trace.h"
#include "pwm.h"
#include "queue.h"
#include "pid.h"
#include "encoder.h"
#include "basic_timer.h"
#include "protocol.h"
#include "trace.h"
#include "math.h"
#include "flash_if.h"
#include "gpio.h"


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
#if defined(_DEBUG)
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
OS_EVENT*  g_dc_motor_rx_queue;

#endif /* _DEBUG */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//static float current_motor_pwm[MOTOR_NUM] = {0, 0, 0};

static double current_motor_pwm[MOTOR_NUM] = {0, 0, 0};
static int current_motor_pwm_backup[MOTOR_NUM] = {0, 0, 0};
static float current_motor_sp_backup[MOTOR_NUM] = {0, 0, 0};
static volatile int motor_run_flag[MOTOR_NUM] = {0, 0, 0};
static vu32 g_dc_motor_distance[MOTOR_NUM] = {0, 0, 0};
static vu32 g_dc_motor_distance_flag[MOTOR_NUM] = {0, 0, 0};
static vu32 g_dc_motor_current_distance[MOTOR_NUM] = {0, 0, 0};


/* unit 100 microsecond */
static u32 g_PID_interval_time = 10;

#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */

//========================================
// PID PARAM default value
//========================================
#define		KP		                100
#define		KPP		                9000
#define		KPI		                1000
#define		KPD		                0

// save param Base @ of Sector 11, 128 Kbyte
#define     PID_PARAM_ADDRESS       ((uint32_t)0x080E0000)
// save buffer size byte
#define     PID_SAVE_BUFFER_SIZE    1024

// USE IQmath
//#define     USE_IQMATH_LIB


/* Private functions ---------------------------------------------------------*/
static void motor_pid(MotorType motor);
static void motor_pid_process(void);
static void set_pid_param(u8 index, uint32_t kp, uint32_t kpp, uint32_t kpi, uint32_t kpd);
#if defined(_DEBUG)
static void app_dc_motor_task(void *p_arg);
static void app_dc_motor_send_task(void *p_arg);
#endif /* _DEBUG */


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
    param->crc = CRC_CalcBlockCRC((u32*)param, sizeof(dc_motor_PID_param_t)/4 - 1);
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
    crc_value = CRC_CalcBlockCRC((void*)param, sizeof(dc_motor_PID_param_t)/4 - 1);
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


static void set_pid_param(PIDType index, uint32_t kp, uint32_t kpp, uint32_t kpi, uint32_t kpd)
{
	float k1, k2, k3, k4;

	k1 = kp/1000.0;
	k2 = kpp/1000.0;
	k3 = kpi/1000.0;
	k4 = kpd/1000.0;

#ifdef USE_IQMATH_LIB
    PIDInit_Q(index);
    PIDSetKp_Q(index, k2*k1);
    PIDSetKi_Q(index, k3*k1);
    PIDSetKd_Q(index, k4*k1);
#else
    PIDInit(index);
    PIDSetKp(index, k2*k1);
    PIDSetKi(index, k3*k1);
    PIDSetKd(index, k4*k1);
#endif
}


/*
 * @brief: init DC motor unit, include init encoder timer interface
 *         and init PWM module
 * @param: pwm_freq, pwm frequency
 * @param: interval_time, 100 microsecond unit
 * @retval: void
 * @NOTE:  ARM1 has 3 DC motors
 *         ARM2 has 2 DC motors
 */
void motor_module_init(uint32_t pwm_freq, uint32_t interval_time)
{
    //------------ PID param init ------------
    dc_motor_PID_param_t pid;

    APP_TRACE("------- DC motor configuration -------\r\n");

    memset((void*)&pid, 0x00, sizeof pid);
    pid.index = MOTOR_NUM1;
    get_pid_param_from_internal_flash(&pid);
    dc_motor_pid_param_config((MotorType)pid.index, pid.kp, pid.kpp, pid.kpi, pid.kpd);
    APP_TRACE("* motor1 PID: %d, %d, %d, %d\r\n", pid.kp, pid.kpp, pid.kpi, pid.kpd);

    pid.index = MOTOR_NUM2;
    get_pid_param_from_internal_flash(&pid);
    dc_motor_pid_param_config((MotorType)pid.index, pid.kp, pid.kpp, pid.kpi, pid.kpd);
    APP_TRACE("* motor2 PID: %d, %d, %d, %d\r\n", pid.kp, pid.kpp, pid.kpi, pid.kpd);

#if defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2) || defined(USE_ARM1_REV_0_2)
    pid.index = MOTOR_NUM3;
    get_pid_param_from_internal_flash(&pid);
    dc_motor_pid_param_config((MotorType)pid.index, pid.kp, pid.kpp, pid.kpi, pid.kpd);
    APP_TRACE("* motor3 PID: %d, %d, %d, %d\r\n", pid.kp, pid.kpp, pid.kpi, pid.kpd);
#endif /* USE_ARM1_REV_0_1 */

    //------------- encoder init -------------
    QET_configuration(ENCODER_NUM1);
    QET_configuration(ENCODER_NUM2);
#if defined(USE_ARM1_REV_0_2)       // USE_ARM1_REV_0_1 TIM5 CH3 and CH4 has no encoder function
    QET_configuration(ENCODER_NUM3);
#endif /* USE_ARM1_REV_0_2 */

    //--------------- PWM init ---------------
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
    /* ARM2 board use TIM1 4 channels */
    TIM1_8_pwm_config(PWM_TIM1,
                      pwm_freq,
                      PWM_CHANNEL_1 | PWM_CHANNEL_2 | PWM_CHANNEL_3 | PWM_CHANNEL_4,
                      100, 100, 100, 100);
#elif defined(USE_ARM1_REV_0_1)
    TIM1_8_pwm_config(PWM_TIM1, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
    TIM2_5_9_14_pwm_config(PWM_TIM4, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
    TIM1_8_pwm_config(PWM_TIM8, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
#elif defined(USE_ARM1_REV_0_2)
    TIM1_8_pwm_config(PWM_TIM1, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
    TIM2_5_9_14_pwm_config(PWM_TIM12, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
    TIM1_8_pwm_config(PWM_TIM8, pwm_freq, PWM_CHANNEL_1 | PWM_CHANNEL_2, 100, 100, 100, 100);
#endif /* USE_ARM2_REV_0_1 */

    basic_timer_init(TIMER_FOR_US, interval_time*100);
    g_PID_interval_time = interval_time;
    basic_timer_start();

    APP_TRACE("* pwm freq: %dKHz\r\n", pwm_freq/1000);
    APP_TRACE("* PID freq: %dHz\r\n", 1000000/(interval_time*100));
    APP_TRACE("--------------------------------------\r\n");

}


/*
 * @brief: config motor PID param
 *
 * @param: MOTOR_NUM1, 1000, 1000, 120, 110, 0
 */
void dc_motor_pid_param_config(MotorType index, uint32_t kp, uint32_t kpp, uint32_t kpi, uint32_t kpd)
{
	assert_param(IS_DC_MOTOR_NUM_TYPE(index));

	set_pid_param((PIDType)index, kp, kpp, kpi, kpd);
}


/*
 * @brief: Set Motor speed
 * @param: index, DC motor number
 * @param: sp, speed, unit RPM
 * @param: distance, 1/10
 */
void set_motor_speed(MotorType index, float sp, u32 distance)
{
    OS_CPU_SR cpu_sr;

	assert_param(IS_DC_MOTOR_NUM_TYPE(index));

    sp = sp * DC_MOTOR_REDUCTION_RATIO * DC_MOTOR_SPEED_PRECISION;

    OS_ENTER_CRITICAL();

    if (distance)
    {
        g_dc_motor_distance[index] = (distance*DC_MOTOR_REDUCTION_RATIO*ENCODER_PPR*4)/DC_MOTOR_DIS_PRECISION;
        g_dc_motor_distance_flag[index]   = 1;
    }

#ifdef USE_IQMATH_LIB
    PIDSetPoint_Q((PIDType)index, sp);
#else
    PIDSetPoint((PIDType)index, sp);
#endif /* USE_IQMATH_LIB */

    start_dc_motor(index);

    OS_EXIT_CRITICAL();
}


/**
  * @brief: get dc motor current run distance circle(*10)
  * @param: index, which dc motor
  * @retval: dc motor has run circle distance(*10 circle)
  */
u32  get_dc_motor_current_distance(MotorType index)
{
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));
    return g_dc_motor_current_distance[index] * DC_MOTOR_DIS_PRECISION / (DC_MOTOR_REDUCTION_RATIO*ENCODER_PPR*4);
}

void clear_dc_motor_current_distance(MotorType index)
{
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));
    g_dc_motor_current_distance[index] = 0;
}


/* uint us */
void start_dc_motor(MotorType index)
{
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));

    g_dc_motor_current_distance[index] = 0;
    encoder_reset((EncoderType)index);
    motor_run_flag[index] = 1;
}


void stop_dc_motor(MotorType index)
{
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));

    // clear the value
    motor_run_flag[index] = 0;
    g_dc_motor_distance_flag[index] = 0;
    current_motor_pwm_backup[index] = 0;
    current_motor_pwm[index] = 0.0;
    current_motor_sp_backup[index] = 0;
    //encoder_reset((EncoderType)index);

    switch (index)
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
#elif defined(USE_ARM1_REV_0_2)
            pwm_change(PWM_TIM12, PWM_CHANNEL_1, 0, 1);
            pwm_change(PWM_TIM12, PWM_CHANNEL_2, 0, 1);
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
u32  get_adjust_pwm_percent(MotorType index, u32* current_pwm, u32* pwm_max)
{
    u32 max_pwm;
    u32 pwm_per;
    s32 current_pwm_temp;

    assert_param(IS_DC_MOTOR_NUM_TYPE(index));

    switch (index)
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

    current_pwm_temp = abs(current_motor_pwm_backup[index]);
    pwm_per = current_pwm_temp*100/max_pwm;
    *pwm_max = max_pwm;
    *current_pwm = current_pwm_temp;

    return pwm_per;
}


float  get_dc_motor_sp(MotorType index)
{
    assert_param(IS_DC_MOTOR_NUM_TYPE(index));
    return current_motor_sp_backup[index]/DC_MOTOR_REDUCTION_RATIO;
}


static __inline void change_motor_pwm_value(MotorType motor)
{
    s32 max_pwm, min_pwm;

    switch (motor)
    {
        case MOTOR_NUM1:
            max_pwm = (s32)get_pwm_max_value(PWM_TIM1);
            min_pwm = (s32)get_pwm_min_value(PWM_TIM1);
            if (current_motor_pwm[motor] > max_pwm)
            {
                current_motor_pwm[motor] = max_pwm;
            }
            else if (current_motor_pwm[motor] < min_pwm)
            {
                current_motor_pwm[motor] = min_pwm;
            }

            if (current_motor_pwm[motor] > 0)
            {
                pwm_change(PWM_TIM1, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_2, (u32)current_motor_pwm[motor], 1);

               //led_trun_on(LED1);

                //pwm_change(PWM_TIM1, PWM_CHANNEL_1, 0, 1);
                //pwm_change(PWM_TIM1, PWM_CHANNEL_2, max_pwm/2, 1);
            }
            else
            {
                pwm_change(PWM_TIM1, PWM_CHANNEL_1, -(s32)current_motor_pwm[motor], 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_2, 0, 1);

                //led_trun_off(LED1);
            }
            break;

        case MOTOR_NUM2:
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
            max_pwm = (s32)get_pwm_max_value(PWM_TIM1);
            min_pwm = (s32)get_pwm_min_value(PWM_TIM1);
            if (current_motor_pwm[motor] > max_pwm)
            {
                current_motor_pwm[motor] = max_pwm;
            }
            else if (current_motor_pwm[motor] < min_pwm)
            {
                current_motor_pwm[motor] = min_pwm;
            }

            if (current_motor_pwm[motor] > 0)
            {
                pwm_change(PWM_TIM1, PWM_CHANNEL_3, (u32)current_motor_pwm[motor], 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_4, 0, 1);
            }
            else
            {
                pwm_change(PWM_TIM1, PWM_CHANNEL_3, 0, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_4, -(s32)current_motor_pwm[motor], 1);
            }
#elif defined(USE_ARM1_REV_0_2)
            max_pwm = (s32)get_pwm_max_value(PWM_TIM12);
            min_pwm = (s32)get_pwm_min_value(PWM_TIM12);
            if (current_motor_pwm[motor] > max_pwm)
            {
                current_motor_pwm[motor] = max_pwm;
            }
            else if (current_motor_pwm[motor] < min_pwm)
            {
                current_motor_pwm[motor] = min_pwm;
            }

            if (current_motor_pwm[motor] > 0)
            {
                pwm_change(PWM_TIM12, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM12, PWM_CHANNEL_2, (u32)current_motor_pwm[motor], 1);
            }
            else
            {
                pwm_change(PWM_TIM12, PWM_CHANNEL_1, -(s32)current_motor_pwm[motor], 1);
                pwm_change(PWM_TIM12, PWM_CHANNEL_2, 0, 1);
            }

#else
            max_pwm = (s32)get_pwm_max_value(PWM_TIM4);
            min_pwm = (s32)get_pwm_min_value(PWM_TIM4);
            if (current_motor_pwm[motor] > max_pwm)
            {
                current_motor_pwm[motor] = max_pwm;
            }
            else if (current_motor_pwm[motor] < min_pwm)
            {
                current_motor_pwm[motor] = min_pwm;
            }

            if (current_motor_pwm[motor] > 0)
            {
                pwm_change(PWM_TIM4, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM4, PWM_CHANNEL_2, (u32)current_motor_pwm[motor], 1);
            }
            else
            {
                pwm_change(PWM_TIM4, PWM_CHANNEL_1, -(s32)current_motor_pwm[motor], 1);
                pwm_change(PWM_TIM4, PWM_CHANNEL_2, 0, 1);
            }
#endif /* USE_ARM2_REV_0_1 */
            break;

        case MOTOR_NUM3:
            max_pwm = (s32)get_pwm_max_value(PWM_TIM8);
            min_pwm = (s32)get_pwm_min_value(PWM_TIM8);
            if (current_motor_pwm[motor] > max_pwm)
            {
                current_motor_pwm[motor] = max_pwm;
            }
            else if (current_motor_pwm[motor] < min_pwm)
            {
                current_motor_pwm[motor] = min_pwm;
            }

            if (current_motor_pwm[motor] > 0)
            {
                pwm_change(PWM_TIM8, PWM_CHANNEL_1, 0, 1);
                pwm_change(PWM_TIM8, PWM_CHANNEL_2, (u32)current_motor_pwm[motor], 1);
            }
            else
            {
                pwm_change(PWM_TIM8, PWM_CHANNEL_1, -(s32)current_motor_pwm[motor], 1);
                pwm_change(PWM_TIM8, PWM_CHANNEL_2, 0, 1);
            }
            break;
    }

}

#ifdef _DEBUG
static __inline void send_debug_msg(MotorType motor, s32 cur_speed)
{
    INT8U os_err;
    void* p_data;
    dc_motor_mem_t data;
    data.index = motor;
    data.pwm   = (s32)current_motor_pwm[motor];
    data.speed = cur_speed;
    p_data = OSMemGet(g_dc_motor_debug_mem, &os_err);
    if (p_data)
    {
        memcpy(p_data, &data, sizeof data);
        OSQPost(g_dc_motor_tx_queue, (void*)p_data);
    }
}
#endif /* _DEBUG */

static __inline void check_motor_distance(MotorType motor, s32 distance)
{
    s32 difference;

    g_dc_motor_current_distance[motor] += abs(distance);

    // check distance
    if (g_dc_motor_distance_flag[motor])
    {
        // stop DC motor if reach the postion
        difference = g_dc_motor_distance[motor] - g_dc_motor_current_distance[motor];
        if (difference <= 0)
        {
            stop_dc_motor(motor);
            return;
        }
    }
}


/*
 *------------------------------------------
 * arm1 3DC motor with encoder interface
 * TIM1 CH1 CH2
 * TIM4 CH1 CH2
 * TIM8 CH1 CH2
 *------------------------------------------
 * arm2 2DC motor with encoder interface
 * TIM1 CH1 CH2
 * TIM1 CH3 CH4
 *------------------------------------------
 *
 */
static __inline void motor_pid(MotorType motor)
{
    float cur_speed;
    s32 distance;

    //------------------- get DC motor speed -------------------
    cur_speed = get_dc_motor_speed((EncoderType)motor, g_PID_interval_time, &distance);

    // BUG TO Fix
    if ( cur_speed > 7000*DC_MOTOR_SPEED_PRECISION || cur_speed < -7000*DC_MOTOR_SPEED_PRECISION)
    {
        //TRACE("Warning: [%d] cur_speed = %d\r\n", motor, cur_speed);
        TRACE(".");
        return;
    }

    //--------------------------- PID --------------------------
#ifdef USE_IQMATH_LIB
    current_motor_pwm[motor] += IncPIDCalc_Q((PIDType)motor, cur_speed);
#else
    current_motor_pwm[motor] += IncPIDCalc((PIDType)motor, cur_speed);
#endif /* USE_IQMATH_LIB */

    //--------------------------- PWM --------------------------
    change_motor_pwm_value(motor);

    //------------------------- distance -----------------------
    check_motor_distance(motor, distance);

    //-------------------------- backup ------------------------
    current_motor_pwm_backup[motor] = (s32)current_motor_pwm[motor];
    current_motor_sp_backup[motor]  = cur_speed;

    //-------------------------- DEBUG -------------------------
#if defined(_DEBUG)
    send_debug_msg(motor, cur_speed);
#endif /* _DEBUG */
}


static void motor_pid_process(void)
{
    int i;

    for (i = 0; i < MOTOR_NUM; i++)
    {
        if (motor_run_flag[i] == 1)
        {
            motor_pid((MotorType)i);
        }
    }
}


void init_dc_motor(void)
{
#if defined(_DEBUG)
    INT8U  os_err = 0;
#endif /* _DEBUG */

#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
    /* init USB module */
    USBD_Init( &USB_OTG_dev,
                USB_OTG_FS_CORE_ID,
                &USR_desc,
                &USBD_CDC_cb,
                &USR_cb);
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */

    /* init DC motor module */
   motor_module_init(20000, 200);
  //   motor_module_init(70000, 200);

#if defined(_DEBUG)
    g_dc_motor_tx_queue = (OS_EVENT*)OSQCreate(g_dc_motor_tx_QueueTbl, DC_MOTOR_QUEUE_SIZE);
    assert_param(g_dc_motor_tx_queue);

    g_dc_motor_rx_queue = (OS_EVENT*)OSQCreate(g_dc_motor_rx_QueueTbl, DC_MOTOR_QUEUE_SIZE);
    assert_param(g_dc_motor_rx_queue);

    // init can mem
    g_dc_motor_debug_mem = OSMemCreate(g_dc_motor_debug_buf, DC_MOTOR_DEBUG_BUF_SIZE, DC_MOTOR_DEBUG_BUF_LEN, &os_err);
    assert_param(OS_ERR_NONE == os_err);

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
#endif /* _DEBUG */
}


#if defined(_DEBUG)
static void app_dc_motor_task(void *p_arg)
{
    INT8U err;
    void* data;

    APP_TRACE("DC motor task enter...\r\n");

#if 0
    OSTimeDlyHMSM(0, 0, 0, 100);
    APP_TRACE("Engine start!\r\n");

    // test DC motor
    //dc_motor_pid_param_config(MOTOR_NUM1, 1000, 120, 110, 0);
    //set_motor_speed(MOTOR_NUM1, -1000);
    //dc_motor_pid_param_config(MOTOR_NUM2, 1000, 120, 110, 0);
    //set_motor_speed(MOTOR_NUM2, -1000);
    //dc_motor_pid_param_config(MOTOR_NUM3, 1000, 120, 110, 0);
    //set_motor_speed(MOTOR_NUM3, -1000);
    //start_dc_motor(MOTOR_NUM1);
    //start_dc_motor(MOTOR_NUM2);
    //start_dc_motor(MOTOR_NUM3);

#endif

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
        snprintf((char*)vl_buf, sizeof(vl_buf), "R%d %d %d;", (u8)data.index + 1, data.speed, data.pwm);
        OSMemPut(g_dc_motor_debug_mem, value);
        //APP_TRACE("%s\r\n", vl_buf);
#if defined(USE_USB_TO_DEBUG_DC_MOTOR)
        VCP_DataTx(vl_buf, strlen((const char*)vl_buf));
#endif /* USE_USB_TO_DEBUG_DC_MOTOR */
    }
}

#endif /* _DEBUG */



/*
 * Handle TIMER6 interrupt
 *
 */
void TIM6_DAC_IRQHandler(void)
{
    OSIntEnter();
	/* Clear the interrupt pending flag */
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
    {
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);
        motor_pid_process();
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


