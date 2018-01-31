/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : input_capture.c
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
#include "input_capture.h"
#include "trace.h"
#include "encoder.h"
#include "app_dc_motor.h"

#define TIM4_PRESCALER      100
#define TIM4_OVERFLOW_CNT   7000

static vu32 g_capture_overflow = 0;


static vu32 g_IC1_capture_value[PWM_CAPTURE_TIM_NUM] = {0};
static vu32 g_IC2_capture_value[PWM_CAPTURE_TIM_NUM] = {0};
static vu32 g_IC3_capture_value[PWM_CAPTURE_TIM_NUM] = {0};
static vu32 g_IC4_capture_value[PWM_CAPTURE_TIM_NUM] = {0};

static double g_IC2_sp_backup[PWM_CAPTURE_TIM_NUM] = {0.0};

/*#########################################
 *#                TIM1                   #
 *#########################################*/
// PE.09 CH1
#define INPUT_CAP_TIM1_GPIO_PORT        GPIOE
#define INPUT_CAP_TIM1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define INPUT_CAP_TIM1_PIN              GPIO_Pin_9
#define INPUT_CAP_TIM1_PIN_SOURCE       GPIO_PinSource9
#define INPUT_CAP_TIM1_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1



/*#########################################
 *#                TIM2                   #
 *#########################################*/
// PA5 CH1
#define INPUT_CAP_TIM2_GPIO_PORT        GPIOA
#define INPUT_CAP_TIM2_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define INPUT_CAP_TIM2_PIN              GPIO_Pin_5
#define INPUT_CAP_TIM2_PIN_SOURCE       GPIO_PinSource5
#define INPUT_CAP_TIM2_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1



/*#########################################
 *#                TIM3                   #
 *#########################################*/
// PA6 CH1
#define INPUT_CAP_TIM3_GPIO_PORT        GPIOA
#define INPUT_CAP_TIM3_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define INPUT_CAP_TIM3_PIN              GPIO_Pin_6
#define INPUT_CAP_TIM3_PIN_SOURCE       GPIO_PinSource6
#define INPUT_CAP_TIM3_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1



/*#########################################
 *#                TIM4                   #
 *#########################################*/
// PD12 CH1
#define INPUT_CAP_TIM4_GPIO_PORT        GPIOD
#define INPUT_CAP_TIM4_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define INPUT_CAP_TIM4_PIN              GPIO_Pin_12
#define INPUT_CAP_TIM4_PIN_SOURCE       GPIO_PinSource12
#define INPUT_CAP_TIM4_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1



/*#########################################
 *#                TIM5                   #
 *#########################################*/
// PA0 CH1
#define INPUT_CAP_TIM5_GPIO_PORT        GPIOA
#define INPUT_CAP_TIM5_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define INPUT_CAP_TIM5_PIN              GPIO_Pin_0
#define INPUT_CAP_TIM5_PIN_SOURCE       GPIO_PinSource0
#define INPUT_CAP_TIM5_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1


/*#########################################
 *#                TIM8                   #
 *#########################################*/
// PC.06 CH1
#define INPUT_CAP_TIM8_GPIO_PORT        GPIOC
#define INPUT_CAP_TIM8_GPIO_CLK         RCC_AHB1Periph_GPIOC
#define INPUT_CAP_TIM8_PIN              GPIO_Pin_6
#define INPUT_CAP_TIM8_PIN_SOURCE       GPIO_PinSource6
#define INPUT_CAP_TIM8_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1



/*#########################################
 *#                TIM9                   #
 *#########################################*/
// PE5 CH1
#define INPUT_CAP_TIM9_GPIO_PORT        GPIOE
#define INPUT_CAP_TIM9_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define INPUT_CAP_TIM9_PIN              GPIO_Pin_5
#define INPUT_CAP_TIM9_PIN_SOURCE       GPIO_PinSource5
#define INPUT_CAP_TIM9_EXTER_SOURCE     TIM_TIxExternalCLK1Source_TI1



/*#########################################
 *#                TIM12                  #
 *#########################################*/
// PB14 CH14
#define INPUT_CAP_TIM12_GPIO_PORT       GPIOB
#define INPUT_CAP_TIM12_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define INPUT_CAP_TIM12_PIN             GPIO_Pin_14
#define INPUT_CAP_TIM12_PIN_SOURCE      GPIO_PinSource14
#define INPUT_CAP_TIM12_EXTER_SOURCE    TIM_TIxExternalCLK1Source_TI1



/* TIM1 TIM2 TIM3 TIM4 TIM5 TIM8 TIM9 TIM12 */
static const u32 INPUT_CAPTURE_TIM_CLK[PWM_CAPTURE_TIM_NUM] = {RCC_APB2Periph_TIM1, RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4,
                                                               RCC_APB1Periph_TIM5, RCC_APB2Periph_TIM8, RCC_APB2Periph_TIM9, RCC_APB1Periph_TIM12};

static TIM_TypeDef* INPUT_CAPTURE_TIM_NUMBER[PWM_CAPTURE_TIM_NUM]  = {TIM1, TIM2, TIM3, TIM4, TIM5, TIM8, TIM9, TIM12};


static const GPIO_TypeDef* INPUT_CAP_PORT[PWM_CAPTURE_TIM_NUM] = {INPUT_CAP_TIM1_GPIO_PORT, INPUT_CAP_TIM2_GPIO_PORT, INPUT_CAP_TIM3_GPIO_PORT,
                                                                  INPUT_CAP_TIM4_GPIO_PORT, INPUT_CAP_TIM5_GPIO_PORT, INPUT_CAP_TIM8_GPIO_PORT,
                                                                  INPUT_CAP_TIM9_GPIO_PORT, INPUT_CAP_TIM12_GPIO_PORT};

static const uint32_t INPUT_CAP_GPIO_CLK[PWM_CAPTURE_TIM_NUM] = {INPUT_CAP_TIM1_GPIO_CLK, INPUT_CAP_TIM2_GPIO_CLK, INPUT_CAP_TIM3_GPIO_CLK,
                                                                 INPUT_CAP_TIM4_GPIO_CLK, INPUT_CAP_TIM5_GPIO_CLK, INPUT_CAP_TIM8_GPIO_CLK,
                                                                 INPUT_CAP_TIM9_GPIO_CLK, INPUT_CAP_TIM12_GPIO_CLK};

static const uint16_t INPUT_CAP_PIN[PWM_CAPTURE_TIM_NUM] = {INPUT_CAP_TIM1_PIN, INPUT_CAP_TIM2_PIN, INPUT_CAP_TIM3_PIN, INPUT_CAP_TIM4_PIN,
                                                            INPUT_CAP_TIM5_PIN, INPUT_CAP_TIM8_PIN, INPUT_CAP_TIM9_PIN, INPUT_CAP_TIM12_PIN};

static const uint16_t INPUT_CAP_PIN_SOURCE[PWM_CAPTURE_TIM_NUM] = {INPUT_CAP_TIM1_PIN_SOURCE, INPUT_CAP_TIM2_PIN_SOURCE, INPUT_CAP_TIM3_PIN_SOURCE,
                                                                   INPUT_CAP_TIM4_PIN_SOURCE, INPUT_CAP_TIM5_PIN_SOURCE, INPUT_CAP_TIM8_PIN_SOURCE,
                                                                   INPUT_CAP_TIM9_PIN_SOURCE, INPUT_CAP_TIM12_PIN_SOURCE};
static const uint16_t INPUT_CAP_AF[PWM_CAPTURE_TIM_NUM] = {GPIO_AF_TIM1, GPIO_AF_TIM2, GPIO_AF_TIM3, GPIO_AF_TIM4, GPIO_AF_TIM5, GPIO_AF_TIM8,
                                                           GPIO_AF_TIM9, GPIO_AF_TIM12};


static const uint16_t INPUT_CAP_EXTER_SOURCE[PWM_CAPTURE_TIM_NUM] = {INPUT_CAP_TIM1_EXTER_SOURCE, INPUT_CAP_TIM2_EXTER_SOURCE, INPUT_CAP_TIM3_EXTER_SOURCE,
                                                                     INPUT_CAP_TIM4_EXTER_SOURCE, INPUT_CAP_TIM5_EXTER_SOURCE, INPUT_CAP_TIM8_EXTER_SOURCE,
                                                                     INPUT_CAP_TIM9_EXTER_SOURCE, INPUT_CAP_TIM12_EXTER_SOURCE};


/**
  * @brief: Be careful not to overflow!!!
  *
  */
void init_input_capture(input_capture_timer_num index)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    assert_param(IS_INPUT_CAP_NUM_TYPE(index));

    /* TIM clock enable */
    // TIM1 TIM8 TIM9               APB2
    // TIM2 TIM3 TIM4 TIM5 TIM12    APB1
    switch (index)
    {
        case PWM_CAPTURE_TIM1:
        case PWM_CAPTURE_TIM8:
        case PWM_CAPTURE_TIM9:
            RCC_APB2PeriphClockCmd(INPUT_CAPTURE_TIM_CLK[index], ENABLE);
            break;
        default:
            RCC_APB1PeriphClockCmd(INPUT_CAPTURE_TIM_CLK[index], ENABLE);
            break;
    }

    /* GPIO clock enable */
    RCC_AHB1PeriphClockCmd(INPUT_CAP_GPIO_CLK[index], ENABLE);

    GPIO_InitStructure.GPIO_Pin =  INPUT_CAP_PIN[index];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init((GPIO_TypeDef*)INPUT_CAP_PORT[index], &GPIO_InitStructure);

    GPIO_PinAFConfig((GPIO_TypeDef*)INPUT_CAP_PORT[index], INPUT_CAP_PIN_SOURCE[index], INPUT_CAP_AF[index]);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period        = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(INPUT_CAPTURE_TIM_NUMBER[index], &TIM_TimeBaseStructure);
    TIM_TIxExternalClockConfig(INPUT_CAPTURE_TIM_NUMBER[index], INPUT_CAP_EXTER_SOURCE[index], TIM_ICPolarity_BothEdge, 2);
    TIM_SetCounter(INPUT_CAPTURE_TIM_NUMBER[index], 0);

#if 0
    TIM_ClearFlag(INPUT_CAPTURE_TIM_NUMBER[index], TIM_FLAG_Update);
    TIM_ITConfig(INPUT_CAPTURE_TIM_NUMBER[index], TIM_IT_Update, ENABLE);

	/* Enable the TIM gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INPUT_CAPTURE_PREEPRIOROTY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = INPUT_CAPTURE_SUBPRIOROTY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
    TIM_Cmd(INPUT_CAPTURE_TIM_NUMBER[index], ENABLE);
}


/**
  * @brief:
  *
  */
void init_pwm_input_capture(input_capture_timer_num index)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;
    TIM_ICInitTypeDef       TIM_ICInitStructure;
    DMA_InitTypeDef         DMA_InitStructure;

    assert_param(IS_INPUT_CAP_NUM_TYPE(index));

    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* GPIOB clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* DMA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period        = TIM4_OVERFLOW_CNT;
    TIM_TimeBaseStructure.TIM_Prescaler     = TIM4_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* TIM4 chennel2 configuration : PD.13 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Connect TIM pin to AF2 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);

    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_CAPTURE_PREEPRIOROTY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PWM_CAPTURE_SUBPRIOROTY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;

    TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);

    /* Select the TIM4 Input Trigger: TI2FP2 */
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2);

    /* Select the slave Mode: Reset Mode */
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);

    TIM_UpdateRequestConfig(TIM4, TIM_UpdateSource_Regular);

    DMA_DeInit(DMA1_Stream3);
    DMA_InitStructure.DMA_Channel            = DMA_Channel_2;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM4->CCR2;   //0x40000838;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&g_IC2_capture_value[PWM_CAPTURE_TIM4];
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = 1;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

    DMA_Init(DMA1_Stream3, &DMA_InitStructure);
}


void enable_pwm_capture(input_capture_timer_num index)
{
    TIM_Cmd(TIM4, ENABLE);
    DMA_Cmd(DMA1_Stream3, ENABLE);
    TIM_DMACmd(TIM4, TIM_DMA_CC2, ENABLE);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

void disable_pwm_capture(input_capture_timer_num index)
{
    TIM_Cmd(TIM4, DISABLE);
}


#if 0
double get_pwm_input_capture_speed(input_capture_timer_num index)
{
    u32 freq;
    double sp_temp;
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    // the DC motor maybe locked
    if (g_capture_overflow)
    {
        //TRACE("o=%d,I=%d\r\n", g_capture_overflow, g_IC2_capture_value[PWM_CAPTURE_TIM4]);
        g_capture_overflow = 0;
        sp_temp = 0.0;
        g_IC2_sp_backup[PWM_CAPTURE_TIM4] = 0;
    }
    else
    {
        freq = RCC_Clocks.SYSCLK_Frequency/(2*TIM4_PRESCALER);
        //TRACE("I=%d\r\n", g_IC2_capture_value[PWM_CAPTURE_TIM4]);
        // no pwm input has been captured
        if (g_IC2_capture_value[PWM_CAPTURE_TIM4] < 22)
        {
            //TRACE("I=%d\r\n", g_IC2_capture_value[PWM_CAPTURE_TIM4]);
            //sp_temp = 0.0;
            sp_temp = g_IC2_sp_backup[PWM_CAPTURE_TIM4];
        }
        else
        {
            sp_temp = (double)freq/(ENCODER_PPR*g_IC2_capture_value[PWM_CAPTURE_TIM4]) * 60 * DC_MOTOR_SPEED_PRECISION;
            g_IC2_sp_backup[PWM_CAPTURE_TIM4] = sp_temp;
            g_IC2_capture_value[PWM_CAPTURE_TIM4] = 0;
        }
    }

    return sp_temp;
}
#endif



/**
  * @brief: get input capture count
  * @note:  the TIM CNT would be clear after you read
  * @note:  Be careful not to overflow
  */
u32 get_input_capture_value(input_capture_timer_num index)
{
    u32 pulse_number = 0;

    pulse_number = INPUT_CAPTURE_TIM_NUMBER[index]->CNT;
    INPUT_CAPTURE_TIM_NUMBER[index]->CNT = 0;

    return pulse_number;
}



#if 1
void TIM4_IRQHandler(void)
{
    if (TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        if (TIM4->CNT == 0)
        {
            g_capture_overflow++;
        }

        //TODO: deal with
    }
}

#endif


