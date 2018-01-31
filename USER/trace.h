/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : trace.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/08
 * Description        : This file contains the software implementation for the
 *                      trace unit
 *******************************************************************************
 * History:
 * 2013/08/08 v1.0
 * 2013/08/17 v1.1 change file name from "app_trace.h" to "trace.h"
 *                 and put this module to the USER directory
 *******************************************************************************/
#ifndef __TRACE_H__
#define __TRACE_H__

#include "stm32f2xx.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


//
// Note: We can use APP_TRACE() or TICK_TRACE() in our application task
//       if we use uCOS-II. Otherwise we use TRACE() only
//
#if defined(_USE_UCOS_FTR_)

/* Exported macro for TRACE --------------------------------------------*/
#if defined(USE_USART1_TO_DEBUG) || defined(USE_USART3_TO_DEBUG)

#define APP_TRACE(...)         app_traceInfo_output(__VA_ARGS__)
#define TICK_TRACE(...)        app_tick_trace_output(__VA_ARGS__)
#define NO_BLOCKING(expr)      ((expr) ? (void)0 : app_no_blocking((uint8_t *)__FILE__, __LINE__))
#define BLOCKING(expr)         ((expr) ? (void)0 : app_blocking((uint8_t *)__FILE__, __LINE__))
#define DUMP_DATA(p_data, len) DumpData(p_data, len);

#else /* no debug message output */

#define APP_TRACE(...)
#define TICK_TRACE(...)
#define NO_BLOCKING(expr)
#define BLOCKING(expr)
#define DUMP_DATA(p_data, len)

#endif


#ifdef USE_UART3_TRACE_FTR
/* Exported macro for UART3 TRACE --------------------------------------------*/
#define APP_TRACE3(...) app_traceInfo_output3(__VA_ARGS__)
#define TICK_TRACE3(...) app_tick_trace_output3(__VA_ARGS__)
#endif /* USE_UART3_TRACE_FTR */


/* Exported function ----------------------------------------------------------*/
void app_traceInfo_output(const char* fmt, ...);
void app_tick_trace_output(const char* fmt, ...);
void app_no_blocking(uint8_t *, uint32_t);
void app_blocking(uint8_t *, uint32_t);


#ifdef USE_UART3_TRACE_FTR
/* Exported function ---------------------------------------------------------*/
void app_traceInfo_output3(const char* fmt, ...);
void app_tick_trace_output3(const char* fmt, ...);
#endif /* USE_UART3_TRACE_FTR */

#endif /* _USE_UCOS_FTR_ */


/**
  * UART1
  */
#define TRACE(...)   traceInfo_output(__VA_ARGS__)

void init_trace(void);
void traceInfo_output(const char* fmt, ...);
void DumpData(uint8_t* pbData, uint32_t ulDataLen);

/**
  * UART3
  */
#ifdef USE_UART3_TRACE_FTR
#define TRACE3(...)  traceInfo_output3(__VA_ARGS__)

void init_trace3(void);
void DumpData3(uint8_t* pbData, uint32_t ulDataLen);
void traceInfo_output3(const char* fmt, ...);
#endif /* USE_UART3_TRACE_FTR */






#endif /* __TRACE_H__ */

