/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : spi_flash.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/21
 * Description        : This file contains the device driver implementation for
 *                      the SPI FLASH unit
 *******************************************************************************
 * History:
 * 2013/08/21 v1.0
 *******************************************************************************/

#include "spi_flash.h"
#include "trace.h"


#define FLASH_DUMMY_BYTE                 0xFF
#define BUF1_WRITE                       0x84
#define BUF2_WRITE                       0x87
#define BUF1_READ                        0xD4
#define BUF2_READ                        0xD6
#define BBUF1_TO_MM_PAGE_PROG_WITH_ERASE 0x83
#define BBUF2_TO_MM_PAGE_PROG_WITH_ERASE 0x86
#define MM_PAGE_TO_B1                    0x53
#define MM_PAGE_TO_B2                    0x55
#define PAGE_ERASE                       0x81   // 512/528 bytes per page
#define SECTOR_ERASE                     0x7C   // 128k bytes per sector
#define READ_STATE_REGISTER              0xD7
#define READ_FLASH_ID                    0x9F
#define CONTINUOUS_ARRAY_READ_LEGACY     0xE8
#define CONTINUOUS_ARRAY_READ            0x0B
#define CONTINUOUS_ARRAY_READ_LOW        0x03



static u8 spi_flash_init_flag = 0;

#define FLASH_CS_ENABLE()                spi_cs_low()
#define FLASH_CS_DISABLE()               spi_cs_high()


static u16 read_flash_status_reg(void);



void spi_flash_init(void)
{
	if (spi_flash_init_flag == 0)
	{
		spi_init();
		spi_flash_init_flag = 1;
	}
}


/**
  * check flash status and waiting if flash is busy
  */
u8 check_flash_status(void)
{
	vu16 status = 0;

	if (spi_flash_init_flag)
	{
		do
		{
			status = read_flash_status_reg() & 0x0080;
		}
		while(status == 0);
	}

	return spi_flash_init_flag;
}


u32 read_flash_id(void)
{
	vu8 status = 0;
	u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0, Temp3 = 0;

	// check FLASH status
	check_flash_status();

	/*!< Select the FLASH: Chip Select low */
	FLASH_CS_ENABLE();

	/*!< Send "RDID" instruction */
	spi_send_byte(READ_FLASH_ID);

	/*!< Read a byte from the FLASH */
	Temp0 = spi_read_byte();
	/*!< Read a byte from the FLASH */
	Temp1 = spi_read_byte();
	/*!< Read a byte from the FLASH */
	Temp2 = spi_read_byte();
	/*!< Read a byte from the FLASH */
	Temp3 = spi_read_byte();

	/*!< Deselect the FLASH: Chip Select high */
	FLASH_CS_DISABLE();

	Temp = (Temp0 << 24) | (Temp1 << 16) | (Temp2 << 8) | Temp3;

	return Temp;
}



/******************************************************************************/
/*Status Register Format:                                                     */
/*   -----------------------------------------------------------------------  */
/*  |  bit7  |  bit6  |  bit5  |  bit4  |  bit3  |  bit2  |  bit1  |  bit0  | */
/*  |--------|--------|--------|--------|--------|--------|--------|--------| */
/*  |RDY/BUSY|  COMP  |   0    |   1    |   1    |   1    |   X    |   X    | */
/*   -----------------------------------------------------------------------  */
/*  bit7 - 忙标记，0为忙1为不忙。                                             */
/*         当Status Register的位0移出之后，接下来的时钟脉冲序列将使SPI器件继续*/
/*         将最新的状态字节送出。                                             */
/*  bit6 - 标记最近一次Main Memory Page和Buffer的比较结果，0相同，1不同。     */
/*  bit5                                                                      */
/*  bit4                                                                      */
/*  bit3                                                                      */
/*  bit2 - 这4位用来标记器件密度，对于AT45DB041B，这4位应该是0111，一共能标记 */
/*         16种不同密度的器件。                                               */
/*  bit1                                                                      */
/*  bit0 - 这2位暂时无效                                                      */
/******************************************************************************/
u16 read_flash_status_reg(void)
{
	u16 status = 0;

	FLASH_CS_ENABLE();
	spi_send_byte(READ_STATE_REGISTER);
	status = spi_read_byte();
	FLASH_CS_DISABLE();

	return status;
}


/**
  * PA:    page address
  * BA:    Byte address
  * p_buf: store data buffer
  * len:   read data len
  */
void continuous_array_read(u32 PA, u32 BA, u8 *p_buf, u32 len)
{
	u32 i = 0;

	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	spi_send_byte(CONTINUOUS_ARRAY_READ);
	// 0 0 0 0 PA10 PA9 PA8 PA7
	spi_send_byte((u8)(PA >> 6));
	// PA6 PA5 PA4 PA3 PA2 PA1 PA0 BA8 (offset for DataFlash Page Size = 264 Bytes)
	spi_send_byte((u8)((PA << 2) | (BA >> 8)));
	// BA7 BA6 BA5 BA4 BA3 BA2 BA1 BA0
	spi_send_byte((u8)BA);
	spi_send_byte(FLASH_DUMMY_BYTE);

	for(i = 0; i < len; i++)
	{
		p_buf[i] = (u8)spi_read_byte();
	}

	FLASH_CS_DISABLE();

}

/**
  * buffer: FLASH internal SRAM buffer number(1 or 2)
  * BA:     Byte address
  * p_buf:  data pointer
  * len:    data len
  */
void flash_buffer_write(u8 buffer, u32 BA, u8 *p_buf, u16 len)
{
	vu32 i;

	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	switch(buffer)
	{
		case 1:
			spi_send_byte(BUF1_WRITE);
			break;
		case 2:
			spi_send_byte(BUF2_WRITE);
			break;
		default:
			break;
	}

	// x x x x x x x x
	spi_send_byte((u8)0);
	// x x x x x x x BA8
	spi_send_byte((u8)(BA >> 8));
	// BA7 BA6 BA5 BA4 BA3 BA2 BA1 BA0
	spi_send_byte((u8)BA);

	for(i = 0; i < len; i++)
	{
		spi_send_byte(p_buf[i]);
	}

	FLASH_CS_DISABLE();

	// delay for some time
	for(i = 0; i < 0x0a; i++);
}


/**
  * Read data from FLASH SRAM buffer
  */
void read_flash_buffer(u8 buffer, u32 BA, u8 *p_buf, u32 len)
{
	u32 i;

	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	switch(buffer)
	{
		case 1:
			spi_send_byte(BUF1_READ);
			break;
		case 2:
			spi_send_byte(BUF2_READ);
			break;
		default:
			break;
	}

	spi_send_byte((u8)0);
	spi_send_byte((u8)(BA >> 8));
	spi_send_byte((u8)BA);

	spi_send_byte(FLASH_DUMMY_BYTE);

	for(i = 0; i < len; i++)
	{
		p_buf[i] = (u8)spi_read_byte();
	}

	FLASH_CS_DISABLE();
}


/**
  * Copy flash page to internal SRAM buffer
  */
void copy_page_to_buffer(u8 buf_num, u32 PA)
{
	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	switch(buf_num)
	{
		case 1:
			spi_send_byte(MM_PAGE_TO_B1);
			break;
		case 2:
			spi_send_byte(MM_PAGE_TO_B2);
			break;
	}

	spi_send_byte((u8)(PA >> 6));
	spi_send_byte((u8)(PA << 2));
	spi_send_byte(FLASH_DUMMY_BYTE);
	FLASH_CS_DISABLE();
}


/**
  * Program buffer data to FLASH
  */
void program_buf_to_flash(u8 buffer, u32 PA)
{
	vu16 i = 0;

	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	switch(buffer)
	{
		case 1:
			spi_send_byte(BBUF1_TO_MM_PAGE_PROG_WITH_ERASE);
			break;
		case 2:
			spi_send_byte(BBUF2_TO_MM_PAGE_PROG_WITH_ERASE);
			break;
	}

	spi_send_byte((u8)(PA >> 6));
	spi_send_byte((u8)(PA << 2));
	spi_send_byte(FLASH_DUMMY_BYTE);

	FLASH_CS_DISABLE();

	// delay some time
	for(i = 0; i < 0xff; i++);
}



/**
  * The entire main memory can be erased at one time by using the Chip Erase command.
  */
void chip_erase(void)
{
	vu32 i = 0;

	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	spi_send_byte(0xC7);
	spi_send_byte(0x94);
	spi_send_byte(0x80);
	spi_send_byte(0x9A);
	FLASH_CS_DISABLE();

	// delay some time
	for(i = 0; i < 0x0a; i++);

	// waiting for flash erase compele
	check_flash_status();
}


void page_erase(u32 PA)
{
	vu32 i = 0;

	// check FLASH status
	check_flash_status();

	FLASH_CS_ENABLE();
	spi_send_byte(PAGE_ERASE);
	spi_send_byte((u8)(PA >> 6));
	spi_send_byte((u8)(PA << 2));
	spi_send_byte(FLASH_DUMMY_BYTE);

	FLASH_CS_DISABLE();

	// delay some time
	for(i = 0; i < 0x0a; i++);

	// waiting for flash erase compele
	check_flash_status();
}





