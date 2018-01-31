/**
  ******************************************************************************
  * @file    switch_value_scan.c
  * @author  su
  * @version V0.0.1
  * @date    2013-11-24
  * @brief
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */
#ifndef _SWITCH_VALUE_SCAN_C_
#define _SWITCH_VALUE_SCAN_C_

/* Includes ------------------------------------------------------------------*/
#include "switch_value_scan.h"


/** @addtogroup
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/*
 *==========================================================================
 * ARM1
 * PD6 RETRANSFUSIONPUMP_FEEDBACK
 * ARM3
 * PA0  liquid leakage sensor
 * PD0  electromagnet   photoswitch 1
 * PD1  electromagnet   photoswitch 2
 * PD2  kasedo motor photoswitch 1
 * PD3  kasedo motor photoswitch 2
 * PD4  door hall switch

 *==========================================================================
 */

#define SCAN_SENSOR_ARM1_NUM               1

#define RETRANSFUSIONPUMP_FEEDBACK_PIN      GPIO_Pin_6
#define RETRANSFUSIONPUMP_FEEDBACK_PORT     GPIOD
#define RETRANSFUSIONPUMP_FEEDBACK_CLK      RCC_AHB1Periph_GPIOD

#define SCAN_SENSOR_ARM3_NUM                   5

#define LIQUIDLEAKAGE_SENSOR_PIN            GPIO_Pin_0
#define LIQUIDLEAKAGE_SENSOR_PORT           GPIOA
#define LIQUIDLEAKAGE_SENSOR_CLK            RCC_AHB1Periph_GPIOA

#define ELECTROMAGNET_OPTICAL1_PIN          GPIO_Pin_3
#define ELECTROMAGNET_OPTICAL1_PORT         GPIOD
#define ELECTROMAGNET_OPTICAL1_CLK          RCC_AHB1Periph_GPIOD

#define ELECTROMAGNET_OPTICAL2_PIN          GPIO_Pin_4
#define ELECTROMAGNET_OPTICAL2_PORT         GPIOD
#define ELECTROMAGNET_OPTICAL2_CLK          RCC_AHB1Periph_GPIOD

#define DOOR_HALL_PIN                       GPIO_Pin_5
#define DOOR_HALL_PORT                      GPIOD
#define DOOR_HALL_CLK                       RCC_AHB1Periph_GPIOD

#define CENTRIFUGE_FAULT_PIN                GPIO_Pin_0
#define CENTRIFUGE_FAULT_PORT               GPIOD
#define CENTRIFUGE_FAULT_CLK                RCC_AHB1Periph_GPIOD

#define NORMAL 1
#define ABNORMAL 0

#define LOCKED 1
#define NOT_LOCKED 0

#define TRIGGER  1
#define NOT_TRIGGER  0

static GPIO_TypeDef* SCAN_SENSOR_ARM1_PORT[SCAN_SENSOR_ARM1_NUM] = { RETRANSFUSIONPUMP_FEEDBACK_PORT};
static const uint16_t SCAN_SENSOR_ARM1_PIN[SCAN_SENSOR_ARM1_NUM] = { RETRANSFUSIONPUMP_FEEDBACK_PIN};
static const uint32_t SCAN_SENSOR_ARM1_CLK[SCAN_SENSOR_ARM1_NUM] = { RETRANSFUSIONPUMP_FEEDBACK_CLK};



static GPIO_TypeDef* SCAN_SENSOR_ARM3_PORT[SCAN_SENSOR_ARM3_NUM ] = {LIQUIDLEAKAGE_SENSOR_PORT,
                                                                ELECTROMAGNET_OPTICAL1_PORT,
                                                                ELECTROMAGNET_OPTICAL2_PORT,
                                                                DOOR_HALL_PORT,
                                                                CENTRIFUGE_FAULT_PORT};
static const uint16_t SCAN_SENSOR_ARM3_PIN[SCAN_SENSOR_ARM3_NUM ] = {LIQUIDLEAKAGE_SENSOR_PIN,
                                                                ELECTROMAGNET_OPTICAL1_PIN,
                                                                ELECTROMAGNET_OPTICAL2_PIN,
                                                                DOOR_HALL_PIN,
                                                                CENTRIFUGE_FAULT_PIN};
static const uint32_t SCAN_SENSOR_ARM3_CLK[SCAN_SENSOR_ARM3_NUM ] = {LIQUIDLEAKAGE_SENSOR_CLK,
                                                                ELECTROMAGNET_OPTICAL1_CLK,
                                                                ELECTROMAGNET_OPTICAL2_CLK,
                                                                DOOR_HALL_CLK,
                                                                CENTRIFUGE_FAULT_CLK};


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

void switch_scanarm1_gpio_init(Scan_Board1_TypeDef SWITCH_SENSORx);

void switch_scanarm3_gpio_init(Scan_Board3_TypeDef SWITCH_SENSORx);

/**
  * @brief  This function is switch sensor GPIO configuration.
  *
  * @param
  * @retval None
*/

void switch_scanarm1_gpio_init(Scan_Board1_TypeDef SWITCH_SENSORx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable switch value sensor clock */
    RCC_AHB1PeriphClockCmd(SCAN_SENSOR_ARM1_CLK[SWITCH_SENSORx], ENABLE);
    /* Configure which pin  as analog input */
    GPIO_InitStructure.GPIO_Pin =  SCAN_SENSOR_ARM1_PIN[SWITCH_SENSORx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SCAN_SENSOR_ARM1_PORT[SWITCH_SENSORx], &GPIO_InitStructure);
}

void switch_scanarm3_gpio_init(Scan_Board3_TypeDef SWITCH_SENSORx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable switch value sensor clock */
    RCC_AHB1PeriphClockCmd(SCAN_SENSOR_ARM3_CLK[SWITCH_SENSORx], ENABLE);
    /* Configure which pin  as PullUp input */
    GPIO_InitStructure.GPIO_Pin =  SCAN_SENSOR_ARM3_PIN[SWITCH_SENSORx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SCAN_SENSOR_ARM3_PORT[SWITCH_SENSORx], &GPIO_InitStructure);
}

void arm1_switchscan_gpio_init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    /* Enable the Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure the Alarm LED & Alarm BUZZER */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    switch_scanarm1_gpio_init(RETRANSFUSIONPUMP_FEEDBACK);

}

void arm3_switchscan_gpio_init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    /* Enable the Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure the Alarm LED & Alarm BUZZER */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    switch_scanarm3_gpio_init(LIQUIDLEAKAGE_SENSOR);
    switch_scanarm3_gpio_init(ELECTROMAGNET_OPTICAL1);
    switch_scanarm3_gpio_init(ELECTROMAGNET_OPTICAL2);
    switch_scanarm3_gpio_init(DOOR_HALL);
    switch_scanarm3_gpio_init(CENTRIFUGE_FAULT);
}

void read_arm1_SensorState(void)
{
    if(!GPIO_ReadInputDataBit(RETRANSFUSIONPUMP_FEEDBACK_PORT,RETRANSFUSIONPUMP_FEEDBACK_PIN ))
    {

		OSTimeDlyHMSM(0, 0, 0, 1);

//        APP_TRACE("enter NORMAL\r\n");
		//If detect a LOW level , means the liquid leakage sensor status is normal,Otherwise the state is abnormal
		if(!GPIO_ReadInputDataBit(RETRANSFUSIONPUMP_FEEDBACK_PORT,RETRANSFUSIONPUMP_FEEDBACK_PIN))
		{
			//for debug
			//scan_state_feedback.feedback_pump_state= NORMAL;
            scan_state_feedback.feedback_pump_state = 0;//add by wq
		}
	}
    else
    {

        OSTimeDlyHMSM(0, 0, 0, 1);
		if(GPIO_ReadInputDataBit(RETRANSFUSIONPUMP_FEEDBACK_PORT,RETRANSFUSIONPUMP_FEEDBACK_PIN))
		{
     //   scan_state_feedback.feedback_pump_state= ABNORMAL;
        scan_state_feedback.feedback_pump_state= 1;//add by wq
	    }
    }
}

void read_arm3_SensorState(void)
{
    if(!GPIO_ReadInputDataBit(LIQUIDLEAKAGE_SENSOR_PORT,LIQUIDLEAKAGE_SENSOR_PIN ))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        //If detect a LOW level , means the liquid leakage sensor status is normal,Otherwise the state is abnormal
        if(!GPIO_ReadInputDataBit(LIQUIDLEAKAGE_SENSOR_PORT,LIQUIDLEAKAGE_SENSOR_PIN))
        {
          //  scan_state_feedback.weeping_detector= NORMAL;
            scan_state_feedback.weeping_detector= 0;
        }
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(LIQUIDLEAKAGE_SENSOR_PORT,LIQUIDLEAKAGE_SENSOR_PIN))
       // scan_state_feedback.weeping_detector= ABNORMAL;
        scan_state_feedback.weeping_detector= 1;
    }

  // APP_TRACE("weeping_detector state = %d!\r\n",scan_state_feedback.weeping_detector);
   
    if(!GPIO_ReadInputDataBit(ELECTROMAGNET_OPTICAL1_PORT,ELECTROMAGNET_OPTICAL1_PIN ))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        /*If detect a LOW level , means the ELECTROMAGNET_OPTICAL1 sensor status is unlocked,
          the door is open.
        */
        if(!GPIO_ReadInputDataBit(ELECTROMAGNET_OPTICAL1_PORT,ELECTROMAGNET_OPTICAL1_PIN ))
        {
            //for debug
            scan_state_feedback.door_switch1= TRIGGER;
        }
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(ELECTROMAGNET_OPTICAL1_PORT,ELECTROMAGNET_OPTICAL1_PIN ))
            scan_state_feedback.door_switch1= NOT_TRIGGER;
    }
   // APP_TRACE("door_switch1 state = %d!!!!!!!!!!!!\r\n",scan_state_feedback.door_switch1);

    
    if(!GPIO_ReadInputDataBit(ELECTROMAGNET_OPTICAL2_PORT,ELECTROMAGNET_OPTICAL2_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
         /*If detect a HIGH level , means the ELECTROMAGNET_OPTICAL2 sensor status is locked,
          the door is closed.
        */
        if(!GPIO_ReadInputDataBit(ELECTROMAGNET_OPTICAL2_PORT,ELECTROMAGNET_OPTICAL2_PIN))
        {
            scan_state_feedback.door_switch2= TRIGGER;
        }
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(ELECTROMAGNET_OPTICAL2_PORT,ELECTROMAGNET_OPTICAL2_PIN))
            scan_state_feedback.door_switch2= NOT_TRIGGER;
    }
  //  APP_TRACE("door_switch2 state = %d!!!!!!!!!!!!\r\n",scan_state_feedback.door_switch2);

    
    if(!GPIO_ReadInputDataBit(DOOR_HALL_PORT ,DOOR_HALL_PIN ))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
         /*If detect a low level , means the DOOR_HALL sensor  is triggered,
          the door is closed.
          */
        if(!GPIO_ReadInputDataBit(DOOR_HALL_PORT ,DOOR_HALL_PIN))
        {
            //for debug
            scan_state_feedback.door_hoare= LOCKED;
        }

    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(DOOR_HALL_PORT ,DOOR_HALL_PIN))
            scan_state_feedback.door_hoare= NOT_LOCKED;
    }
   // APP_TRACE("door_hoare state = %d!!!!!!!!!!!!\r\n",scan_state_feedback.door_hoare);


// TODO:
    scan_state_feedback.centrifuge_motor_state = 0;

#if 0

    if(!GPIO_ReadInputDataBit(CENTRIFUGE_FAULT_PORT ,CENTRIFUGE_FAULT_PIN))
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(!GPIO_ReadInputDataBit(CENTRIFUGE_FAULT_PORT ,CENTRIFUGE_FAULT_PIN))
        {
            //for debug
            scan_state_feedback.centrifuge_motor_state= TRIGGER;
            APP_TRACE("centrifuge_motor_state is 1\r\n");
        }
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
        if(GPIO_ReadInputDataBit(CENTRIFUGE_FAULT_PORT ,CENTRIFUGE_FAULT_PIN))
        {
            scan_state_feedback.centrifuge_motor_state= NOT_TRIGGER;
        }
    }
#endif
//    APP_TRACE("111centrifuge_motor_state = %d!!!\r\n",scan_state_feedback.centrifuge_motor_state);
}
#endif

