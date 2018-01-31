/**
  ******************************************************************************
  * @file    exti_board2.c
  * @author  su
  * @version V0.0.1
  * @date    2013-11-14
  * @brief   board2 external interrupt HAL functions.

  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */
#ifndef _EXTI_BOARD2_C_
#define _EXTI_BOARD2_C_

/* Includes ------------------------------------------------------------------*/
#include "exti_board2.h"

/** @addtogroup
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/*
 *==========================================================================
 * ARM2 has 11  external interrupt sensors:
 * PLASMAPUMP_HALL
 * PLTPUMP_HALL
 * PLTVALVE_OPTICAL2
 * PLAVALVE_OPTICAL1
 * PLAVALVE_OPTICAL2
 * PLAVALVE_OPTICAL3
 * RBCVALVE_OPTICAL1
 * RBCVALVE_OPTICAL2
 * RBCVALVE_OPTICAL3
 * PLTVALVE_OPTICAL3
 * PLTVALVE_OPTICAL1
 *==========================================================================
 */

#define SWITCH_SENSOR_NUM               11

#define PLASMAPUMP_HALL_PIN         GPIO_Pin_11
#define PLASMAPUMP_HALL_PORT        GPIOC
#define PLASMAPUMP_HALL_CLK         RCC_AHB1Periph_GPIOC
#define PLASMAPUMP_HALL_TRIGGERTYP  EXTI_Trigger_Falling


#define PLTPUMP_HALL_PIN            GPIO_Pin_12
#define PLTPUMP_HALL_PORT           GPIOC
#define PLTPUMP_HALL_CLK            RCC_AHB1Periph_GPIOC
#define PLTPUMP_HALL_TRIGGERTYP     EXTI_Trigger_Falling

/*PLT middle optical*/
#define PLTVALVE_OPTICAL2_PIN       GPIO_Pin_0
#define PLTVALVE_OPTICAL2_PORT      GPIOD
#define PLTVALVE_OPTICAL2_CLK       RCC_AHB1Periph_GPIOD
#define PLTVALVE_OPTICAL2_TRIGGERTYP  EXTI_Trigger_Rising


/*PLA left optical*/
#define PLAVALVE_OPTICAL1_PIN       GPIO_Pin_1
#define PLAVALVE_OPTICAL1_PORT      GPIOD
#define PLAVALVE_OPTICAL1_CLK       RCC_AHB1Periph_GPIOD
#define PLAVALVE_OPTICAL1_TRIGGERTYP  EXTI_Trigger_Rising


/*PLA middle optical*/
#define PLAVALVE_OPTICAL2_PIN       GPIO_Pin_2
#define PLAVALVE_OPTICAL2_PORT      GPIOD
#define PLAVALVE_OPTICAL2_CLK       RCC_AHB1Periph_GPIOD
#define PLAVALVE_OPTICAL2_TRIGGERTYP  EXTI_Trigger_Rising


/*PLA right optical*/
#define PLAVALVE_OPTICAL3_PIN       GPIO_Pin_3
#define PLAVALVE_OPTICAL3_PORT      GPIOD
#define PLAVALVE_OPTICAL3_CLK       RCC_AHB1Periph_GPIOD
#define PLAVALVE_OPTICAL3_TRIGGERTYP  EXTI_Trigger_Rising

/*RBC left optical*/
#define RBCVALVE_OPTICAL1_PIN       GPIO_Pin_4
#define RBCVALVE_OPTICAL1_PORT      GPIOD
#define RBCVALVE_OPTICAL1_CLK       RCC_AHB1Periph_GPIOD
#define RBCVALVE_OPTICAL1_TRIGGERTYP  EXTI_Trigger_Rising

/*RBC middle optical*/
#define RBCVALVE_OPTICAL2_PIN       GPIO_Pin_5
#define RBCVALVE_OPTICAL2_PORT      GPIOD
#define RBCVALVE_OPTICAL2_CLK       RCC_AHB1Periph_GPIOD
#define RBCVALVE_OPTICAL2_TRIGGERTYP  EXTI_Trigger_Rising

/*RBC right optical*/
#define RBCVALVE_OPTICAL3_PIN       GPIO_Pin_6
#define RBCVALVE_OPTICAL3_PORT      GPIOD
#define RBCVALVE_OPTICAL3_CLK       RCC_AHB1Periph_GPIOD
#define RBCVALVE_OPTICAL3_TRIGGERTYP  EXTI_Trigger_Rising

/*PLT right optical*/
#define PLTVALVE_OPTICAL3_PIN       GPIO_Pin_7
#define PLTVALVE_OPTICAL3_PORT      GPIOD
#define PLTVALVE_OPTICAL3_CLK       RCC_AHB1Periph_GPIOD
#define PLTVALVE_OPTICAL3_TRIGGERTYP  EXTI_Trigger_Rising

/*PLT left optical*/
#define PLTVALVE_OPTICAL1_PIN       GPIO_Pin_8
#define PLTVALVE_OPTICAL1_PORT      GPIOD
#define PLTVALVE_OPTICAL1_CLK       RCC_AHB1Periph_GPIOD
#define PLTVALVE_OPTICAL1_TRIGGERTYP  EXTI_Trigger_Rising


#define OPTICAL_TRIGGER         0X01
#define OPTICAL_NOT_TRIGGER     0X00
#define INIT_FINISH      0X01
#define INIT_NOT_FINISH  0X00

static GPIO_TypeDef* SWITCH_SENSOR_PORT[SWITCH_SENSOR_NUM ] = { PLASMAPUMP_HALL_PORT,
                                                                PLTPUMP_HALL_PORT,
                                                                PLTVALVE_OPTICAL2_PORT,
                                                                PLAVALVE_OPTICAL1_PORT,
                                                                PLAVALVE_OPTICAL2_PORT,
                                                                PLAVALVE_OPTICAL3_PORT,
                                                                RBCVALVE_OPTICAL1_PORT,
                                                                RBCVALVE_OPTICAL2_PORT,
                                                                RBCVALVE_OPTICAL3_PORT,
                                                                PLTVALVE_OPTICAL3_PORT,
                                                                PLTVALVE_OPTICAL1_PORT};

static const uint16_t SWITCH_SENSOR_PIN[SWITCH_SENSOR_NUM] = {  PLASMAPUMP_HALL_PIN,
                                                                PLTPUMP_HALL_PIN,
                                                                PLTVALVE_OPTICAL2_PIN,
                                                                PLAVALVE_OPTICAL1_PIN,
                                                                PLAVALVE_OPTICAL2_PIN,
                                                                PLAVALVE_OPTICAL3_PIN,
                                                                RBCVALVE_OPTICAL1_PIN,
                                                                RBCVALVE_OPTICAL2_PIN,
                                                                RBCVALVE_OPTICAL3_PIN,
                                                                PLTVALVE_OPTICAL3_PIN,
                                                                PLTVALVE_OPTICAL1_PIN};

static const uint32_t SWITCH_SENSOR_CLK[SWITCH_SENSOR_NUM] = {  PLASMAPUMP_HALL_CLK,
                                                                PLTPUMP_HALL_CLK,
                                                                PLTVALVE_OPTICAL2_CLK,
                                                                PLAVALVE_OPTICAL1_CLK,
                                                                PLAVALVE_OPTICAL2_CLK,
                                                                PLAVALVE_OPTICAL3_CLK,
                                                                RBCVALVE_OPTICAL1_CLK,
                                                                RBCVALVE_OPTICAL2_CLK,
                                                                RBCVALVE_OPTICAL3_CLK,
                                                                PLTVALVE_OPTICAL3_CLK,
                                                                PLTVALVE_OPTICAL1_CLK};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USER_TMR *PLTvalve_optical2,*PLAvalve_optical1,*PLAvalve_optical2,
         *PLAvalve_optical3,*RBCvalve_optical1,*RBCvalve_optical2,
         *RBCvalve_optical3,*PLTvalve_optical3,*PLTvalve_optical1,
         *PLAsmapump_hall,*PLTpump_hall;

volatile u8 arm2_actsensor_type;

/* Private functions ---------------------------------------------------------*/
void sensor_gpio_init(EXTI_Board2_TypeDef EXTI_SENSORx);

/* callback functions*/
void PLAsmapump_hall_callback(USER_TMR *ptmr, void *p_arg);
void PLTpump_hall_callback(USER_TMR *ptmr, void *p_arg);
void PLTvalve_optical2_callback(USER_TMR *ptmr, void *p_arg);
void PLAvalve_optical1_callback(USER_TMR *ptmr, void *p_arg);
void PLAvalve_optical2_callback(USER_TMR *ptmr, void *p_arg);
void PLAvalve_optical3_callback(USER_TMR *ptmr, void *p_arg);
void RBCvalve_optical1_callback(USER_TMR *ptmr, void *p_arg);
void RBCvalve_optical2_callback(USER_TMR *ptmr, void *p_arg);
void RBCvalve_optical3_callback(USER_TMR *ptmr, void *p_arg);
void PLTvalve_optical3_callback(USER_TMR *ptmr, void *p_arg);
void PLTvalve_optical1_callback(USER_TMR *ptmr, void *p_arg);


static void PLA_pump_hall_NVIC_config(void);
static void PLT_pump_hall_NVIC_config(void);
static void PLTvalve_optical2_NVIC_config(void) ;
static void PLAvalve_optical1_NVIC_config(void) ;
static void PLAvalve_optical2_NVIC_config(void) ;
static void PLAvalve_optical3_NVIC_config(void) ;
static void RBCvalve_optical1_NVIC_config(void) ;
static void RBCvalve_optical2_NVIC_config(void) ;
static void RBCvalve_optical3_NVIC_config(void) ;
static void PLTvalve_optical3_NVIC_config(void) ;
static void PLTvalve_optical1_NVIC_config(void) ;

static void exti2_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority);


/**
  * @brief  This function is EXTI GPIO configuration.
  *         sensor_gpio_init(EXTI_SENSORx)
  * @param  EXTI_SENSORx   you can set EXTI_SENSORx as
  *                        PLASMAPUMP_HALL    PLTPUMP_HALL       PLTVALVE_OPTICAL2
  *                        PLAVALVE_OPTICAL1  PLAVALVE_OPTICAL2  PLAVALVE_OPTICAL3
  *                        RBCVALVE_OPTICAL1  RBCVALVE_OPTICAL2  RBCVALVE_OPTICAL3
  *                        PLTVALVE_OPTICAL3  PLTVALVE_OPTICAL1
  * @retval None
*/

void sensor_gpio_init(EXTI_Board2_TypeDef EXTI_SENSORx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable ADC clock */
    RCC_AHB1PeriphClockCmd(SWITCH_SENSOR_CLK[EXTI_SENSORx], ENABLE);
    /* Configure which pin  as analog input */
    GPIO_InitStructure.GPIO_Pin =  SWITCH_SENSOR_PIN[EXTI_SENSORx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SWITCH_SENSOR_PORT[EXTI_SENSORx], &GPIO_InitStructure);
}


/**
  * @brief  This function is PLA pump hall sensor NVIC configuration.
  *         PLA_pump_hall_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLA_pump_hall_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);

    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLASMAPUMP_HALL_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLASMAPUMP_HALL_PRE_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLASMAPUMP_HALL_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function is PLT pump hall sensor NVIC configuration.
  *         PLT_pump_hall_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLT_pump_hall_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);

    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLTPUMP_HALL_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLTPUMP_HALL_PRE_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLTPUMP_HALL_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function is PLT middle optical sensor NVIC configuration.
  *         PLTvalve_optical2_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLTvalve_optical2_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource0);

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLTVALVE_OPTICAL2_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLTVALVE_OPTICAL2_PRE_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLTVALVE_OPTICAL2_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is PLA left optical sensor NVIC configuration.
  *         PLAvalve_optical1_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLAvalve_optical1_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLAVALVE_OPTICAL1_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLAVALVE_OPTICAL1_PRE_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLAVALVE_OPTICAL1_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is PLA middle optical sensor NVIC configuration.
  *         PLAvalve_optical2_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLAvalve_optical2_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLAVALVE_OPTICAL2_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLAVALVE_OPTICAL2_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLAVALVE_OPTICAL2_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is PLA right optical sensor NVIC configuration.
  *         PLAvalve_optical3_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLAvalve_optical3_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource3);

    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLAVALVE_OPTICAL3_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLAVALVE_OPTICAL3_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLAVALVE_OPTICAL3_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is rbc valve left optical sensor NVIC configuration.
  *         RBCvalve_optical1_NVIC_config()
  * @param  None
  * @retval None
*/

static void RBCvalve_optical1_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = RBCVALVE_OPTICAL1_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RBCVALVE_OPTICAL1_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = RBCVALVE_OPTICAL1_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is rbc valve middle optical sensor NVIC configuration.
  *         RBCvalve_optical2_NVIC_config()
  * @param  None
  * @retval None
*/

static void RBCvalve_optical2_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource5);

    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = RBCVALVE_OPTICAL2_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RBCVALVE_OPTICAL2_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = RBCVALVE_OPTICAL2_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is rbc valve right optical sensor NVIC configuration.
  *         RBCvalve_optical3_NVIC_config()
  * @param  None
  * @retval None
*/

static void RBCvalve_optical3_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);

    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = RBCVALVE_OPTICAL3_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RBCVALVE_OPTICAL3_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = RBCVALVE_OPTICAL3_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function is PLT valve right optical sensor NVIC configuration.
  *         PLTvalve_optical3_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLTvalve_optical3_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource7);

    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLTVALVE_OPTICAL3_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLTVALVE_OPTICAL3_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLTVALVE_OPTICAL3_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function is PLT valve left optical sensor NVIC configuration.
  *         PLTvalve_optical1_NVIC_config()
  * @param  None
  * @retval None
*/

static void PLTvalve_optical1_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource8);

    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = PLTVALVE_OPTICAL1_TRIGGERTYP ;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PLTVALVE_OPTICAL1_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = PLTVALVE_OPTICAL1_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  This function can init all external interrupt sensors of arm2.
  *         arm2_exti_gpio_init()
  * @param  None
  * @retval None
*/

void arm2_exti_gpio_init(void)
{
    sensor_gpio_init(PLASMAPUMP_HALL);
    sensor_gpio_init(PLTPUMP_HALL);
    sensor_gpio_init(PLTVALVE_OPTICAL2);
    sensor_gpio_init(PLAVALVE_OPTICAL1);
    sensor_gpio_init(PLAVALVE_OPTICAL2);
    sensor_gpio_init(PLAVALVE_OPTICAL3);
    sensor_gpio_init(RBCVALVE_OPTICAL1);
    sensor_gpio_init(RBCVALVE_OPTICAL2);
    sensor_gpio_init(RBCVALVE_OPTICAL3);
    sensor_gpio_init(PLTVALVE_OPTICAL3);
    sensor_gpio_init(PLTVALVE_OPTICAL1);

    PLA_pump_hall_NVIC_config();
    PLT_pump_hall_NVIC_config();
    PLTvalve_optical2_NVIC_config();
    PLAvalve_optical1_NVIC_config();
    PLAvalve_optical2_NVIC_config();
    PLAvalve_optical3_NVIC_config();
    RBCvalve_optical1_NVIC_config();
    RBCvalve_optical2_NVIC_config();
    RBCvalve_optical3_NVIC_config();
    PLTvalve_optical3_NVIC_config();
    PLTvalve_optical1_NVIC_config();
}

/**
  * @brief  This function can init user timers of arm2
  *         init_arm2sensor_tmr()
  * @param  None
  * @retval None
*/

void init_arm2sensor_tmr(void)
{
    INT8U os_err;

    PLTvalve_optical3= user_timer_create(PLTVALVE_OPTICAL3_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLTvalve_optical3_callback,
                                       &os_err);
    PLTvalve_optical1= user_timer_create(PLTVALVE_OPTICAL1_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLTvalve_optical1_callback,
                                       &os_err);

    PLTvalve_optical2= user_timer_create(PLTVALVE_OPTICAL2_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLTvalve_optical2_callback,
                                       &os_err);

    PLAvalve_optical1= user_timer_create(PLAVALVE_OPTICAL1_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLAvalve_optical1_callback,
                                       &os_err);

    PLAvalve_optical2= user_timer_create(PLAVALVE_OPTICAL2_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLAvalve_optical2_callback,
                                       &os_err);

    PLAvalve_optical3= user_timer_create(PLAVALVE_OPTICAL3_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLAvalve_optical3_callback,
                                       &os_err);
    RBCvalve_optical1= user_timer_create(RBCVALVE_OPTICAL1_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK) RBCvalve_optical1_callback,
                                       &os_err);

    RBCvalve_optical2= user_timer_create(RBCVALVE_OPTICAL2_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK) RBCvalve_optical2_callback,
                                       &os_err);
    RBCvalve_optical3= user_timer_create(RBCVALVE_OPTICAL3_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK) RBCvalve_optical3_callback,
                                       &os_err);

    PLAsmapump_hall = user_timer_create(PLASMAPUMP_HALL_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLAsmapump_hall_callback,
                                       &os_err);
    PLTpump_hall = user_timer_create(PLTPUMP_HALL_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)PLTpump_hall_callback,
                                       &os_err);
}
// TODO: jiulong debug 2014.8.2
#if 1
/**
  * @brief  write the status of PLAsmapump hall,mark whether the plasma pump finish init.
  *
  * @param  None
  * @retval None
*/

void read_optical_hall_status_arm2(void)
{
    if(!GPIO_ReadInputDataBit(PLASMAPUMP_HALL_PORT,PLASMAPUMP_HALL_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLASMAPUMP_HALL_PORT,PLASMAPUMP_HALL_PIN))
            arm2_sensor_status.pump_init.bit.plasma_pump_init = INIT_FINISH ;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLASMAPUMP_HALL_PORT,PLASMAPUMP_HALL_PIN))
            arm2_sensor_status.pump_init.bit.plasma_pump_init = INIT_NOT_FINISH ;
    }
    if(!GPIO_ReadInputDataBit(PLTPUMP_HALL_PORT,PLTPUMP_HALL_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLTPUMP_HALL_PORT,PLTPUMP_HALL_PIN))
            arm2_sensor_status.pump_init.bit.PLT_pump_init = INIT_FINISH ;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLTPUMP_HALL_PORT,PLTPUMP_HALL_PIN))
            arm2_sensor_status.pump_init.bit.PLT_pump_init = INIT_NOT_FINISH ;
    }



    if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL2_PORT,PLTVALVE_OPTICAL2_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL2_PORT,PLTVALVE_OPTICAL2_PIN))
            arm2_sensor_status.valve.bit.PLT_valve_mid = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLTVALVE_OPTICAL2_PORT,PLTVALVE_OPTICAL2_PIN))
            arm2_sensor_status.valve.bit.PLT_valve_mid = OPTICAL_TRIGGER;
    }

    if(!GPIO_ReadInputDataBit(PLAVALVE_OPTICAL1_PORT,PLAVALVE_OPTICAL1_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLAVALVE_OPTICAL1_PORT,PLAVALVE_OPTICAL1_PIN))
           arm2_sensor_status.valve.bit.plsama_valve_left = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLAVALVE_OPTICAL1_PORT,PLAVALVE_OPTICAL1_PIN))
            arm2_sensor_status.valve.bit.plsama_valve_left = OPTICAL_TRIGGER;

    }
    if(!GPIO_ReadInputDataBit(PLAVALVE_OPTICAL2_PORT,PLAVALVE_OPTICAL2_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLAVALVE_OPTICAL2_PORT,PLAVALVE_OPTICAL2_PIN))
           arm2_sensor_status.valve.bit.plsama_valve_mid = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLAVALVE_OPTICAL2_PORT,PLAVALVE_OPTICAL2_PIN))
            arm2_sensor_status.valve.bit.plsama_valve_mid = OPTICAL_TRIGGER;
    }

    if(!GPIO_ReadInputDataBit(PLAVALVE_OPTICAL3_PORT,PLAVALVE_OPTICAL3_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLAVALVE_OPTICAL3_PORT,PLAVALVE_OPTICAL3_PIN))
           arm2_sensor_status.valve.bit.plsama_valve_right = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLAVALVE_OPTICAL3_PORT,PLAVALVE_OPTICAL3_PIN))
            arm2_sensor_status.valve.bit.plsama_valve_right = OPTICAL_TRIGGER;
    }

    if(!GPIO_ReadInputDataBit(RBCVALVE_OPTICAL1_PORT,RBCVALVE_OPTICAL1_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(RBCVALVE_OPTICAL1_PORT,RBCVALVE_OPTICAL1_PIN))
            arm2_sensor_status.valve.bit.RBC_valve_left = OPTICAL_NOT_TRIGGER;
    }
    else//IO=1
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(RBCVALVE_OPTICAL1_PORT,RBCVALVE_OPTICAL1_PIN))
            arm2_sensor_status.valve.bit.RBC_valve_left = OPTICAL_TRIGGER;
    }

    if(!GPIO_ReadInputDataBit(RBCVALVE_OPTICAL2_PORT,RBCVALVE_OPTICAL2_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(RBCVALVE_OPTICAL2_PORT,RBCVALVE_OPTICAL2_PIN))
            arm2_sensor_status.valve.bit.RBC_valve_mid = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(RBCVALVE_OPTICAL2_PORT,RBCVALVE_OPTICAL2_PIN))
            arm2_sensor_status.valve.bit.RBC_valve_mid = OPTICAL_TRIGGER;
    }

    if(!GPIO_ReadInputDataBit(RBCVALVE_OPTICAL3_PORT,RBCVALVE_OPTICAL3_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(RBCVALVE_OPTICAL3_PORT,RBCVALVE_OPTICAL3_PIN))
            arm2_sensor_status.valve.bit.RBC_valve_right = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(RBCVALVE_OPTICAL3_PORT,RBCVALVE_OPTICAL3_PIN))
        arm2_sensor_status.valve.bit.RBC_valve_right = OPTICAL_TRIGGER;

    }
    if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL3_PORT,PLTVALVE_OPTICAL3_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL3_PORT,PLTVALVE_OPTICAL3_PIN))
            arm2_sensor_status.valve.bit.PLT_valve_right = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLTVALVE_OPTICAL3_PORT,PLTVALVE_OPTICAL3_PIN))
            arm2_sensor_status.valve.bit.PLT_valve_right = OPTICAL_TRIGGER;
    }


    if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL1_PORT,PLTVALVE_OPTICAL1_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL1_PORT,PLTVALVE_OPTICAL1_PIN))
            arm2_sensor_status.valve.bit.PLT_valve_left = OPTICAL_NOT_TRIGGER;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(PLTVALVE_OPTICAL1_PORT,PLTVALVE_OPTICAL1_PIN))
            arm2_sensor_status.valve.bit.PLT_valve_left = OPTICAL_TRIGGER;
    }

}
#endif

// TODO: jiulong debug 2014.8.2


void PLAsmapump_hall_callback(USER_TMR *ptmr, void *p_arg)
{
    if(!GPIO_ReadInputDataBit(PLASMAPUMP_HALL_PORT,PLASMAPUMP_HALL_PIN))
    {
        arm2_sensor_status.pump_init.bit.plasma_pump_init = INIT_FINISH ;
        
        TRACE("plasma_pump_init\r\n");
    }
    else
    {
        arm2_sensor_status.pump_init.bit.plasma_pump_init = INIT_NOT_FINISH;
    }
    exti2_irq_operation(ENABLE,11,PLASMAPUMP_HALL_PRE_PRI,PLASMAPUMP_HALL_SUB_PRI);
}


/**
  * @brief  write the status of PLT pump hall,mark whether the sensor is triggered.
  *
  * @param  None
  * @retval None
*/

void PLTpump_hall_callback(USER_TMR *ptmr, void *p_arg)
{
    if(!GPIO_ReadInputDataBit(PLTVALVE_OPTICAL2_PORT,PLTVALVE_OPTICAL2_PIN))
    {
        arm2_sensor_status.pump_init.bit.PLT_pump_init = INIT_FINISH ;
     //   TRACE("PLT_pump_init\r\n");
    }
    else
    {
        arm2_sensor_status.pump_init.bit.PLT_pump_init  = INIT_NOT_FINISH;
    }
    exti2_irq_operation(ENABLE,11,PLASMAPUMP_HALL_PRE_PRI,PLASMAPUMP_HALL_SUB_PRI);

}

/**
  * @brief  write the status of PLA valve left optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void PLTvalve_optical2_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(PLTVALVE_OPTICAL2_PORT,PLTVALVE_OPTICAL2_PIN))
    {
        if(PLTVALVE_MIDDLE_OPTICAL_FLAG)
        {
            /*stop the plt valve*/
            stop_valve(PLTVALVE) ;
        //    TRACE("stop_PLTVALVE2");
        }
        else
        {
            //返回光电传感器出错
        }
         /*clear the flag while optical will be trigger*/
        PLTVALVE_MIDDLE_OPTICAL_FLAG = 0;
     }
    exti2_irq_operation(ENABLE,0,PLTVALVE_OPTICAL2_PRE_PRI,PLTVALVE_OPTICAL2_SUB_PRI);
  //  TRACE("PLTvalve_optical2_callback!\r\n");

}

/**
  * @brief  write the status of PLA valve left optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void PLAvalve_optical1_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(PLAVALVE_OPTICAL1_PORT,PLAVALVE_OPTICAL1_PIN))
    {
        if(PLAVALVE_LEFT_OPTICAL_FLAG)
        {
            /*stop the plasma valve*/
            stop_valve(PLAVALVE) ;
         //   TRACE("stop_PLAVALVE1\r\n");
            
        }
        else
        {
            //返回光电传感器出错
        }
         /*clear the flag while optical will be trigger*/
        PLAVALVE_LEFT_OPTICAL_FLAG = 0;
    }
    exti2_irq_operation(ENABLE,1,PLAVALVE_OPTICAL1_PRE_PRI,PLAVALVE_OPTICAL1_SUB_PRI);
   // TRACE("PLAvalve_optical1_callback\r\n");
}
/**
  * @brief  write the status of PLA valve middle optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void PLAvalve_optical2_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(PLAVALVE_OPTICAL2_PORT,PLAVALVE_OPTICAL2_PIN))
    {
        if(PLAVALVE_MIDDLE_OPTICAL_FLAG)
        {
            /*stop the plasma valve*/
            stop_valve(PLAVALVE) ;
         //   TRACE("stop_PLAVALVE2\r\n");
         }
        else
        {
            //返回光电传感器出错
        }
        /*clear the flag while optical will be trigger*/
        PLAVALVE_MIDDLE_OPTICAL_FLAG = 0;
    }
    exti2_irq_operation(ENABLE,2,PLAVALVE_OPTICAL2_PRE_PRI,PLAVALVE_OPTICAL2_SUB_PRI);        
  //  TRACE("PLAvalve_optical2_callback!\r\n");
}
/**
  * @brief  write the status of PLA valve right optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void PLAvalve_optical3_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(PLAVALVE_OPTICAL3_PORT,PLAVALVE_OPTICAL3_PIN))
    {
        if(PLAVALVE_RIGHT_OPTICAL_FLAG)
        {
            /*stop the plasma valve*/
            stop_valve(PLAVALVE) ;
         //   TRACE("stop_PLAVALVE3\r\n");
        }
        else
        {
            //返回光电传感器出错
        }
        /*clear the flag while optical will be trigger*/
        PLAVALVE_RIGHT_OPTICAL_FLAG = 0;
    }
    exti2_irq_operation(ENABLE,3,PLAVALVE_OPTICAL3_PRE_PRI,PLAVALVE_OPTICAL3_SUB_PRI);
    //TRACE("PLAvalve_optical3_callback!\r\n");
}
/**
  * @brief  write the status of RBC valve left optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void RBCvalve_optical1_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(RBCVALVE_OPTICAL1_PORT,RBCVALVE_OPTICAL1_PIN))
    {
        if(RBCVALVE_LEFT_OPTICAL_FLAG)
        {
            /*stop the red blood corpuscle valve*/
            stop_valve(RBCVALVE) ;
            TRACE("stop_RBCvalve1\r\n");
            
        }
        else
        {
            //返回光电传感器出错
        }
        /*clear the flag while optical will be trigger*/
        RBCVALVE_LEFT_OPTICAL_FLAG = 0;
        exti2_irq_operation(ENABLE,4,RBCVALVE_OPTICAL1_PRE_PRI,RBCVALVE_OPTICAL1_SUB_PRI);
    }
   // TRACE("RBCvalve_optical1_callback!\r\n");
}
/**
  * @brief  write the status of RBC valve middle optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void RBCvalve_optical2_callback(USER_TMR *ptmr, void *p_arg)
{
        if(GPIO_ReadInputDataBit(RBCVALVE_OPTICAL2_PORT,RBCVALVE_OPTICAL2_PIN))
        {
            if(RBCVALVE_MIDDLE_OPTICAL_FLAG)
            {
                /*stop the red blood corpuscle valve*/
                stop_valve(RBCVALVE) ;
                TRACE("stop_RBCvalve2\r\n");
            }
            else
            {
                //返回光电传感器出错
            }
            /*clear the flag while optical will be trigger*/
            RBCVALVE_MIDDLE_OPTICAL_FLAG = 0;
        }
        exti2_irq_operation(ENABLE,5,RBCVALVE_OPTICAL2_PRE_PRI,RBCVALVE_OPTICAL2_SUB_PRI);
       // TRACE("RBCvalve_optical2_callback!\r\n");

}
/**
  * @brief  write the status of RBC valve right optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void RBCvalve_optical3_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(RBCVALVE_OPTICAL3_PORT,RBCVALVE_OPTICAL3_PIN))
    {
        if(RBCVALVE_RIGHT_OPTICAL_FLAG)
        {
            /*stop the red blood corpuscle valve*/
            stop_valve(RBCVALVE) ;
            TRACE("stop_RBCvalve3\r\n");
        }
        else
        {
            //返回光电传感器出错
        }
        /*clear the flag while optical will be trigger*/
        RBCVALVE_RIGHT_OPTICAL_FLAG = 0;
    }
    exti2_irq_operation(ENABLE,6,RBCVALVE_OPTICAL3_PRE_PRI,RBCVALVE_OPTICAL3_SUB_PRI);
   // TRACE("RBCvalve_optical3_callback!\r\n");
}
/**
  * @brief  write the status of PLT valve right optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/



void PLTvalve_optical3_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(PLTVALVE_OPTICAL3_PORT,PLTVALVE_OPTICAL3_PIN))
    {
        if(PLTVALVE_RIGHT_OPTICAL_FLAG)
        {
           /*stop the blood platelet valve*/
            stop_valve(PLTVALVE) ;
            TRACE("stop_ PLTvalve3\r\n");
         }
        else
        {
            //返回光电传感器出错
        }
        /*clear the flag while optical will be trigger*/
        PLTVALVE_RIGHT_OPTICAL_FLAG = 0;
    }
    exti2_irq_operation(ENABLE,7,PLTVALVE_OPTICAL3_PRE_PRI,PLTVALVE_OPTICAL3_SUB_PRI);
  //  TRACE("PLTvalve_optical3_callback!\r\n");
}
/**
  * @brief  write the status of PLT valve left optical,mark whether the optical is triggered.
  *
  * @param  None
  * @retval None
*/

void PLTvalve_optical1_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(PLTVALVE_OPTICAL1_PORT,PLTVALVE_OPTICAL1_PIN))
    {
        if(PLTVALVE_LEFT_OPTICAL_FLAG)
        {
           /*stop the blood platelet valve*/
            stop_valve(PLTVALVE) ;
            TRACE("stop_PLTVALVE1!\r\n");
        }
        else
        {
            //返回光电传感器出错
        }
        /*clear the flag while optical will be trigger*/
        PLTVALVE_LEFT_OPTICAL_FLAG = 0;

    }
  //  TRACE("PLTvalve_optical1_callback!\r\n");
    exti2_irq_operation(ENABLE,8,PLTVALVE_OPTICAL1_PRE_PRI,PLTVALVE_OPTICAL1_SUB_PRI);
    
}

static void exti2_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority)

{
    NVIC_InitTypeDef   NVIC_InitStructure;
    uint32_t    EXTI_Linex;
    IRQn_Type   EXTIx_IRQn;
    switch(line_num)
    {
        case 0:
            EXTI_Linex = EXTI_Line0;
            EXTIx_IRQn = EXTI0_IRQn;
            break;
        case 1:
            EXTI_Linex = EXTI_Line1;
            EXTIx_IRQn = EXTI1_IRQn;
            break;
        case 2:
            EXTI_Linex = EXTI_Line2;
            EXTIx_IRQn = EXTI2_IRQn;
            break;
        case 3:
            EXTI_Linex = EXTI_Line3;
            EXTIx_IRQn = EXTI3_IRQn;
            break;
        case 4:
            EXTI_Linex = EXTI_Line4;
            EXTIx_IRQn = EXTI4_IRQn;
            break;
        case 5:
            EXTI_Linex = EXTI_Line5;
            EXTIx_IRQn = EXTI9_5_IRQn;
            break;
        case 6:
            EXTI_Linex = EXTI_Line6;
            EXTIx_IRQn = EXTI9_5_IRQn;
            break;
        case 7:
            EXTI_Linex = EXTI_Line7;
            EXTIx_IRQn = EXTI9_5_IRQn;
            break;
        case 8:
            EXTI_Linex = EXTI_Line8;
            EXTIx_IRQn = EXTI9_5_IRQn;
            break;
        case 9:
            EXTI_Linex = EXTI_Line9;
            EXTIx_IRQn = EXTI9_5_IRQn;
            break;
        case 10:
            EXTI_Linex = EXTI_Line10;
            EXTIx_IRQn = EXTI15_10_IRQn;
            break;
        case 11:
            EXTI_Linex = EXTI_Line11;
            EXTIx_IRQn = EXTI15_10_IRQn;
            break;
        case 12:
            EXTI_Linex = EXTI_Line12;
            EXTIx_IRQn = EXTI15_10_IRQn;
            break;
        default:
            break;
    }

    EXTI_ClearFlag(EXTI_Linex);
    NVIC_InitStructure.NVIC_IRQChannel = EXTIx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = P_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_Priority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;
    NVIC_Init(&NVIC_InitStructure);
}


/* remove sensor shake*/
void EXTI0_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        /* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);
        //lock the irq
        exti2_irq_operation(DISABLE,0,PLTVALVE_OPTICAL2_PRE_PRI,PLTVALVE_OPTICAL2_SUB_PRI);

        arm2_actsensor_type = PLTVALVE_OPTICAL2_TRIGGER;
        user_timer_start(PLTvalve_optical2, &err);
        assert_param(USER_ERR_NONE == err);
       // TRACE("PLTvalve_optical2 \r\n");
    }
    OSIntExit();
}

void EXTI1_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        /* Clear the EXTI line 1 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line1);
        //lock the irq

        exti2_irq_operation(DISABLE,1,PLAVALVE_OPTICAL1_PRE_PRI,PLAVALVE_OPTICAL1_SUB_PRI);

        arm2_actsensor_type = PLAVALVE_OPTICAL1_TRIGGER;
        user_timer_start(PLAvalve_optical1, &err);
        assert_param(USER_ERR_NONE == err);
      //  TRACE("PLAvalve_optical1\r\n");
    }
    OSIntExit();
}

void EXTI2_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        /* Clear the EXTI line 2 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line2);
        //lock the irq

        exti2_irq_operation(DISABLE,2,PLAVALVE_OPTICAL2_PRE_PRI,PLAVALVE_OPTICAL2_SUB_PRI);

        arm2_actsensor_type = PLAVALVE_OPTICAL2_TRIGGER;
        user_timer_start(PLAvalve_optical2, &err);
        assert_param(USER_ERR_NONE == err);
        
       // TRACE("PLAvalve_optical2\r\n");
    }
    OSIntExit();
}

void EXTI3_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        /* Clear the EXTI line 3 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line3);
        //lock the irq
        exti2_irq_operation(DISABLE,3,PLAVALVE_OPTICAL3_PRE_PRI,PLAVALVE_OPTICAL3_SUB_PRI);
        arm2_actsensor_type = PLAVALVE_OPTICAL3_TRIGGER;
        user_timer_start(PLAvalve_optical3, &err);
        assert_param(USER_ERR_NONE == err);
      //  TRACE("PLAvalve_optical3\r\n");

    }
    OSIntExit();
}

void EXTI4_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        /* Clear the EXTI line 4 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line4);
        //lock the irq
        exti2_irq_operation(DISABLE,4,RBCVALVE_OPTICAL1_PRE_PRI,RBCVALVE_OPTICAL1_SUB_PRI);
        arm2_actsensor_type = RBCVALVE_OPTICAL1_TRIGGER;
        user_timer_start(RBCvalve_optical1, &err);
        assert_param(USER_ERR_NONE == err);
      //  TRACE("RBCVALVE_OPTICAL1\r\n");
    }
    OSIntExit();
}

void EXTI9_5_IRQHandler(void)
{
    INT8U err;
   // TRACE("59 ");
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        /* Clear the EXTI line 5 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line5);
        //lock the irq
        exti2_irq_operation(DISABLE,5,RBCVALVE_OPTICAL2_PRE_PRI,RBCVALVE_OPTICAL2_SUB_PRI);
        arm2_actsensor_type = RBCVALVE_OPTICAL2_TRIGGER;
        user_timer_start(RBCvalve_optical2, &err);
        assert_param(USER_ERR_NONE == err);
      //  TRACE("RBCvalve_optical2\r\n");//add by wq
    }
    else if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        /* Clear the EXTI line 6 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line6);
        //lock the irq
        exti2_irq_operation(DISABLE,6,RBCVALVE_OPTICAL3_PRE_PRI,RBCVALVE_OPTICAL3_SUB_PRI);
        arm2_actsensor_type = RBCVALVE_OPTICAL3_TRIGGER;
        user_timer_start(RBCvalve_optical3, &err);
        assert_param(USER_ERR_NONE == err);
      // TRACE("RBCvalve_optical3\r\n");//add by wq
    }
     else if(EXTI_GetITStatus(EXTI_Line7) != RESET)
     {
        /* Clear the EXTI line 7 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line7);
        //lock the irq
        exti2_irq_operation(DISABLE,7,PLTVALVE_OPTICAL3_PRE_PRI,PLTVALVE_OPTICAL3_SUB_PRI);
        user_timer_start(PLTvalve_optical3, &err);
        assert_param(USER_ERR_NONE == err);

       // TRACE("PLTvalve_optical3\r\n");
      }
     else if(EXTI_GetITStatus(EXTI_Line8) != RESET)
     {
        /* Clear the EXTI line 8 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line8);
        //lock the irq
        exti2_irq_operation(DISABLE,8,PLTVALVE_OPTICAL1_PRE_PRI,PLTVALVE_OPTICAL1_SUB_PRI);
        user_timer_start(PLTvalve_optical1, &err);
        assert_param(USER_ERR_NONE == err);
       // TRACE("PLTvalve_optical1\r\n");
      }
    OSIntExit();
}

void EXTI15_10_IRQHandler(void)
{
    INT8U err;
    OSIntEnter();
    //TRACE("222\r\n");
    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        /* Clear the EXTI line 11 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line11);
        //lock the irq
        exti2_irq_operation(DISABLE,11,PLASMAPUMP_HALL_PRE_PRI,PLASMAPUMP_HALL_SUB_PRI);
        arm2_actsensor_type = PLASMAPUMP_HALL_TRIGGER;
        user_timer_start(PLAsmapump_hall,&err);
        assert_param(USER_ERR_NONE == err);
      //  TRACE("PLASMA_PUMP_HALL_11\r\n");
    }
    else if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
       // TRACE("111\r\n");
        /* Clear the EXTI line 12 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line12);
        //lock the irq
        exti2_irq_operation(DISABLE,12,PLTPUMP_HALL_PRE_PRI,PLTPUMP_HALL_SUB_PRI);
        arm2_actsensor_type = PLTPUMP_HALL_TRIGGER;
        user_timer_start(PLTpump_hall, &err);
        assert_param(USER_ERR_NONE == err);
     //   TRACE("PLT_PUMP_HALL_IRQHandler-12\r\n");
    }
    OSIntExit();
}


#endif /*_EXTI_BOARD2_C_*/

