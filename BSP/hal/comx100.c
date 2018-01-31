/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : comx100.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/25
 * Description        : This file contains the software implementation for the
 *                      comx100 module unit
 *******************************************************************************
 * History:
 * 2013/07/25 v1.0
 *******************************************************************************/
#include "comx100.h"


/**
  * @brief  FSMC_NE2
  */
#define Bank1_COMX100_ADDR  ((uint32_t)0x64000000)


/**
  * @brief  Configures the FSMC and GPIOs to interface with the COMX100.
  *         This function must be called before any write/read operation
  *         on the COMX100.
  * @param  None
  * @retval None
  */
void COMX100_init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
	                     RCC_AHB1Periph_GPIOG, ENABLE);

	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+-------------------+
	+                       COMX100 pins assignment                                 +
	+-------------------+--------------------+------------------+-------------------+
	| PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0  <-> FSMC_A10 |
	| PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1  <-> FSMC_A11 |
	| PD4  <-> FSMC_NOE | PE5  <-> DPM_SIRn  | PF2  <-> FSMC_A2 | PG2  <-> FSMC_A12 |
	| PD5  <-> FSMC_NWE | PE7  <-> FSMC_D4   | PF3  <-> FSMC_A3 | PG3  <-> FSMC_A13 |
	| PD6  <-> FSMC_NWAIT|PE8  <-> FSMC_D5   | PF4  <-> FSMC_A4 | PG4  <-> FSMC_A14 |
	| PD11 <-> FSMC_A16 | PE9  <-> FSMC_D6   | PF5  <-> FSMC_A5 | PG5  <-> FSMC_A15 |
	| PD12 <-> FSMC_A17 | PE10 <-> FSMC_D7   | PF12 <-> FSMC_A6 | PG9  <-> FSMC_NE2 |
	| PD14 <-> FSMC_D0  |                    PF13 <-> FSMC_A7   | PF15 <-> FSMC_A9 +-------------------+
	| PD15 <-> FSMC_D1  |                    PF14 <-> FSMC_A8   +------------------+
	+-------------------+--------------------+
	*/
	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+-------------------+
	+						SRAM pins assignment									+
	+-------------------+--------------------+------------------+-------------------+
	| PD0  <-> FSMC_D2	| PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0  <-> FSMC_A10 |
	| PD1  <-> FSMC_D3	| PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1  <-> FSMC_A11 |
	| PD4  <-> FSMC_NOE   | PE7  <-> FSMC_D4   | PF2  <-> FSMC_A2 | PG2  <-> FSMC_A12 |
	| PD5  <-> FSMC_NWE   | PE8  <-> FSMC_D5   | PF3  <-> FSMC_A3 | PG3  <-> FSMC_A13 |
	| PD8  <-> FSMC_D13   | PE9  <-> FSMC_D6   | PF4  <-> FSMC_A4 | PG4  <-> FSMC_A14 |
	| PD9  <-> FSMC_D14   | PE10 <-> FSMC_D7   | PF5  <-> FSMC_A5 | PG5  <-> FSMC_A15 |
	| PD10 <-> FSMC_D15   | PE11 <-> FSMC_D8   | PF12 <-> FSMC_A6 | PG10 <-> FSMC_NE3 |
	| PD11 <-> FSMC_A16   | PE12 <-> FSMC_D9   | PF13 <-> FSMC_A7 |-------------------+
	| PD12 <-> FSMC_A17   | PE13 <-> FSMC_D10  | PF14 <-> FSMC_A8 | 
	| PD14 <-> FSMC_D0	| PE14 <-> FSMC_D11  | PF15 <-> FSMC_A9 | 
	| PD15 <-> FSMC_D1	| PE15 <-> FSMC_D12  |------------------+
	+-------------------+--------------------+
	*/

	/**
	  * GPIOE PIN5 <---> DPM_SIRQn
	  * HIGH: sets the data width of 8 bit
	  * LOW:  sets the data width of 16 bit
	  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);

    /**
      * RESET comX100 module
      * PE4 ---> DPM_RESETn
      */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_4);

	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | 
	                              GPIO_Pin_8  | GPIO_Pin_9 | GPIO_Pin_10 |
	                              GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
								  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* GPIOF configuration */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
	                            GPIO_Pin_4  | GPIO_Pin_5  | 
	                            GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* GPIOG configuration */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
	                            GPIO_Pin_4  | GPIO_Pin_5  |GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/*-- FSMC Configuration ------------------------------------------------------*/
	p.FSMC_AddressSetupTime = 4;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 14;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Enable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/*!< Enable FSMC Bank1_SRAM2 Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);
}

/**
  * @brief  Writes a byte buffer to the comX100 module DPM.
  * @param  pvDevInstance : no care about it
  * @param  pvData : pointer to buffer.
  * @param  ulAddr : DPM internal address from which the data will be
  *         written.
  * @param  ulLen : number of bytes to write.
  * @retval None
  */
void* COMX100_write_byte(void* pvDevInstance, uint32_t ulAddr, void* pvData, uint32_t ulLen)
{
	for (; ulLen != 0; ulLen--) /* while there is data to write */
	{
		/* Transfer data to the memory */
		*(uint8_t *) (Bank1_COMX100_ADDR + ulAddr) = *(uint8_t *)pvData;
		pvData = (uint8_t *)pvData + 1;
		/* Increment the address*/
		ulAddr += 1;
	}

	return pvData;
}

/**
  * @brief  Reads a block of data from the comX100 module DPM.
  *         typedef void* (*PFN_HWIF_MEMCPY) ( void* pvDevInstance, uint32_t ulAddr, void* pvData, uint32_t ulLen);
  * @param  pvDevInstance : no care about it
  * @param  pvData : pointer to the buffer that receives the data read from the
  *         DPM.
  * @param  ulAddr : DPM internal address to read from.
  * @param  ulLen : number of bytes to read.
  * @retval None
  */
void* COMX100_read_byte(void* pvDevInstance, uint32_t ulAddr, void* pvData, uint32_t ulLen)
{
	pvDevInstance = pvDevInstance;

	for (; ulLen != 0; ulLen--) /* while there is data to read */
	{
		/* Read a half-word from the memory */
		*(__IO uint8_t*)pvData = *(__IO uint8_t*) (Bank1_COMX100_ADDR + ulAddr);
		pvData = (uint8_t*)pvData + 1;

		/* Increment the address*/
		ulAddr += 1;
	}

	return pvData;
}


/**
  * PULL PE4 and let comX100 start!
  *
  */
void start_comX100_module(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_4);
}



