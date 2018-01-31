/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : spi_flash.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/21
 * Description        : This file contains the software implementation for the
 *                      SPI FLASH unit
 *******************************************************************************
 * History:
 * 2013/08/21 v1.0
 *******************************************************************************/
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "stm32f2xx.h"
#include "spi.h"


#define AT45DB041D         0x1F240000
#define AT45DB321D         0x1F270100


#define FLASH_PAGE_SIZE         512
#define FLASH_SECTOR_COUNT      8192


u8 check_flash_status(void);
void spi_flash_init(void);
u32 read_flash_id(void);
void chip_erase(void);
void page_erase(u32 PA);
void continuous_array_read(u32 PA, u32 BA, u8 *p_buf, u32 len);
void flash_buffer_write(u8 buffer, u32 BA, u8 *p_buf, u16 len);
void program_buf_to_flash(u8 buffer, u32 PA);
void copy_page_to_buffer(u8 buf_num, u32 PA);



#endif /* _SPI_FLASH_H_ */


