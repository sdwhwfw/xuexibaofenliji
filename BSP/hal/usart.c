/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : usart.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/04/15
 * Description        : This file contains the driver implementation for the
 *                      USART unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/04/15 | v1.0  |            | initial released
 * 2013/09/24 | v1.1  | Bruce.zhu  | change USART framework with DMA feature
 *                                 | Merge some source code from WQ
 *******************************************************************************/

#include "usart.h"
#include "trace.h"

#define UARTn                         COM_NUM


/*#########################################
 *#                 USART1                #
 *#########################################*/
#if defined(USE_OPEN_207V_BOARD) || defined(NO_RTOS_BOARD) || defined(USE_OPEN_207Z_MINI_BOARD)
// PA09 - TX
// PA10 - RX
#define USART1_RX_GPIO_PORT          GPIOA
#define USART1_RX_PIN                GPIO_Pin_10
#define USART1_RX_SOURCE             GPIO_PinSource10
#define USART1_RX_AF                 GPIO_AF_USART1
#define USART1_RX_GPIO_CLK           RCC_AHB1Periph_GPIOA

#define USART1_TX_GPIO_PORT          GPIOA
#define USART1_TX_PIN                GPIO_Pin_9
#define USART1_TX_SOURCE             GPIO_PinSource9
#define USART1_TX_AF                 GPIO_AF_USART1
#define USART1_TX_GPIO_CLK           RCC_AHB1Periph_GPIOA

#define USART1_CLK                   RCC_APB2Periph_USART1
#elif defined(USE_OPEN_207Z_BOARD) || defined(USE_ARM0_REV_0_1) || \
      defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2) || \
      defined(USE_ARM2_REV_0_1) || defined(USE_ARM3_REV_0_1)
// PB6 - TX
// PB7 - RX
#define USART1_RX_GPIO_PORT          GPIOB
#define USART1_RX_PIN                GPIO_Pin_7
#define USART1_RX_SOURCE             GPIO_PinSource7
#define USART1_RX_AF                 GPIO_AF_USART1
#define USART1_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define USART1_TX_GPIO_PORT          GPIOB
#define USART1_TX_PIN                GPIO_Pin_6
#define USART1_TX_SOURCE             GPIO_PinSource6
#define USART1_TX_AF                 GPIO_AF_USART1
#define USART1_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define USART1_CLK                   RCC_APB2Periph_USART1

#else

// PB6 - TX
// PB7 - RX
#define USART1_RX_GPIO_PORT          GPIOB
#define USART1_RX_PIN                GPIO_Pin_7
#define USART1_RX_SOURCE             GPIO_PinSource7
#define USART1_RX_AF                 GPIO_AF_USART1
#define USART1_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define USART1_TX_GPIO_PORT          GPIOB
#define USART1_TX_PIN                GPIO_Pin_6
#define USART1_TX_SOURCE             GPIO_PinSource6
#define USART1_TX_AF                 GPIO_AF_USART1
#define USART1_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define USART1_CLK                   RCC_APB2Periph_USART1

#endif


/*#########################################
 *#                USART2                 #
 *#########################################*/

/*
 * RX: PA3
 * TX: PA2
 */
#define USART2_RX_GPIO_PORT          GPIOA
#define USART2_RX_PIN                GPIO_Pin_3
#define USART2_RX_SOURCE             GPIO_PinSource3
#define USART2_RX_AF                 GPIO_AF_USART2
#define USART2_RX_GPIO_CLK           RCC_AHB1Periph_GPIOA

#define USART2_TX_GPIO_PORT          GPIOA
#define USART2_TX_PIN                GPIO_Pin_2
#define USART2_TX_SOURCE             GPIO_PinSource2
#define USART2_TX_AF                 GPIO_AF_USART2
#define USART2_TX_GPIO_CLK           RCC_AHB1Periph_GPIOA

#define USART2_CLK                   RCC_APB1Periph_USART2


/*#########################################
 *#                USART3                 #
 *#########################################*/

#if defined(USE_OPEN_207V_BOARD) || defined(USE_ARM0_REV_0_1)
/*
 * RX: PC11
 * TX: PC10
 */
#define USART3_RX_GPIO_PORT          GPIOC
#define USART3_RX_PIN                GPIO_Pin_11
#define USART3_RX_SOURCE             GPIO_PinSource11
#define USART3_RX_AF                 GPIO_AF_USART3
#define USART3_RX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART3_TX_GPIO_PORT          GPIOC
#define USART3_TX_PIN                GPIO_Pin_10
#define USART3_TX_SOURCE             GPIO_PinSource10
#define USART3_TX_AF                 GPIO_AF_USART3
#define USART3_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART3_CLK                   RCC_APB1Periph_USART3

#else
/*
 * RX: PB11
 * TX: PB10
 */
#define USART3_RX_GPIO_PORT          GPIOB
#define USART3_RX_PIN                GPIO_Pin_11
#define USART3_RX_SOURCE             GPIO_PinSource11
#define USART3_RX_AF                 GPIO_AF_USART3
#define USART3_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define USART3_TX_GPIO_PORT          GPIOB
#define USART3_TX_PIN                GPIO_Pin_10
#define USART3_TX_SOURCE             GPIO_PinSource10
#define USART3_TX_AF                 GPIO_AF_USART3
#define USART3_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define USART3_CLK                   RCC_APB1Periph_USART3

#endif


/*#########################################
 *#                UART4                  #
 *#########################################*/
/*
 * RX: PC11
 * TX: PC10
 */
#define USART4_RX_GPIO_PORT          GPIOC
#define USART4_RX_PIN                GPIO_Pin_11
#define USART4_RX_SOURCE             GPIO_PinSource11
#define USART4_RX_AF                 GPIO_AF_UART4
#define USART4_RX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART4_TX_GPIO_PORT          GPIOC
#define USART4_TX_PIN                GPIO_Pin_10
#define USART4_TX_SOURCE             GPIO_PinSource10
#define USART4_TX_AF                 GPIO_AF_UART4
#define USART4_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART4_CLK                   RCC_APB1Periph_UART4


/*#########################################
 *#                UART5                  #
 *#########################################*/
/*
 * RX: PD02
 * TX: PC12
 */
#define USART5_RX_GPIO_PORT          GPIOD
#define USART5_RX_PIN                GPIO_Pin_2
#define USART5_RX_SOURCE             GPIO_PinSource2
#define USART5_RX_AF                 GPIO_AF_UART5
#define USART5_RX_GPIO_CLK           RCC_AHB1Periph_GPIOD

#define USART5_TX_GPIO_PORT          GPIOC
#define USART5_TX_PIN                GPIO_Pin_12
#define USART5_TX_SOURCE             GPIO_PinSource12
#define USART5_TX_AF                 GPIO_AF_UART5
#define USART5_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART5_CLK                   RCC_APB1Periph_UART5



/*#########################################
 *#                USART6                 #
 *#########################################*/
/*
 * RX: PC7
 * TX: PC6
 */
#define USART6_RX_GPIO_PORT          GPIOC
#define USART6_RX_PIN                GPIO_Pin_7
#define USART6_RX_SOURCE             GPIO_PinSource7
#define USART6_RX_AF                 GPIO_AF_USART6
#define USART6_RX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART6_TX_GPIO_PORT          GPIOC
#define USART6_TX_PIN                GPIO_Pin_6
#define USART6_TX_SOURCE             GPIO_PinSource6
#define USART6_TX_AF                 GPIO_AF_USART6
#define USART6_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC

#define USART6_CLK                   RCC_APB2Periph_USART6



static USART_TypeDef* USART_NUM[UARTn] = {USART1, USART2, USART3, UART4, UART5, USART6};

static GPIO_TypeDef* USART_PORT[2][UARTn] = { /* RX */
											{USART1_RX_GPIO_PORT, USART2_RX_GPIO_PORT, USART3_RX_GPIO_PORT,
									 		 USART4_RX_GPIO_PORT, USART5_RX_GPIO_PORT, USART6_RX_GPIO_PORT},
											/* TX */
											{USART1_TX_GPIO_PORT, USART2_TX_GPIO_PORT, USART3_TX_GPIO_PORT,
									 		 USART4_TX_GPIO_PORT, USART5_TX_GPIO_PORT, USART6_TX_GPIO_PORT}
											};

static const uint16_t USART_PIN[2][UARTn] = {/* RX */
											 {USART1_RX_PIN, USART2_RX_PIN, USART3_RX_PIN,
											  USART4_RX_PIN, USART5_RX_PIN, USART6_RX_PIN},
											 /* TX */
											 {USART1_TX_PIN, USART2_TX_PIN, USART3_TX_PIN,
											  USART4_TX_PIN, USART5_TX_PIN, USART6_TX_PIN}
											};

static const uint16_t USART_PIN_SOURCE[2][UARTn] = {/* RX */
													{USART1_RX_SOURCE, USART2_RX_SOURCE, USART3_RX_SOURCE,
													 USART4_RX_SOURCE, USART5_RX_SOURCE, USART6_RX_SOURCE},
													/* TX */
													{USART1_TX_SOURCE, USART2_TX_SOURCE, USART3_TX_SOURCE,
													 USART4_TX_SOURCE, USART5_TX_SOURCE, USART6_TX_SOURCE}
													};

static const uint16_t USART_AF[2][UARTn]     = {/* RX */
												{USART1_RX_AF, USART2_RX_AF, USART3_RX_AF,
												 USART4_RX_AF, USART5_RX_AF, USART6_RX_AF},
												/* TX*/
												{USART1_TX_AF, USART2_TX_AF, USART3_TX_AF,
												 USART4_TX_AF, USART5_TX_AF, USART6_TX_AF}
												};

static const uint32_t USART_GPIO_CLK[2][UARTn] = {/* RX */
												  {USART1_RX_GPIO_CLK, USART2_RX_GPIO_CLK, USART3_RX_GPIO_CLK,
												   USART4_RX_GPIO_CLK, USART5_RX_GPIO_CLK, USART6_RX_GPIO_CLK},
												  /* TX */
												  {USART1_TX_GPIO_CLK, USART2_TX_GPIO_CLK, USART3_TX_GPIO_CLK,
												   USART4_TX_GPIO_CLK, USART5_TX_GPIO_CLK, USART6_TX_GPIO_CLK}
												 };

static const uint32_t USART_CLK[UARTn] = {USART1_CLK, USART2_CLK, USART3_CLK, USART4_CLK, USART5_CLK, USART6_CLK};

static const uint8_t  USART_IRQ[UARTn] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};

static const uint8_t  USART_PRE_EMPTION[UARTn]   = {UART1_PREEPRIOROTY, UART2_PREEPRIOROTY, UART3_PREEPRIOROTY,
													UART4_PREEPRIOROTY, UART5_PREEPRIOROTY, UART6_PREEPRIOROTY};

static const uint8_t  USART_SUB_LEVEL[UARTn]     = {UART1_SUBPRIOROTY, UART2_SUBPRIOROTY, UART3_SUBPRIOROTY,
													UART4_SUBPRIOROTY, UART5_SUBPRIOROTY, UART6_SUBPRIOROTY};


static void init_usart_with_dma(uart_num_def_t COMx);

/********************************************************************************
 *                 HOW TO USE THIS DRIVER
 *
 * 1. init USART
 *    init_usart(COM1, 115200, 8, PARITY_NONE, USART_MODE_INT);
 * 2. If you want to use DMA feature you need to
 *    ...
 ********************************************************************************/


/*
 * @brief: init usart
 * @param: COMx        which USART do you want to use
 * @param: baud_rate   USART baudrate
 * @param: word_length the number of data bits transmitted or received in a frame.
 * @param: parity      ODD or EVEN or NONE
 * @param: usart_mode  one byte interrupt or DMA mode
 * @return: 0 -- success
 *          1 -- fail
 */
uint8_t init_usart(uart_num_def_t COMx, u32 baud_rate, u8 word_length, parity_mode parity, usart_mode_type usart_mode)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	// check com param
	assert_param(IS_USART_COM_NUM(COMx));
	assert_param(IS_USART_COM_PARITY(parity));
	assert_param(word_length == 8 || word_length == 9);
	assert_param(IS_USART_DMA_MODE(usart_mode));

	// init usart param struct
	USART_InitStructure.USART_BaudRate            = baud_rate;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	switch (word_length)
	{
		case 8:
			USART_InitStructure.USART_WordLength  = USART_WordLength_8b;
			break;
		case 9:
			USART_InitStructure.USART_WordLength  = USART_WordLength_9b;
			break;
		default:
			return 1;
	}

	switch (parity)
	{
		case PARITY_NONE:
			USART_InitStructure.USART_Parity      = USART_Parity_No;
			break;
		case PARITY_EVEN:
			USART_InitStructure.USART_Parity      = USART_Parity_Even;
			break;
		case PARITY_ODD:
			USART_InitStructure.USART_Parity      = USART_Parity_Odd;
			break;
		default:
			return 1;
	}

	USART_DeInit(USART_NUM[COMx]);

	// USART1 and USART6 are mounted in APB2 BUS (60MHz)
	// USART2 USART3 UART4 UART5 are mounted in APB1 BUS (30MHz)
	switch (COMx)
	{
		case COM1:
		case COM6:
			RCC_APB2PeriphClockCmd(USART_CLK[COMx], ENABLE);
			break;
		case COM2:
		case COM3:
		case COM4:
		case COM5:
			RCC_APB1PeriphClockCmd(USART_CLK[COMx], ENABLE);
			break;
	}

	// enable GPIO clock: RX and TX
	RCC_AHB1PeriphClockCmd(USART_GPIO_CLK[0][COMx] | USART_GPIO_CLK[1][COMx], ENABLE);

	// AF pin: RX and TX
	GPIO_PinAFConfig(USART_PORT[0][COMx], USART_PIN_SOURCE[0][COMx], USART_AF[0][COMx]);
	GPIO_PinAFConfig(USART_PORT[1][COMx], USART_PIN_SOURCE[1][COMx], USART_AF[1][COMx]);

	// Configure USART RX and Tx as alternate function
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; //GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = USART_PIN[0][COMx];
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_PORT[0][COMx], &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = USART_PIN[1][COMx];
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(USART_PORT[1][COMx], &GPIO_InitStructure);

	/* USART configuration */
	USART_Init(USART_NUM[COMx], &USART_InitStructure);

	if (usart_mode == USART_MODE_INT)
	{
		USART_ITConfig(USART_NUM[COMx], USART_IT_RXNE, ENABLE);
	}
	else if (usart_mode == USART_MODE_DMA)
	{
		// USART IDLE interrupt
		USART_ITConfig(USART_NUM[COMx], USART_IT_IDLE, ENABLE);
	}

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel                   = USART_IRQ[COMx];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_PRE_EMPTION[COMx];
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = USART_SUB_LEVEL[COMx];
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// init DMA feature
	if (usart_mode == USART_MODE_DMA)
	{
		if (COMx == COM1 || COMx == COM6)
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		}
		else
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
		}

		init_usart_with_dma(COMx);
	}

    USART_ClearFlag(USART_NUM[COMx], USART_FLAG_TC);
    /* Enable USART */
    USART_Cmd(USART_NUM[COMx], ENABLE);

	return 0;
}


/**
  * Note: See STM32F2xx RU V5.pdf page 173
  *
  * ----- USART1 -----
  * RX DMA Stream: DMA2_STREAM2
  * TX DMA Stream: DMA2_STREAM7
  *
  * ----- USART2 -----
  * RX DMA Stream: DMA1_STREAM5
  * TX DMA Stream: DMA1_STREAM6
  *
  * ----- USART3 -----
  * RX DMA Stream: DMA1_STREAM1
  * TX DMA Stream: DMA1_STREAM3
  *
  * ----- USART4 -----
  * RX DMA Stream: DMA1_STREAM2
  * TX DMA Stream: DMA1_STREAM4
  *
  * ----- USART5 -----
  * RX DMA Stream: DMA1_STREAM0
  * TX DMA Stream: DMA1_STREAM7
  *
  * ----- USART6 -----
  * RX DMA Stream: DMA2_STREAM1
  * TX DMA Stream: DMA2_STREAM6
  *
  *
  */
void init_usart_with_dma(uart_num_def_t COMx)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	// Check COM num
	assert_param(IS_USART_COM_NUM(COMx));

	switch (COMx)
	{
		case COM1:
			{
				// FOR USART1 DMA RX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA2_Stream2_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_STREAM2_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA2_STREAM2_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);

				// FOR USART1 DMA TX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA2_Stream7_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_STREAM7_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA2_STREAM7_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
			}
			break;
		case COM2:
			{
				// FOR USART2 DMA RX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream5_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM5_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM5_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);

				// FOR USART2 DMA TX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream6_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM6_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM6_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
			}
			break;
		case COM3:
			{
				// FOR USART3 DMA RX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream1_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM1_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM1_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);

				// FOR USART3 DMA TX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream3_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM3_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM3_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
			}
			break;

		case COM4:
			{
				// FOR UART4 DMA RX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream2_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM2_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM2_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);

				// FOR UART4 DMA TX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream4_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM4_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM4_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
			}
			break;
		case COM5:
			{
				// FOR UART5 DMA RX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream0_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM0_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM0_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);

				// FOR UART5 DMA TX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Stream7_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_STREAM7_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA1_STREAM7_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
			}
		case COM6:
			{
				// FOR USART6 DMA RX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA2_Stream1_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_STREAM1_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA2_STREAM1_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);

				// FOR USART6 DMA TX
				NVIC_InitStructure.NVIC_IRQChannel					 = DMA2_Stream6_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_STREAM6_PRE_PRI;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA2_STREAM6_SUB_PRI;
				NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
			}
			break;
	}

}



/**
  * Note: See STM32F2xx RU V5.pdf page 173
  *
  * ----- USART1 -----
  * RX DMA Stream: DMA2_STREAM2
  * TX DMA Stream: DMA2_STREAM7
  *
  * ----- USART2 -----
  * RX DMA Stream: DMA1_STREAM5
  * TX DMA Stream: DMA1_STREAM6
  *
  * ----- USART3 -----
  * RX DMA Stream: DMA1_STREAM1
  * TX DMA Stream: DMA1_STREAM3
  *
  * ----- USART4 -----
  * RX DMA Stream: DMA1_STREAM2
  * TX DMA Stream: DMA1_STREAM4
  *
  * ----- USART5 -----
  * RX DMA Stream: DMA1_STREAM0
  * TX DMA Stream: DMA1_STREAM7
  *
  * ----- USART6 -----
  * RX DMA Stream: DMA2_STREAM1
  * TX DMA Stream: DMA2_STREAM6
  *
  *
  */
void reload_usart_dma_rx(uart_num_def_t COMx, u8* p_data, u16 rx_size)
{
	DMA_InitTypeDef     DMA_InitStructure;
	DMA_Stream_TypeDef* dma_stream;
	uint32_t            dma_channel;
	uint32_t            dma_flag;

	// check COM number
	assert_param(IS_USART_COM_NUM(COMx));

	/* Configure DMA controller to manage USART TX DMA request ----------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(USART_NUM[COMx]) + 0x04;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

	switch (COMx)
	{
		case COM1:
			dma_stream  = DMA2_Stream2;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF1;
			break;
		case COM2:
			dma_stream  = DMA1_Stream5;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF5;
			break;
		case COM3:
			dma_stream  = DMA1_Stream1;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF1;
			break;
		case COM4:
			dma_stream  = DMA1_Stream2;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF2;
			break;
		case COM5:
			dma_stream  = DMA1_Stream0;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF0;
			break;
		case COM6:
			dma_stream  = DMA2_Stream1;
			dma_channel = DMA_Channel_5;
			dma_flag    = DMA_FLAG_TCIF1;
			break;
	}

	DMA_DeInit(dma_stream);
	DMA_InitStructure.DMA_Channel = dma_channel;
	DMA_InitStructure.DMA_DIR     = DMA_DIR_PeripheralToMemory;

	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)p_data;
	DMA_InitStructure.DMA_BufferSize      = (uint16_t)rx_size;
	DMA_Init(dma_stream, &DMA_InitStructure);

	DMA_ClearFlag(dma_stream, dma_flag);
	DMA_ITConfig(dma_stream, DMA_IT_TC, ENABLE);

	//Enable the DMA RX Stream 
	DMA_Cmd(dma_stream, ENABLE);

    // Enable the USART Rx DMA request 
	USART_DMACmd(USART_NUM[COMx], USART_DMAReq_Rx, ENABLE);
    
}





/**
  * Note: See STM32F2xx RU V5.pdf page 173
  *
  * ----- USART1 -----
  * RX DMA Stream: DMA2_STREAM2
  * TX DMA Stream: DMA2_STREAM7
  *
  * ----- USART2 -----
  * RX DMA Stream: DMA1_STREAM5
  * TX DMA Stream: DMA1_STREAM6
  *
  * ----- USART3 -----
  * RX DMA Stream: DMA1_STREAM1
  * TX DMA Stream: DMA1_STREAM3
  *
  * ----- USART4 -----
  * RX DMA Stream: DMA1_STREAM4
  * TX DMA Stream: DMA1_STREAM4
  *
  * ----- USART5 -----
  * RX DMA Stream: DMA1_STREAM0
  * TX DMA Stream: DMA1_STREAM7
  *
  * ----- USART6 -----
  * RX DMA Stream: DMA2_STREAM1
  * TX DMA Stream: DMA2_STREAM6
  *
  *
  */
void reload_usart_dma_tx(uart_num_def_t COMx, u8* p_data, u16 size)
{
	DMA_InitTypeDef     DMA_InitStructure;
	DMA_Stream_TypeDef* dma_stream;
	uint32_t            dma_channel;
	uint32_t            dma_flag;

	// check COM number
	assert_param(IS_USART_COM_NUM(COMx));

	/* Configure DMA controller to manage USART TX DMA request ----------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(USART_NUM[COMx]) + 0x04;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

	switch (COMx)
	{
		case COM1:
			dma_stream  = DMA2_Stream7;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF7;
			break;
		case COM2:
			dma_stream  = DMA1_Stream6;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF6;
			break;
		case COM3:
			dma_stream  = DMA1_Stream3;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF3;
			break;
		case COM4:
			dma_stream  = DMA1_Stream4;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF4;
			break;
		case COM5:
			dma_stream  = DMA1_Stream7;
			dma_channel = DMA_Channel_4;
			dma_flag    = DMA_FLAG_TCIF7;
			break;
		case COM6:
			dma_stream  = DMA2_Stream6;
			dma_channel = DMA_Channel_5;
			dma_flag    = DMA_FLAG_TCIF6;
			break;
	}

	DMA_DeInit(dma_stream);
	DMA_InitStructure.DMA_Channel = dma_channel;
	DMA_InitStructure.DMA_DIR     = DMA_DIR_MemoryToPeripheral;

	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)p_data;
	DMA_InitStructure.DMA_BufferSize      = (uint16_t)size;
	DMA_Init(dma_stream, &DMA_InitStructure);

	DMA_ClearFlag(dma_stream, dma_flag);
	/* Enable the DMA Stream Interrupt */
	DMA_ITConfig(dma_stream, DMA_IT_TC, ENABLE);

    /* Clear the TC bit in the SR register by writing 0 to it */
    USART_ClearFlag(USART_NUM[COMx], USART_FLAG_TC);
    //USART_ClearFlag(USART_NUM[COMx], USART_FLAG_RXNE);
    //USART_ClearFlag(USART_NUM[COMx], USART_FLAG_LBD);
    /* Enable the DMA TX Stream */
    DMA_Cmd(dma_stream, ENABLE);

	/* Enable the USART DMA requests */
	USART_DMACmd(USART_NUM[COMx], USART_DMAReq_Tx, ENABLE);
}


void usart_send_data(uart_num_def_t COMx, uint8_t ch)
{
	assert_param(IS_USART_COM_NUM(COMx));

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART_NUM[COMx], USART_FLAG_TC) == RESET);

	/* write a character to the USART */
	USART_SendData(USART_NUM[COMx], (u8)ch);
}


void usart_send_string(uart_num_def_t COMx, uint8_t* str)
{
	u32 i = 0;

	while(str[i] != 0)
	{
		usart_send_data(COMx, (u8)str[i]);
		i++;
	}
}



