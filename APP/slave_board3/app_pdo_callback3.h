/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pdo_callback3.h
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      callback function of PDO
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#ifndef _APP_PDO_CALLBACK3_H_
#define _APP_PDO_CALLBACK3_H_


//
// NOTE: This part of RPDO and TPDO code maybe need to be
//       merged to other app task in the future
//
u8 RPDO1_callback(u8* p_data, u16 len);
u8 RPDO2_callback(u8* p_data, u16 len);
u8 RPDO3_callback(u8* p_data, u16 len);
u8 RPDO4_callback(u8* p_data, u16 len);
u8 RPDO5_callback(u8* p_data, u16 len);

u8 TPDO_callback(u8* p_data, u16 len);




#endif

