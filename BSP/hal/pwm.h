/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : pwm.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/16
 * Description        : This file contains the software implementation for the
 *                      pwm unit
 *******************************************************************************
 * History:
 * 2013/05/16 v1.0
 *******************************************************************************/
#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f2xx.h"


typedef enum {
	PWM_TIM1,       /* 4ch 0  */
	PWM_TIM2,       /* 4ch 1  */
	PWM_TIM3,       /* 4ch 2  */
	PWM_TIM4,       /* 4ch 3  */
	PWM_TIM5,       /* 4ch 4  */
	PWM_TIM8,       /* 4ch 5  */
	PWM_TIM9,       /* 2ch 6  */
	PWM_TIM10,      /* 1ch 7  */
	PWM_TIM11,      /* 1ch 8  */
	PWM_TIM12,      /* 2ch 9  */
	PWM_TIM13,      /* 1ch 10 */
	PWM_TIM14,      /* 1ch 11 */
	PWM_TIM_NUM
} pwm_timer_num;

#define IS_PWM_TIM1_8_TYPE(TYPE)      (((TYPE) == PWM_TIM1) || ((TYPE) == PWM_TIM8))


#define IS_PWM_TIM2_5_9_14_TYPE(TYPE) (((TYPE) == PWM_TIM2) || \
                                        ((TYPE) == PWM_TIM3) || \
                                        ((TYPE) == PWM_TIM4) || \
                                        ((TYPE) == PWM_TIM5) || \
                                        ((TYPE) == PWM_TIM9) || \
                                        ((TYPE) == PWM_TIM10) || \
                                        ((TYPE) == PWM_TIM11) || \
                                        ((TYPE) == PWM_TIM12) || \
                                        ((TYPE) == PWM_TIM13) || \
                                        ((TYPE) == PWM_TIM14))


#define IS_PWM_TIM_TYPE(TYPE)    (((TYPE) == PWM_TIM1)  || \
                                  ((TYPE) == PWM_TIM2)  || \
                                  ((TYPE) == PWM_TIM3)  || \
                                  ((TYPE) == PWM_TIM4)  || \
                                  ((TYPE) == PWM_TIM5)  || \
                                  ((TYPE) == PWM_TIM8)  || \
                                  ((TYPE) == PWM_TIM9)  || \
                                  ((TYPE) == PWM_TIM10) || \
                                  ((TYPE) == PWM_TIM11) || \
                                  ((TYPE) == PWM_TIM12) || \
                                  ((TYPE) == PWM_TIM13) || \
                                  ((TYPE) == PWM_TIM14))


#define PWM_CHANNEL_1  (u16)0x0001
#define PWM_CHANNEL_2  (u16)0x0002
#define PWM_CHANNEL_3  (u16)0x0004
#define PWM_CHANNEL_4  (u16)0x0008

#define IS_PWM_CHANNEL_TYPE(TYPE)    (((TYPE) == PWM_CHANNEL_1)  || \
                                     ((TYPE)  == PWM_CHANNEL_2)  || \
                                     ((TYPE)  == PWM_CHANNEL_3)  || \
                                     ((TYPE)  == PWM_CHANNEL_4))



/*------------------------------------------------------------------------------*/

void TIM1_8_pwm_config( pwm_timer_num index,
                                    u32           feq,
                                    u16           channel_start_flag,
                                    u8            duty_cycle1,
                                    u8            duty_cycle2,
                                    u8            duty_cycle3,
                                    u8            duty_cycle4);

void TIM2_5_9_14_pwm_config( pwm_timer_num index,
                                           u32     feq,
                                           u16     channel_start_flag,
                                           u8      duty_cycle1,
                                           u8      duty_cycle2,
                                           u8      duty_cycle3,
                                           u8      duty_cycle4);

void pwm_change_cycle(pwm_timer_num index,
                                 u16           channel_flag,
                                 u8            duty_cycle,
                                 u8            opposition);

void pwm_change(pwm_timer_num index, u16 channel_flag, u32 duty_cycle, u8 opposition);

u32 get_pwm_max_value(pwm_timer_num index);
s32 get_pwm_min_value(pwm_timer_num index);



#endif /* __PWM_H__ */

