/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_common_function.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/23
 * Description        : This file contains the common function of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/23 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_COMMON_FUNCTION_H_
#define _APP_CONTROL_COMMON_FUNCTION_H_

#include "stdint.h"

#define FN_CREATE               0x00
#define FN_CHECK                0x01

// enum the fn source
typedef enum FN_SOURCE_ENUM
{
    IPC_DEST,
    ARM1_DEST,
    ARM2_DEST,
    ARM3_DEST
} FN_SOURCE;

/*
 * Function: uart_crc_check_func
 * Description: excute crc check for the uart recv frame
 * Param: pData,the whole recv frame; len,the frame length;
 * Return: uint8_t,0-OK,1-Error;
*/
uint8_t uart_crc_check_func(uint8_t *pData, uint8_t len);

/*
 * Function: uart_crc_generate_func
 * Description: get crc data for the uart send frame
 * Param: pData,the whole send frame; len,the frame length;
 * Return: uint8_t,the crc data;
*/
uint8_t uart_crc_generate_func(uint8_t *pData, uint8_t len);

/*
 * Function: get_current_fn
 * Description: get the FN for the send frame
 * Param: source,where the fn create; bCheck, 
 *       FN_CREATE-create fn,FN_CHECK-get current fn to check
 * Return: uint8_t,the FN;
*/
uint8_t get_current_fn(FN_SOURCE source, uint8_t fn_opt);

/*
 * Function: arm0_get_cur_prio
 * Description: get arm0's current task priority
 * Param: None
 * Return: uint8_t,the priority
*/
uint8_t arm0_get_cur_prio(void);

/*
 * Function: judge_array_null
 * Description: is the array null?
 * Param: p_data,point to array; len,array length
 * Return: uint8_t,1-yes;0-no
*/
uint8_t judge_array_null(uint8_t *p_data, uint8_t len);

/*
 * Function: get_array_max_value
 * Description: get the max value in p_data
 * Param: p_data,point to array; len,array length
 * Return: uint16_t,the max value
*/
uint32_t get_array_max_value(uint32_t *p_data, uint8_t len);

/*
* Function: arm_resend_cmd
* Description: add the cmd tpdo param5 to indicate the resend cmd
* Param: p_data,point to array TPDO; number,tpdo number
* Return: NONE
*/
void arm_resend_cmd(uint8_t *p_data, uint8_t number);

float get_average_value(uint16_t *p_data, uint8_t number);

#endif /* _APP_CONTROL_COMMON_FUNCTION_H_ */

