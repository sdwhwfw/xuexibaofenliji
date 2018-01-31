/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_usart_dma_route.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/26
 * Description        : This file contains the software implementation for the
 *                      usart dma routing data task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/26 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#ifndef _APP_USART_DMA_ROUTE_H_
#define _APP_USART_DMA_ROUTE_H_

#include "stm32f2xx.h"
#include "usart.h"

/*
 * callback function definition
 */
typedef u8 (*p_usart_dma_recv)(u8* p_data, u16 len);


/**
  * @brief: init usart dma module
  *
  */
void init_usart_dma_route_task(void);


/**
 * @brief: call this function to init callback function
 */
void init_usart_with_recv_callback(uart_num_def_t uart_num, p_usart_dma_recv p_fun, u32 baud_rate, parity_mode even_flag);


/**
 * @brief: Use DMA send cmd data, so it may not send complete after
 *         this function return.
 * @param: uart, COM1-COM5
 * @param: p_cmd, pointer to your send data
 * @param: cmd_len, send cmd data length,
 *                  please Not more than USART3_DMA_RECV_SIZE bytes.
 * @retval 0 ------> success
 *         other --> error
 */
u8 send_data_packet(uart_num_def_t uart, u8* p_cmd, u16 cmd_len);





#endif /* _APP_USART_DMA_ROUTE_H_ */

