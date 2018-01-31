/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cmd_send.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/31
 * Description        : This file contains the software implementation for the
 *                      uart/TPDO send task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/31 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_CMD_SEND_H_
#define _APP_CONTROL_CMD_SEND_H_

#include "struct_control_board_usart.h"

// uart frame size
#define UART_FRAME_TEXT_SIZE    0X08
#define UART_FRAME_ALL_SIZE     0X0D
#define ARM_TPDO_MAX_SIZE       0X32
#define STX                     0X02
#define ETX                     0X03
#define LF                      0X0A
#define ACK                     0X06
#define NAK                     0X15

#define COMM_MODULE_FLAG        0XA5

//#define ACK_WAIT_TIMEOUT        0X12C
#define ACK_WAIT_TIMEOUT        0X320//change by wq  800ms
#define NAK_REPEAT_COUNT        3

/*
 * Function: control_init_send_task
 * Description: Init the send task of arm0
 * Param: None
 * Return: None
*/
void  control_init_send_task(void);

/*
 * Function: uart_send_frame
 * Description: uart send frame call this function
 * Param: text_frame,the frame need to send
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t uart_send_frame(UART_FRAME text_frame);

/*
 * Function: uart_answer_cmd
 * Description: uart answer the uart recv frame
 * Param: ACKorNAK,ack or nak answer
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t uart_answer_cmd(uint8_t ACKorNAK);

/*
 * Function: arm1_send_frame
 * Description: ARM0 send TPDO to ARM1 need to call this function
 * Param: p_data,the whole data; tpdo_offset,the tpdo's position; tpdo_num,tpdo num
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t arm1_send_frame(uint8_t *p_data, uint8_t tpdo_offset, uint8_t tpdo_num);

/*
 * Function: arm2_send_frame
 * Description: ARM0 send TPDO to ARM1 need to call this function
 * Param: p_data,the whole data; tpdo_offset,the tpdo's position; tpdo_num,tpdo num
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t arm2_send_frame(uint8_t *p_data, uint8_t tpdo_offset, uint8_t tpdo_num);

/*
 * Function: arm3_send_frame
 * Description: ARM0 send TPDO to ARM3 need to call this function
 * Param: p_data,the whole data; tpdo_offset,the tpdo's position; tpdo_num,tpdo num
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t arm3_send_frame(uint8_t *p_data, uint8_t tpdo_offset, uint8_t tpdo_num);

/*
 * Function: arm1_answer_cmd
 * Description: arm1 answer the arm1 tpdo5 recv frame
 * Param: arm1_fn,answer cmd's fn; ACKorNAK,ack or nak answer
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t arm1_answer_cmd(uint8_t arm1_fn, uint8_t ACKorNAK);

/*
 * Function: arm2_answer_cmd
 * Description: arm2 answer the arm2 tpdo5 recv frame
 * Param: arm2_fn,answer cmd's fn; ACKorNAK,ack or nak answer
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t arm2_answer_cmd(uint8_t arm2_fn, uint8_t ACKorNAK);

/*
 * Function: arm3_answer_cmd
 * Description: arm3 answer the arm3 tpdo5 recv frame
 * Param: arm3_fn,answer cmd's fn; ACKorNAK,ack or nak answer
 * Return: uint8_t,0-OK,other-Error
*/
uint8_t arm3_answer_cmd(uint8_t arm3_fn, uint8_t ACKorNAK);




#endif /* _APP_CONTROL_CMD_SEND_H_ */

