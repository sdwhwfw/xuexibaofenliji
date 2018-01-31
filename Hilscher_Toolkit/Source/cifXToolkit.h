/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: cifXToolkit.h $
   Last Modification:
    $Author: Robert $
    $Modtime: 3.11.09 15:33 $
    $Revision: 3298 $

   Targets:
     Win32/ANSI   : yes
     Win32/Unicode: yes (define _UNICODE)
     WinCE        : no

   Description:
    cifX toolkit function declaration.

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     29        28.02.2011  SS       Declarations for cifX hardware functions separated 
                                    from cifXToolkit.h
     28        31.01.2011  SS       Hardware interface layer added to support 
                                    serial DPM access (e.g. via SPI)
     27        23.04.2010  MT       DEV_ReadHandshakeFlags changed to provide locking
                                    of systemstart with the internal handshake bit state
     26        29.03.2010  MT       Added function DEV_GetIOBitstate()
                                    Added functions cifXTKitEnableHWInterrupt / 
                                                    cifXTKitDisableHWInterrupt
     25        04.03.2009  RM       CHANNELINSTANCE restructured
     
     24        03.11.2009  MT       Slotnumber included in DEVICEINSTANCE
     23        30.09.2009  RM       DEV_DMAState included

     22        30.09.2009  RM       DEV_ReadHostState extended by fReadHostCOS

     21        07.07.2009  RM       DEV_BusState() function added:

     20        04.06.2009  RM       Changed:
                                      Definitions RCX_FLAGS_SET and RCX_FLAGS_CLEAR moved
                                      to rcX_User.h

     19        28.04.2009  RM       Module handling improved

     18        13.02.2009  MT       Added pvInitMutex to CHANNEL_INSTANCE

     17        02.02.2009  MT       Changed:
                                     - pfnPostLoaderInit was changed to pfnNotify to allow
                                       signalling different states in the toolkit (some users
                                       need to know when HW reset is done, etc)
     16        29.01.2009  MT       Added
                                     - eDeviceType to device instance
                                     - eDeviceType now decides it a device has
                                       * RAM only (cifX cards)
                                       * FLASH only (comX, dpm)
                                     - User is able to select RAM or flash config
                                       from outside. AUTODETECT feature is available which
                                       results in same handling as "old" toolkits
     15        22.01.2009  MT       Added eChipType to device instance to be able
                                    to select the bootloader via chiptype

     14        16.01.2009  RM       DEV_IsNXOFile() and DEV_IsNXFFile() included

     13        27.11.2008  RM       Added function USER_GetOSFile, needed for downloadable modules,
                                    that need a rcX base system to be downloaded before

     12        26.11.2008  MT       cifXTKitISRHandler() extended by new parameter
                                    fPCIIgnoreGlobalIntFlag

     11        16.06.2008  RM       include definition for OS_Includes changed to be able
                                    to overwrite the path

     10        21.02.2008  RM       changed version to B0.923

     9         10.04.2007  RM       - DEV_TransferPacket() function extended to
                                      accept a callback pointer for
                                      unsolicited packets

     8         10.04.2007  RM       - DEV_DoWarmstart/DEV_DoColdStart changed to
                                      DEV_ChannelInit/DEV_SystemStart

     7         28.03.2007  RM       Parameters from DEV_Upload and DEV_Download changed,
                                    so the module can be used in the cifX32DLL.

     6         27.03.2007  RM       TRCAE_LEVEL definitions adjusted to existing driver settings
                                    Instances extended by mutex variables for Mailbox and I/O areas

     5         20.03.2007  MT       Added:
                                    - DEV_Upload function
                                    - cifXTKitCyclicTimer (needing to be called cylic by user
                                      recommended cycletime approx. 500ms)
                                    - cifXTKitISRHandler
                                    - cifXTKitDSRHandler
                                    - Device Instance now stores pointer to handshake block
                                    - DEV_DoWarmstart/DEV_DoColdStart
                                    - USER_GetInterruptEnable
     4         09.01.2007  RM       changed version to 0.912
     3         13.10.2006  RM       changed version to 0.904
     2         06.10.2006  MT       changed version to 0.903
     1         07.07.2006  MT       initial version

**************************************************************************************/

/*****************************************************************************/
/*!  \file                                                                   *
*    cifX toolkit function declaration                                       */
/*****************************************************************************/

#ifndef CIFX_TOOLKIT__H
#define CIFX_TOOLKIT__H

#ifdef __cplusplus
extern "C"
{
#endif


#include "OS_Dependent.h"
#include <OS_Includes.h>
#include "cifXHWFunctions.h"
#include "cifXUser.h"
#include "TLR_Types.h"
#include "rcX_User.h"
#include "NetX_RegDefs.h"
#include "HilFileHeaderV3.h"


#ifndef min
  #define min(a,b)  ((a > b)? b : a)
#endif

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_STRUCTURE Toolkit Structure Definitions
*    \{                                                                      */
/*****************************************************************************/

#define TOOLKIT_VERSION       "cifX Toolkit 1.1.2.0"

typedef struct DEVICE_CHANNEL_DATAtag
{
  int       fModuleLoaded;                          /* Module loaded */
  int       fCNFLoaded;                             /* CNF file loaded */
  char      szFileName[16];                         /* Module short file name 8.3 */
  uint32_t  ulFileSize;
} DEVICE_CHANNEL_DATA;

typedef struct DEVICE_CHANNEL_CONFIGtag
{
  int                 fFWLoaded;                /* FW file loaded */
  DEVICE_CHANNEL_DATA atChannelData[CIFX_MAX_NUMBER_OF_CHANNELS];
} DEVICE_CHANNEL_CONFIG, *PDEVICE_CHANNEL_CONFIG;

#define CIFXTKIT_DOWNLOAD_NONE      0x00 /*!< Set when file download was skipped. Only valid if CIFX_NO_ERROR is returned */
#define CIFXTKIT_DOWNLOAD_FIRMWARE  0x01 /*!< Successfully downloaded a firmware */
#define CIFXTKIT_DOWNLOAD_MODULE    0x02 /*!< Successfully downloaded a firmware */
#define CIFXTKIT_DOWNLOAD_EXECUTED  0x80 /*!< Download was executed */

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/

/* Toolkit Global Functions */
int32_t cifXTKitInit         (void);
void    cifXTKitDeinit       (void);
int32_t cifXTKitAddDevice    (PDEVICEINSTANCE ptDevInstance);
int32_t cifXTKitRemoveDevice (char* szBoard, int fForceRemove);

void cifXTKitDisableHWInterrupt(PDEVICEINSTANCE ptDevInstance);
void cifXTKitEnableHWInterrupt(PDEVICEINSTANCE ptDevInstance);

void cifXTKitCyclicTimer  (void);

/* Toolkit Internal Functions */
int DEV_RemoveChannelFiles    (PCHANNELINSTANCE ptChannel, uint32_t ulChannel,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPacket,
                               void*                  pvUser,
                               char*                  szExceptFile);

int  DEV_RemoveFWFiles        (PCHANNELINSTANCE   ptChannel,    uint32_t ulChannel,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPacket,
                               void*                  pvUser);

int32_t DEV_DeleteFile        (void* pvChannel, uint32_t ulChannelNumber, char* pszFileName,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPacket,
                               void*                  pvUser);

int32_t DEV_CheckForDownload  (void* pvChannel,   uint32_t ulChannelNumber, int*      pfDownload,
                               char* pszFileName, void*    pvFileData,      uint32_t  ulFileSize,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPacket,
                               void*                  pvUser);


int  DEV_IsFWFile             (char* pszFileName);
int  DEV_IsNXFFile            (char* pszFileName);
int  DEV_IsNXOFile            (char* pszFileName);

int32_t DEV_GetFWTransferTypeFromFileName (char* pszFileName, uint32_t* pulTransperType);

int32_t DEV_ProcessFWDownload (PDEVICEINSTANCE        ptDevInstance,
                               uint32_t               ulChannel,
                               char*                  pszFullFileName,
                               char*                  pszFileName,
                               uint32_t               ulFileLength,
                               uint8_t*               pbBuffer,
                               uint8_t*               pbLoadState,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_PROGRESS_CALLBACK  pfnCallback,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPktCallback,
                               void*                  pvUser);

int32_t DEV_DownloadFile      (void*                  pvChannel,
                               uint32_t               ulChannel,
                               uint32_t               ulMailboxSize,
                               uint32_t               ulTransferType,
                               char*                  szFileName,
                               uint32_t               ulFileLength,
                               void*                  pvData,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_PROGRESS_CALLBACK  pfnCallback,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPacket,
                               void*                  pvUser);

int32_t DEV_UploadFile        (void*                  pvChannel,
                               uint32_t               ulChannel,
                               uint32_t               ulMailboxSize,
                               uint32_t               ulTransfertype,
                               char*                  szFilename,
                               uint32_t*              pulFileLength,
                               void*                  pvData,
                               PFN_TRANSFER_PACKET    pfnTransferPacket,
                               PFN_PROGRESS_CALLBACK  pfnCallback,
                               PFN_RECV_PKT_CALLBACK  pfnRecvPacket,
                               void*                  pvUser);

/*****************************************************************************/
/*! \addtogroup CIFX_TK_STRUCTURE Toolkit Structure Definitions
*   \{                                                                       */
/*****************************************************************************/

/*****************************************************************************/
/*! Global driver information structure used internally in the toolkit       */
/*****************************************************************************/
typedef struct TKIT_DRIVER_INFORMATIONtag
{
  uint32_t ulOpenCount;  /*!< Number of xDriverOpen calls */
  int      fInitialized; /*!< !=1 if the toolkit was initialized successfully */

} TKIT_DRIVER_INFORMATION;


/*****************************************************************************/
/*! Structure passed to USER implemented function, for reading device        *
* specific configuration options                                             */
/*****************************************************************************/
typedef struct CIFX_DEVICE_INFORMATIONtag
{
  uint32_t   ulDeviceNumber;   /*!< Device number of the cifX card */
  uint32_t   ulSerialNumber;   /*!< Serial number                  */
  uint32_t   ulChannel;        /*!< Channel number (0..6)          */
  PDEVICEINSTANCE ptDeviceInstance; /*!< Pointer to device instance     */

} CIFX_DEVICE_INFORMATION, *PCIFX_DEVICE_INFORMATION;

/*****************************************************************************/
/*! Structure passed to USER implemented function, for getting device        *
*   specific configuration files                                             */
/*****************************************************************************/
typedef struct CIFX_FILE_INFORMATIONtag
{
  char  szShortFileName[16];                        /*!< Short filename (8.3) of the file       */
  char  szFullFileName[CIFX_MAX_FILE_NAME_LENGTH];  /*!< Full filename (including path) to file */
} CIFX_FILE_INFORMATION, *PCIFX_FILE_INFORMATION;

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/

/******************************************************************************
* Functions to be implemented by USER                                         *
******************************************************************************/

int       USER_GetOSFile                (PCIFX_DEVICE_INFORMATION ptDevInfo, PCIFX_FILE_INFORMATION ptFileInfo);

uint32_t  USER_GetFirmwareFileCount     (PCIFX_DEVICE_INFORMATION ptDevInfo);
int       USER_GetFirmwareFile          (PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulIdx, PCIFX_FILE_INFORMATION ptFileInfo);
uint32_t  USER_GetConfigurationFileCount(PCIFX_DEVICE_INFORMATION ptDevInfo);
int       USER_GetConfigurationFile     (PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulIdx, PCIFX_FILE_INFORMATION ptFileInfo);
int       USER_GetWarmstartParameters   (PCIFX_DEVICE_INFORMATION ptDevInfo, CIFX_PACKET* ptPacket);
void      USER_GetAliasName             (PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulMaxLen, char* szAlias);
void      USER_GetBootloaderFile        (PDEVICEINSTANCE ptDevInstance, PCIFX_FILE_INFORMATION ptFileInfo);
int       USER_GetInterruptEnable       (PCIFX_DEVICE_INFORMATION ptDevInfo);
int       USER_GetDMAMode               (PCIFX_DEVICE_INFORMATION ptDevInfo);


extern uint32_t g_ulTraceLevel;


#ifdef __cplusplus
}
#endif

#endif /* CIFX_TOOLKIT__H */
