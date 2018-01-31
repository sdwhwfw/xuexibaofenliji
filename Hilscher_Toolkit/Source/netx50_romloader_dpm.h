/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: netx50_romloader_dpm.h $
   Last Modification:
    $Author: MichaelT $
    $Modtime: 23.01.09 11:31 $
    $Revision: 792 $

   Targets:
     Win32/ANSI   : yes
     Win32/Unicode: yes (define _UNICODE)
     WinCE        : no

   Description:
    netX50 ROM Loader DPM layout

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         22.01.2009  MT       initial version

**************************************************************************************/

#ifndef __NETX50_ROMLOADER_DPM__H
#define __NETX50_ROMLOADER_DPM__H

#define NETX50_BOOTID_DPM             0x4C42584E  /*!< 'NXBL' DPM boot identifier ('NXBL') */
#define NETX50_BOOTID_DPM_STRING      "NXBL"      /*!< 'NXBL' DPM boot identifier ('NXBL') */

#define NETX50_DPM_TONETXMBX_MSK      0x01
#define NETX50_DPM_TOHOSTMBX_MSK      0x02

#define NETX50_DPM_HANDSHAKE_OFFSET   63          /*!< Offset of the command handshake flags            */

#define NETX50_DPM_BLLAYOUT_OFFSET    118         /*!< Offset of the bootloader DPM layout in aulDpmHsRegs */
#define MSK_NETX50_DPM_BLLAYOUT       0xFF00
#define SRT_NETX50_DPM_BLLAYOUT       8

typedef union NETX50_BL_HSREGISTERtag
{
  struct
  {
    uint16_t           usReserved;
    volatile uint8_t   bNetXFlags; /*!< Flags signalled by netX50 */
    volatile uint8_t   bHostFlags; /*!< Flags signalled by Host   */
  } t8Bit;
  uint32_t ulVal;


} NETX50_BL_HSREGISTER;

typedef struct NETX50_ROMLOADER_DPMtag
{
  volatile uint32_t     ulDpmBootId;
  volatile uint32_t     ulDpmByteSize;
  volatile uint32_t     ulSdramGeneralCtrl;
  volatile uint32_t     ulSdramTimingCtrl;
  volatile uint32_t     ulSdramByteSize;
  volatile uint32_t     aulReserved14[249];
  volatile uint32_t     ulHostToNetxDataSize;
  volatile uint32_t     ulNetxToHostDataSize;
  volatile uint8_t      abHostToNetxData[4096];
  volatile uint8_t      abNetxToHostData[2048];
  NETX50_BL_HSREGISTER  atHandshakeRegs[128];
  volatile uint32_t     aulDpmHsRegs[128];

} NETX50_ROMLOADER_DPM, *PNETX50_ROMLOADER_DPM;

#endif /* __NETX50_ROMLOADER_DPM__H */
