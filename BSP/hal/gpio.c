/**
  ******************************************************************************
  * @file    gpio.c
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-15
  * @brief   GPIO HAL functions.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "gpio.h"

/*
 *==========================================================================
 * Open207V has four LEDs: LED1 (PC6)  LED2 (PC7)  LED3 (PC8)  LED4 (PC9)
 * Open207Z has four LEDs: LED1 (PF6)  LED2 (PF7)  LED3 (PF8)  LED4 (PF9)
 *==========================================================================
 */

#if defined(USE_OPEN_207Z_BOARD)

#define LEDn                        4

#define LED1_PIN                    GPIO_Pin_6
#define LED1_GPIO_PORT              GPIOF
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED2_PIN                    GPIO_Pin_7
#define LED2_GPIO_PORT              GPIOF
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED3_PIN                    GPIO_Pin_8
#define LED3_GPIO_PORT              GPIOF
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED4_PIN                    GPIO_Pin_9
#define LED4_GPIO_PORT              GPIOF
#define LED4_GPIO_CLK               RCC_AHB1Periph_GPIOF
#elif defined(USE_PORT_207Z_BOARD)

#define LEDn                        4

#define LED1_PIN                    GPIO_Pin_4
#define LED1_GPIO_PORT              GPIOA
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define LED2_PIN                    GPIO_Pin_5
#define LED2_GPIO_PORT              GPIOA
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define LED3_PIN                    GPIO_Pin_6
#define LED3_GPIO_PORT              GPIOA
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define LED4_PIN                    GPIO_Pin_7
#define LED4_GPIO_PORT              GPIOA
#define LED4_GPIO_CLK               RCC_AHB1Periph_GPIOA

#elif defined(USE_OPEN_207V_BOARD)

#define LEDn                        4

#define LED1_PIN                    GPIO_Pin_6
#define LED1_GPIO_PORT              GPIOF
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED2_PIN                    GPIO_Pin_7
#define LED2_GPIO_PORT              GPIOF
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED3_PIN                    GPIO_Pin_8
#define LED3_GPIO_PORT              GPIOF
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED4_PIN                    GPIO_Pin_9
#define LED4_GPIO_PORT              GPIOF
#define LED4_GPIO_CLK               RCC_AHB1Periph_GPIOF

#elif defined(USE_ARM0_REV_0_1)
#define LEDn                        3

#define LED1_PIN                    GPIO_Pin_0
#define LED1_GPIO_PORT              GPIOB
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define LED2_PIN                    GPIO_Pin_1
#define LED2_GPIO_PORT              GPIOB
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define LED3_PIN                    GPIO_Pin_2
#define LED3_GPIO_PORT              GPIOB
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOB

#elif defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2) || \
      defined(USE_ARM2_REV_0_1) || defined(USE_ARM3_REV_0_1)
#define LEDn                        3

#define LED1_PIN                    GPIO_Pin_12
#define LED1_GPIO_PORT              GPIOE
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOE

#define LED2_PIN                    GPIO_Pin_13
#define LED2_GPIO_PORT              GPIOE
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOE

#define LED3_PIN                    GPIO_Pin_14
#define LED3_GPIO_PORT              GPIOE
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOE


#elif defined(NO_RTOS_BOARD)

#define LEDn                        4

#define LED1_PIN                    GPIO_Pin_6
#define LED1_GPIO_PORT              GPIOF
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED2_PIN                    GPIO_Pin_7
#define LED2_GPIO_PORT              GPIOF
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED3_PIN                    GPIO_Pin_8
#define LED3_GPIO_PORT              GPIOF
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define LED4_PIN                    GPIO_Pin_9
#define LED4_GPIO_PORT              GPIOF
#define LED4_GPIO_CLK               RCC_AHB1Periph_GPIOF

#elif defined(USE_OPEN_207Z_MINI_BOARD)

#define LEDn                        4

#define LED1_PIN                    GPIO_Pin_4
#define LED1_GPIO_PORT              GPIOA
#define LED1_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define LED2_PIN                    GPIO_Pin_5
#define LED2_GPIO_PORT              GPIOA
#define LED2_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define LED3_PIN                    GPIO_Pin_6
#define LED3_GPIO_PORT              GPIOA
#define LED3_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define LED4_PIN                    GPIO_Pin_7
#define LED4_GPIO_PORT              GPIOA
#define LED4_GPIO_CLK               RCC_AHB1Periph_GPIOA

#endif

#if defined(USE_PORT_207Z_BOARD) || defined(USE_OPEN_207Z_BOARD) || defined(NO_RTOS_BOARD) || defined(USE_OPEN_207Z_MINI_BOARD)
static GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT,
                                 		LED4_GPIO_PORT};
static const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN,
                                 		LED4_PIN};
static const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK,
                                 		LED4_GPIO_CLK};

#elif defined(USE_ARM0_REV_0_1) || defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2) || \
      defined(USE_ARM2_REV_0_1) || defined(USE_ARM3_REV_0_1)
static GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT};
static const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN};
static const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK};
#endif





/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void init_led_gpio(Led_TypeDef Led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(GPIO_CLK[Led], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}


#if defined(USE_ARM0_REV_0_1) || defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2) || \
    defined(USE_ARM2_REV_0_1) || defined(USE_ARM3_REV_0_1)
/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void led_trun_on(Led_TypeDef Led)
{
	GPIO_PORT[Led]->BSRRH = GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void led_trun_off(Led_TypeDef Led)
{
	GPIO_PORT[Led]->BSRRL = GPIO_PIN[Led];
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */

#else
/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void led_trun_on(Led_TypeDef Led)
{
	GPIO_PORT[Led]->BSRRL = GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void led_trun_off(Led_TypeDef Led)
{
	GPIO_PORT[Led]->BSRRH = GPIO_PIN[Led];
}

#endif

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void led_toggle(Led_TypeDef Led)
{
	GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}


/*-----------------------------------
 * USER KEY    (PB1)
 * WAKEUP KEY  (PA0)
 *
 */
void user_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the PB1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


#if defined(CONTROL_MAIN_BOARD)
void init_pause_key(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    EXTI_InitTypeDef    EXTI_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable the PB13 Interrupt */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);

    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PAUSE_KEY_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PAUSE_KEY_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void init_stop_key(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    EXTI_InitTypeDef    EXTI_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable the PB12 Interrupt */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);

    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = STOP_KEY_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = STOP_KEY_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
/*
 * STOP KEY         (PB12)
 * PAUSE/RESUME KEY (PB13)
 * ALARM LED        (PB14)
 * ALARM BUZZER     (PB15)
 */
void init_arm0_exter_gpio(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

	/* Enable the Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure the Alarm LED & Alarm BUZZER */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // init pause key
    init_pause_key();
    // init stop key
    init_stop_key();

    user_key_init();
}

void alarm_led_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void alarm_led_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void alarm_buzzer_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void alarm_buzzer_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

u8 get_alarm_status(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
}

#endif


/* power gate control only for arm3*/
#ifdef SLAVE_BOARD_3

/*
PD8 =>gate1
PD9 => gate2
*/


void power_gate_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the power_gate Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void disable_power_gate(GATE_TypeDef gate)
{
   assert_param(IS_GATE_TYPEDEF(gate));

    if(gate == GATE1)
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_8);
    }
    else
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_9);
    }

}

void enable_power_gate(GATE_TypeDef gate)
{
    assert_param(IS_GATE_TYPEDEF(gate));

    if(gate == GATE1)
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_8);
    }
    else
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_9);
    }
}



#endif




