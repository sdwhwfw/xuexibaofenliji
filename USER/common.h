/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : common.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/20
 * Description        : This file contains the software implementation for the
 *                      common unit
 *******************************************************************************
 * History:
 * 2013/07/20 v1.0
 *******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H__
#define __COMMON_H__

#include "stm32f2xx.h"



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);

#endif  /* __COMMON_H__ */

