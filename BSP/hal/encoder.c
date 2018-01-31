
/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : encoder.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/07
 * Description        : This file contains the software implementation for the
 *                      encoder unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/05/07 | v1.0  |            | initial released
 * 2013/05/31 | v1.1  | Bruce.zhu  | add three timer for encoder
 * 2013/11/04 | v1.2  | Bruce.zhu  | change framework to support multiple
 *                                   configurations for encoder timer
 *******************************************************************************/
#include "encoder.h"
#include "trace.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define COUNTER_RESET                   (u16)0
#define ICx_FILTER                      (u8)9
#define SP_BUF_SIZE                     8
#define ENCODER_TIM_PERIOD              (0xffff)
#define MAX_COUNT                       (2500)


#if defined(USE_OPEN_207Z_BOARD) || defined(USE_ARM1_REV_0_1) || defined(USE_ARM2_REV_0_1)

// --------------- Encoder 1 ---------------
#define ENCODER_TIM1                    TIM2
#define ENCODER_TIM1_IRQ_NUM            TIM2_IRQn
#define ENCODER_TIM1_CLK                RCC_APB1Periph_TIM2
// encoder 1 GPIO Pin configuration
// PA0 PA1
#define ENCODER_TIM1_GPIO_PORT          GPIOA
#define ENCODER_TIM1_PH_A_PIN           GPIO_Pin_0
#define ENCODER_TIM1_PH_A_PIN_SOURCE    GPIO_PinSource0
#define ENCODER_TIM1_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM1_PH_A_AF            GPIO_AF_TIM2
#define ENCODER_TIM1_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM1_PH_B_PIN           GPIO_Pin_1
#define ENCODER_TIM1_PH_B_PIN_SOURCE    GPIO_PinSource1
#define ENCODER_TIM1_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM1_PH_B_AF            GPIO_AF_TIM2
#define ENCODER_TIM1_PH_B_CHANNEL       TIM_Channel_2


// --------------- Encoder 2 ---------------
#define ENCODER_TIM2                    TIM3
#define ENCODER_TIM2_IRQ_NUM            TIM3_IRQn
#define ENCODER_TIM2_CLK                RCC_APB1Periph_TIM3

#if defined(USE_OPEN_207Z_BOARD)
// encoder 2 GPIO Pin configuration
// PC6 PC7
#define ENCODER_TIM2_GPIO_PORT          GPIOC
#define ENCODER_TIM2_PH_A_PIN           GPIO_Pin_6
#define ENCODER_TIM2_PH_A_PIN_SOURCE    GPIO_PinSource6
#define ENCODER_TIM2_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOC
#define ENCODER_TIM2_PH_A_AF            GPIO_AF_TIM3
#define ENCODER_TIM2_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM2_PH_B_PIN           GPIO_Pin_7
#define ENCODER_TIM2_PH_B_PIN_SOURCE    GPIO_PinSource7
#define ENCODER_TIM2_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOC
#define ENCODER_TIM2_PH_B_AF            GPIO_AF_TIM3
#define ENCODER_TIM2_PH_B_CHANNEL       TIM_Channel_2
#else
// encoder 2 GPIO Pin configuration
// PA6 PA7
#define ENCODER_TIM2_GPIO_PORT          GPIOA
#define ENCODER_TIM2_PH_A_PIN           GPIO_Pin_6
#define ENCODER_TIM2_PH_A_PIN_SOURCE    GPIO_PinSource6
#define ENCODER_TIM2_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM2_PH_A_AF            GPIO_AF_TIM3
#define ENCODER_TIM2_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM2_PH_B_PIN           GPIO_Pin_7
#define ENCODER_TIM2_PH_B_PIN_SOURCE    GPIO_PinSource7
#define ENCODER_TIM2_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM2_PH_B_AF            GPIO_AF_TIM3
#define ENCODER_TIM2_PH_B_CHANNEL       TIM_Channel_2
#endif

// --------------- Encoder 3 ---------------
#define ENCODER_TIM3                    TIM5
#define ENCODER_TIM3_IRQ_NUM            TIM5_IRQn
#define ENCODER_TIM3_CLK                RCC_APB1Periph_TIM5

// encoder 3 GPIO Pin configuration
// PD12
// PD13
#define ENCODER_TIM3_GPIO_PORT          GPIOD
#define ENCODER_TIM3_PH_A_PIN           GPIO_Pin_12
#define ENCODER_TIM3_PH_A_PIN_SOURCE    GPIO_PinSource12
#define ENCODER_TIM3_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOD
#define ENCODER_TIM3_PH_A_AF            GPIO_AF_TIM4
#define ENCODER_TIM3_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM3_PH_B_PIN           GPIO_Pin_13
#define ENCODER_TIM3_PH_B_PIN_SOURCE    GPIO_PinSource13
#define ENCODER_TIM3_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOD
#define ENCODER_TIM3_PH_B_AF            GPIO_AF_TIM4
#define ENCODER_TIM3_PH_B_CHANNEL       TIM_Channel_2

#endif // USE_OPEN_207Z_BOARD || USE_ARM1_REV_0_1 || USE_ARM2_REV_0_1

#if defined(USE_ARM1_REV_0_2)
// --------------- Encoder 1 ---------------
#define ENCODER_TIM1                    TIM2
#define ENCODER_TIM1_IRQ_NUM            TIM2_IRQn
#define ENCODER_TIM1_CLK                RCC_APB1Periph_TIM2
// encoder 1 GPIO Pin configuration
// PA0 PA1
#define ENCODER_TIM1_GPIO_PORT          GPIOA
#define ENCODER_TIM1_PH_A_PIN           GPIO_Pin_0
#define ENCODER_TIM1_PH_A_PIN_SOURCE    GPIO_PinSource0
#define ENCODER_TIM1_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM1_PH_A_AF            GPIO_AF_TIM2
#define ENCODER_TIM1_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM1_PH_B_PIN           GPIO_Pin_1
#define ENCODER_TIM1_PH_B_PIN_SOURCE    GPIO_PinSource1
#define ENCODER_TIM1_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM1_PH_B_AF            GPIO_AF_TIM2
#define ENCODER_TIM1_PH_B_CHANNEL       TIM_Channel_2


// --------------- Encoder 2 ---------------
#define ENCODER_TIM2                    TIM3
#define ENCODER_TIM2_IRQ_NUM            TIM3_IRQn
#define ENCODER_TIM2_CLK                RCC_APB1Periph_TIM3

// encoder 2 GPIO Pin configuration
// PA6 PA7
#define ENCODER_TIM2_GPIO_PORT          GPIOA
#define ENCODER_TIM2_PH_A_PIN           GPIO_Pin_6
#define ENCODER_TIM2_PH_A_PIN_SOURCE    GPIO_PinSource6
#define ENCODER_TIM2_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM2_PH_A_AF            GPIO_AF_TIM3
#define ENCODER_TIM2_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM2_PH_B_PIN           GPIO_Pin_7
#define ENCODER_TIM2_PH_B_PIN_SOURCE    GPIO_PinSource7
#define ENCODER_TIM2_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ENCODER_TIM2_PH_B_AF            GPIO_AF_TIM3
#define ENCODER_TIM2_PH_B_CHANNEL       TIM_Channel_2


// --------------- Encoder 3 ---------------
#define ENCODER_TIM3                    TIM4
#define ENCODER_TIM3_IRQ_NUM            TIM4_IRQn
#define ENCODER_TIM3_CLK                RCC_APB1Periph_TIM4

// encoder 3 GPIO Pin configuration
// PD12
// PD13
#define ENCODER_TIM3_GPIO_PORT          GPIOD
#define ENCODER_TIM3_PH_A_PIN           GPIO_Pin_12
#define ENCODER_TIM3_PH_A_PIN_SOURCE    GPIO_PinSource12
#define ENCODER_TIM3_PH_A_GPIO_CLK      RCC_AHB1Periph_GPIOD
#define ENCODER_TIM3_PH_A_AF            GPIO_AF_TIM4
#define ENCODER_TIM3_PH_A_CHANNEL       TIM_Channel_1

#define ENCODER_TIM3_PH_B_PIN           GPIO_Pin_13
#define ENCODER_TIM3_PH_B_PIN_SOURCE    GPIO_PinSource13
#define ENCODER_TIM3_PH_B_GPIO_CLK      RCC_AHB1Periph_GPIOD
#define ENCODER_TIM3_PH_B_AF            GPIO_AF_TIM4
#define ENCODER_TIM3_PH_B_CHANNEL       TIM_Channel_2

#endif /* USE_ARM1_REV_0_2 */



/* Private variables ---------------------------------------------------------*/
static TIM_TypeDef*  ENCODER_TIM_NUM[ENCODER_NUM]       = {ENCODER_TIM1, ENCODER_TIM2, ENCODER_TIM3};

static u32           ENCODER_TIM_CLK[ENCODER_NUM]       = {ENCODER_TIM1_CLK, ENCODER_TIM2_CLK, ENCODER_TIM3_CLK};

static GPIO_TypeDef* ENCODER_TIM_PORT[ENCODER_NUM]      = {ENCODER_TIM1_GPIO_PORT, ENCODER_TIM2_GPIO_PORT, ENCODER_TIM3_GPIO_PORT};

static u8           ENCODER_TIM_CHANNEL[2][ENCODER_NUM] = {
                                                           /* A */
                                                           {ENCODER_TIM1_PH_A_CHANNEL, ENCODER_TIM2_PH_A_CHANNEL, ENCODER_TIM3_PH_A_CHANNEL},
                                                           /* B */
                                                           {ENCODER_TIM1_PH_B_CHANNEL, ENCODER_TIM2_PH_B_CHANNEL, ENCODER_TIM3_PH_B_CHANNEL}};

static const u16     ENCODER_PIN[2][ENCODER_NUM]        = {
                                                         /* A */
                                                         {ENCODER_TIM1_PH_A_PIN, ENCODER_TIM2_PH_A_PIN, ENCODER_TIM3_PH_A_PIN},
                                                         /* B */
                                                         {ENCODER_TIM1_PH_B_PIN, ENCODER_TIM2_PH_B_PIN, ENCODER_TIM3_PH_B_PIN}
                                                          };

static const u8 ENCODER_PIN_SOURCE[2][ENCODER_NUM]      = {
                                                            /* A */
                                                            {ENCODER_TIM1_PH_A_PIN_SOURCE, ENCODER_TIM2_PH_A_PIN_SOURCE, ENCODER_TIM3_PH_A_PIN_SOURCE},
                                                            /* B */
                                                            {ENCODER_TIM1_PH_B_PIN_SOURCE, ENCODER_TIM2_PH_B_PIN_SOURCE, ENCODER_TIM3_PH_B_PIN_SOURCE}
                                                            };

static const u8      ENCODER_AF[2][ENCODER_NUM]         = {
                                                            /* A */
                                                            {ENCODER_TIM1_PH_A_AF, ENCODER_TIM2_PH_A_AF, ENCODER_TIM3_PH_A_AF},
                                                            /* B */
                                                            {ENCODER_TIM1_PH_B_AF, ENCODER_TIM2_PH_B_AF, ENCODER_TIM3_PH_B_AF}
                                                            };

static const u8 ENCODER_GPIO_CLK[2][ENCODER_NUM]        = {
                                                            /* A */
                                                            {ENCODER_TIM1_PH_A_GPIO_CLK, ENCODER_TIM2_PH_A_GPIO_CLK, ENCODER_TIM3_PH_A_GPIO_CLK},
                                                            /* B */
                                                            {ENCODER_TIM1_PH_B_GPIO_CLK, ENCODER_TIM2_PH_B_GPIO_CLK, ENCODER_TIM3_PH_B_GPIO_CLK}
                                                            };



/* Private functions ---------------------------------------------------------*/
static void clear_speed_buf(EncoderType encoder);
static s32 ENC_get_electrical_angle(EncoderType encoder);


/* Private variables ---------------------------------------------------------*/
static vs32 speed_buf[ENCODER_NUM][SP_BUF_SIZE + 1];
static float speed_val[ENCODER_NUM] = {0, 0, 0};
static vu32 g_last_count[ENCODER_NUM] = {0, 0, 0};

static void clear_speed_buf(EncoderType encoder)
{
    int i;

    assert_param(IS_ENCODER_TYPE(encoder));

    for (i = 0; i < SP_BUF_SIZE; i++)
    {
        speed_buf[encoder][i] = 0;
    }

    speed_val[encoder] = 0;
}




/*
 * Quadrature Encoder Timer configuration
 * Note: Only TIM1 TIM8 and TIM2-TIM5 have encoder function
 *
 */
void QET_configuration(EncoderType encoder)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef       TIM_ICInitStructure;
    //NVIC_InitTypeDef        NVIC_InitStructure;

    assert_param(IS_ENCODER_TYPE(encoder));

    // A B GPIO
    RCC_AHB1PeriphClockCmd(ENCODER_GPIO_CLK[0][encoder] | ENCODER_GPIO_CLK[1][encoder], ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = ENCODER_PIN[0][encoder] | ENCODER_PIN[1][encoder];
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(ENCODER_TIM_PORT[encoder], &GPIO_InitStructure);
    GPIO_PinAFConfig(ENCODER_TIM_PORT[encoder], ENCODER_PIN_SOURCE[0][encoder], ENCODER_AF[0][encoder]);
    GPIO_PinAFConfig(ENCODER_TIM_PORT[encoder], ENCODER_PIN_SOURCE[1][encoder], ENCODER_AF[1][encoder]);

    //Timer configuration in Encoder mode
    TIM_DeInit(ENCODER_TIM_NUM[encoder]);

    if (ENCODER_TIM_NUM[encoder] == TIM1 ||
        ENCODER_TIM_NUM[encoder] == TIM8)
    {
        RCC_APB2PeriphClockCmd(ENCODER_TIM_CLK[encoder], ENABLE);
    }
    else if (ENCODER_TIM_NUM[encoder] == TIM2 ||
             ENCODER_TIM_NUM[encoder] == TIM3 ||
             ENCODER_TIM_NUM[encoder] == TIM4 ||
             ENCODER_TIM_NUM[encoder] == TIM5)
    {
        RCC_APB1PeriphClockCmd(ENCODER_TIM_CLK[encoder], ENABLE);
    }
    else
    {
        // other timers have no quadrature encoder
        assert_param(0);
    }

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER_TIM_NUM[encoder], &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(ENCODER_TIM_NUM[encoder], TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel     = ENCODER_TIM_CHANNEL[0][encoder];
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICFilter    = ICx_FILTER;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInit(ENCODER_TIM_NUM[encoder], &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel     = ENCODER_TIM_CHANNEL[1][encoder];
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICFilter    = ICx_FILTER;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInit(ENCODER_TIM_NUM[encoder], &TIM_ICInitStructure);

    //Clear all pending interrupts
    TIM_ClearFlag(ENCODER_TIM_NUM[encoder], TIM_FLAG_Update);
    TIM_ITConfig(ENCODER_TIM_NUM[encoder], TIM_IT_Update, ENABLE);
    TIM_SetCounter(ENCODER_TIM_NUM[encoder], COUNTER_RESET);
    TIM_Cmd(ENCODER_TIM_NUM[encoder], ENABLE);
    clear_speed_buf(encoder);

}



static s32 ENC_get_electrical_angle(EncoderType encoder)
{
    u32         curCount;
    s32         dAngle;

    assert_param(IS_ENCODER_TYPE(encoder));

    switch (encoder)
    {
        case ENCODER_NUM1:
            curCount = ENCODER_TIM1->CNT;
            break;
        case ENCODER_NUM2:
            curCount = ENCODER_TIM2->CNT;
            break;
        case ENCODER_NUM3:
            curCount = ENCODER_TIM3->CNT;
            break;
    }

    dAngle = curCount - g_last_count[encoder];
    if (dAngle >= MAX_COUNT)
    {
        dAngle -= ENCODER_TIM_PERIOD;
    }
    else if (dAngle < -MAX_COUNT)
    {
        dAngle += ENCODER_TIM_PERIOD;
    }

    g_last_count[encoder] = curCount;

    return dAngle;
}


/**
  * for debug
  *
  */
u32 get_encoder_count(EncoderType encoder)
{
    assert_param(IS_ENCODER_TYPE(encoder));

    return ENCODER_TIM_NUM[encoder]->CNT;
}


/*
 * reset speed buffer
 */
void encoder_reset(EncoderType encoder)
{
    assert_param(IS_ENCODER_TYPE(encoder));

    clear_speed_buf(encoder);
    g_last_count[encoder] = 0;

    switch (encoder)
    {
        case ENCODER_NUM1:
            TIM_SetCounter(ENCODER_TIM1, COUNTER_RESET);
            break;
        case ENCODER_NUM2:
            TIM_SetCounter(ENCODER_TIM2, COUNTER_RESET);
            break;
        case ENCODER_NUM3:
            TIM_SetCounter(ENCODER_TIM3, COUNTER_RESET);
            break;
    }
}



/*
 * @brief: This function read encoder timer number and computer speed in RPM
 * @param: encoder, encoder number
 * @param: time_interval, call this function interval time, 
 *         100 microsecond unit (100uS)
 * @return: speed in RPM
 */
float get_dc_motor_speed(EncoderType encoder, u16 time_interval, s32* distance)
{
    s32 temp = 0;

    assert_param(IS_ENCODER_TYPE(encoder));

    temp = ENC_get_electrical_angle(encoder);
    *distance = temp;

    // Here we calculated once a 100 microsecond, so
    //
    //  temp/(4*PPR)        x
    // -------------- = ----------
    //   100*n(uS)         60s
    //
    // =====> x = temp*60000/(4*PPR*n)   (RPM)
    //512*4

#if 0
    speed_buf[encoder][SP_BUF_SIZE] = temp * 150000/(time_interval * ENCODER_PPR);

    // software filter
    for (i = 0; i < SP_BUF_SIZE; i++)
    {
        speed_buf[encoder][i] = speed_buf[encoder][i + 1];
        sum += speed_buf[encoder][i];
    }
    speed_val[encoder] = sum/SP_BUF_SIZE;
#endif

    speed_val[encoder] = temp * 150000.0/(time_interval * ENCODER_PPR);

    return speed_val[encoder];
}

/*
 * @brief: This function is used for other function to get current
 *         DC motor speed
 * @param: encoder number
 * @return DC motor speed, RPM
 */
s32 get_current_dc_motor_speed(EncoderType encoder)
{
    assert_param(IS_ENCODER_TYPE(encoder));

    return speed_val[encoder];
}



//-------------------------------------------
// *END*
//-------------------------------------------


