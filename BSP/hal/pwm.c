/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : pwm.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/16
 * Description        : This file contains the software implementation for the
 *                      pwm unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/05/16 | v1.0  |            | initial released
 * 2013/11/05 | v1.1  | Bruce.zhu  | change framework to support multiple
 *                                   configurations for PWM timer
 *******************************************************************************/
#include "pwm.h"



/*#########################################
 *#                TIM1                   #
 *#########################################*/
#if defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2) || \
    defined(USE_ARM2_REV_0_1)|| defined(USE_ARM3_REV_0_1)

/*
 * PA8
 * PA9
 * PA10
 * PA11
 */
#define PWM_TIM1_GPIO_PORT          GPIOA
#define PWM_TIM1_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define PWM_TIM1_CH1_PIN            GPIO_Pin_8
#define PWM_TIM1_CH2_PIN            GPIO_Pin_9
#define PWM_TIM1_CH3_PIN            GPIO_Pin_10
#define PWM_TIM1_CH4_PIN            GPIO_Pin_11

#define PWM_TIM1_CH1_SOURCE         GPIO_PinSource8
#define PWM_TIM1_CH2_SOURCE         GPIO_PinSource9
#define PWM_TIM1_CH3_SOURCE         GPIO_PinSource10
#define PWM_TIM1_CH4_SOURCE         GPIO_PinSource11

#elif defined(USE_OPEN_207Z_BOARD)

/*
 * PE9
 * PE11
 * PE13
 * PE14
 */
#define PWM_TIM1_GPIO_PORT          GPIOE
#define PWM_TIM1_GPIO_CLK           RCC_AHB1Periph_GPIOE
#define PWM_TIM1_CH1_PIN            GPIO_Pin_9
#define PWM_TIM1_CH2_PIN            GPIO_Pin_11
#define PWM_TIM1_CH3_PIN            GPIO_Pin_13
#define PWM_TIM1_CH4_PIN            GPIO_Pin_14

#define PWM_TIM1_CH1_SOURCE         GPIO_PinSource9
#define PWM_TIM1_CH2_SOURCE         GPIO_PinSource11
#define PWM_TIM1_CH3_SOURCE         GPIO_PinSource13
#define PWM_TIM1_CH4_SOURCE         GPIO_PinSource14

#endif


/*#########################################
 *#                TIM8                   #
 *#########################################*/
/*
 * PC6,7,8,9
 *
 */
#define PWM_TIM8_GPIO_PORT          GPIOC
#define PWM_TIM8_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define PWM_TIM8_CH1_PIN            GPIO_Pin_6
#define PWM_TIM8_CH2_PIN            GPIO_Pin_7
#define PWM_TIM8_CH3_PIN            GPIO_Pin_8
#define PWM_TIM8_CH4_PIN            GPIO_Pin_9

#define PWM_TIM8_CH1_SOURCE         GPIO_PinSource6
#define PWM_TIM8_CH2_SOURCE         GPIO_PinSource7
#define PWM_TIM8_CH3_SOURCE         GPIO_PinSource8
#define PWM_TIM8_CH4_SOURCE         GPIO_PinSource9


/*#########################################
 *#                TIM2                   #
 *#########################################*/
/*
 * PC6,7,8,9
 *
 */
#define PWM_TIM2_GPIO_PORT          GPIOC
#define PWM_TIM2_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define PWM_TIM2_CH1_PIN            GPIO_Pin_6
#define PWM_TIM2_CH2_PIN            GPIO_Pin_7
#define PWM_TIM2_CH3_PIN            GPIO_Pin_8
#define PWM_TIM2_CH4_PIN            GPIO_Pin_9

#define PWM_TIM2_CH1_SOURCE         GPIO_PinSource6
#define PWM_TIM2_CH2_SOURCE         GPIO_PinSource7
#define PWM_TIM2_CH3_SOURCE         GPIO_PinSource8
#define PWM_TIM2_CH4_SOURCE         GPIO_PinSource9


/*#########################################
 *#                TIM3                   #
 *#########################################*/
/*
 * PC6,7,8,9
 *
 */
#define PWM_TIM3_GPIO_PORT          GPIOC
#define PWM_TIM3_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define PWM_TIM3_CH1_PIN            GPIO_Pin_6
#define PWM_TIM3_CH2_PIN            GPIO_Pin_7
#define PWM_TIM3_CH3_PIN            GPIO_Pin_8
#define PWM_TIM3_CH4_PIN            GPIO_Pin_9

#define PWM_TIM3_CH1_SOURCE         GPIO_PinSource6
#define PWM_TIM3_CH2_SOURCE         GPIO_PinSource7
#define PWM_TIM3_CH3_SOURCE         GPIO_PinSource8
#define PWM_TIM3_CH4_SOURCE         GPIO_PinSource9


/*#########################################
 *#                TIM4                   #
 *#########################################*/
/*
 * PD12, PD13
 *
 */
#define PWM_TIM4_GPIO_PORT          GPIOD
#define PWM_TIM4_GPIO_CLK           RCC_AHB1Periph_GPIOD
#define PWM_TIM4_CH1_PIN            GPIO_Pin_12
#define PWM_TIM4_CH2_PIN            GPIO_Pin_13
// don't care about channel 3 and channel 4
#define PWM_TIM4_CH3_PIN            GPIO_Pin_14
#define PWM_TIM4_CH4_PIN            GPIO_Pin_15

#define PWM_TIM4_CH1_SOURCE         GPIO_PinSource12
#define PWM_TIM4_CH2_SOURCE         GPIO_PinSource13
// don't care about channel 3 and channel 4
#define PWM_TIM4_CH3_SOURCE         GPIO_PinSource14
#define PWM_TIM4_CH4_SOURCE         GPIO_PinSource15


/*#########################################
 *#                TIM5                   #
 *#########################################*/
/*
 * PC6,7,8,9
 *
 */
#define PWM_TIM5_GPIO_PORT          GPIOC
#define PWM_TIM5_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define PWM_TIM5_CH1_PIN            GPIO_Pin_6
#define PWM_TIM5_CH2_PIN            GPIO_Pin_7
#define PWM_TIM5_CH3_PIN            GPIO_Pin_8
#define PWM_TIM5_CH4_PIN            GPIO_Pin_9

#define PWM_TIM5_CH1_SOURCE         GPIO_PinSource6
#define PWM_TIM5_CH2_SOURCE         GPIO_PinSource7
#define PWM_TIM5_CH3_SOURCE         GPIO_PinSource8
#define PWM_TIM5_CH4_SOURCE         GPIO_PinSource9


/*#########################################
 *#                TIM9                   #
 *#########################################*/
#if defined(USE_ARM2_REV_0_1) || defined(USE_OPEN_207Z_BOARD)
/*
 * PA2 PA3
 *
 */
#define PWM_TIM9_GPIO_PORT          GPIOA
#define PWM_TIM9_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define PWM_TIM9_CH1_PIN            GPIO_Pin_2
#define PWM_TIM9_CH2_PIN            GPIO_Pin_3

#define PWM_TIM9_CH1_SOURCE         GPIO_PinSource2
#define PWM_TIM9_CH2_SOURCE         GPIO_PinSource3

#else

/*
 * PE5 PE6
 *
 */
#define PWM_TIM9_GPIO_PORT          GPIOE
#define PWM_TIM9_GPIO_CLK           RCC_AHB1Periph_GPIOE
#define PWM_TIM9_CH1_PIN            GPIO_Pin_5
#define PWM_TIM9_CH2_PIN            GPIO_Pin_6

#define PWM_TIM9_CH1_SOURCE         GPIO_PinSource5
#define PWM_TIM9_CH2_SOURCE         GPIO_PinSource6
#endif

/*#########################################
 *#                TIM10                  #
 *#########################################*/
/*
 * PF6
 *
 */
#define PWM_TIM10_GPIO_PORT          GPIOF
#define PWM_TIM10_GPIO_CLK           RCC_AHB1Periph_GPIOF
#define PWM_TIM10_CH1_PIN            GPIO_Pin_6

#define PWM_TIM10_CH1_SOURCE         GPIO_PinSource6



/*#########################################
 *#                TIM11                  #
 *#########################################*/
/*
 * PF7
 *
 */
#define PWM_TIM11_GPIO_PORT          GPIOF
#define PWM_TIM11_GPIO_CLK           RCC_AHB1Periph_GPIOF
#define PWM_TIM11_CH1_PIN            GPIO_Pin_7
#define PWM_TIM11_CH1_SOURCE         GPIO_PinSource7



/*#########################################
 *#                TIM12                  #
 *#########################################*/
/*
 * PB14,PB15
 *
 */
#define PWM_TIM12_GPIO_PORT          GPIOB
#define PWM_TIM12_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define PWM_TIM12_CH1_PIN            GPIO_Pin_14
#define PWM_TIM12_CH2_PIN            GPIO_Pin_15
#define PWM_TIM12_CH1_SOURCE         GPIO_PinSource14
#define PWM_TIM12_CH2_SOURCE         GPIO_PinSource15




/*#########################################
 *#                TIM13                  #
 *#########################################*/
/*
 * PA6
 *
 */
#define PWM_TIM13_GPIO_PORT          GPIOA
#define PWM_TIM13_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define PWM_TIM13_CH1_PIN            GPIO_Pin_6
#define PWM_TIM13_CH1_SOURCE         GPIO_PinSource6


/*#########################################
 *#                TIM14                  #
 *#########################################*/
/*
 * PA7
 *
 */
#define PWM_TIM14_GPIO_PORT          GPIOA
#define PWM_TIM14_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define PWM_TIM14_CH1_PIN            GPIO_Pin_7
#define PWM_TIM14_CH1_SOURCE         GPIO_PinSource7






static u32      PWM_TIM_CLK[PWM_TIM_NUM]          = {RCC_APB2Periph_TIM1, RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4,
                                                     RCC_APB1Periph_TIM5, RCC_APB2Periph_TIM8, RCC_APB2Periph_TIM9, RCC_APB2Periph_TIM10,
                                                     RCC_APB2Periph_TIM11, RCC_APB1Periph_TIM12, RCC_APB1Periph_TIM13, RCC_APB1Periph_TIM14};

static TIM_TypeDef*  PWM_TIM_NUMBER[PWM_TIM_NUM]  = {TIM1, TIM2, TIM3, TIM4, TIM5, TIM8, TIM9, TIM10, TIM11, TIM12, TIM13, TIM14};


/* Private define ------------------------------------------------------------*/
static u32 TIM_period[PWM_TIM_NUM];


#define TIM_CC_FREQ   60000000




/*----------------------------------- TIM1 and TIM8 -----------------------------------*/
/*
 * @brief: set TIM1 and TIM8 PWM
 * @param: index, select TIM to output PWM
 * @param: feq, Frequency of PWM
 * @param: channel_start_flag, select PWM output channel
 *         PWM_CHANNEL_1
 *         PWM_CHANNEL_2
 *         PWM_CHANNEL_3
 *         PWM_CHANNEL_4
 * @param: duty_cycle
 * @retval: void
 */
void TIM1_8_pwm_config( pwm_timer_num index,
                                    u32           feq,
                                    u16           channel_start_flag,
                                    u8            duty_cycle1,
                                    u8            duty_cycle2,
                                    u8            duty_cycle3,
                                    u8            duty_cycle4)
{
    uint32_t                    cycle;
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;
    uint16_t                    PrescalerValue = 0;

    assert_param(IS_PWM_TIM1_8_TYPE(index));

    GPIO_InitStructure.GPIO_Pin = 0;
    RCC_APB2PeriphClockCmd(PWM_TIM_CLK[index], ENABLE);
    switch (index)
    {
        case PWM_TIM1:
            RCC_AHB1PeriphClockCmd(PWM_TIM1_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM1_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM1_CH2_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM1_CH3_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM1_CH4_PIN;
            }
            break;
        case PWM_TIM8:
            RCC_AHB1PeriphClockCmd(PWM_TIM8_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM8_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM8_CH2_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM8_CH3_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM8_CH4_PIN;
            }
            break;
    }

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    switch (index)
    {
        case PWM_TIM1:
            GPIO_Init(PWM_TIM1_GPIO_PORT, &GPIO_InitStructure);
            break;
        case PWM_TIM8:
            GPIO_Init(PWM_TIM8_GPIO_PORT, &GPIO_InitStructure);
            break;
    }

    switch (index)
    {
        case PWM_TIM1:
            /* Connect TIM1 pins to AF1 */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM1_GPIO_PORT, PWM_TIM1_CH1_SOURCE, GPIO_AF_TIM1);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM1_GPIO_PORT, PWM_TIM1_CH2_SOURCE, GPIO_AF_TIM1);
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_PinAFConfig(PWM_TIM1_GPIO_PORT, PWM_TIM1_CH3_SOURCE, GPIO_AF_TIM1);
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_PinAFConfig(PWM_TIM1_GPIO_PORT, PWM_TIM1_CH4_SOURCE, GPIO_AF_TIM1);
            }
            break;
        case PWM_TIM8:
            /* Connect TIM8 pins to AF3 */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM8_GPIO_PORT, PWM_TIM8_CH1_SOURCE, GPIO_AF_TIM8);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM8_GPIO_PORT, PWM_TIM8_CH2_SOURCE, GPIO_AF_TIM8);
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_PinAFConfig(PWM_TIM8_GPIO_PORT, PWM_TIM8_CH3_SOURCE, GPIO_AF_TIM8);
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_PinAFConfig(PWM_TIM8_GPIO_PORT, PWM_TIM8_CH4_SOURCE, GPIO_AF_TIM8);
            }
            break;
    }

    if (feq <= 2000)
    {
        PrescalerValue = (uint16_t) ((SystemCoreClock) / (TIM_CC_FREQ/1000)) - 1;
        TIM_period[index] = (TIM_CC_FREQ/1000) / feq - 1;
    }
    else
    {
        /* Compute the prescaler value 60MHz */
        PrescalerValue = (uint16_t) ((SystemCoreClock) / TIM_CC_FREQ) - 1;
        TIM_period[index] = TIM_CC_FREQ / feq - 1;
    }

    TIM_TimeBaseStructure.TIM_Period = TIM_period[index];
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(PWM_TIM_NUMBER[index], &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;

    if (channel_start_flag & PWM_CHANNEL_1)
    {
        cycle = (TIM_period[index] + 1) * duty_cycle1 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC1Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
    }

    if (channel_start_flag & PWM_CHANNEL_2)
    {
        cycle = (TIM_period[index] + 1) * duty_cycle2 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC2Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
    }

    if (channel_start_flag & PWM_CHANNEL_3)
    {
        cycle = (TIM_period[index] + 1) * duty_cycle3 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC3Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
    }

    if (channel_start_flag & PWM_CHANNEL_4)
    {
        cycle = (TIM_period[index] + 1) * duty_cycle4 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC4Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
    }

    TIM_Cmd(PWM_TIM_NUMBER[index], ENABLE);

    TIM_CtrlPWMOutputs(PWM_TIM_NUMBER[index], ENABLE);

	}	


/*
 * @brief: set TIM2-TIM5 and TIM9-TIM14 PWM
 * @param: index, select TIM to output PWM
 * @param: feq, Frequency of PWM
 * @param: channel_start_flag, select PWM output channel
 *          PWM_CHANNEL_1
 *          PWM_CHANNEL_2
 *          PWM_CHANNEL_3
 *          PWM_CHANNEL_4
 * @param: duty_cycle
 * @retval: void
 */
void TIM2_5_9_14_pwm_config( pwm_timer_num index,
                                           u32     feq,
                                           u16     channel_start_flag,
                                           u8      duty_cycle1,
                                           u8      duty_cycle2,
                                           u8      duty_cycle3,
                                           u8      duty_cycle4)
{
    u32                         cycle;
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;
    u16                         PrescalerValue = 0;

    assert_param(IS_PWM_TIM2_5_9_14_TYPE(index));

    /* TIM clock enable */
    switch (index)
    {
        case PWM_TIM9:
        case PWM_TIM10:
        case PWM_TIM11:
            RCC_APB2PeriphClockCmd(PWM_TIM_CLK[index], ENABLE);
            break;
        case PWM_TIM2:
        case PWM_TIM3:
        case PWM_TIM4:
        case PWM_TIM5:
        case PWM_TIM12:
        case PWM_TIM13:
        case PWM_TIM14:
            RCC_APB1PeriphClockCmd(PWM_TIM_CLK[index], ENABLE);
            break;
    }

    /* GPIOA clock enable */
    GPIO_InitStructure.GPIO_Pin = 0;
    switch (index)
    {
        case PWM_TIM2:
            RCC_AHB1PeriphClockCmd(PWM_TIM2_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM2_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM2_CH2_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM2_CH3_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM2_CH4_PIN;
            }
            break;
        case PWM_TIM3:
            RCC_AHB1PeriphClockCmd(PWM_TIM3_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM3_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM3_CH2_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM3_CH3_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM3_CH4_PIN;
            }
            break;
        case PWM_TIM4:
            RCC_AHB1PeriphClockCmd(PWM_TIM4_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM4_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM4_CH2_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM4_CH3_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM4_CH4_PIN;
            }
            break;
        case PWM_TIM5:
            RCC_AHB1PeriphClockCmd(PWM_TIM5_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM5_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM5_CH2_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM5_CH3_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM5_CH4_PIN;
            }
            break;
        case PWM_TIM9:
            RCC_AHB1PeriphClockCmd(PWM_TIM9_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM9_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM9_CH2_PIN;
            }
            break;
        case PWM_TIM10:
            RCC_AHB1PeriphClockCmd(PWM_TIM10_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM10_CH1_PIN;
            }
            break;
        case PWM_TIM11:
            RCC_AHB1PeriphClockCmd(PWM_TIM11_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM11_CH1_PIN;
            }
            break;
        case PWM_TIM12:
            RCC_AHB1PeriphClockCmd(PWM_TIM12_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM12_CH1_PIN;
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM12_CH2_PIN;
            }
            break;
        case PWM_TIM13:
            RCC_AHB1PeriphClockCmd(PWM_TIM13_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM13_CH1_PIN;
            }
            break;
        case PWM_TIM14:
            RCC_AHB1PeriphClockCmd(PWM_TIM14_GPIO_CLK, ENABLE);
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_InitStructure.GPIO_Pin |= PWM_TIM14_CH1_PIN;
            }
            break;
    }

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    switch (index)
    {
        case PWM_TIM2:
            GPIO_Init(PWM_TIM2_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM2_GPIO_PORT, PWM_TIM2_CH1_SOURCE, GPIO_AF_TIM2);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM2_GPIO_PORT, PWM_TIM2_CH2_SOURCE, GPIO_AF_TIM2);
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_PinAFConfig(PWM_TIM2_GPIO_PORT, PWM_TIM2_CH3_SOURCE, GPIO_AF_TIM2);
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_PinAFConfig(PWM_TIM2_GPIO_PORT, PWM_TIM2_CH4_SOURCE, GPIO_AF_TIM2);
            }
            break;
        case PWM_TIM3:
            GPIO_Init(PWM_TIM3_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM3_GPIO_PORT, PWM_TIM3_CH1_SOURCE, GPIO_AF_TIM3);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM3_GPIO_PORT, PWM_TIM3_CH2_SOURCE, GPIO_AF_TIM3);
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_PinAFConfig(PWM_TIM3_GPIO_PORT, PWM_TIM3_CH3_SOURCE, GPIO_AF_TIM3);
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_PinAFConfig(PWM_TIM3_GPIO_PORT, PWM_TIM3_CH4_SOURCE, GPIO_AF_TIM3);
            }
            break;
        case PWM_TIM4:
            GPIO_Init(PWM_TIM4_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM4_GPIO_PORT, PWM_TIM4_CH1_SOURCE, GPIO_AF_TIM4);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM4_GPIO_PORT, PWM_TIM4_CH2_SOURCE, GPIO_AF_TIM4);
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_PinAFConfig(PWM_TIM4_GPIO_PORT, PWM_TIM4_CH3_SOURCE, GPIO_AF_TIM4);
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_PinAFConfig(PWM_TIM4_GPIO_PORT, PWM_TIM4_CH4_SOURCE, GPIO_AF_TIM4);
            }
            break;
        case PWM_TIM5:
            GPIO_Init(PWM_TIM5_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM5_GPIO_PORT, PWM_TIM5_CH1_SOURCE, GPIO_AF_TIM5);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM5_GPIO_PORT, PWM_TIM5_CH2_SOURCE, GPIO_AF_TIM5);
            }
            if (channel_start_flag & PWM_CHANNEL_3)
            {
                GPIO_PinAFConfig(PWM_TIM5_GPIO_PORT, PWM_TIM5_CH3_SOURCE, GPIO_AF_TIM5);
            }
            if (channel_start_flag & PWM_CHANNEL_4)
            {
                GPIO_PinAFConfig(PWM_TIM5_GPIO_PORT, PWM_TIM5_CH4_SOURCE, GPIO_AF_TIM5);
            }
            break;
        case PWM_TIM9:
            GPIO_Init(PWM_TIM9_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM9_GPIO_PORT, PWM_TIM9_CH1_SOURCE, GPIO_AF_TIM9);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM9_GPIO_PORT, PWM_TIM9_CH2_SOURCE, GPIO_AF_TIM9);
            }
            break;
        case PWM_TIM10:
            GPIO_Init(PWM_TIM10_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM10_GPIO_PORT, PWM_TIM10_CH1_SOURCE, GPIO_AF_TIM10);
            }
            break;
        case PWM_TIM11:
            GPIO_Init(PWM_TIM11_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM11_GPIO_PORT, PWM_TIM11_CH1_SOURCE, GPIO_AF_TIM11);
            }
            break;
        case PWM_TIM12:
            GPIO_Init(PWM_TIM12_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM12_GPIO_PORT, PWM_TIM12_CH1_SOURCE, GPIO_AF_TIM12);
            }
            if (channel_start_flag & PWM_CHANNEL_2)
            {
                GPIO_PinAFConfig(PWM_TIM12_GPIO_PORT, PWM_TIM12_CH2_SOURCE, GPIO_AF_TIM12);
            }
            break;
        case PWM_TIM13:
            GPIO_Init(PWM_TIM13_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM13_GPIO_PORT, PWM_TIM13_CH1_SOURCE, GPIO_AF_TIM13);
            }
            break;
        case PWM_TIM14:
            GPIO_Init(PWM_TIM14_GPIO_PORT, &GPIO_InitStructure);
            /* init AF */
            if (channel_start_flag & PWM_CHANNEL_1)
            {
                GPIO_PinAFConfig(PWM_TIM14_GPIO_PORT, PWM_TIM14_CH1_SOURCE, GPIO_AF_TIM14);
            }
            break;
    }

    // TIM1 TIM8 TIM9 TIM10 TIM11 ---- 120MHz
    // others                     ---- 60MHz
    // Freq(min) = 120Mhz/(0xffff+1) = 1831.05 ~~ 2000
    // PrescalerValue/1000  ~~~~ 2Hz(min)
    switch (index)
    {
        // 120MHz
        case PWM_TIM9:
        case PWM_TIM10:
        case PWM_TIM11:
            if (feq < 2000)
            {
                PrescalerValue = (uint16_t) ((SystemCoreClock) / (TIM_CC_FREQ/1000)) - 1;
                TIM_period[index] = (TIM_CC_FREQ/1000) / feq - 1;
            }
            else
            {
                PrescalerValue = (uint16_t) ((SystemCoreClock) / TIM_CC_FREQ) - 1;
                TIM_period[index] = TIM_CC_FREQ / feq - 1;
            }
            break;
        // 60MHz
        default:
            if (feq < 2000)
            {
                PrescalerValue = (uint16_t) ((SystemCoreClock/2) / (TIM_CC_FREQ/1000)) - 1;
                TIM_period[index] = (TIM_CC_FREQ/1000) / feq - 1;
            }
            else
            {
                PrescalerValue = (uint16_t) ((SystemCoreClock/2) / TIM_CC_FREQ) - 1;
                TIM_period[index] = TIM_CC_FREQ / feq - 1;
            }
            break;
    }

    TIM_TimeBaseStructure.TIM_Period        = TIM_period[index];
    TIM_TimeBaseStructure.TIM_Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(PWM_TIM_NUMBER[index], &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    if (channel_start_flag & PWM_CHANNEL_1)
    {
        /* PWM1 Mode configuration: Channel1 */
        cycle = (TIM_period[index] + 1) * duty_cycle1 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC1Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(PWM_TIM_NUMBER[index], TIM_OCPreload_Enable);
    }

    if (channel_start_flag & PWM_CHANNEL_2)
    {
        /* PWM1 Mode configuration: Channel2 */
        cycle = (TIM_period[index] + 1) * duty_cycle2 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC2Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(PWM_TIM_NUMBER[index], TIM_OCPreload_Enable);
    }

    if (channel_start_flag & PWM_CHANNEL_3)
    {
        /* PWM1 Mode configuration: Channel3 */
        cycle = (TIM_period[index] + 1) * duty_cycle3 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC3Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(PWM_TIM_NUMBER[index], TIM_OCPreload_Enable);
    }

    if (channel_start_flag & PWM_CHANNEL_4)
    {
        /* PWM1 Mode configuration: Channel4 */
        cycle = (TIM_period[index] + 1) * duty_cycle4 / 100;
        TIM_OCInitStructure.TIM_Pulse = cycle;
        TIM_OC4Init(PWM_TIM_NUMBER[index], &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(PWM_TIM_NUMBER[index], TIM_OCPreload_Enable);
    }

    TIM_ARRPreloadConfig(PWM_TIM_NUMBER[index], ENABLE);
    TIM_Cmd(PWM_TIM_NUMBER[index], ENABLE);

}


/*
 * @brief: set PWM value: 0% - 100%
 * @param: index, PWM_TIM1, PWM_TIM2...
 * @param: channel_flag
 *         PWM_CHANNEL_1
 *         PWM_CHANNEL_2
 *         PWM_CHANNEL_3
 *         PWM_CHANNEL_4
 * @param: opposition, output is opposition or normal
 * @retval: void
 */
void pwm_change_cycle(pwm_timer_num index,
                                 u16           channel_flag,
                                 u8            duty_cycle,
                                 u8            opposition)
{
	u32 cycle;
    TIM_TypeDef * p_timer = TIM1;

    assert_param(duty_cycle <= 100);
    assert_param(IS_PWM_CHANNEL_TYPE(channel_flag));

    if (opposition)
    {
        duty_cycle = 100 - duty_cycle;
    }

    switch (index)
    {
        case PWM_TIM1:
            p_timer = TIM1;
            break;
        case PWM_TIM2:
            p_timer = TIM2;
            break;
        case PWM_TIM3:
            p_timer = TIM3;
            break;
        case PWM_TIM4:
            p_timer = TIM4;
            break;
        case PWM_TIM5:
            p_timer = TIM5;
            break;
        case PWM_TIM8:
            p_timer = TIM8;
            break;
        case PWM_TIM9:
            p_timer = TIM9;
            break;
        case PWM_TIM10:
            p_timer = TIM10;
            break;
        case PWM_TIM11:
            p_timer = TIM11;
            break;
        case PWM_TIM12:
            p_timer = TIM12;
            break;
        case PWM_TIM13:
            p_timer = TIM13;
            break;
        case PWM_TIM14:
            p_timer = TIM14;
            break;
    }

    cycle = (TIM_period[index] + 1) * duty_cycle / 100;
    switch (channel_flag)
    {
        case PWM_CHANNEL_1:
            TIM_SetCompare1(p_timer, cycle);
            break;
        case PWM_CHANNEL_2:
            TIM_SetCompare2(p_timer, cycle);
            break;
        case PWM_CHANNEL_3:
            TIM_SetCompare3(p_timer, cycle);
            break;
        case PWM_CHANNEL_4:
            TIM_SetCompare4(p_timer, cycle);
            break;
    }
}


/*
 * @brief: change TIM1 - TIM14 except TIM6 and TIM7 PWM value
 * @param: index, selcet TIM to output PWM
 * @param: channel_flag, can be set as:
 *         PWM_CHANNEL_1
 *         PWM_CHANNEL_2
 *         PWM_CHANNEL_3
 *         PWM_CHANNEL_4
 * @param: duty_cycle, this value can't beyond TIM1_period[index]
 * @opposition: 1 ---> opposition output PWM
 *              0 ---> normal output PWM
 * @retval: void
 */
void pwm_change(pwm_timer_num index,
                        u16           channel_flag,
                        u32           duty_cycle,
                        u8            opposition)
{
	u32     cycle;

    assert_param(IS_PWM_TIM_TYPE(index));
    assert_param(IS_PWM_CHANNEL_TYPE(channel_flag));
    assert_param(duty_cycle <= TIM_period[index] + 1);

	if (opposition == 0)
		cycle = duty_cycle;
	else
		cycle = (TIM_period[index] + 1) - duty_cycle;

	switch (channel_flag)
	{
		case PWM_CHANNEL_1:
			TIM_SetCompare1(PWM_TIM_NUMBER[index], cycle);
			break;
		case PWM_CHANNEL_2:
			TIM_SetCompare2(PWM_TIM_NUMBER[index], cycle);
			break;
		case PWM_CHANNEL_3:
			TIM_SetCompare3(PWM_TIM_NUMBER[index], cycle);
			break;
		case PWM_CHANNEL_4:
			TIM_SetCompare4(PWM_TIM_NUMBER[index], cycle);
			break;
	}

}


/**
  * 回输泵正反转，其余4个泵全部反转
  * 在这里我们做一下保护
  *
  */
u32 get_pwm_max_value(pwm_timer_num index)
{
	return (TIM_period[index] + 1);
}


s32 get_pwm_min_value(pwm_timer_num index)
{
	return -(TIM_period[index] + 1);
}




