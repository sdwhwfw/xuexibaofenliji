/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_callback.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/18
 * Description        : This file contains the software implementation for the
 *                      RPDO callback function of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/18 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_CALLBACK_H_
#define _APP_CONTROL_CALLBACK_H_

/************** RPDO Function ***********************/
#define CONTROL_RPDO_NUM            0X12

typedef void (*control_pRPDO_function)(u8* pData, u16 len);

/**************** RPDO from AMR1 ****************/
void control_RPDO1_callback(u8 *pData, u16 len);
void control_RPDO2_callback(u8 *pData, u16 len);
void control_RPDO3_callback(u8 *pData, u16 len);
void control_RPDO4_callback(u8 *pData, u16 len);
void control_RPDO5_callback(u8 *pData, u16 len);
void control_RPDO6_callback(u8 *pData, u16 len);

/**************** RPDO from AMR2 ****************/
void control_RPDO7_callback(u8 *pData, u16 len);
void control_RPDO8_callback(u8 *pData, u16 len);
void control_RPDO9_callback(u8 *pData, u16 len);
void control_RPDO10_callback(u8 *pData, u16 len);
void control_RPDO11_callback(u8 *pData, u16 len);
void control_RPDO12_callback(u8 *pData, u16 len);

/**************** RPDO from AMR3 ****************/
void control_RPDO13_callback(u8 *pData, u16 len);
void control_RPDO14_callback(u8 *pData, u16 len);
void control_RPDO15_callback(u8 *pData, u16 len);
void control_RPDO16_callback(u8 *pData, u16 len);
void control_RPDO17_callback(u8 *pData, u16 len);
void control_RPDO18_callback(u8 *pData, u16 len);

#endif /* _APP_CONTROL_CALLBACK_H_ */

