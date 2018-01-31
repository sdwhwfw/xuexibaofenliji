/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : input_capture.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/11/12
 * Description        : This file contains the software implementation for the
 *                      pwm capture unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/11/12 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/
#ifndef _PWM_CAPTURE_H_
#define _PWM_CAPTURE_H_

#include "stm32f2xx.h"



typedef enum {
	PWM_CAPTURE_TIM1,
	PWM_CAPTURE_TIM2,
	PWM_CAPTURE_TIM3,
	PWM_CAPTURE_TIM4,
	PWM_CAPTURE_TIM5,
	PWM_CAPTURE_TIM8,
	PWM_CAPTURE_TIM9,
	PWM_CAPTURE_TIM12,
	PWM_CAPTURE_TIM_NUM
} input_capture_timer_num;


#define IS_INPUT_CAP_NUM_TYPE(TYPE)    (((TYPE) == PWM_CAPTURE_TIM1) || \
                                        ((TYPE) == PWM_CAPTURE_TIM2) || \
                                        ((TYPE) == PWM_CAPTURE_TIM3) || \
                                        ((TYPE) == PWM_CAPTURE_TIM4) || \
                                        ((TYPE) == PWM_CAPTURE_TIM5) || \
                                        ((TYPE) == PWM_CAPTURE_TIM8) || \
                                        ((TYPE) == PWM_CAPTURE_TIM9) || \
                                        ((TYPE) == PWM_CAPTURE_TIM12))

void init_input_capture(input_capture_timer_num index);

void init_pwm_input_capture(input_capture_timer_num index);
void enable_pwm_capture(input_capture_timer_num index);
void disable_pwm_capture(input_capture_timer_num index);

u32 get_input_capture_value(input_capture_timer_num index);
//double get_pwm_input_capture_speed(input_capture_timer_num index);








#endif /* _PWM_CAPTURE_H_ */

