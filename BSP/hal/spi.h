/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : spi.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/21
 * Description        : This file contains the software implementation for the
 *                      SPI unit
 *******************************************************************************
 * History:
 * 2013/08/21 v1.0
 *******************************************************************************/
#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f2xx.h"


//===========================
// SPI
//===========================
void spi_init(void);
void spi_deinit(void);
u16  spi_send_byte(uint8_t byte);
u16  spi_read_byte(void);

/* Select: Chip Select pin low */
void spi_cs_low(void);
/* Deselect: Chip Select pin high */
void spi_cs_high(void);



#endif /* _SPI_H_ */

