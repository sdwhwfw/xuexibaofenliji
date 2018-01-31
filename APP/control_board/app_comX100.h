/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_comX100.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/29
 * Description        : This file contains the software implementation for the
 *                      comX100 module app unit
 *******************************************************************************
 * History:
 * 2013/07/29 v1.0
 *******************************************************************************/
#ifndef __APP_COMX100_H__
#define __APP_COMX100_H__

#include "stm32f2xx.h"

#if defined(CONTROL_MAIN_BOARD)
#include "cifXToolkit.h"
#include "app_control_pdo_callback.h"


void init_comX100_module_task(control_pRPDO_function* p_control_callback, u8 call_num);
u8 comX100_send_packet(void* p_data, u8 TPDO_offset, u8 TPDO_num);
void create_comX100_isr_task(void* p_data);


#endif /* _COMX100_MODULE_FTR_ */

#endif /* __APP_COMX100_H__ */

