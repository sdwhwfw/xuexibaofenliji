/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : trace.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/08
 * Description        : This file contains the software implementation for the
 *                      trace unit
 *******************************************************************************
 * History:
 * 2013/08/08 v1.0
 *******************************************************************************/
#include "trace.h"

#include "usart.h"
#include "ucos_ii.h"

#if defined(USE_USART1_TO_DEBUG)
#define TRACE_USART_NUM    COM1

#elif defined(USE_USART3_TO_DEBUG)
    #if defined(USE_UART3_TRACE_FTR)
        #error "Please don't use USART3 to TRACE and DEBUG!!! Only one can be used at one time."
    #endif /* USE_UART3_TRACE_FTR */
#define TRACE_USART_NUM    COM3

#else
	#error "Please don't add trace.c to your project!"
#endif


#if defined(_USE_UCOS_FTR_)
    static OS_EVENT *app_trace_mutex;
    static uint8_t   output_msg[1024];
#ifdef USE_UART3_TRACE_FTR
    static OS_EVENT *app_trace_mutex3;
    static uint8_t   output_msg3[1024];
#endif /* USE_UART3_TRACE_FTR */
#endif /* _USE_UCOS_FTR_ */

static uint8_t   isr_output_msg[1024];

#ifdef USE_UART3_TRACE_FTR
static uint8_t   isr_output_msg3[1024];
#endif /* USE_UART3_TRACE_FTR */


/////////////////////////////////////////////////////////////////
// Function                                                    //
/////////////////////////////////////////////////////////////////
#if defined(_USE_UCOS_FTR_)
/**
  * Displays app debug messages on the console
  * Note: The max debug message is 1024
  *       This function can't be called from isr
  */
void app_traceInfo_output(const char* fmt, ...)
{
	va_list ap;
	INT8U err;
	OSMutexPend(app_trace_mutex, 0, &err);
	assert_param(OS_ERR_NONE == err);
	va_start(ap, fmt);
	vsnprintf((char*)output_msg, sizeof(output_msg), fmt, ap);
	va_end(ap);
	usart_send_string(TRACE_USART_NUM, output_msg);
	err = OSMutexPost(app_trace_mutex);
	assert_param(OS_ERR_NONE == err);
}

/**
  * Displays app debug messages with tick on the console
  * Note: The max debug message is 1024
  *       This function can't be called from isr
  */
void app_tick_trace_output(const char* fmt, ...)
{
	uint8_t vl_tick_buf[32];
	va_list ap;
	INT8U err;
	
	OSMutexPend(app_trace_mutex, 0, &err);
	assert_param(OS_ERR_NONE == err);
	va_start(ap, fmt);
	vsnprintf((char*)output_msg, sizeof(output_msg), fmt, ap);
	va_end(ap);

	/* add tick to debug message */
	snprintf((char*)vl_tick_buf, sizeof(vl_tick_buf), "[%8d]", OSTimeGet());
	usart_send_string(TRACE_USART_NUM, vl_tick_buf);
	usart_send_string(TRACE_USART_NUM, output_msg);
	err = OSMutexPost(app_trace_mutex);
	assert_param(OS_ERR_NONE == err);
}

/**
  * NO_BLOCKING
  */
void app_no_blocking(uint8_t* file, uint32_t line)
{
	uint8_t vl_buf[64];
	INT8U err;

	OSMutexPend(app_trace_mutex, 0, &err);
	assert_param(OS_ERR_NONE == err);

	snprintf((char*)vl_buf, sizeof(vl_buf), "Warn[%d:%s]: ### NO BLOCKING ###\r\n", line, file);
	usart_send_string(TRACE_USART_NUM, vl_buf);

	err = OSMutexPost(app_trace_mutex);
	assert_param(OS_ERR_NONE == err);
}



/**
  * BLOCKING
  */
void app_blocking(uint8_t* file, uint32_t line)
{
	uint8_t vl_buf[64];

	snprintf((char*)vl_buf, sizeof(vl_buf), "Err[%d:%s]: ###!!! BLOCKING !!!###\r\n", line, file);
	usart_send_string(TRACE_USART_NUM, vl_buf);

	for(;;);
}


#ifdef USE_UART3_TRACE_FTR
/**
  * Displays app debug messages on the console
  * Note: The max debug message is 1024
  *       This function can't be called from isr
  */
void app_traceInfo_output3(const char* fmt, ...)
{
	va_list ap;
	INT8U err;

	OSMutexPend(app_trace_mutex3, 0, &err);
	assert_param(OS_ERR_NONE == err);
	va_start(ap, fmt);
	vsnprintf((char*)output_msg3, sizeof(output_msg3), fmt, ap);
	va_end(ap);

	usart_send_string(COM3, output_msg3);
	err = OSMutexPost(app_trace_mutex3);
	assert_param(OS_ERR_NONE == err);
}

/**
  * Displays app debug messages with tick on the console
  * Note: The max debug message is 1024
  *       This function can't be called from isr
  */
void app_tick_trace_output3(const char* fmt, ...)
{
	uint8_t vl_tick_buf[32];
	va_list ap;
	INT8U err;

	OSMutexPend(app_trace_mutex3, 0, &err);
	assert_param(OS_ERR_NONE == err);
	va_start(ap, fmt);
	vsnprintf((char*)output_msg3, sizeof(output_msg3), fmt, ap);
	va_end(ap);

	/* add tick to debug message */
	snprintf((char*)vl_tick_buf, sizeof(vl_tick_buf), "[%8d]", OSTimeGet());
	usart_send_string(COM3, vl_tick_buf);
	usart_send_string(COM3, output_msg3);
	err = OSMutexPost(app_trace_mutex3);
	assert_param(OS_ERR_NONE == err);
}

#endif /* USE_UART3_TRACE_FTR */

#endif /* _USE_UCOS_FTR_ */



/**
  * We use MUTEX to protect debug message in APP_TRACE() in OS environment
  * This is use UART1 to output message.
  */
void init_trace(void)
{
#if defined(_USE_UCOS_FTR_)

	INT8U err;
	
	app_trace_mutex = OSMutexCreate(APP_TRACE_MUTEX_PRIO, &err);
	assert_param(app_trace_mutex);
#endif /* _USE_UCOS_FTR_ */

	init_usart(TRACE_USART_NUM, 115200, 8, PARITY_NONE, USART_MODE_INT);
}



/**
  * Displays isr debug messages on the console
  * Note: The max debug message is 1024
  *       This function can be called from isr
  */
void traceInfo_output(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf((char*)isr_output_msg, sizeof(isr_output_msg), fmt, ap);
	va_end(ap);

	usart_send_string(TRACE_USART_NUM, isr_output_msg);
}


/**
  * Displays a hex dump on the debug console (16 bytes per line)
  * \param pbData     Pointer to dump data
  * \param ulDataLen  Length of data dump
  */
void DumpData(uint8_t* pbData, uint32_t ulDataLen)
{
	uint8_t  vl_buf[16];
	uint32_t ulIdx = 0;
	char     word[16];
	u8       i = 0;

	for(ulIdx = 0; ulIdx < ulDataLen; ++ulIdx)
	{
		if(0 == (ulIdx % 16) && ulIdx)
		{
			for (i = 0; i < 16; i++)
			{
				usart_send_data(TRACE_USART_NUM, word[i]);
			}
			usart_send_string(TRACE_USART_NUM, "\r\n");
		}

		if (pbData[ulIdx] > 31 && pbData[ulIdx] < 127)
		{
			word[ulIdx%16] = pbData[ulIdx];
		}
		else
		{
			word[ulIdx%16] = '.';
		}

		snprintf((char*)vl_buf, sizeof(vl_buf), "%02X ", pbData[ulIdx]);
		usart_send_string(TRACE_USART_NUM, vl_buf);
	}

	if ((ulIdx % 16) == 0)
		ulIdx = 16;
	else
		ulIdx = ulIdx % 16;

	for (i = 0; i < ulIdx; i++)
	{
		usart_send_data(TRACE_USART_NUM, word[i]);
	}

	usart_send_string(TRACE_USART_NUM, "\r\n");
}


#if defined(USE_UART3_TRACE_FTR)
/**
  * We use MUTEX to protect debug message in APP_TRACE() in OS environment
  *
  * Note: This is use UART3 to output message.
  */
void init_trace3(void)
{
#if defined(_USE_UCOS_FTR_)

	INT8U err;

	app_trace_mutex3 = OSMutexCreate(APP_TRACE3_MUTEX_PRIO, &err);
	assert_param(app_trace_mutex3);
#endif /* _USE_UCOS_FTR_ */

	/* usart3 */
	init_usart(COM3, 115200, 8, PARITY_NONE, USART_MODE_INT);
}

/**
  * Displays a hex dump on the debug console (16 bytes per line)
  * \param pbData     Pointer to dump data
  * \param ulDataLen  Length of data dump
  */
void DumpData3(uint8_t* pbData, uint32_t ulDataLen)
{
	uint8_t  vl_buf[16];
	uint32_t ulIdx = 0;

	usart_send_string(COM3, "================ DUMP DATA ================");
	for(ulIdx = 0; ulIdx < ulDataLen; ++ulIdx)
	{
		if(0 == (ulIdx % 16))
			usart_send_string(COM3, "\r\n");

		snprintf((char*)vl_buf, sizeof(vl_buf), "%02X ", pbData[ulIdx]);
		usart_send_string(COM3, vl_buf);
	}
	usart_send_string(COM3, "\r\n");
}

/**
  * Displays isr debug messages on the console
  * Note: The max debug message is 1024
  *       This function can be called from isr
  */
void traceInfo_output3(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf((char*)isr_output_msg3, sizeof(isr_output_msg3), fmt, ap);
	va_end(ap);

	usart_send_string(COM3, isr_output_msg3);
}

#endif /* USE_UART3_TRACE_FTR */


