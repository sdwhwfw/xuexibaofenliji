/**
  ******************************************************************************
  * @file    exti_board3.c
  * @author  su
  * @version V0.0.1
  * @date    2013-11-14
  * @brief   board3 external interrupt HAL functions.

  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */
#ifndef _EXTI_BOARD3_C_
#define _EXTI_BOARD3_C_

/* Includes ------------------------------------------------------------------*/
#include "exti_board3.h"

/** @addtogroup
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*
 *==========================================================================
 * ARM3 has 2 external interrupt lines :
 * PD2  kasedo motor photoswitch 1
 * PD3  kasedo motor photoswitch 2
 *==========================================================================
 */

#define SWITCH_SENSOR_NUM                   2

#define KASEDO_OPTICAL1_PIN                 GPIO_Pin_1
#define KASEDO_OPTICAL1_PORT                GPIOD
#define KASEDO_OPTICAL1_CLK                 RCC_AHB1Periph_GPIOD
#define KASEDO_OPTICAL1_Trigger             EXTI_Trigger_Rising

#define KASEDO_OPTICAL2_PIN                 GPIO_Pin_2
#define KASEDO_OPTICAL2_PORT                GPIOD
#define KASEDO_OPTICAL2_CLK                 RCC_AHB1Periph_GPIOD
#define KASEDO_OPTICAL2_Trigger             EXTI_Trigger_Falling

#define UP_LOCATION      0x01
#define DOWN_LOCATION     0x02
#define UNKNOW_LOCATION   0x00


static GPIO_TypeDef* SWITCH_SENSOR_PORT[SWITCH_SENSOR_NUM] = { KASEDO_OPTICAL1_PORT,
                                                               KASEDO_OPTICAL2_PORT};
static const uint16_t SWITCH_SENSOR_PIN[SWITCH_SENSOR_NUM] = { KASEDO_OPTICAL1_PIN,
                                                               KASEDO_OPTICAL2_PIN};
static const uint32_t SWITCH_SENSOR_CLK[SWITCH_SENSOR_NUM] = { KASEDO_OPTICAL1_CLK,
                                                               KASEDO_OPTICAL2_CLK};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USER_TMR *kasedo_optical1,*kasedo_optical2,*kasedo_optical_status;

/* */
volatile u8 arm3_actsensor_type;

/* callback function*/
void sensor_gpio_init(EXTI_Board3_TypeDef EXTI_SENSORx);
void kasedo_optical1_NVIC_config(void) ;
void kasedo_optical2_NVIC_config(void) ;
void kasedo_optical1_callback(USER_TMR *ptmr, void *p_arg);
void kasedo_optical2_callback(USER_TMR *ptmr, void *p_arg);
void kasedo_optical_status_callback(USER_TMR *ptmr, void *p_arg);

void exti3_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority);

/**
  * @brief  This function is EXTI GPIO configuration.
  *         sensor_gpio_init(EXTI_SENSORx)
  * @param  EXTI_SENSORx   you can set EXTI_SENSORx as AC_BUBBLE,LIQUIDLEVEL_HIGH,LIQUIDLEVEL_LOW
  * @retval None
*/
void sensor_gpio_init(EXTI_Board3_TypeDef EXTI_SENSORx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable  clock */
    RCC_AHB1PeriphClockCmd(SWITCH_SENSOR_CLK[EXTI_SENSORx], ENABLE);
    /* Configure which pin as input pull up*/
    GPIO_InitStructure.GPIO_Pin =  SWITCH_SENSOR_PIN[EXTI_SENSORx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SWITCH_SENSOR_PORT[EXTI_SENSORx], &GPIO_InitStructure);
}
/**
  * @brief  This function is kasedo up(high) optical sensor NVIC configuration.
  *         kasedo_optical1_NVIC_config()
  * @param  None
  * @retval None
*/
void kasedo_optical1_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = KASEDO_OPTICAL1_Trigger;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = KASEDO_OPTICAL1_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = KASEDO_OPTICAL1_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    APP_TRACE("kasedo_optical1_NVIC_config!\r\n");
}
/**
  * @brief  This function is kasedo down(low) optical sensor NVIC configuration.
  *         kasedo_optical2_NVIC_config()
  * @param  None
  * @retval None
*/
void kasedo_optical2_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = KASEDO_OPTICAL2_Trigger;
    EXTI_Init(&EXTI_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = KASEDO_OPTICAL2_PRE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = KASEDO_OPTICAL2_SUB_PRI ;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function can init all external interrupt sensors of arm3.
  *         arm3_exti_gpio_init()
  * @param  None
  * @retval None
*/

void arm3_exti_gpio_init(void)
{
    //for debug
    sensor_gpio_init(KASEDO_OPTICAL1);
    sensor_gpio_init(KASEDO_OPTICAL2);
    kasedo_optical1_NVIC_config();
    kasedo_optical2_NVIC_config();

	exti3_irq_operation(DISABLE,1,KASEDO_OPTICAL1_PRE_PRI,KASEDO_OPTICAL1_SUB_PRI);
	exti3_irq_operation(DISABLE,2,KASEDO_OPTICAL2_PRE_PRI,KASEDO_OPTICAL2_SUB_PRI);

	
}

/**
  * @brief  This function can init user timers of arm3
  *         init_arm3sensor_tmr()
  * @param  None
  * @retval None
*/

void init_arm3sensor_tmr(void)
{
    INT8U os_err;
    kasedo_optical1 = user_timer_create(KASEDO_OPTICAL1_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)kasedo_optical1_callback,
                                       &os_err);

    kasedo_optical2 = user_timer_create(KASEDO_OPTICAL2_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)kasedo_optical2_callback,
                                       &os_err);
    kasedo_optical_status = user_timer_create(KASEDO_OPTICAL_STATUS_DELAY_TIME,
                                       0,
                                       USER_TMR_OPT_ONE_SHOT,
                                       (void*)0,
                                       (OS_TMR_CALLBACK)kasedo_optical_status_callback,
                                       &os_err);
}


void read_optical_status_arm3(void)
{
    u8 err;
    if(GPIO_ReadInputDataBit(KASEDO_OPTICAL1_PORT,KASEDO_OPTICAL1_PIN) &&
       GPIO_ReadInputDataBit(KASEDO_OPTICAL2_PORT,KASEDO_OPTICAL2_PIN))
    {
        user_timer_start(kasedo_optical2, &err);
        assert_param(USER_ERR_NONE == err);
    }
}
/**
  * @brief  write the status of kasedo down(low) optical,mark whether the plasma pump finish init.
  *
  * @param  None
  * @retval None
*/

void kasedo_optical1_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(KASEDO_OPTICAL1_PORT,KASEDO_OPTICAL1_PIN))
    {
        TRACE("CASSETTE_DOWN_OPTICAL_FLAG= %d \r\n",CASSETTE_DOWN_OPTICAL_FLAG);
        if(CASSETTE_DOWN_OPTICAL_FLAG)
       // if(1)
        {
            //for debug
           stop_cassette();
           arm3_sensor_status.sensor.bit.cassette_location = DOWN_LOCATION;
           
           TRACE("The cassette down optical is triggered!\r\n");
        }
        else
        {
            stop_cassette();
            //返回光电传感器出错
            TRACE("error,the cassette down optical is triggered,but the commanded optical is not this one!\r\n");
        }
        CASSETTE_DOWN_OPTICAL_FLAG=0;
    }
    else//add  by wq
    {
        stop_cassette();//add by wq
        TRACE("kasedo_optical1_callback IO is missed\r\n");
    }

   // exti3_irq_operation(ENABLE,1,KASEDO_OPTICAL1_PRE_PRI,KASEDO_OPTICAL1_SUB_PRI);

}
/**
  * @brief  write the status of kasedo up(high) optical,mark whether the plasma pump finish init.
  *
  * @param  None
  * @retval None
*/
void kasedo_optical2_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(KASEDO_OPTICAL2_PORT,KASEDO_OPTICAL2_PIN))
    {
        TRACE("CASSETTE_UP_OPTICAL_FLAG= %d \r\n",CASSETTE_UP_OPTICAL_FLAG);
        if(CASSETTE_UP_OPTICAL_FLAG)
       // if(1)
        {
            stop_cassette();
            TRACE("The cassette up optical is triggered!\r\n");
            //for debug
            arm3_sensor_status.sensor.bit.cassette_location = UP_LOCATION;
        }
        else
        {
            stop_cassette();
            //返回光电传感器出错
            TRACE("error,the cassette up optical is triggered,but the commanded optical is not this one!\r\n");
        }
        CASSETTE_UP_OPTICAL_FLAG=0;
    }
    else//add by wq
    {
        TRACE("kasedo_optical2_callback IO is missed\r\n");
        stop_cassette();
    }
    // exti3_irq_operation(ENABLE,2,KASEDO_OPTICAL2_PRE_PRI,KASEDO_OPTICAL2_SUB_PRI);
}

void kasedo_optical_status_callback(USER_TMR *ptmr, void *p_arg)
{
    if(GPIO_ReadInputDataBit(KASEDO_OPTICAL1_PORT,KASEDO_OPTICAL1_PIN) &&
       GPIO_ReadInputDataBit(KASEDO_OPTICAL2_PORT,KASEDO_OPTICAL2_PIN))
    {
        arm3_sensor_status.sensor.bit.cassette_location = UNKNOW_LOCATION;
    }
}


 void exti3_irq_operation(FunctionalState state,u8 line_num,uint8_t P_Priority,uint8_t sub_Priority)

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

/* remove sensor shake*/
void EXTI1_IRQHandler(void)
{
    INT8U err;
    OSIntEnter();
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        /* Clear the EXTI line 1 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line1);
        exti3_irq_operation(DISABLE,1,KASEDO_OPTICAL1_PRE_PRI,KASEDO_OPTICAL1_SUB_PRI);
 //       arm3_actsensor_type = KASEDO_OPTICAL1_ACT;
        user_timer_start(kasedo_optical1, &err);
        assert_param(USER_ERR_NONE == err);

       //up positon  add by wq
       stop_cassette();
       // start_cassette(downward);
       
        TRACE("KASEDO_OPTICAL1 EXTI1_IRQHandler!\r\n");
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
        exti3_irq_operation(DISABLE,2,KASEDO_OPTICAL2_PRE_PRI,KASEDO_OPTICAL2_SUB_PRI);
//        arm3_actsensor_type = KASEDO_OPTICAL2_ACT;
        user_timer_start(kasedo_optical2, &err);
        assert_param(USER_ERR_NONE == err);

       //down positon add by wq
      stop_cassette();
     //  start_cassette(upward);


        TRACE("KASEDO_OPTICAL2 EXTI2_IRQHandler!\r\n");
    }

    OSIntExit();
}
#endif/*_EXTI_BOARD3_C_*/

