/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : sram_test.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/06/29
 * Description        : This file contains the software test for the
 *                      sram unit
 *******************************************************************************
 * History:
 * 2013/06/29 v1.0
 *******************************************************************************/
#include "sram_test.h"

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE        0x6000
#define WRITE_READ_ADDR    0x1000

/* Private variables ---------------------------------------------------------*/
uint8_t TxBuffer[BUFFER_SIZE], RxBuffer[BUFFER_SIZE];
__IO uint32_t WriteReadStatus = 0;
uint32_t Index = 0;

static void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
	uint16_t IndexTmp = 0;

	/* Put in global buffer different values */
	for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
	{
		pBuffer[IndexTmp] = IndexTmp + Offset;
	}
}

static void Fill_byte(uint8_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
	uint16_t IndexTmp = 0;

	/* Put in global buffer different values */
	for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
	{
		pBuffer[IndexTmp] = (uint8_t)(IndexTmp + Offset);
	}
}


static void led_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//LED1(PF6)  LED2(PF7)  LED3(PF8)  LED4(PF9)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

static void sram_test_byte(void)
{
	Fill_byte(TxBuffer, BUFFER_SIZE, 0x00);
	sram_write_byte(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
	delay(100);
	memset(RxBuffer, 0x00, sizeof(RxBuffer));
	sram_read_byte(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
	/* Read back SRAM memory and check content correctness */
	for (Index = 0x00; (Index < BUFFER_SIZE) /*&& (WriteReadStatus == 0)*/; Index++)
	{
		if (RxBuffer[Index] != TxBuffer[Index])
		{
			WriteReadStatus++;
		}

		usart_send_data(COM1, RxBuffer[Index]);
	}

	if (WriteReadStatus == 0)
	{
		/* OK */
		GPIO_SetBits(GPIOF, GPIO_Pin_6);
		usart_send_string(COM1, " OK!");
	}
	else
	{
		/* KO */
		/* Turn on LD2 */
		GPIO_SetBits(GPIOF, GPIO_Pin_7);
		usart_send_string(COM1, " ERROR");
	}

	delay(500);
}


static void sram_test(void)
{
	static int i = 0;
	static __IO uint8_t *p = (__IO uint8_t *)0x68000000;

	memset(p, 0x00, 128);
	sprintf(p, "Hello, this is Bruce test SRAM[%d][0x%p]\r\n", i++, p);
	usart_send_string(COM1, p);

	p += 128;
}



void sram_test_process(void)
{
	// IMPORTANT: SRAM must be first init!
	sram_init();

	delay_init(DELAY_FOR_MS);
	led_gpio_init();
	init_usart(COM1, 115200);
	usart_send_string(COM1, "\r\n------------------------\r\n");
	usart_send_string(COM1, "- SRAM TEST PROGRAM    -");
	usart_send_string(COM1, "\r\n------------------------\r\n");

	for(;;)
	{
		sram_test();
		//sram_test_byte();

		delay(10);
	}
}


