/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: OS_Win32.c $
   Last Modification:
    $Author: Robert $
    $Modtime: 28.04.09 14:49 $
    $Revision: 3175 $
   
   Targets:
    None
 
   Description:
     Target system abstraction layer
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         07.08.2006  MT       initial version
     
**************************************************************************************/

/*****************************************************************************/
/*! \file OS_Custom.c
*    Sample Target system abstraction layer. Implementation must be done
*    according to used target system                                         */
/*****************************************************************************/

/* Standard includes */
#include <ctype.h>
#include <stdlib.h> // for malloc...
#include <string.h> // for memcpy...

/* STM32 includes */
#include "stm32f2xx.h"


/* uC/OS-II includes */
#include "app_cfg.h"
#include "ucos_ii.h"

/* Toolkit includes */
#include "OS_Dependent.h"
#include "cifXErrors.h"
#include "OS_Includes.h"
#include "cifXToolkit.h"
#include "app_comX100.h"
#include "trace.h"


//#error "Implement target system abstraction in this file"

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
*    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Memory allocation function
*   \param ulSize    Length of memory to allocate
*   \return Pointer to allocated memory                                      */
/*****************************************************************************/
void* OS_Memalloc(uint32_t ulSize)
{
	return (void*)malloc(ulSize);
}

/*****************************************************************************/
/*! Memory freeing function
*   \param pvMem Memory block to free                                        */
/*****************************************************************************/
void OS_Memfree(void* pvMem)
{
	free(pvMem);
}

/*****************************************************************************/
/*! Memory reallocating function (used for resizing dynamic toolkit arrays)
*   \param pvMem     Memory block to resize
*   \param ulNewSize new size of the memory block
*   \return pointer to the resized memory block                              */
/*****************************************************************************/
void* OS_Memrealloc(void* pvMem, uint32_t ulNewSize)
{
	return (void*)realloc(pvMem, ulNewSize);
}

/*****************************************************************************/
/*! Memory setting
*   \param pvMem     Memory block
*   \param bFill     Byte to use for memory initialization
*   \param ulSize    Memory size for initialization)                         */
/*****************************************************************************/
void OS_Memset(void* pvMem, uint8_t bFill, uint32_t ulSize)
{
	memset(pvMem, bFill, ulSize);
}

/*****************************************************************************/
/*! Copy memory from one block to another
*   \param pvDest    Destination memory block
*   \param pvSrc     Source memory block
*   \param ulSize    Copy size in bytes                                      */
/*****************************************************************************/
void OS_Memcpy(void* pvDest, void* pvSrc, uint32_t ulSize)
{
	memcpy(pvDest, pvSrc, ulSize);
}

/*****************************************************************************/
/*! Compare two memory blocks
*   \param pvBuf1    First memory block
*   \param pvBuf2    Second memory block
*   \param ulSize    Compare size in bytes
*   \return 0 if both buffers are equal                                      */
/*****************************************************************************/
int OS_Memcmp(void* pvBuf1, void* pvBuf2, uint32_t ulSize)
{
	return memcmp(pvBuf1, pvBuf2, ulSize);
}

/*****************************************************************************/
/*! Move memory
*   \param pvDest    Destination memory
*   \param pvSrc     Source memory
*   \param ulSize    Size in byte to move                                    */
/*****************************************************************************/
void OS_Memmove(void* pvDest, void* pvSrc, uint32_t ulSize)
{
	memmove(pvDest, pvSrc, ulSize);
}


/*****************************************************************************/
/*! Sleep for a specific time
*   \param ulSleepTimeMs  Time in ms to sleep for                            */
/*****************************************************************************/
void OS_Sleep(uint32_t ulSleepTimeMs)
{
	OSTimeDly(ulSleepTimeMs * OS_TICKS_PER_SEC / 1000);
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
*   \return Current counter value (resolution of this value will influence
*           timeout monitoring in driver/toolkit functions(                  */
/*****************************************************************************/
uint32_t OS_GetMilliSecCounter(void)
{
	return OSTimeGet();
}

/*****************************************************************************/
/*! Create an auto reset event
*   \return handle to the created event                                      */
/*****************************************************************************/
void* OS_CreateEvent(void)
{
	OS_EVENT* toolkit_event;

	toolkit_event = (OS_EVENT*)OSSemCreate(0);
	assert_param(toolkit_event);
	return (void*)toolkit_event;
}

/*****************************************************************************/
/*! Set an event
*   \param pvEvent Handle to event being signalled                           */
/*****************************************************************************/
void OS_SetEvent(void* pvEvent)
{
	INT8U ret;

	ret = OSSemPost(pvEvent);
	if (OS_ERR_NONE != ret)
	{
		APP_TRACE("OS_SetEvent ret = %d\r\n", ret);
	}
	assert_param(OS_ERR_NONE == ret);

	OSSemSet((OS_EVENT*)pvEvent, 1, &ret);
}

/*****************************************************************************/
/*! Reset an event
*   \param pvEvent Handle to event being reset                               */
/*****************************************************************************/
void OS_ResetEvent(void* pvEvent)
{
	UNREFERENCED_PARAMETER(pvEvent);
}

/*****************************************************************************/
/*! Delete an event
*   \param pvEvent Handle to event being deleted                             */
/*****************************************************************************/
void OS_DeleteEvent(void* pvEvent)
{
	OS_EVENT* p_event;
	INT8U     error;

	p_event = OSSemDel((OS_EVENT*)pvEvent, OS_DEL_ALWAYS, &error);
	p_event = p_event;
	assert_param(0 == p_event);
}

/*****************************************************************************/
/*! Wait for the signalling of an event
*   \param pvEvent   Handle to event being wait for
*   \param ulTimeout Timeout in ms to wait for event
*   \return 0 if event was signalled                                         */
/*****************************************************************************/
uint32_t OS_WaitEvent(void* pvEvent, uint32_t ulTimeout)
{
	INT8U err;

	OSSemPend((OS_EVENT*)pvEvent, ulTimeout, &err);
	OSSemSet((OS_EVENT*)pvEvent, 0, &err);
	return err;
}

/*****************************************************************************/
/*! Compare two ASCII string
*   \param pszBuf1   First buffer
*   \param pszBuf2   Second buffer
*   \return 0 if strings are equal                                           */
/*****************************************************************************/
int OS_Strcmp(const char* pszBuf1, const char* pszBuf2)
{
	return strcmp(pszBuf1, pszBuf2);
}

/*****************************************************************************/
/*! Compare characters of two strings without regard to case
*   \param pszBuf1   First buffer
*   \param pszBuf2   Second buffer
*   \param ulLen     Number of characters to compare
*   \return 0 if strings are equal                                           */
/*****************************************************************************/
int OS_Strnicmp(const char* pszBuf1, const char* pszBuf2, uint32_t ulLen)
{
	char* szBuff1Copy = malloc(strlen(pszBuf1) + 1);
	char* szBuff2Copy = malloc(strlen(pszBuf2) + 1);
	int   iRet;
	int   iTmp;

	for(iTmp = 0; iTmp < min(strlen(pszBuf1), ulLen); ++iTmp)
	  szBuff1Copy[iTmp] = (char)tolower((int)pszBuf1[iTmp]);

	for(iTmp = 0; iTmp < min(strlen(pszBuf2), ulLen); ++iTmp)
	  szBuff2Copy[iTmp] = (char)tolower((int)pszBuf2[iTmp]);

	iRet = strncmp(szBuff1Copy, szBuff2Copy, ulLen);

	free(szBuff1Copy);
	free(szBuff2Copy);

	return iRet;
}  

/*****************************************************************************/
/*! Query the length of an ASCII string
*   \param szText    ASCII string
*   \return character count of szText                                        */
/*****************************************************************************/
int OS_Strlen(const char* szText)
{
	return (int)strlen(szText);
}

/*****************************************************************************/
/*! Copies one string to another monitoring the maximum length of the target
*   buffer.
*   \param szDest    Destination buffer
*   \param szSource  Source buffer
*   \param ulLength  Maximum length to copy
*   \return pointer to szDest                                                */
/*****************************************************************************/
char* OS_Strncpy(char* szDest, const char* szSource, uint32_t ulLength)
{
	return strncpy(szDest, szSource, ulLength);
}


/*****************************************************************************/
/*! Create an interrupt safe locking mechanism (Spinlock/critical section)
*   \return handle to the locking object                                     */
/*****************************************************************************/
void* OS_CreateLock(void)
{
	OS_EVENT* p_event;

	p_event = OSSemCreate(1);
	assert_param(p_event);

	return (void*)p_event;
}

/*****************************************************************************/
/*! Enter a critical section/spinlock
*   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_EnterLock(void* pvLock)
{
	INT8U err;

	OSSemPend((OS_EVENT*)pvLock, 0, &err);
	assert_param(OS_ERR_NONE == err);
}

/*****************************************************************************/
/*! Leave a critical section/spinlock
*   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_LeaveLock(void* pvLock)
{
	INT8U ret;

	ret = OSSemPost((OS_EVENT*)pvLock);
	ret = ret;
	assert_param(OS_ERR_NONE == ret);
}

/*****************************************************************************/
/*! Delete a critical section/spinlock object
*   \param pvLock Handle to the locking object being deleted                 */
/*****************************************************************************/
void OS_DeleteLock(void* pvLock)
{
	INT8U err;

	OSSemDel((OS_EVENT*)pvLock, OS_DEL_ALWAYS, &err);
	assert_param(OS_ERR_NONE == err);
}

/*****************************************************************************/
/*! Create an Mutex object for locking code sections 
*   \return handle to the mutex object                                       */
/*****************************************************************************/
void* OS_CreateMutex(void)
{
	OS_EVENT* p_event;

	p_event = OSSemCreate(1);
	assert_param(p_event);

	return (void*)p_event;
}

/*****************************************************************************/
/*! Wait for mutex
*   \param pvMutex    Handle to the Mutex locking object
*   \param ulTimeout  Wait timeout                                           
*   \return !=0 on success                                                   */
/*****************************************************************************/
int OS_WaitMutex(void* pvMutex, uint32_t ulTimeout)
{
	INT8U err;

	OSSemPend((OS_EVENT*)pvMutex, ulTimeout, &err);
	assert_param(OS_ERR_NONE == err);

	return 1;
}

/*****************************************************************************/
/*! Release a mutex section section
*   \param pvMutex Handle to the locking object                              */
/*****************************************************************************/
void OS_ReleaseMutex(void* pvMutex)
{
	INT8U ret;

	ret = OSSemPost((OS_EVENT*)pvMutex);
	assert_param(OS_ERR_NONE == ret);
}

/*****************************************************************************/
/*! Delete a Mutex object
*   \param pvMutex Handle to the mutex object being deleted                  */
/*****************************************************************************/
void OS_DeleteMutex(void* pvMutex)
{
	INT8U err;

	OSSemDel((OS_EVENT*)pvMutex, OS_DEL_ALWAYS, &err);
	assert_param(OS_ERR_NONE == err);
}

/*****************************************************************************/
/*! Opens a file in binary mode
*   \param szFile     Full file name of the file to open
*   \param pulFileLen Returned length of the opened file
*   \return handle to the file, NULL mean file could not be opened           */
/*****************************************************************************/
void* OS_FileOpen(char* szFile, uint32_t* pulFileLen)
{
	UNREFERENCED_PARAMETER(szFile);
	UNREFERENCED_PARAMETER(pulFileLen);

	return 0;
}

/*****************************************************************************/
/*! Closes a previously opened file
*   \param pvFile Handle to the file being closed                            */
/*****************************************************************************/
void OS_FileClose(void* pvFile)
{
	UNREFERENCED_PARAMETER(pvFile);
}

/*****************************************************************************/
/*! Read a specific amount of bytes from the file
*   \param pvFile   Handle to the file being read from
*   \param ulOffset Offset inside the file, where read starts at
*   \param ulSize   Size in bytes to be read
*   \param pvBuffer Buffer to place read bytes in
*   \return number of bytes actually read from file                          */
/*****************************************************************************/
uint32_t OS_FileRead(void* pvFile, uint32_t ulOffset, uint32_t ulSize, void* pvBuffer)
{
	UNREFERENCED_PARAMETER(pvFile);
	UNREFERENCED_PARAMETER(ulOffset);
	UNREFERENCED_PARAMETER(ulSize);
	UNREFERENCED_PARAMETER(pvBuffer);
	return 0;
}

/*****************************************************************************/
/*! OS specific initialization (if needed), called during cifXTKitInit()     
 *!  \return CIFX_NO_ERROR on success
 *****************************************************************************/
int32_t OS_Init()
{
	return CIFX_NO_ERROR;
}

/*****************************************************************************/
/*! OS specific de-initialization (if needed), called during cifXTKitInit()  */
/*****************************************************************************/
void OS_Deinit()
{
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
* write back all BAR's (Base address registers), Interrupt and Command
* Register. These registers are invalidated during cifX Reset and need to be
* re-written after the reset has succeeded
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice
*   \param pvPCIConfig   Configuration returned by OS_ReadPCIConfig
*                        (implementation dependent)                          */
/*****************************************************************************/
void OS_WritePCIConfig(void* pvOSDependent, void* pvPCIConfig)
{
	UNREFERENCED_PARAMETER(pvOSDependent);
	UNREFERENCED_PARAMETER(pvPCIConfig);
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
* read all BAR's (Base address registers), Interrupt and Command Register.
* These registers are invalidated during cifX Reset and need to be
* re-written after the reset has succeeded
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice
*   \return pointer to stored register copies (implementation dependent)     */
/*****************************************************************************/
void* OS_ReadPCIConfig(void* pvOSDependent)
{
	UNREFERENCED_PARAMETER(pvOSDependent);
	return 0;
}

/*****************************************************************************/
/*! This function Maps a DPM pointer to a user application if needed.
*   This example just returns the pointer valid inside the driver.
*   \param pvDriverMem   Pointer to be mapped
*   \param ulMemSize     Size to be mapped
*   \param ppvMappedMem  Returned mapped pointer (usable by application)
*   \param pvOSDependent OS Dependent variable passed during call to
*                        cifXTKitAddDevice
*   \return Handle that is needed for unmapping NULL is a mapping failure    */
/*****************************************************************************/
void* OS_MapUserPointer(void* pvDriverMem, uint32_t ulMemSize, void** ppvMappedMem, void* pvOSDependent)
{
	UNREFERENCED_PARAMETER(ulMemSize);
	UNREFERENCED_PARAMETER(pvOSDependent);
	/* We are running in user mode, so it is not necessary to map anything to user space */ 
	*ppvMappedMem = pvDriverMem;

	return pvDriverMem;
}

/*****************************************************************************/
/*! This function unmaps a previously mapped user application pointer 
*   \param phMapping      Handle that was returned by OS_MapUserPointer
*   \param pvOSDependent  OS Dependent variable passed during call to
*                         cifXTKitAddDevice
*   \return !=0 on success                                                   */
/*****************************************************************************/
int OS_UnmapUserPointer(void* phMapping, void* pvOSDependent)
{
	UNREFERENCED_PARAMETER(phMapping);
	UNREFERENCED_PARAMETER(pvOSDependent);
	/* We are running in user mode, so it is not necessary to map anything to user space */ 
	return 1;
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
 *   \param pvOSDependent OS Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   
 *  PE3
 *****************************************************************************/
void OS_EnableInterrupts(void* pvOSDependent)
{
#if defined(_COMX100_MODULE_FTR_)

	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;

	/* Start interrupt service thread */
	create_comX100_isr_task(pvOSDependent);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif /* _COMX100_MODULE_FTR_ */

}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_DisableInterrupts(void* pvOSDependent)
{
	UNREFERENCED_PARAMETER(pvOSDependent);
}


#ifdef CIFX_TOOLKIT_TIME
/*****************************************************************************/
/*! Get System time
 *   \param ptTime   Pointer to store the time value
 *   \return actual time value in seconds sincd 01.01.1970
 *****************************************************************************/
uint32_t OS_Time( uint32_t *ptTime)
{
  if (NULL != ptTime)
    *ptTime = 0;
  
  return 0;
}
#endif

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
