/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "stm32f2xx.h"
#include "spi_flash.h"
#include "trace.h"


/* Definitions of physical drive number for each media */
#define SPI_FLASH		           0



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = STA_NOINIT;
	u32     flash_id;

	switch (pdrv)
	{
		case SPI_FLASH:
			{
				spi_flash_init();
				flash_id = read_flash_id();
				if (AT45DB321D == flash_id)
				{
					stat = 0;
				}
			}
			break;

		default:
			break;
	}

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	if (check_flash_status() == 1)
		return 0;
	else
		return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res = RES_PARERR;
	u32     i;

	switch (pdrv)
	{
		case SPI_FLASH:
			for (i = 0; i < count; i++)
			{
				continuous_array_read(sector, 0, (u8*)buff, FLASH_PAGE_SIZE);
				sector++;
				buff += FLASH_PAGE_SIZE;
			}
			res = RES_OK;
			break;

		default:
			break;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res = RES_PARERR;
	u32     i;
	u8* 	p;

	p = (u8*)buff;

#if 0
	APP_TRACE("==== disk_write [%d,%d] ====\r\n", sector, count);
	DumpData((u8*)buff, 512*count);
#endif

	switch (pdrv)
	{
		case SPI_FLASH:
			for (i = 0; i < count; i++)
			{
				// first write data to FLASH internal buffer 1
				flash_buffer_write(1, 0, (u8*)p, FLASH_PAGE_SIZE);
				// second program buffer 1 to FLASH with erase
				program_buf_to_flash(1, sector + i);
				// increase page address
				p += FLASH_PAGE_SIZE;
			}

#if 0
			p = (u8*)buff;
			for (i = 0; i < count * 2; i++)
			{
				continuous_array_read(sector + i, 0, (u8*)p, AT45DB041D_PAGE_SIZE);
				p += AT45DB041D_PAGE_SIZE;
			}

			APP_TRACE("==== read [%d,%d] ====\r\n", sector, count);
			DumpData((u8*)buff, 512*count);
#endif

			res = RES_OK;
			break;
		default:
			break;
	}

	return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;

	switch (cmd)
	{
		case CTRL_SYNC:
			res = RES_OK;
			break;
		case GET_SECTOR_SIZE:
			*(DWORD*)buff = FLASH_PAGE_SIZE;
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE:
			*(DWORD*)buff = 512;
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:
			*(DWORD*)buff = FLASH_SECTOR_COUNT;
			res = RES_OK;
			break;
		default:
			break;
	}

	return res;
}
#endif



DWORD get_fattime (void)
{
	return 0;
}



