/**
  ******************************************************************************
  * @file    exti_board1.c
  * @author  su
  * @version V0.0.1
  * @date    2013-11-14
  * @brief   board1 external interrupt HAL functions.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

#ifndef _EXTI_BOARD1_C_
#define _EXTI_BOARD1_C_

/* Includes ------------------------------------------------------------------*/
#include "exti_board1.h"

/** @addtogroup
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/



/*
 *==========================================================================
 * ARM1 has 3 external interrupt lines:
 * PD3 AC_BUBBLE
 * PD4 LIQUIDLEVEL_HIGH
 * PD5 LIQUIDLEVEL_LOW
 *==========================================================================
 */

#define SWITCH_SENSOR_NUM                       6

#define DRAWBLOODPUMP_HALL_PIN                  GPIO_Pin_0
#define DRAWBLOODPUMP_HALL_PORT                 GPIOD
#define DRAWBLOODPUMP_HALL_CLK                  RCC_AHB1Periph_GPIOD
#define DRAWBLOODPUMP_HALL_TRIGGERTYP           EXTI_Trigger_Falling


#define ACPUMP_HALL_PIN                         GPIO_Pin_1
#define ACPUMP_HALL_PORT                        GPIOD
#define ACPUMP_HALL_CLK                         RCC_AHB1Periph_GPIOD
#define ACPUMP_HALL_TRIGGERTYP                  EXTI_Trigger_Falling


#define RETRANSFUSIONPUMP_HALL_PIN              GPIO_Pin_2
#define RETRANSFUSIONPUMP_HALL_PORT             GPIOD
#define RETRANSFUSIONPUMP_HALL_CLK              RCC_AHB1Periph_GPIOD
#define RETRANSFUSIONPUMP_HALL_TRIGGERTYP       EXTI_Trigger_Falling


#define AC_BUBBLE_PIN                           GPIO_Pin_3
#define AC_BUBBLE_PORT                          GPIOD
#define AC_BUBBLE_CLK                           RCC_AHB1Periph_GPIOD
#define AC_BUBBLE_TRIGGERTYP                    EXTI_Trigger_Falling


#define LIQUIDLEVEL_HIGH_PIN                    GPIO_Pin_4
#define LIQUIDLEVEL_HIGH_PORT                   GPIOD
#define LIQUIDLEVEL_HIGH_CLK                    RCC_AHB1Periph_GPIOD
//#define LIQUIDLEVEL_HIGH_TRIGGERTYP             EXTI_Trigger_Falling
#define LIQUIDLEVEL_HIGH_TRIGGERTYP             EXTI_Trigger_Rising_Falling

#define LIQUIDLEVEL_LOW_PIN                     GPIO_Pin_5
#define LIQUIDLEVEL_LOW_PORT                    GPIOD
#define LIQUIDLEVEL_LOW_CLK                     RCC_AHB1Periph_GPIOD
//#define LIQUIDLEVEL_LOW_TRIGGERTYP              EXTI_Trigger_Falling
#define LIQUIDLEVEL_LOW_TRIGGERTYP              EXTI_Trigger_Rising_Falling


static GPIO_TypeDef* SWITCH_SENSOR_PORT[SWITCH_SENSOR_NUM] = { DRAWBLOODPUMP_HALL_PORT,
                                                               ACPUMP_HALL_PORT,
                                                               RETRANSFUSIONPUMP_HALL_PORT,
                                                               AC_BUBBLE_PORT,
                                                               LIQUIDLEVEL_HIGH_PORT,
                                                               LIQUIDLEVEL_LOW_PORT};
static const uint16_t SWITCH_SENSOR_PIN[SWITCH_SENSOR_NUM] = { DRAWBLOODPUMP_HALL_PIN,
                                                               ACPUMP_HALL_PIN,
                                                               RETRANSFUSIONPUMP_HALL_PIN,
                                                               AC_BUBBLE_PIN,
                                                               LIQUIDLEVEL_HIGH_PIN,
                                                               LIQUIDLEVEL_LOW_PIN};
static const uint32_t SWITCH_SENSOR_CLK[SWITCH_SENSOR_NUM] = { DRAWBLOODPUMP_HALL_CLK,
                                                               ACPUMP_HALL_CLK,
                                                               RETRANSFUSIONPUMP_HALL_CLK,
                                                               AC_BUBBLE_CLK,
                                                               LIQUIDLEVEL_HIGH_CLK,
                                                               LIQUIDLEVEL_LOW_CLK};


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

USER_TMR *drawbloodpump_hall,*acpump_hall,*retransfusionpump_hall,
         *ac_bubble_sensor,*liquidlevel_high_sensor,*liquidlevel_low_sensor;
volatile u8 arm1_actsensor_type;


/* Private functions ---------------------------------------------------------*/

void sensor_gpio_init(EXTI_Board1_TypeDef EXTI_SENSORx);

void ac_pump_hall_NVIC_config(void);
void retransfusion_pump_hall_NVIC_config(void);
void draw_blood_pump_hall_NVIC_config(void);
void ac_bubble_NVIC_config(void) ;
void liquidlevel_high_NVIC_config(void) ;
void liquidlevel_low_NVIC_config(void) ;

static void exti1_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority);



/* callback function*/

void draw_blood_pump_hall_callback(USER_TMR *ptmr, void *p_arg);
void ac_pump_hall_callback(USER_TMR *ptmr, void *p_arg);
void retransfusion_pump_hall_callback(USER_TMR *ptmr, void *p_arg);
void ac_bubble_callback(USER_TMR *ptmr, void *p_arg);
void liquidlevel_high_callback(USER_TMR *ptmr, void *p_arg);
void liquidlevel_low_callback(USER_TMR *ptmr, void *p_arg);

/**
  * @brief  This function is EXTI GPIO configuration.
  *         sensor_gpio_init(EXTI_SENSORx)
  * @param  EXTI_SENSORx   you can set EXTI_SENSORx as AC_BUBBLE,LIQUIDLEVEL_HIGH,LIQUIDLEVEL_LOW
  * @retval None
*/

void sensor_gpio_init(EXTI_Board1_TypeDef EXTI_SENSORx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable  clock */
    RCC_AHB1PeriphClockCmd(SWITCH_SENSOR_CLK[EXTI_SENSORx], ENABLE);
    /* Configure which pin as input pull up*/
    GPIO_InitStructure.GPIO_Pin =  SWITCH_SENSOR_PIN[EXTI_SENSORx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SWITCH_SENSOR_PORT[EXTI_SENSORx], &GPIO_InitStructure);
}


void draw_blood_pump_hall_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource0);

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = DRAWBLOODPUMP_HALL_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DRAWBLOODPUMP_HALL_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DRAWBLOODPUMP_HALL_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void ac_pump_hall_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = ACPUMP_HALL_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ACPUMP_HALL_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = ACPUMP_HALL_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void retransfusion_pump_hall_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = RETRANSFUSIONPUMP_HALL_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RETRANSFUSIONPUMP_HALL_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = RETRANSFUSIONPUMP_HALL_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function is ac bubble sensor NVIC configuration.
  *         ac_bubble_NVIC_config()
  * @param  None
  * @retval None
*/

void ac_bubble_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource3);

    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = AC_BUBBLE_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AC_BUBBL_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = AC_BUBBL_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  This function is high liquid level sensor NVIC configuration.
  *         liquidlevel_high_NVIC_config()
  * @param  None
  * @retval None
*/

void liquidlevel_high_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = LIQUIDLEVEL_HIGH_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LIQUIDLEVEL_HIGH_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = LIQUIDLEVEL_HIGH_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  This function is low liquid level sensor NVIC configuration.
  *         liquidlevel_high_NVIC_config()
  * @param  None
  * @retval None
*/

void liquidlevel_low_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource5);

    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = LIQUIDLEVEL_LOW_TRIGGERTYP;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LIQUIDLEVEL_LOW_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = LIQUIDLEVEL_LOW_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  This function can init all external interrupt sensors of arm1.
  *         arm1_exti_gpio_init()
  * @param  None
  * @retval None
*/

void arm1_exti_gpio_init(void)
{

    sensor_gpio_init(DRAWBLOODPUMP_HALL);
    sensor_gpio_init(ACPUMP_HALL);
    sensor_gpio_init(RETRANSFUSIONPUMP_HALL);
    sensor_gpio_init(AC_BUBBLE);
    sensor_gpio_init(LIQUIDLEVEL_HIGH);
    sensor_gpio_init(LIQUIDLEVEL_LOW);

    draw_blood_pump_hall_NVIC_config();
    ac_pump_hall_NVIC_config();
    retransfusion_pump_hall_NVIC_config();
    ac_bubble_NVIC_config();
    //liquidlevel_high_NVIC_config();
    //liquidlevel_low_NVIC_config();
}



/**
  * @brief  This function can init user timers of arm1
  *         init_arm1sensor_tmr()
  * @param  None
  * @retval None
*/

void arm1sensor_tmr_init(void)
{
    INT8U os_err;

    drawbloodpump_hall= user_timer_create(DRAWBLOODPUMP_HALL_DELAY_TIME ,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)draw_blood_pump_hall_callback,
                                       &os_err);

    acpump_hall= user_timer_create(ACPUMP_HALL_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)ac_pump_hall_callback,
                                       &os_err);
    retransfusionpump_hall= user_timer_create(RETRANSFUSIONPUMP_HALL_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)retransfusion_pump_hall_callback,
                                       &os_err);


    ac_bubble_sensor= user_timer_create(AC_BUBBLE_DELAY_TIME ,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)ac_bubble_callback,
                                       &os_err);

    liquidlevel_high_sensor= user_timer_create(LIQUIDLEVEL_HIGH_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)liquidlevel_high_callback,
                                       &os_err);
    liquidlevel_low_sensor= user_timer_create(LIQUIDLEVEL_LOW_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)liquidlevel_low_callback,
                                       &os_err);
}

/*
void read_hall_liquid_level_status(void)
{
    u8 err;
    if(!GPIO_ReadInputDataBit(DRAWBLOODPUMP_HALL_PORT,DRAWBLOODPUMP_HALL_PIN))
    {
        user_timer_start(drawbloodpump_hall, &err);
        assert_param(USER_ERR_NONE == err);
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(DRAWBLOODPUMP_HALL_PORT,DRAWBLOODPUMP_HALL_PIN))
            arm1_sensor_status.pump_init.bit.draw_pump_init = INIT_NOT_FINISH;
    }
    if(!GPIO_ReadInputDataBit(ACPUMP_HALL_PORT,ACPUMP_HALL_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(ACPUMP_HALL_PORT,ACPUMP_HALL_PIN))
            arm1_sensor_status.pump_init.bit.AC_pump_init = INIT_FINISH ;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(ACPUMP_HALL_PORT,ACPUMP_HALL_PIN))
            arm1_sensor_status.pump_init.bit.AC_pump_init = INIT_NOT_FINISH;
    }
    if(!GPIO_ReadInputDataBit(RETRANSFUSIONPUMP_HALL_PORT,RETRANSFUSIONPUMP_HALL_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(RETRANSFUSIONPUMP_HALL_PORT,RETRANSFUSIONPUMP_HALL_PIN))
             arm1_sensor_status.pump_init.bit.feed_back_pump_init = INIT_FINISH ;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(ACPUMP_HALL_PORT,ACPUMP_HALL_PIN))
           arm1_sensor_status.pump_init.bit.feed_back_pump_init = INIT_NOT_FINISH;
    }
}
*/


static void exti1_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority)

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


void draw_blood_pump_hall_callback(USER_TMR *ptmr, void *p_arg)
{
    if (DRAWBLOODPUMP_HALL_TRIGGER == arm1_actsensor_type)
    {
        if(!GPIO_ReadInputDataBit(DRAWBLOODPUMP_HALL_PORT,DRAWBLOODPUMP_HALL_PIN))
        {

            arm1_sensor_status.pump_init.bit.draw_pump_init = INIT_FINISH;
          //  TRACE("DRAWBLOODPUMP_HALL_INIT_FINISH!\r\n");
        }
        else
            arm1_sensor_status.pump_init.bit.draw_pump_init = INIT_NOT_FINISH;
        exti1_irq_operation(ENABLE,0,DRAWBLOODPUMP_HALL_PRE_PRI,DRAWBLOODPUMP_HALL_SUB_PRI);
    }
}

void ac_pump_hall_callback(USER_TMR *ptmr, void *p_arg)
{

    if (ACPUMP_HALL_TRIGGER == arm1_actsensor_type)
    {
        if(!GPIO_ReadInputDataBit(ACPUMP_HALL_PORT,ACPUMP_HALL_PIN))
        {
            arm1_sensor_status.pump_init.bit.AC_pump_init = INIT_FINISH ;
           // TRACE("ACPUMP_HALL INIT_FINISH!\r\n");
        }
        else
            arm1_sensor_status.pump_init.bit.AC_pump_init = INIT_NOT_FINISH;
        exti1_irq_operation(ENABLE,1,ACPUMP_HALL_PRE_PRI,ACPUMP_HALL_SUB_PRI);
    }

}

void retransfusion_pump_hall_callback(USER_TMR *ptmr, void *p_arg)
{
    if (RETRANSFUSIONPUMP_HALL_TRIGGER == arm1_actsensor_type)
    {
        if(!GPIO_ReadInputDataBit(RETRANSFUSIONPUMP_HALL_PORT,RETRANSFUSIONPUMP_HALL_PIN))
        {
            arm1_sensor_status.pump_init.bit.feed_back_pump_init = INIT_FINISH ;
            TRACE("RETRANSFUSIONPUMP_HALL INIT_FINISH!\r\n");
        }
        else
        {
            arm1_sensor_status.pump_init.bit.feed_back_pump_init = INIT_NOT_FINISH;
        }
        exti1_irq_operation(ENABLE,2,RETRANSFUSIONPUMP_HALL_PRE_PRI,RETRANSFUSIONPUMP_HALL_SUB_PRI);
    }
}

/**
  * @brief  write the status of ac bubble,mark whether the AC pipeline have bubble.
  *
  * @param  None
  * @retval None
*/


void ac_bubble_callback(USER_TMR *ptmr, void *p_arg)
{
    u8 err;
    static u8 low_level_cnt = 0;
    static u8 read_cnt = 0;

    read_cnt++;
    if(read_cnt < 15)
    {
        if(!GPIO_ReadInputDataBit(AC_BUBBLE_PORT,AC_BUBBLE_PIN))//for debug,AC_BUBBLE can not remove shake
        {
            low_level_cnt++;
            if(low_level_cnt <10)
            {
                user_timer_start(ac_bubble_sensor, &err);
                assert_param(USER_ERR_NONE == err);
            }
            else
            {
                low_level_cnt = 0;
                read_cnt = 0;
                arm1_sensor_status.liquid_level.bit.AC_bubble = AIR_BUBBLE_PASS;
                exti1_irq_operation(ENABLE,3,AC_BUBBL_PRE_PRI,AC_BUBBL_SUB_PRI);
            }
        }
        else
        {
             exti1_irq_operation(ENABLE,3,AC_BUBBL_PRE_PRI,AC_BUBBL_SUB_PRI);
        }
       TRACE("AC_bubble is %d\r\n",arm1_sensor_status.liquid_level.bit.AC_bubble);
    }
    else
    {
        read_cnt = 0;
        exti1_irq_operation(ENABLE,3,AC_BUBBL_PRE_PRI,AC_BUBBL_SUB_PRI);
    }

}



void clear_ac_bubble_status(void)
{
    arm1_sensor_status.liquid_level.bit.AC_bubble =NONE_AIR_BUBBLE;
}



/**
  * @brief  write the status of high liquid level sensor,mark whether the container is full.
  *
  * @param  None
  * @retval None
*/

void liquidlevel_high_callback(USER_TMR *ptmr, void *p_arg)
{
    if(!GPIO_ReadInputDataBit(LIQUIDLEVEL_HIGH_PORT,LIQUIDLEVEL_HIGH_PIN))
    {
        arm1_sensor_status.liquid_level.bit.high_level = HIGH_LEVEL_TRIGGER;
    }
    else
        arm1_sensor_status.liquid_level.bit.high_level = HIGH_LEVEL_NOT_TRIGGER;
    exti1_irq_operation(ENABLE,4,LIQUIDLEVEL_HIGH_PRE_PRI,LIQUIDLEVEL_HIGH_SUB_PRI);
    
    TRACE("liquid_level high is %d\r\n",arm1_sensor_status.liquid_level.bit.high_level);
}


/**
  * @brief  write the status of low liquid level sensor,mark whether the container is empty.
  *
  * @param  None
  * @retval None
*/

void liquidlevel_low_callback(USER_TMR *ptmr, void *p_arg)
{
    if(!GPIO_ReadInputDataBit(LIQUIDLEVEL_LOW_PORT,LIQUIDLEVEL_LOW_PIN))
    {
        arm1_sensor_status.liquid_level.bit.low_level = LOW_LEVEL_TRIGGER;
    }
    else
        arm1_sensor_status.liquid_level.bit.low_level = LOW_LEVEL_NOT_TRIGGER;
    
    exti1_irq_operation(ENABLE,5,LIQUIDLEVEL_HIGH_PRE_PRI,LIQUIDLEVEL_HIGH_SUB_PRI);
    
    TRACE("liquid_level low is %d\r\n",arm1_sensor_status.liquid_level.bit.low_level);
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
        exti1_irq_operation(DISABLE,0,DRAWBLOODPUMP_HALL_PRE_PRI,DRAWBLOODPUMP_HALL_SUB_PRI);

        arm1_actsensor_type = DRAWBLOODPUMP_HALL_TRIGGER;

        user_timer_start(drawbloodpump_hall, &err);
        assert_param(USER_ERR_NONE == err);

        //TRACE("ARM1 EXTI0_IRQHandler draw pump hall\r\n");
    }
    OSIntExit();
}
/* remove sensor shake*/
void EXTI1_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        /* Clear the EXTI line 1 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line1);
        //lock the irq
        exti1_irq_operation(DISABLE,1,ACPUMP_HALL_PRE_PRI,ACPUMP_HALL_SUB_PRI);

        arm1_actsensor_type = ACPUMP_HALL_TRIGGER;

        user_timer_start(acpump_hall, &err);
        assert_param(USER_ERR_NONE == err);

        //TRACE("ARM1 EXTI1_IRQHandler  AC pump hall\r\n");
    }
    OSIntExit();
}

/* remove sensor shake*/
void EXTI2_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        /* Clear the EXTI line 2 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line2);
        //lock the irq
        exti1_irq_operation(DISABLE,2,RETRANSFUSIONPUMP_HALL_PRE_PRI,RETRANSFUSIONPUMP_HALL_SUB_PRI);

        arm1_actsensor_type = RETRANSFUSIONPUMP_HALL_TRIGGER;

        user_timer_start(retransfusionpump_hall, &err);
        assert_param(USER_ERR_NONE == err);

       // TRACE("ARM1 EXTI2_IRQHandler feedback pump hall\r\n");
    }
    OSIntExit();
}



/* remove sensor shake*/
void EXTI3_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        /* Clear the EXTI line 3 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line3);
        //lock the irq
        exti1_irq_operation(DISABLE,3,AC_BUBBL_PRE_PRI,AC_BUBBL_SUB_PRI);

        arm1_actsensor_type = AC_BUBBLE_TRIGGER;

        user_timer_start(ac_bubble_sensor, &err);
        assert_param(USER_ERR_NONE == err);

       // TRACE("ARM1 EXTI3_IRQHandler AC bubble\r\n");
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
        exti1_irq_operation(DISABLE,4,LIQUIDLEVEL_HIGH_PRE_PRI,LIQUIDLEVEL_HIGH_SUB_PRI);
        user_timer_start(liquidlevel_high_sensor, &err);
        assert_param(USER_ERR_NONE == err);
        TRACE("ARM1 EXTI4_IRQHandler liquid high \r\n");
    }
    OSIntExit();
}

void EXTI9_5_IRQHandler(void)
{
    INT8U err;

    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        /* Clear the EXTI line 5 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line5);

        //lock the irq
        exti1_irq_operation(DISABLE,5,LIQUIDLEVEL_LOW_PRE_PRI,LIQUIDLEVEL_LOW_SUB_PRI);
        arm1_actsensor_type = LIQUIDLEVEL_LOW_TRIGGER;
        user_timer_start(liquidlevel_low_sensor, &err);
        assert_param(USER_ERR_NONE == err);
        TRACE("ARM1 EXTI9_5_IRQHandler---5 liquid low\r\n");
    }
    OSIntExit();
}

#endif/*_EXTI_BOARD1_C_*/

