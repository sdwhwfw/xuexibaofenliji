/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : sram_test.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/06/29
 * Description        : This file contains the software test for the
 *                      sram unit
 *******************************************************************************
 * History:
 * 2013/06/29 v1.0
 *******************************************************************************/
#ifndef _SRAM_TEST_H_
#define _SRAM_TEST_H_

#include "stm32f2xx.h"

#include "sram.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>


/*
 * call this function in main if you want to test sram
 */
void sram_test_process(void);






#endif /* _SRAM_TEST_H_ */

