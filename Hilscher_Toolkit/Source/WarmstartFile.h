/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: WarmstartFile.h $
   Last Modification:
    $Author: MichaelT $
    $Modtime: 30.07.07 16:02 $
    $Revision: 792 $

   Targets:
     Win32/ANSI   : yes
     Win32/Unicode: yes (define _UNICODE)
     WinCE        : no

   Description:
    Read/Write access functions for warm start parameter files

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     2         13.10.2006  RM/MT    Changed to be usable in the toolkit

     1         08.06.2006  MT       initial version

**************************************************************************************/

/*****************************************************************************/
/*!\file WarmstartFile.h
*   Read/Write access functions for warm start parameter files                */
/*****************************************************************************/

#ifndef __WARMSTART_FILE__H
#define __WARMSTART_FILE__H

#include "cifXToolkit.h"

#define CIFX_WS_FIELDBUS_INVALID    0xFFFFFFFFUL  /**< Content of fieldbus header element, when data is not validated */
#define CIFX_WS_FIELDBUS_CAN_OPEN   0UL           /**< Content of fieldbus header element for CANOpen slaves          */
#define CIFX_WS_FIELDBUS_PROFIBUS   1UL           /**< Content of fieldbus header element for Profibus slaves         */
#define CIFX_WS_FIELDBUS_DEVICENET  2UL           /**< Content of fieldbus header element for Devicenet slaves        */
#define CIFX_WS_FIELDBUS_ETHERNETIP 3UL           /**< Content of fieldbus header element for Ethernet/IP slaves      */
#define CIFX_WS_FIELDBUS_ETHERCAT   4UL           /**< Content of fieldbus header element for Ethercat slaves         */
#define CIFX_WS_FIELDBUS_SERCOS3    5UL           /**< Content of fieldbus header element for Sercos III slaves       */
#define CIFX_WS_FIELDBUS_PROFINET   6UL           /**< Content of fieldbus header element for Profinet slaves         */

#define CIFX_WS_WARMSTART_FILE_COOKIE     0x12345678 /* First DWORD of a warmstart file, identifying it as such     */

/*****************************************************************************/
/* File header structure (prepends every warm start file)                    */
/*****************************************************************************/
typedef struct CIFX_WS_FILEHEADERtag
{
  uint32_t ulCookie;       /**< identifier                       */
  uint32_t ulCRC32;        /**< CRC of data portion              */
  uint32_t ulFieldbus;     /**< fieldbus the data is for         */
  uint32_t ulDataLen;      /**< length of the following data     */
  uint8_t  abReserved[16]; /**< reserved values                  */

} CIFX_WS_FILEHEADER, *PCIFX_WS_FILEHEADER;

#endif /* __WARMSTART_FILE__H */
