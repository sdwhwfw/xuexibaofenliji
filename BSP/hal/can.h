/**
  ******************************************************************************
  * @file    can.h
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-15
  * @brief   This file contains CAN HAL functions.
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

#ifndef __CAN_H__
#define __CAN_H__

#include "stm32f2xx.h"
#include "app.h"


typedef enum
{
	USER_CAN_1 = 0,
	USER_CAN_2 = 1,
	USER_CAN_NUM
} USER_CAN_TypeDef;

#ifdef __CAN_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif


GLOBAL void CAN_Config(USER_CAN_TypeDef CANx);
GLOBAL void CAN_NVIC_Config(USER_CAN_TypeDef CANx);
GLOBAL void can_send_data(USER_CAN_TypeDef CANx, uint16_t ID, uint8_t* p_data, uint8_t len);


#undef GLOBAL
#endif /* __CAN_H__ */

