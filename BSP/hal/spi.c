/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : spi.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/21
 * Description        : This file contains the software implementation for the
 *                      SPI unit
 *******************************************************************************
 * History:
 * 2013/08/21 v1.0
 *******************************************************************************/

#include "spi.h"

#define _USE_SPI1_FTR_
//#define _USE_SPI2_FTR_


#if defined(_USE_SPI1_FTR_)
//=============================================
// SPI1
// SPI1_NSS:  PA4
// SPI1_SCK:  PA5
// SPI1_MISO: PA6
// SPI1_MOSI: PA7
//=============================================
#define SPI                           SPI1
#define SPI_CLK                       RCC_APB2Periph_SPI1
#define SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define SPI_CS_PIN                    GPIO_Pin_4
#define SPI_CS_GPIO_PORT              GPIOA
#define SPI_CS_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define SPI_SCK_PIN                   GPIO_Pin_5
#define SPI_SCK_GPIO_PORT             GPIOA
#define SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define SPI_SCK_SOURCE                GPIO_PinSource5
#define SPI_SCK_AF                    GPIO_AF_SPI1

#define SPI_MISO_PIN                  GPIO_Pin_6
#define SPI_MISO_GPIO_PORT            GPIOA
#define SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPI_MISO_SOURCE               GPIO_PinSource6
#define SPI_MISO_AF                   GPIO_AF_SPI1

#define SPI_MOSI_PIN                  GPIO_Pin_7
#define SPI_MOSI_GPIO_PORT            GPIOA
#define SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPI_MOSI_SOURCE               GPIO_PinSource7
#define SPI_MOSI_AF                   GPIO_AF_SPI1

#elif defined(_USE_SPI2_FTR_)
//=============================================
// SPI2
// SPI2_NSS:  PB12
// SPI2_SCK:  PB13
// SPI2_MISO: PB14
// SPI2_MOSI: PB15
//=============================================
#define SPI                           SPI2
#define SPI_CLK                       RCC_APB1Periph_SPI2
#define SPI_CLK_INIT                  RCC_APB1PeriphClockCmd

#define SPI_CS_PIN                    GPIO_Pin_12
#define SPI_CS_GPIO_PORT              GPIOB
#define SPI_CS_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define SPI_SCK_PIN                   GPIO_Pin_13
#define SPI_SCK_GPIO_PORT             GPIOB
#define SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPI_SCK_SOURCE                GPIO_PinSource13
#define SPI_SCK_AF                    GPIO_AF_SPI2

#define SPI_MISO_PIN                  GPIO_Pin_14
#define SPI_MISO_GPIO_PORT            GPIOB
#define SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPI_MISO_SOURCE               GPIO_PinSource14
#define SPI_MISO_AF                   GPIO_AF_SPI2

#define SPI_MOSI_PIN                  GPIO_Pin_15
#define SPI_MOSI_GPIO_PORT            GPIOB
#define SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPI_MOSI_SOURCE               GPIO_PinSource15
#define SPI_MOSI_AF                   GPIO_AF_SPI2

#else
	#error "must defined one SPI!"
#endif

/**
  * @brief  Initializes the peripherals
  * @param  None
  * @retval None
  */
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/*!< Enable the SPI clock */
	SPI_CLK_INIT(SPI_CLK, ENABLE);

	/*!< Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPI_SCK_GPIO_CLK  | SPI_MISO_GPIO_CLK | 
						   SPI_MOSI_GPIO_CLK | SPI_CS_GPIO_CLK, ENABLE);

	/*!< SPI pins configuration *************************************************/

	/*!< Connect SPI pins to AF5 */
	GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_SOURCE, SPI_SCK_AF);
	GPIO_PinAFConfig(SPI_MISO_GPIO_PORT, SPI_MISO_SOURCE, SPI_MISO_AF);
	GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_SOURCE, SPI_MOSI_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	/*!< SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
	GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI_MOSI_PIN;
	GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< Deselect: Chip Select high */
	spi_cs_high();

	/*!< SPI configuration */
	SPI_I2S_DeInit(SPI);
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI, &SPI_InitStructure);

	/*!< Enable the sFLASH_SPI	*/
	SPI_Cmd(SPI, ENABLE);
}

/**
  * @brief  DeInitializes the peripherals
  * @param  None
  * @retval None
  */
void spi_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< Disable the sFLASH_SPI  ************************************************/
	SPI_Cmd(SPI, DISABLE);

	/*!< DeInitializes the sFLASH_SPI *******************************************/
	SPI_I2S_DeInit(SPI);

	/*!< sFLASH_SPI Periph clock disable ****************************************/
	SPI_CLK_INIT(SPI_CLK, DISABLE);

	/*!< Configure all pins used by the SPI as input floating *******************/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
	GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
	GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
	GPIO_Init(SPI_CS_GPIO_PORT, &GPIO_InitStructure);

}


u16 spi_send_byte(uint8_t byte)
{
	u16 i = 0;

	// first, check SPI TX state and send data to SPI if state is OK
	do
	{
		i++;
		if (i == 0xffff)
			return 0;
	}while(SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);

	SPI_I2S_SendData(SPI, byte);

	// second, wait to receive data
	i = 0;
	do
	{
		i++;
		if (i == 0xffff)
			return 0;
	}
	while(SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI);
}


u16 spi_read_byte(void)
{
	return spi_send_byte(0xAA);
}



void spi_cs_low(void)
{
	GPIO_ResetBits(SPI_CS_GPIO_PORT, SPI_CS_PIN);
}


void spi_cs_high(void)
{
	GPIO_SetBits(SPI_CS_GPIO_PORT, SPI_CS_PIN);
}



