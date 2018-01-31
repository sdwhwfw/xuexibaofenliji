/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_comX100.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/29
 * Description        : This file contains the software implementation for the
 *                      comX100 module app unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/07/29 | v1.0  |            | initial released
 * 2013/08/01 | v1.1  | Bruce.zhu  | change all app file name to 'app_xxx'
 * 2013/10/17 | v1.2  | Bruce.zhu  | reset comX100 when STM32 reset
 * 2013/10/23 | v1.3  | Bruce.zhu  | remove comX100 send task
 *******************************************************************************/
#ifndef _APP_COMX100_C_
#define _APP_COMX100_C_

#include "app_comX100.h"
#if defined(_COMX100_MODULE_FTR_)
#include "comx100.h"
#include "cifXErrors.h"
#include "trace.h"
#include <string.h>
#include "app_control_pdo_callback.h"


static PDEVICEINSTANCE g_ptDevInstance = 0;

static OS_STK          comX100_task_stk[COMX100_TASK_STK_SIZE];
static OS_STK          comX100_isr_task_stk[COMX100_TASK_STK_SIZE];

static OS_EVENT*       comX100_isr_event;
static PDEVICEINSTANCE pt_comX100_DevInst;

// handle communction channel
static CIFXHANDLE      g_comX100_driver = NULL;

// save the PDO data
static u8              g_RPDO_saved_buf[CONTROL_RPDO_NUM][8];
static u8              g_comX100_init_complete_flag = 0;

static OS_EVENT*       g_comX100_write_read_sem;
static CIFXHANDLE      g_comX100_Channel;

static control_pRPDO_function g_p_RPDO_callback[CONTROL_RPDO_NUM];

static void comX100_module_task(void *p_arg);
static void comX100_isr_task(void *p_arg);
static void comX100_register_notification(CIFXHANDLE handle);
static void show_comX100_error(int32_t lError);


/*****************************************************************************/
/*! Displays cifX error
 *   \param  ptVTable Pointer to cifX API function table
 *   \param lError     Error code
 *****************************************************************************/
void show_comX100_error(int32_t lError)
{
	char szError[128] ={0};

	if( lError != CIFX_NO_ERROR)
	{
		xDriverGetErrorDescription( lError,  szError, sizeof(szError));
		APP_TRACE("Error: 0x%X, <%s>\r\n", (unsigned int)lError, szError);
	}
}


/*****************************************************************************/
/*! Dumps a rcX packet to debug console
*   \param ptPacket Pointer to packed being dumped                           */
/*****************************************************************************/
static void DumpPacket(CIFX_PACKET* ptPacket)
{
	APP_TRACE("Dest   : 0x%08X      ID   : 0x%08X\r\n", (int)ptPacket->tHeader.ulDest,   (int)ptPacket->tHeader.ulId);
	APP_TRACE("Src    : 0x%08X      Sta  : 0x%08X\r\n", (int)ptPacket->tHeader.ulSrc,    (int)ptPacket->tHeader.ulState);
	APP_TRACE("DestID : 0x%08X      Cmd  : 0x%08X\r\n", (int)ptPacket->tHeader.ulDestId, (int)ptPacket->tHeader.ulCmd);
	APP_TRACE("SrcID  : 0x%08X      Ext  : 0x%08X\r\n", (int)ptPacket->tHeader.ulSrcId,  (int)ptPacket->tHeader.ulExt);
	APP_TRACE("Len    : 0x%08X      Rout : 0x%08X\r\n", (int)ptPacket->tHeader.ulLen,    (int)ptPacket->tHeader.ulRout);

	APP_TRACE("Data:");
	DumpData(ptPacket->abData, ptPacket->tHeader.ulLen);
}


/*****************************************************************************/
/*! Function to demonstrate the board/channel enumeration
 *   \return CIFX_NO_ERROR on success                                        */
/*****************************************************************************/
int32_t EnumBoardDemo(void)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  APP_TRACE("\r\n---------- Board/Channel enumeration demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    uint32_t          ulBoard    = 0;
    BOARD_INFORMATION tBoardInfo = {0};

    /* Iterate over all boards */
    while(CIFX_NO_ERROR == xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo))
    {
      uint32_t            ulChannel    = 0;
      CHANNEL_INFORMATION tChannelInfo = {{0}};
   	  
      APP_TRACE("Found Board %s\r\n", tBoardInfo.abBoardName);
      if(strlen( (char*)tBoardInfo.abBoardAlias) != 0)
        APP_TRACE(" Alias        : %s\r\n", tBoardInfo.abBoardAlias);

      APP_TRACE(" DeviceNumber : %u\r\n", (int)tBoardInfo.tSystemInfo.ulDeviceNumber);
      APP_TRACE(" SerialNumber : %u\r\n", (int)tBoardInfo.tSystemInfo.ulSerialNumber);
      APP_TRACE(" Board ID     : %u\r\n", (int)tBoardInfo.ulBoardID);
      APP_TRACE(" System Error : 0x%08X\r\n", (int)tBoardInfo.ulSystemError);
      APP_TRACE(" Channels     : %u\r\n", (int)tBoardInfo.ulChannelCnt);
      APP_TRACE(" DPM Size     : %u\r\n", (int)tBoardInfo.ulDpmTotalSize);

      /* iterate over all channels on the current board */
      while(CIFX_NO_ERROR == xDriverEnumChannels(hDriver, ulBoard, ulChannel, sizeof(tChannelInfo), &tChannelInfo))
      {
        APP_TRACE(" - Channel %u:\r\n", (int)ulChannel);
        APP_TRACE("    Firmware : %s\r\n", tChannelInfo.abFWName);
        APP_TRACE("    Version  : %u.%u.%u build %u\r\n", 
               tChannelInfo.usFWMajor,
               tChannelInfo.usFWMinor,
               tChannelInfo.usFWRevision,
               tChannelInfo.usFWBuild);
        APP_TRACE("    Date     : %02u/%02u/%04u\r\n", 
               tChannelInfo.bFWMonth,
               tChannelInfo.bFWDay,
               tChannelInfo.usFWYear);

        ++ulChannel;
      }

      ++ulBoard;
    }

    /* close previously opened driver */
    xDriverClose(hDriver);
  }

  APP_TRACE(" State = 0x%08X\r\n", (int)lRet);
  APP_TRACE("----------------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate system channel functionality (PacketTransfer)
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t SysdeviceDemo(void)
{
  CIFXHANDLE hDriver = NULL;
  int32_t    lRet    = xDriverOpen(&hDriver);

  APP_TRACE("\r\n---------- System Device handling demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hSys = NULL;
    lRet = xSysdeviceOpen(hDriver, "cifX0", &hSys);

    if(CIFX_NO_ERROR != lRet)
    {
      APP_TRACE("Error opening SystemDevice!\r\n");

    } else
    {
      SYSTEM_CHANNEL_SYSTEM_INFO_BLOCK tSysInfo = {{0}};
      uint32_t ulSendPktCount = 0;
      uint32_t ulRecvPktCount = 0;
      CIFX_PACKET tSendPkt = {{0}};
      CIFX_PACKET tRecvPkt = {{0}};
      
      /* System channel successfully opened, try to read the System Info Block */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_INFO_BLOCK, sizeof(tSysInfo), &tSysInfo)))
      {
        APP_TRACE("Error querying system information block\r\n");
      } else
      {
        APP_TRACE("System Channel Info Block:\r\n");
		APP_TRACE("abCookie         : %c%c%c%c\r\n", tSysInfo.abCookie[0], 
													 tSysInfo.abCookie[1],
													 tSysInfo.abCookie[2],
													 tSysInfo.abCookie[3]);
        APP_TRACE("DPM Size         : %u\r\n", (int)tSysInfo.ulDpmTotalSize);
        APP_TRACE("Device Number    : %u\r\n", (int)tSysInfo.ulDeviceNumber);
        APP_TRACE("Serial Number    : %u\r\n", (int)tSysInfo.ulSerialNumber);
        APP_TRACE("Manufacturer     : %u\r\n", (int)tSysInfo.usManufacturer);
        APP_TRACE("Production Date  : %u\r\n", (int)tSysInfo.usProductionDate);
        APP_TRACE("Device Class     : %u\r\n", (int)tSysInfo.usDeviceClass);
        APP_TRACE("HW Revision      : %u\r\n", (int)tSysInfo.bHwRevision);
        APP_TRACE("HW Compatibility : %u\r\n", (int)tSysInfo.bHwCompatibility);
      }

      /* Do a simple Packet exchange via system channel */
      xSysdeviceGetMBXState(hSys, &ulRecvPktCount, &ulSendPktCount);
      APP_TRACE("System Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
             (int)ulSendPktCount, (int)ulRecvPktCount);

      if(CIFX_NO_ERROR != (lRet = xSysdevicePutPacket(hSys, &tSendPkt, 10)))
      {
        APP_TRACE("Error sending packet to device!\r\n");
      } else
      {
        APP_TRACE("Send Packet:\r\n");
        DumpPacket(&tSendPkt);

        xSysdeviceGetMBXState(hSys, &ulRecvPktCount, &ulSendPktCount);
        APP_TRACE("System Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
              (int)ulSendPktCount, (int)ulRecvPktCount);

        if(CIFX_NO_ERROR != (lRet = xSysdeviceGetPacket(hSys, sizeof(tRecvPkt), &tRecvPkt, 20)) )
        {
          APP_TRACE("Error getting packet from device!\r\n");
        } else
        {
          APP_TRACE("Received Packet:\r\n");
          DumpPacket(&tRecvPkt);

          xSysdeviceGetMBXState(hSys, &ulRecvPktCount, &ulSendPktCount);
          APP_TRACE("System Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
               (int)ulSendPktCount, (int)ulRecvPktCount);
        }
      }
      xSysdeviceClose(hSys);
    }

    xDriverClose(hDriver);
  }

  APP_TRACE(" State = 0x%08X\r\n", (int)lRet);
  APP_TRACE("----------------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate communication channel functionality
*   Packet Transfer and I/O Data exchange
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t ChannelDemo(void)
{
  CIFXHANDLE  hDriver = NULL;
  int32_t     lRet    = xDriverOpen(&hDriver);
  uint8_t     vl_buf[64];

  APP_TRACE("\r\n---------- Communication Channel demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hChannel = NULL;
    lRet = xChannelOpen(NULL, "cifX0", 0, &hChannel);

    if(CIFX_NO_ERROR != lRet)
    {
      APP_TRACE("Error opening Channel!");

    } else
    {
      CHANNEL_INFORMATION tChannelInfo = {{0}};
      uint32_t ulCycle = 0;
      uint8_t abSendData[8] = {0};
      uint8_t abRecvData[8] = {0};
      CIFX_PACKET tSendPkt = {{0}};
      CIFX_PACKET tRecvPkt = {{0}};
      
      /* Channel successfully opened, so query basic information */
      if( CIFX_NO_ERROR != (lRet = xChannelInfo(hChannel, sizeof(CHANNEL_INFORMATION), &tChannelInfo)))
      {
        APP_TRACE("Error querying system information block\r\n");
      } else
      {
        APP_TRACE("Communication Channel Info:\r\n");
        APP_TRACE("Device Number    : %u\r\n", (int)tChannelInfo.ulDeviceNumber);
        APP_TRACE("Serial Number    : %u\r\n", (int)tChannelInfo.ulSerialNumber);
        APP_TRACE("Firmware         : %s\r\n", tChannelInfo.abFWName);
        APP_TRACE("FW Version       : %u.%u.%u build %u\r\n", 
                tChannelInfo.usFWMajor,
                tChannelInfo.usFWMinor,
                tChannelInfo.usFWRevision,
                tChannelInfo.usFWBuild);
        APP_TRACE("FW Date          : %02u/%02u/%04u\r\n", 
                tChannelInfo.bFWMonth,
                tChannelInfo.bFWDay,
                tChannelInfo.usFWYear);

        APP_TRACE("Mailbox Size     : %u\r\n", (int)tChannelInfo.ulMailboxSize);
      }

      /* Do a basic Packet Transfer */
      if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(hChannel, &tSendPkt, 10)))
      {
        APP_TRACE("Error sending packet to device!\r\n");
      } else
      {
        APP_TRACE("Send Packet:\r\n");
        DumpPacket(&tSendPkt);

        if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(hChannel, sizeof(tRecvPkt), &tRecvPkt, 20)) )
        {
          APP_TRACE("Error getting packet from device!\r\n");
        } else
        {
          APP_TRACE("Received Packet:\r\n");
          DumpPacket(&tRecvPkt);
        }
      }

      /* Read and write I/O data (32Bytes). Output data will be incremented each cyle */
      xChannelIOReadSendData(hChannel, 0, 0, sizeof(abRecvData), abRecvData);

      for( ulCycle = 0; ulCycle < 10; ++ulCycle)
      {
        if(CIFX_NO_ERROR != (lRet = xChannelIORead(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10)))
        {
          APP_TRACE("Error reading IO Data area!\r\n");
          break;
        } else
        {
          APP_TRACE("IORead Data:");
          DumpData(abRecvData, sizeof(abRecvData));

          if(CIFX_NO_ERROR != (lRet = xChannelIOWrite(hChannel, 0, 0, sizeof(abSendData), abSendData, 10)))
          {
            APP_TRACE("Error writing to IO Data area!\r\n");
            break;
          } else
          {
            APP_TRACE("IOWrite Data:");
            DumpData(abSendData, sizeof(abSendData));

            memset(abSendData, (int)ulCycle + 1, sizeof(abSendData));
          }
        }
      }

      xChannelClose(hChannel);
    }

    xDriverClose(hDriver);
  }

  xDriverGetErrorDescription(lRet, (char*)vl_buf, 64);
  APP_TRACE(" State = 0x%08X--->[%s]\r\n", (int)lRet, vl_buf);
  APP_TRACE("----------------------------------------------------\r\n");

  return lRet;

}

/*****************************************************************************/
/*! Function to demonstrate control/status block functionality
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
int32_t BlockDemo(void)
{
  int32_t lRet = CIFX_NO_ERROR;
  CIFXHANDLE hDevice = NULL;
  uint8_t vl_buf[64];
  CIFXHANDLE  hDriver = NULL;

  APP_TRACE("\n--- Read / Write Block Information ---\r\n");

  lRet = xDriverOpen(&hDriver);
  if(lRet != CIFX_NO_ERROR)
  {
  	xDriverGetErrorDescription(lRet, (char*)vl_buf, 64);
  	APP_TRACE("Error opening Channel: [%08X]--->[%s]\r\n", lRet, (char*)vl_buf);
  }

  /* Open channel */
  lRet = xChannelOpen(NULL, "cifX0", 0, &hDevice);
  if(lRet != CIFX_NO_ERROR)
  {
  	xDriverGetErrorDescription(lRet, (char*)vl_buf, 64);
    APP_TRACE("Error opening Channel: [%08X]--->[%s]\r\n", lRet, (char*)vl_buf);
  } else
  {
    uint8_t abBuffer[4] = {0};

    /* Read / Write control block */
    APP_TRACE("Read CONTROL Block \r\n");  
    memset( abBuffer, 0, sizeof(abBuffer));
    lRet = xChannelControlBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);

    DumpData(abBuffer, 4);

    APP_TRACE("Write CONTROL Block \r\n");  
    lRet = xChannelControlBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

    APP_TRACE("Read COMMON Status Block \r\n");  
    memset( abBuffer, 0, sizeof(abBuffer));
    lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);

    DumpData(abBuffer, 4);

    APP_TRACE("Write COMMON Status Block \r\n");  
    lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

    /* this is expected to fail, as this block must not be written by Host */
    if(CIFX_NO_ERROR != lRet)
    {
      xDriverGetErrorDescription(lRet, (char*)vl_buf, 64);
	  APP_TRACE("Error writing to common status block. [%08X]--->[%s]\r\n", lRet, (char*)vl_buf);
    }

    APP_TRACE("Read EXTENDED Status Block \r\n");  
    memset( abBuffer, 0, sizeof(abBuffer));
    lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
    DumpData(abBuffer, 4);
    
    APP_TRACE("Write EXTENDED Status Block \r\n");  
    lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

    /* this is expected to fail, as this block must not be written by Host */
    if(CIFX_NO_ERROR != lRet)
    {
		xDriverGetErrorDescription(lRet, (char*)vl_buf, 64);
		APP_TRACE("Error writing to extended status block. [%08X]--->[%s]\r\n", lRet, (char*)vl_buf);
    }

    xChannelClose(hDevice);
  } 

  xDriverClose(hDriver);

  return lRet;
}

/*****************************************************************************/
/*! Toolkit Example Task
*   \return 0                                                                */
/*****************************************************************************/
int32_t cifXToolkitInit(void)
{
	int32_t lTkRet = CIFX_NO_ERROR;

	/* First of all initialize toolkit */
	lTkRet = cifXTKitInit();

	if(CIFX_NO_ERROR == lTkRet)
	{
		g_ptDevInstance = (PDEVICEINSTANCE)OS_Memalloc(sizeof(*g_ptDevInstance));
		OS_Memset(g_ptDevInstance, 0, sizeof(*g_ptDevInstance));

		/* Set trace level of toolkit */
		g_ulTraceLevel = TRACE_LEVEL_ERROR   | 
						 TRACE_LEVEL_WARNING | 
						 TRACE_LEVEL_INFO    | 
						 TRACE_LEVEL_DEBUG;

		/* Insert the basic device information , into the DeviceInstance structure 
		for the toolkit. The DPM address must be zero, as we only transfer address 
		offsets via the SPI interface.
		NOTE: The physical address and irq number are for information use 
		only, so we skip them here. Interrupt is currently not supported
		and ignored, so we dont need to set it */
		g_ptDevInstance->fPCICard          = 0;
		g_ptDevInstance->pvOSDependent     = g_ptDevInstance;
		g_ptDevInstance->ulDPMSize         = 0x10000;
		OS_Strncpy( g_ptDevInstance->szName,
					"cifX0",
					sizeof(g_ptDevInstance->szName));

		g_ptDevInstance->pbDPM         = (void*)0;
#ifdef CIFX_TOOLKIT_HWIF
		g_ptDevInstance->pfnHwIfRead   = COMX100_read_byte;
		g_ptDevInstance->pfnHwIfWrite  = COMX100_write_byte;
#endif
		/* Add the device to the toolkits handled device list */
		lTkRet = cifXTKitAddDevice(g_ptDevInstance);

		/* If it succeeded do device tests */
		if(CIFX_NO_ERROR != lTkRet)
		{
			/* Uninitialize Toolkit, this will remove all handled boards from the toolkit and 
			deallocate the device instance */
			cifXTKitDeinit();
		}
	}

	return lTkRet;
}


static void init_communication_channel(void)
{
    int32_t lRet;
    CHANNEL_INFORMATION tChannelInfo = {{0}};

    /* open Driver/Toolkit */
    lRet = xDriverOpen(&g_comX100_driver);
    assert_param(lRet == CIFX_NO_ERROR);

    /* open Channel */
    lRet = xChannelOpen(NULL, "cifX0", 0, &g_comX100_Channel);
    assert_param(lRet == CIFX_NO_ERROR);

    /* query basic channel information */
    lRet = xChannelInfo(g_comX100_Channel, sizeof(CHANNEL_INFORMATION), &tChannelInfo);
    assert_param(lRet == CIFX_NO_ERROR);
    APP_TRACE("Communication Channel Info:\r\n");
    APP_TRACE("Device Number      : %u\r\n", (int)tChannelInfo.ulDeviceNumber);
    APP_TRACE("Serial Number      : %u\r\n", (int)tChannelInfo.ulSerialNumber);
    APP_TRACE("Firmware           : %s\r\n", tChannelInfo.abFWName);
    APP_TRACE("FW Version         : %u.%u.%u build %u\r\n", 
                tChannelInfo.usFWMajor,
                tChannelInfo.usFWMinor,
                tChannelInfo.usFWRevision,
                tChannelInfo.usFWBuild);
    APP_TRACE("FW Date            : %02u/%02u/%04u\r\n", 
                tChannelInfo.bFWMonth,
                tChannelInfo.bFWDay,
                tChannelInfo.usFWYear);
    APP_TRACE("Mailbox Size       : %u\r\n", (int)tChannelInfo.ulMailboxSize);

    comX100_register_notification(g_comX100_Channel);

    OSSemPost(g_comX100_write_read_sem);
    g_comX100_init_complete_flag = 1;
}



/**
  * Deal with IO if new I/O data received
  *
  * @param index:   NODE ID: ARM1 ARM2 ARM3
  * @param p_buf:   buffer which srote I/O data
  * @param len  :   buffer data length
  */
static void check_new_IO_data(u8* p_buf, u8 len)////?????????????????????
{
	u8 i;

    for (i = 0; i < CONTROL_RPDO_NUM; i++)
    {
        if (memcmp(g_RPDO_saved_buf[i], p_buf, 8) == 0)
        {
            // do nothing if data has not change
        }
        else
        {
            // I/O data is changed and we need to deal with it...
            // save it
            memcpy(g_RPDO_saved_buf[i], p_buf, 8);
            if (g_p_RPDO_callback[i])
            {
                g_p_RPDO_callback[i](p_buf, 8);
            }
            else
            {
                APP_TRACE("Warning, g_p_RPDO_callback[%d] is not set!\r\n", i);
            }
        }
        p_buf += 8;
    }

}


void init_comX100_module_task(control_pRPDO_function* p_control_callback, u8 call_num)
{
    INT8U os_err;
    u8    num;

    // check param
    assert_param(call_num == CONTROL_RPDO_NUM);
    assert_param(p_control_callback);

    for (num = 0; num < CONTROL_RPDO_NUM; num++)
    {
        g_p_RPDO_callback[num] = p_control_callback[num];
    }

    g_comX100_write_read_sem = (OS_EVENT*)OSSemCreate(0);
    assert_param(g_comX100_write_read_sem);

    /* comX100 module task ***************************************************/
    os_err = OSTaskCreateExt((void (*)(void *)) comX100_module_task,
                             (void          *) 0,
                             (OS_STK        *)&comX100_task_stk[COMX100_TASK_STK_SIZE - 1],
                             (INT8U          ) COMX100_TASK_PRIO,
                             (INT16U         ) COMX100_TASK_PRIO,
                             (OS_STK        *)&comX100_task_stk[0],
                             (INT32U         ) COMX100_TASK_STK_SIZE,
                             (void          *) 0,
                             (INT16U         )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(COMX100_TASK_PRIO, (INT8U *)"comX100", &os_err);
}



/*
 *   ARM0 TPDO summary
 * -----------------------------------------------------------------------------------------------------------------
 *
 *     TPDO1      TPDO2          TPDO6      TPDO7      TPDO8         TPDO12      TPDO13      TPDO8         TPDO18  
 *   |       |  |       |      |       |  |       |  |       |      |       |  |       |  |       |      |       |  
 *   | <---> |  | <---> | .... | <---> |  | <---> |  | <---> | .... | <---> |  | <---> |  | <---> | .... | <---> |
 *   | 8Byte |  | 8Byte |      | 8Byte |  | 8Byte |  | 8Byte |      | 8Byte |  | 8Byte |  | 8Byte |      | 8Byte |
 *
 *   |<--------------ARM1------------->|  |<--------------ARM2------------->|  |<--------------ARM3------------->|  
 * 
 * @param TPDO_offset, which TPDO do you want to send first
 * @param TPDO_num,    how many TPDOs do you want to send from TPDO_offset one time
 * @return 0  ----> success
 *         1  ----> fail
 * @note: the TPDO number maybe changed in the feature
 */
u8 comX100_send_packet(void* p_data, u8 TPDO_offset, u8 TPDO_num)
{
    u32   lRet;
    INT8U err;
    u8    ret = 0;

    // wait comX100 DPM ready...
    OSSemPend(g_comX100_write_read_sem, 0, &err);
    assert_param(OS_ERR_NONE == err);
    
    lRet = xChannelIOWrite(g_comX100_Channel, 0, (TPDO_offset-1) * 8, TPDO_num * 8, p_data, 20);
    if (CIFX_NO_ERROR != lRet)
    {
        show_comX100_error(lRet);
        ret = 1;
    }

    return ret;
}


static void comX100_module_task(void *p_arg)
{
	u32   lRet;
	u8*   reac_pdo_buf = 0;
	INT8U err;

    /* delay some time to reset comX100 */
    OSTimeDlyHMSM(0, 0, 1, 0);
    start_comX100_module();
	APP_TRACE("Waiting 5 seconds for comX100 module start...\r\n");
	OSTimeDlyHMSM(0, 0, 5, 0);

	/* cifXToolkit init */
	APP_TRACE("comX100 module init...\r\n");
	if (CIFX_NO_ERROR == cifXToolkitInit())
	{
		APP_TRACE("comX100 init OK\r\n");
		init_communication_channel();
	}
	else
	{
		APP_TRACE("comX100 init ERROR!\r\n");
		return;
	}

	/* init recv buffer */
	memset(g_RPDO_saved_buf, 0x55, sizeof g_RPDO_saved_buf);

    /* malloc recv buffer */
    reac_pdo_buf = (u8*)malloc(CONTROL_RPDO_NUM*8);
    assert_param(reac_pdo_buf);

	while (1)
	{
		OSTimeDlyHMSM(0, 0, 0, 10);

		/* 3 Node: ARM1 ARM2 ARM3 */
		memset(reac_pdo_buf, 0x00, sizeof(reac_pdo_buf));

		// 4 RPDO
		OSSemPend(g_comX100_write_read_sem, 0, &err);
		assert_param(OS_ERR_NONE == err);
		lRet = xChannelIORead(g_comX100_Channel, 0, 0, CONTROL_RPDO_NUM*8, reac_pdo_buf, 20);

		if (CIFX_NO_ERROR != lRet)
		{
			show_comX100_error(lRet);
		}
		else
		{
			check_new_IO_data(reac_pdo_buf, CONTROL_RPDO_NUM*8);
		}
	}
}


static void handle_notification(uint32_t ulNotification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
	uint32_t ulErrorCount;
	int32_t  ret;
	INT8U    err;

	switch (ulNotification)
	{
		case CIFX_NOTIFY_RX_MBX_FULL:
			APP_TRACE("CIFX_NOTIFY_RX_MBX_FULL\r\n");
			/* TODO: There must be some errors happened */
			break;
		case CIFX_NOTIFY_TX_MBX_EMPTY:
			APP_TRACE("CIFX_NOTIFY_TX_MBX_EMPTY\r\n");
			/* TODO: There must be some errors happened */
			break;
		case CIFX_NOTIFY_PD0_IN:
			if (g_comX100_init_complete_flag)
			{
				err = OSSemPost(g_comX100_write_read_sem);
				assert_param(OS_ERR_NONE == err);
			}
			else
			{
				APP_TRACE("CIFX_NOTIFY_PD0_IN\r\n");
			}
			break;
		case CIFX_NOTIFY_PD1_IN:
			APP_TRACE("CIFX_NOTIFY_PD1_IN\r\n");
			break;
		case CIFX_NOTIFY_PD0_OUT:
			if (g_comX100_init_complete_flag)
			{
				err = OSSemPost(g_comX100_write_read_sem);
				assert_param(OS_ERR_NONE == err);
			}
			else
			{
				APP_TRACE("CIFX_NOTIFY_PD0_OUT\r\n");
			}
			break;
		case CIFX_NOTIFY_PD1_OUT:
			APP_TRACE("CIFX_NOTIFY_PD1_OUT\r\n");
			break;
		case CIFX_NOTIFY_SYNC:
			APP_TRACE("CIFX_NOTIFY_SYNC\r\n");
			ret = xChannelSyncState( g_comX100_Channel, CIFX_SYNC_ACKNOWLEDGE_CMD, 20, &ulErrorCount);
			assert_param(CIFX_NO_ERROR == ret);
			break;
		case CIFX_NOTIFY_COM_STATE:
			APP_TRACE("CIFX_NOTIFY_COM_STATE\r\n");
			break;
		default:
			break;
	}
}


static void comX100_register_notification(CIFXHANDLE handle)
{
	int32_t  ret;
	uint32_t state;

	APP_TRACE("\r\n-------------- comX100_register_notification ---------------\r\n");

    if( (CIFX_NO_ERROR != (ret = xChannelRegisterNotification( handle, CIFX_NOTIFY_RX_MBX_FULL,  handle_notification,  (void*)1))) ||
        (CIFX_NO_ERROR != (ret = xChannelRegisterNotification( handle, CIFX_NOTIFY_TX_MBX_EMPTY,  handle_notification,  (void*)2))) ||   
        (CIFX_NO_ERROR != (ret = xChannelRegisterNotification( handle, CIFX_NOTIFY_PD0_IN,        handle_notification,  (void*)3))) ||  
        (CIFX_NO_ERROR != (ret = xChannelRegisterNotification( handle, CIFX_NOTIFY_PD0_OUT,       handle_notification,  (void*)5))) ||
        (CIFX_NO_ERROR != (ret = xChannelRegisterNotification( handle, CIFX_NOTIFY_SYNC,          handle_notification,  (void*)7))) )
    {
		show_comX100_error(ret);
    }
	else
	{
		/* Get actual host state */
		if( (ret = xChannelHostState( handle, CIFX_HOST_STATE_READ, &state, 0L)) != CIFX_NO_ERROR)
		{
			// Read driver error description
			show_comX100_error(ret);
		}

		APP_TRACE("state1 = %d\r\n", state);

		/* Set host ready */
		if( (ret = xChannelHostState( handle, CIFX_HOST_STATE_READY, NULL, 2000L)) != CIFX_NO_ERROR)
		{
			// Read driver error description
			show_comX100_error(ret);
		}

		/* Get actual host state */
		if( (ret = xChannelHostState( handle, CIFX_HOST_STATE_READ, &state, 0L)) != CIFX_NO_ERROR)
		{
			// Read driver error description
			show_comX100_error(ret);
		}

		APP_TRACE("state2 = %d\r\n", state);

	}

	APP_TRACE("--------------------------- done ---------------------------\r\n\r\n");
}


void create_comX100_isr_task(void* p_data)
{
	INT8U  os_err = 0;

	/* check param */
	assert_param(p_data);

	APP_TRACE("create_comX100_isr_task...\r\n");

	/* create a event */
	comX100_isr_event = (OS_EVENT*)OSSemCreate(0);
	assert_param(comX100_isr_event);

	pt_comX100_DevInst = (PDEVICEINSTANCE)p_data;

	/* start comX100 isr task ***************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) comX100_isr_task,
							(void		   * ) 0,
							(OS_STK 	   * )&comX100_isr_task_stk[COMX100_ISR_TASK_STK_SIZE - 1],
							(INT8U			 ) COMX100_ISR_TASK_PRIO,
							(INT16U 		 ) COMX100_ISR_TASK_PRIO,
							(OS_STK 	   * )&comX100_isr_task_stk[0],
							(INT32U 		 ) COMX100_ISR_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(COMX100_ISR_TASK_PRIO, (INT8U *)"comX100 isr", &os_err);

}


/**
  * comX100 module DSR handler
  */
static void comX100_isr_task(void *p_arg)//////?????????????????????
{
	INT8U err;

	for (;;)
	{
		OSSemPend(comX100_isr_event, 0, &err);
		assert_param(OS_ERR_NONE == err);
		//APP_TRACE("*** comX100_isr_task\r\n");

		cifXTKitDSRHandler(pt_comX100_DevInst);
	}
}


/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/******************************************************************************/

/**
  * @brief  This function handles External interrupt line 3(PE3) request.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
	PDEVICEINSTANCE ptDevInst = (PDEVICEINSTANCE)pt_comX100_DevInst;
	int             iISRRet   = CIFX_TKIT_IRQ_OTHERDEVICE;

	assert_param(ptDevInst);

	OSIntEnter();
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		iISRRet = cifXTKitISRHandler(ptDevInst, 1);

		switch(iISRRet)
		{
			case CIFX_TKIT_IRQ_DSR_REQUESTED:
				/* Signal interrupt thread to call dsr handler */
				OSSemPost(comX100_isr_event);
				break;

			case CIFX_TKIT_IRQ_HANDLED:
				/* Everything was done by ISR, no need to call DSR */
				break;

			case CIFX_TKIT_IRQ_OTHERDEVICE:
			default:
				/* This is an interrupt from another device. pciIntConnect creates a linked 
				list of ISRs that are sharing the same interrupt line. When the interrupt 
				occurs, it sequentially invokes all the ISRs connected to it.  */
				break;
		}

		EXTI_ClearITPendingBit(EXTI_Line3);
	}

	OSIntExit();
}

#endif /* _COMX100_MODULE_FTR_ */

#endif /* _APP_COMX100_C_ */

