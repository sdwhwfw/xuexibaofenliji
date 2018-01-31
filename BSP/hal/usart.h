/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : usart.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/04/15
 * Description        : This file contains the driver implementation for the
 *                      USART unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/04/15 | v1.0  |            | initial released
 * 2013/09/24 | v1.1  | Bruce.zhu  | change USART framework with DMA feature
 *******************************************************************************/

#ifndef __USART_H__
#define __USART_H__

#include "stm32f2xx.h"


typedef enum
{
	COM1 = 0,
	COM2 = 1,
	COM3 = 2,
	COM4 = 3,
	COM5 = 4,
	COM6 = 5,
	COM_NUM
} uart_num_def_t;

#define IS_USART_COM_NUM(COM) (((COM) == COM1)  || \
                                ((COM) == COM2) || \
                                ((COM) == COM3) || \
                                ((COM) == COM4) || \
                                ((COM) == COM5) || \
                                ((COM) == COM6))

typedef enum
{
	PARITY_NONE,
	PARITY_EVEN,
	PARITY_ODD
} parity_mode;

#define IS_USART_COM_PARITY(PAR) (((PAR) == PARITY_NONE)  || \
                                 ((PAR) == PARITY_EVEN)   || \
                                 ((PAR) == PARITY_ODD))


typedef enum
{
	USART_MODE_INT,
	USART_MODE_DMA
} usart_mode_type;

#define IS_USART_DMA_MODE(mode) (((mode) == USART_MODE_INT)  || \
                     	        ((mode) == USART_MODE_DMA))


uint8_t init_usart(uart_num_def_t COMx, u32 baud_rate, u8 word_length, parity_mode parity, usart_mode_type usart_mode);

void usart_send_data(uart_num_def_t COMx, uint8_t ch);
void usart_send_string(uart_num_def_t COMx, uint8_t* str);

void reload_usart_dma_rx(uart_num_def_t COMx, u8* p_data, u16 rx_size);
void reload_usart_dma_tx(uart_num_def_t COMx, u8* p_data, u16 size);

#endif /* __USART_H__ */

