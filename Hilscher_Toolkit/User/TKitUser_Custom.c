/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: TKitUser.c $
   Last Modification:
    $Author: Robert $
    $Modtime: 6.11.09 15:55 $
    $Revision: 2785 $
   
   Targets:
     None
 
   Description:
     USER implemented functions called by the cifX Toolkit. 
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        07.08.2006  MT       initial version
 
**************************************************************************************/

/*****************************************************************************/
/*! \file TKitUser.c                                                         *
*    USER implemented functions called by the cifX Toolkit                   */
/*****************************************************************************/

/* Standard includes */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* STM32 includes */
#include "trace.h"

/* Toolkit includes */
#include "cifXToolkit.h"
#include "cifXErrors.h"

//#error "Implement target system specifc user functions in this file"

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_USER User specific implementation
*    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Returns the number of firmware files associated with the card/channel,
*   passed as argument.
*   \param ptDevInfo DeviceInfo including channel number, for which the
*                    firmware file count should be read
*   \return number for firmware files, to download. The returned value will
*           be used as maximum value for ulIdx in calls to
*           USER_GetFirmwareFile                                             */
/*****************************************************************************/
uint32_t USER_GetFirmwareFileCount(PCIFX_DEVICE_INFORMATION ptDevInfo)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	return 0;
}

/*****************************************************************************/
/*! Returns firmware file information for the given device/channel and Idx
*   passed as argument.
*   \param ptDevInfo  DeviceInfo including channel number, for which the
*                     firmware file should be delivered
*   \param ulIdx      Index of the returned file
*                     (0..USER_GetFirmwareFileCount() - 1)
*   \param ptFileInfo Short and full file name of the firmware. Used in
*                     calls to OS_OpenFile()
*   \return !=0 on success                                                   */
/*****************************************************************************/
int32_t USER_GetFirmwareFile(PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulIdx, PCIFX_FILE_INFORMATION ptFileInfo)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	UNREFERENCED_PARAMETER(ulIdx);
	UNREFERENCED_PARAMETER(ptFileInfo);
	return 0;
}

/*****************************************************************************/
/*! Returns the number of configuration files associated with the card/
*   channel, passed as argument.
*   \param ptDevInfo DeviceInfo including channel number, for which the
*                    configuration file count should be read
*   \return number for confgiuration files, to download. The returned value
*           will be used as maximum value for ulIdx in calls to
*           USER_GetConfgirationFile                                         */
/*****************************************************************************/
uint32_t USER_GetConfigurationFileCount(PCIFX_DEVICE_INFORMATION ptDevInfo)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	return 0;
}

/*****************************************************************************/
/*! Returns configuration file information for the given device/channel and
*   Idx passed as argument.
*   \param ptDevInfo  DeviceInfo including channel number, for which the
*                     configuration file should be delivered
*   \param ulIdx      Index of the returned file
*                     (0..USER_GetConfigurationFileCount() - 1)
*   \param ptFileInfo Short and full file name of the configuration. Used in
*                     calls to OS_OpenFile()
*   \return !=0 on success                                                   */
/*****************************************************************************/
int32_t USER_GetConfigurationFile(PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulIdx, PCIFX_FILE_INFORMATION ptFileInfo)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	UNREFERENCED_PARAMETER(ulIdx);
	UNREFERENCED_PARAMETER(ptFileInfo);

	return 0;
}

/*****************************************************************************/
/*! Returns OS file information for the given device/channel and Idx
*   passed as argument. This is needed for module downloading
*   \param ptDevInfo  DeviceInfo for which the
*                     firmware file should be delivered
*   \param ptFileInfo Short and full file name of the firmware. Used in
*                     calls to OS_OpenFile()
*   \return != 0 on success                                                   */
/*****************************************************************************/
int32_t USER_GetOSFile(PCIFX_DEVICE_INFORMATION ptDevInfo, PCIFX_FILE_INFORMATION ptFileInfo)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	UNREFERENCED_PARAMETER(ptFileInfo);

	return 0;
}

/*****************************************************************************/
/*! Retrieve the full file name of the cifX Romloader binary image
*   \param ptDevInstance  Pointer to the device instance
*   \param ptFileInfo Short and full file name of the bootloader. Used in
*                     calls to OS_OpenFile()
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
void USER_GetBootloaderFile(PDEVICEINSTANCE ptDevInstance, PCIFX_FILE_INFORMATION ptFileInfo)
{
	UNREFERENCED_PARAMETER(ptDevInstance);
	UNREFERENCED_PARAMETER(ptFileInfo);
}

/*****************************************************************************/
/*! Retrieve the alias name of a cifX Board depending on the Device and
*   Serialnumber passed during this call
*   \param ptDevInfo Device- and Serial number of the card
*   \param ulMaxLen  Maximum length of alias
*   \param szAlias   Buffer to copy alias to. A string of length 0 means
*                    no alias                                                */
/*****************************************************************************/
void USER_GetAliasName(PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulMaxLen, char* szAlias)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	UNREFERENCED_PARAMETER(ulMaxLen);
	UNREFERENCED_PARAMETER(szAlias);
}


/*****************************************************************************/
/*! Read the warmstart data from a given warmstart file
*   \param ptDevInfo Device- and Serial number of the card
*   \param ptPacket  Buffer for the warmstart packet                         */
/*****************************************************************************/
int32_t USER_GetWarmstartParameters(PCIFX_DEVICE_INFORMATION ptDevInfo, CIFX_PACKET* ptPacket)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	UNREFERENCED_PARAMETER(ptPacket);
	return 0;
}

/*****************************************************************************/
/*! User trace function
*   right while cifXTKitAddDevice is being processed
*   \param ptDevInstance  Device instance
*   \param ulTraceLevel   Trace level
*   \param szFormat       Format string                                      */
/*****************************************************************************/
void USER_Trace(PDEVICEINSTANCE ptDevInstance, uint32_t ulTraceLevel, const char* szFormat, ...)
{
	char abBuf[128] = {0};
	va_list vaList;

	if(g_ulTraceLevel & ulTraceLevel)
	{
		va_start(vaList, szFormat);
		vsnprintf(abBuf, sizeof(abBuf), szFormat, vaList);
		va_end(vaList);
		if (strlen(abBuf) > 2)
			TICK_TRACE("%s\r\n", abBuf);
	}

	UNREFERENCED_PARAMETER(ptDevInstance);
}

/*****************************************************************************/
/*! Check if interrupts should be enabled for this device
*   \param ptDevInfo  Device Infotmation
*   \return !=0 to enable interrupts                                         */
/*****************************************************************************/
int32_t USER_GetInterruptEnable(PCIFX_DEVICE_INFORMATION ptDevInfo)
{
	UNREFERENCED_PARAMETER(ptDevInfo);
	return 1;
}

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
