/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : comx100.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/25
 * Description        : This file contains the software implementation for the
 *                      COMX100 module unit
 *******************************************************************************
 * History:
 * 2013/07/25 v1.0
 * 2013/07/30 v1.1 change function declaration for cifX toolkit
 *******************************************************************************/
#ifndef _COMMX100_H_
#define _COMMX100_H_

#include "stm32f2xx.h"


void COMX100_init(void);
void* COMX100_write_byte(void* pvDevInstance, uint32_t ulAddr, void* pvData, uint32_t ulLen);
void* COMX100_read_byte(void* pvDevInstance, uint32_t ulAddr, void* pvData, uint32_t ulLen);
void start_comX100_module(void);


#endif /* _COMMX100_H_ */

