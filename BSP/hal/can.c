/**
  ******************************************************************************
  * @file    can.c
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-15
  * @brief   CAN HAL functions.
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */
#ifndef __CAN_C__
#define __CAN_C__

/* Includes ------------------------------------------------------------------*/

#include "can.h"

typedef struct
{
	uint16_t can_rx_pin;
	uint16_t can_tx_pin;
} CAN_pin_TypeDef;

typedef struct
{
	uint16_t can_rx_pin;
	uint16_t can_tx_pin;
} CAN_pin_source_TypeDef;


#define CANn                        2

//------------- CAN1 -------------
#define CAN1_RX_PIN                 GPIO_Pin_11
#define CAN1_RX_PIN_SOURCE          GPIO_PinSource11
#define CAN1_TX_PIN                 GPIO_Pin_12
#define CAN1_TX_PIN_SOURCE          GPIO_PinSource12
#define CAN1_CLK                    RCC_APB1Periph_CAN1
#define CAN1_GPIO_PORT              GPIOA
#define CAN1_GPIO_CLK               RCC_AHB1Periph_GPIOA

//------------- CAN2 -------------
#define CAN2_RX_PIN                 GPIO_Pin_5
#define CAN2_RX_PIN_SOURCE          GPIO_PinSource5
#define CAN2_TX_PIN                 GPIO_Pin_13
#define CAN2_TX_PIN_SOURCE          GPIO_PinSource13
#define CAN2_CLK                    RCC_APB1Periph_CAN2
#define CAN2_GPIO_PORT              GPIOB
#define CAN2_GPIO_CLK               RCC_AHB1Periph_GPIOA


static GPIO_TypeDef*                  CAN_PORT[CANn] = {CAN1_GPIO_PORT, CAN2_GPIO_PORT};
static const CAN_pin_TypeDef          CAN_PIN[CANn]  = {{CAN1_RX_PIN, CAN1_TX_PIN}, 
													 	{CAN2_RX_PIN, CAN2_TX_PIN}};
static const CAN_pin_source_TypeDef   CAN_PIN_SOURCE[CANn] = {{CAN1_RX_PIN_SOURCE, CAN1_TX_PIN_SOURCE}, 
															{CAN2_RX_PIN_SOURCE, CAN2_TX_PIN_SOURCE}};
static const uint32_t                 CAN_CLK[CANn] = {CAN1_CLK, CAN2_CLK};
static const uint8_t                  CAN_AF[CANn]  = {GPIO_AF_CAN1, GPIO_AF_CAN2};
static CAN_TypeDef*                   CAN_Type[CANn] = {CAN1, CAN2};


#define IS_CAN_NUM(NUM)       (((NUM) == USER_CAN_1) || ((NUM) == USER_CAN_2))


/**
  * @brief  Configures the CAN1.
  * @param  None
  * @retval None
  *
  * --- CAN1 ---
  * RX --> PB8
  * TX --> PB9
  *
  * --- CAN2 ---
  * RX --> PB5
  * TX --> PB6
  *
  */
void CAN_Config(USER_CAN_TypeDef CANx)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	CAN_InitTypeDef       CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	assert_param(IS_CAN_NUM(CANx));

	/* CAN register init */
	CAN_DeInit(CAN_Type[CANx]);

	CAN_NVIC_Config(CANx);

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(CAN_CLK[CANx], ENABLE);

	GPIO_PinAFConfig(CAN_PORT[CANx], CAN_PIN_SOURCE[CANx].can_rx_pin, CAN_AF[CANx]);
	GPIO_PinAFConfig(CAN_PORT[CANx], CAN_PIN_SOURCE[CANx].can_tx_pin, CAN_AF[CANx]);

	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = CAN_PIN[CANx].can_rx_pin | CAN_PIN[CANx].can_tx_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN_PORT[CANx], &GPIO_InitStructure);

	/* CAN configuration ********************************************************/	
	/* Enable CAN clock */
	RCC_APB1PeriphClockCmd(CAN_CLK[CANx], ENABLE);

	CAN_StructInit(&CAN_InitStructure);
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 1MBps (CAN clocked at 30 MHz) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
	CAN_InitStructure.CAN_Prescaler = 2;
	CAN_Init(CAN_Type[CANx], &CAN_InitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN_Type[CANx], CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN_Type[CANx], CAN_IT_EWG, ENABLE);
	CAN_ITConfig(CAN_Type[CANx], CAN_IT_EPV, ENABLE);
	CAN_ITConfig(CAN_Type[CANx], CAN_IT_BOF, ENABLE);
	CAN_ITConfig(CAN_Type[CANx], CAN_IT_LEC, ENABLE);
	CAN_ITConfig(CAN_Type[CANx], CAN_IT_ERR, ENABLE);

}

/**
  * @brief  Configures the NVIC for CAN.
  * @param  None
  * @retval None
  */
void CAN_NVIC_Config(USER_CAN_TypeDef CANx)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	if (USER_CAN_1 == CANx)
	{
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	}
	else if (USER_CAN_2 == CANx)
	{
		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	}
	else
	{
		return;
	}

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x05;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



void can_send_data(USER_CAN_TypeDef CANx, uint16_t ID, uint8_t* p_data, uint8_t len)
{
	CanTxMsg TxMessage;
	uint8_t  i;

	if (len > 8)
		return;

	/* transmit */
	TxMessage.StdId = ID;
	TxMessage.ExtId = 0x01;
	TxMessage.RTR   = CAN_RTR_DATA;
	TxMessage.IDE   = CAN_ID_STD;
	TxMessage.DLC   = len;

	for(i = 0; i < len; i++)
	{
		TxMessage.Data[i] = p_data[i];
	}

	if(CANx == USER_CAN_1)
		CAN_Transmit(CAN1, &TxMessage);
	else if(CANx == USER_CAN_2)
		CAN_Transmit(CAN2, &TxMessage);

}



#endif /* __CAN_C__ */


