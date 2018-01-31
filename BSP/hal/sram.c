/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : sram.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/06/29
 * Description        : This file contains the software implementation for the
 *                      sram unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/06/29 | v1.0  |            | initial released
 * 2013/10/17 | v1.1  | Bruce.zhu  | we need set NE2 high if comX100 exist 
 *                                   but not init it
 *******************************************************************************/
#include "sram.h"

#if defined(USE_ISSI_SRAM_FTR)

/**
  * @brief  Configures the FSMC and GPIOs to interface with the SRAM memory.
  *         This function must be called before any write/read operation
  *         on the SRAM.
  * @param  None
  * @retval None
  */
void sram_init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
	                     RCC_AHB1Periph_GPIOG, ENABLE);

	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    /**
     * 2013.10.17
     * BUG fix: the comX100 maybe affected the SRAM data if we don't init NE2(PG9)
     */
#if defined(CONTROL_MAIN_BOARD) && !defined(_COMX100_MODULE_FTR_)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG, GPIO_Pin_9);
#endif
	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+-------------------+
	+                       SRAM pins assignment                                    +
	+-------------------+--------------------+------------------+-------------------+
	| PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0  <-> FSMC_A10 |
	| PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1  <-> FSMC_A11 |
	| PD4  <-> FSMC_NOE | PE7  <-> FSMC_D4   | PF2  <-> FSMC_A2 | PG2  <-> FSMC_A12 |
	| PD5  <-> FSMC_NWE | PE8  <-> FSMC_D5   | PF3  <-> FSMC_A3 | PG3  <-> FSMC_A13 |
	| PD8  <-> FSMC_D13 | PE9  <-> FSMC_D6   | PF4  <-> FSMC_A4 | PG4  <-> FSMC_A14 |
	| PD9  <-> FSMC_D14 | PE10 <-> FSMC_D7   | PF5  <-> FSMC_A5 | PG5  <-> FSMC_A15 |
	| PD10 <-> FSMC_D15 | PE11 <-> FSMC_D8   | PF12 <-> FSMC_A6 | PD7  <-> FSMC_NE1 |
	| PD11 <-> FSMC_A16 | PE12 <-> FSMC_D9   | PF13 <-> FSMC_A7 |-------------------+
	| PD12 <-> FSMC_A17 | PE13 <-> FSMC_D10  | PF14 <-> FSMC_A8 | 
	| PD14 <-> FSMC_D0  | PE14 <-> FSMC_D11  | PF15 <-> FSMC_A9 | 
	| PD15 <-> FSMC_D1  | PE15 <-> FSMC_D12  |------------------+
	+-------------------+--------------------+
	*/
	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
	                            GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
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
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_7 |
	                            GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
	                            GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
	                            GPIO_Pin_4  | GPIO_Pin_5;

	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/*-- FSMC Configuration ------------------------------------------------------*/
	//FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);

	p.FSMC_AddressSetupTime = 2;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 2;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/*!< Enable FSMC Bank1_SRAM1 Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/**
  * @brief  Writes a Half-word buffer to the FSMC SRAM memory.
  * @param  pBuffer : pointer to buffer.
  * @param  WriteAddr : SRAM memory internal address from which the data will be
  *         written.
  * @param  NumHalfwordToWrite : number of half-words to write.
  * @retval None
  */
void sram_write_buffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
	for (; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /* while there is data to write */
	{
		/* Transfer data to the memory */
		*(uint16_t *) (Bank1_SRAM_ADDR + WriteAddr) = *pBuffer++;

		/* Increment the address*/
		WriteAddr += 2;
	}
}

/**
  * @brief  Reads a block of data from the FSMC SRAM memory.
  * @param  pBuffer : pointer to the buffer that receives the data read from the
  *         SRAM memory.
  * @param  ReadAddr : SRAM memory internal address to read from.
  * @param  NumHalfwordToRead : number of half-words to read.
  * @retval None
  */
void sram_read_buffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
	for (; NumHalfwordToRead != 0; NumHalfwordToRead--) /* while there is data to read */
	{
		/* Read a half-word from the memory */
		*pBuffer++ = *(__IO uint16_t*) (Bank1_SRAM_ADDR + ReadAddr);

		/* Increment the address*/
		ReadAddr += 2;
	}
}



/**
  * @brief  Writes a Half-word buffer to the FSMC SRAM memory.
  * @param  pBuffer : pointer to buffer.
  * @param  WriteAddr : SRAM memory internal address from which the data will be
  *         written.
  * @param  NumHalfwordToWrite : number of half-words to write.
  * @retval None
  */
void sram_write_byte(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
	for (; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /* while there is data to write */
	{
		/* Transfer data to the memory */
		*(uint8_t *) (Bank1_SRAM_ADDR + WriteAddr) = *pBuffer++;

		/* Increment the address*/
		WriteAddr += 1;
	}
}

/**
  * @brief  Reads a block of data from the FSMC SRAM memory.
  * @param  pBuffer : pointer to the buffer that receives the data read from the
  *         SRAM memory.
  * @param  ReadAddr : SRAM memory internal address to read from.
  * @param  NumHalfwordToRead : number of half-words to read.
  * @retval None
  */
void sram_read_byte(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
	for (; NumHalfwordToRead != 0; NumHalfwordToRead--) /* while there is data to read */
	{
		/* Read a half-word from the memory */
		*pBuffer++ = *(__IO uint8_t*) (Bank1_SRAM_ADDR + ReadAddr);

		/* Increment the address*/
		ReadAddr += 1;
	}
}



u32 fsmc_sram_test(u32 start_add, u32 size)
{
	u32  ret = 1;
	vu32 addr;

	// check param
	assert_param(start_add + size <= ISSI_SRAM_SIZE);

	for( addr = start_add + Bank1_SRAM_ADDR; addr < Bank1_SRAM_ADDR + start_add + size + 1; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}

	for( addr = start_add + Bank1_SRAM_ADDR; addr < Bank1_SRAM_ADDR + start_add + size + 1; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}

	if( addr >=  Bank1_SRAM_ADDR + start_add + size + 1 )
	{
	    ret = 0;
	}
	else
	{
		ret = addr;
	}

	return ret;
}


#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)

#pragma arm section zidata = "ESRAM_ARRAY"
u32 external_sram_test_buffer[0x1000];
#pragma arm section zidata

u32 get_fsmc_sram_bandwidth(u8 write_read_flag)
{
    u32 read_data = 0;
    u32 i, j;
    u32 os_tick1, os_tick2, os_tick;
    double speed;

    read_data = read_data;

    if (write_read_flag == 1)
    {
        os_tick1 = OSTimeGet();
        for (j = 0; j < 10000; j++)
        {
            for (i = 0; i < 0x1000; i++)
            {
                external_sram_test_buffer[i] = i;
            }
        }
        
        os_tick2 = OSTimeGet();
        os_tick  = os_tick2 - os_tick1;
        speed = (0x1000 * 4.0 * 10000.0 / (os_tick * 1024.0)) * 1000.0;
    }
    else
    {
        os_tick1 = OSTimeGet();
        for (j = 0; j < 10000; j++)
        {
            for (i = 0; i < 0x1000; i++)
            {
                read_data = external_sram_test_buffer[i];
            }
        }
        
        os_tick2 = OSTimeGet();
        os_tick  = os_tick2 - os_tick1;
        speed = (0x1000 * 4.0 * 10000.0 / (os_tick * 1024.0)) * 1000.0;
    }

    return (u32)speed;
}

#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */


#endif /* USE_ISSI_SRAM_FTR */

