/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : sram.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/06/29
 * Description        : This file contains the software implementation for the
 *                      sram unit
 *******************************************************************************
 * History:
 * 2013/06/29 v1.0
 *******************************************************************************/
#ifndef _SRAM_H_
#define _SRAM_H_

#include "stm32f2xx.h"


/**
  * @brief  FSMC Bank 1 NOR/SRAM
  *         NE1 ---> ((uint32_t)0x60000000)
  *         NE2 ---> ((uint32_t)0x64000000)
  *         NE3 ---> ((uint32_t)0x68000000)
  *
  */
#define Bank1_SRAM_ADDR  ((uint32_t)0x60000000)

/* 512KB */
#define ISSI_SRAM_SIZE   0x80000



void sram_init(void);
void sram_write_buffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void sram_read_buffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);
void sram_write_byte(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void sram_read_byte(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);


/* for test SRAM */
u32 fsmc_sram_test(u32 start_add, u32 size);

#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)
u32 get_fsmc_sram_bandwidth(u8 write_read_flag);
#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */







#endif /* _SRAM_H_ */

