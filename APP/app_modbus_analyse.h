/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_modbus_analyse.h
 * Author             : WQ
 * Date First Issued  : 2013/08/12
 * Description        : This file contains the software implementation for the
 *                      modbus analyse task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/08/12 | v1.0  | WQ         | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | merge source code to NEW framework
 *******************************************************************************/
#ifndef _APP_MODBUS_ANALYSE_H_
#define _APP_MODBUS_ANALYSE_H_

#include "stm32f2xx.h"

#if defined(SLAVE_BOARD_1)
#define MB_SLAVE_ID            0x02
#define SLAVE_RPDO_NUM         0x05
#elif defined(SLAVE_BOARD_2)
#define MB_SLAVE_ID            0x03
#define SLAVE_RPDO_NUM         0x05
#elif defined(SLAVE_BOARD_3)
#define MB_SLAVE_ID            0x04
#define SLAVE_RPDO_NUM         0x05
#else
	#error "must define one slave board!!!"
#endif /* MB_SLAVE_ID */


/*
 * callback function definition
 */
typedef u8 (*p_RPDO_function)(u8* p_data, u16 len);
typedef u8 (*p_TPDO_function)(u8* p_data, u16 len);


void init_RPDO_call_back(p_RPDO_function* p_fun, u8 RPDO_num);
void init_TPDO_call_back(p_RPDO_function p_fun);


//void modbus_analyse_task(void *p_arg);
void init_modbus_analyse_task(void);



#endif /* _APP_MODBUS_ANALYSE_H_ */

