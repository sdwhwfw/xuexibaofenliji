/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : iap.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/06/21
 * Description        : This file contains the software implementation for the
 *                      iap unit
 *******************************************************************************
 * History:
 * 2013/06/21 v1.0
 *******************************************************************************/
#ifndef _IAP_H_
#define _IAP_H_

#include "stm32f2xx.h"




typedef  void (*p_app_fun)(void);

void IAP_process(void);




#endif /* _IAP_H_ */

