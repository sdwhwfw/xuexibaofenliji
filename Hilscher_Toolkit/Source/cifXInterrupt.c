/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile: cifXInterrupt.c $
   Last Modification:
    $Author: Robert $
    $Modtime: 19.08.09 10:59 $
    $Revision: 3294 $

   Targets:
     Win32/ANSI   : yes
     Win32/Unicode: yes (define _UNICODE)
     WinCE        : no

   Description:
    cifX Toolkit Interrupt handling routines (ISR/DSR)

   Changes:

     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      20       07.02.2012  RM       Notification order changed (I/O, Recv, Send) and receive callback lock removed
      19       02.02.2012  SD       Add COM-Flag notification and locked (receive) mailbox notification call
      18       14.06.2011  RM       Updated unsigned long data types to uint32_t
      17       29.03.2011  MT       DSR now ignores handshake bit changes while in system reset
      16       31.01.2011  SS       Hardware interface layer added to support
                                    serial DPM access (e.g. via SPI)

      15       01.06.2010  MT       Changed: 
                                    - DSR Handler now only processes flags if NSF_READY is set. This prevents
                                      interference during system reset (xSysdeviceReset)
      14       30.05.2010  SS       Bugfix: 
                                    - DSR Handler crashes due to invalid access on a communication channel instance

      13       29.03.2010  MT       - Added define CIFX_TOOLKIT_ENABLE_DSR_LOCK to allow locking against DSR for
                                      system that cannot guarantee that DSR has lower Prio that ISR.
                                    - IOMode / HandshakeBitState is now read from common status block if available
                                      Fallback is to use the modes read at startup during channel enumeration
      12       12.03.2010  RM       ISR handler, confirm interrupts only if we have active
                                    interrupts

      11       20.07.2009  RM       ISR handshake buffer handling extended by a valid flag

      10       29.06.2009  MT       Bugfix:
                                    - ISR Handler may clear IRQ status flags on PCI, due to hardware readahead FIFO

      9        26.05.2009  RM       Bugfix:
                                    - ISR Handler for DPM hardware changed, because of active firmware IRQs

      8        25.05.2009  MT       Bugfix:
                                    - ISR Handler may clear IRQ status flags twice on PCI and DPM devices with 64kB DPM
                                      This results in a missed Interrupt

      7        04.03.2009  MT       Bugfix:
                                    - ISRHandler did not convert IRQ status flags from Intel to
                                      Host byte order. This could result in Global IRQ bit not being
                                      detected.
      6        20.01.2009  MT       Added:
                                    - Big endian host support added (set #define CIFX_TOOLKIT_BIGENDIAN)
      5        26.11.2008  MT       - cifXTKitISRHandler modified to allow ignoring shared IRQs
                                      on PCI cards.
      4        06.05.2008  MT       - Signalling interrupts from DSR changed, so that systems
                                      that may get interrupted during DSR are working
                                    - DSRhandler changed to signal netX Handshake bit changes (since previous irq)
                                      instead of Host/Netx flag differences. This allows DEV_WaitForBitstate_Irq
                                      to correctly wait for RCX_FLAGS_EQUAL events.
      3        04.04.2007  RM       Host COS flag handling in cifXTKitDSRHandler() included

      2        23.03.2007  MT       The handshake block must be read inside the IST again
                                    (only in PCI mode) due to a missed interrupts cause
                                    by a FIFO/cache behaviour of the netX.
      1        21.03.2007  MT       initial version

**************************************************************************************/

#include "cifXHWFunctions.h"
#include "OS_Dependent.h"
#include "cifXErrors.h"
#include "cifXEndianess.h"

/*****************************************************************************/
/*!  \addtogroup CIFX_TOOLKIT_FUNCS cifX DPM Toolkit specific functions
*    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Low-Level interrupt handler
*   \param ptDevInstance Instance that propably generated an IRQ (on PCI devices
*                        the routine decides if it was an IRQ for shared interrupt lines)
*   \param fPCIIgnoreGlobalIntFlag  Ignore the global interupt flag on PCI cards,
*                                   to detect shared interrupts. This might be neccessary
*                                   if the user has already filtered out all shared IRQs
*   \return CIFX_TKIT_IRQ_DSR_REQUESTED/CIFX_TKIT_IRQ_HANDLED on success
*           CIFX_TKIT_IRQ_OTHERDEVICE if the IRQ is not from the device      */
/*****************************************************************************/
int cifXTKitISRHandler(PDEVICEINSTANCE ptDevInstance, int fPCIIgnoreGlobalIntFlag)
{
  int iRet;

  /* Check if DPM is available, if not, it cannot be our card, that caused the interrupt */
  if( HWIF_READ32(ptDevInstance, *(uint32_t*)ptDevInstance->pbDPM) == CIFX_DPM_INVALID_CONTENT)
    return CIFX_TKIT_IRQ_OTHERDEVICE;

  if(!ptDevInstance->fIrqEnabled)
  {
    /* Irq is disabled on device, so we assume the user activated the interrupts,
       but wants to poll the card. */

    USER_Trace(ptDevInstance,
               TRACE_LEVEL_ERROR,
               "cifXTKitISRHandler() : We received an interrupt, but IRQs are disabled!");

    iRet = CIFX_TKIT_IRQ_OTHERDEVICE;

  } else
  {
    /* We are working in interrupt mode */
    uint32_t              ulChannel;
    int                   iIrqToDsrBuffer   = ptDevInstance->iIrqToDsrBuffer;
    IRQ_TO_DSR_BUFFER_T*  ptIsrToDsrBuffer  = &ptDevInstance->atIrqToDsrBuffer[iIrqToDsrBuffer];
    NETX_HANDSHAKE_ARRAY* ptHandshakeBuffer = &ptIsrToDsrBuffer->tHandshakeBuffer;

    ptIsrToDsrBuffer->fValid = 1;

    /* on a DPM module every handshake cell can be read individually,
       on a PCI module the complete handshake register block must be read sequencially */
    if( (!ptDevInstance->fPCICard) ||
        (!ptDevInstance->pbHandshakeBlock) )
    {
      /* DPM card */

      /* If the complete DPM is available, clear interrupts using status register
         This is needed if the global register block at end of DPM is available */
      if(NETX_DPM_MEMORY_SIZE == ptDevInstance->ulDPMSize)
      {
        uint32_t ulIrqState0 = LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_0));
        uint32_t ulIrqState1 = LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_1));

        /* We can safely clear handshake interrupts here, as we are reading the handshake flags below,
           so we won't miss an IRQ.*/
        HWIF_WRITE32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_0, HOST_TO_LE32(ulIrqState0));
        HWIF_WRITE32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_1, HOST_TO_LE32(ulIrqState1));
      }

      ++ptDevInstance->ulIrqCounter;

      /* Check if we have a handshake block, if so, we read it completely on DPM hardwares
         to make sure, illegally activated handshake cells, don't cause interrupts */
      if (NULL != ptDevInstance->pbHandshakeBlock)
      {
        HWIF_READN( ptDevInstance,
                    ptHandshakeBuffer,
                    ptDevInstance->pbHandshakeBlock,
                    sizeof(*ptHandshakeBuffer));
      } else
      {
        /* We do not have a handshake block, so we have to read them one by one */
        /* and only for the available channels */
        ptHandshakeBuffer->atHsk[0].ulValue = HWIF_READ32(ptDevInstance, ptDevInstance->tSystemDevice.ptHandshakeCell->ulValue);

        for(ulChannel = 0; ulChannel < ptDevInstance->ulCommChannelCount; ++ulChannel)
        {
          PCHANNELINSTANCE ptChannel = (PCHANNELINSTANCE)ptDevInstance->pptCommChannels[ulChannel];
          uint32_t         ulBlockID = ptChannel->ulBlockID;

          ptHandshakeBuffer->atHsk[ulBlockID].ulValue = HWIF_READ32(ptDevInstance, ptChannel->ptHandshakeCell->ulValue);
        }
      }

      /* we need to check in DSR which handshake bits have changed */
      iRet = CIFX_TKIT_IRQ_DSR_REQUESTED;

    } else
    {
      /* PCI card */

      uint32_t ulIrqState0 = LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_0));
      uint32_t ulIrqState1 = LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_1));

      /* First check if we have generated this interrupt by reading the global IRQ status bit */
      if(  !fPCIIgnoreGlobalIntFlag &&
          (0 == (ulIrqState0 & MSK_IRQ_STA0_INT_REQ)) )
      {
        /* we have not generated this interrupt, so it must be another device on shared IRQ */
        iRet = CIFX_TKIT_IRQ_OTHERDEVICE;

      } else
      {
        NETX_HANDSHAKE_ARRAY* ptHandshakeBlock = (NETX_HANDSHAKE_ARRAY*)ptDevInstance->pbHandshakeBlock;
        uint32_t              ulCell;

        /* confirm all interrupts.
           We can safely clear handshake interrupts here, as we are reading the handshake flags below,
           so we won't miss an IRQ.*/
        HWIF_WRITE32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_0, HOST_TO_LE32(ulIrqState0));
        HWIF_WRITE32(ptDevInstance, ptDevInstance->ptGlobalRegisters->ulIRQState_1, HOST_TO_LE32(ulIrqState1));

        ++ptDevInstance->ulIrqCounter;

        /* Only read first 8 Handshake cells, due to a netX hardware issue. Reading flags 8-15 may
           also confirm IRQs for Handshake cell 0-7 due to an netX internal readahead buffer */
        for(ulCell = 0; ulCell < 8; ++ulCell)
          ptHandshakeBuffer->atHsk[ulCell].ulValue = HWIF_READ32(ptDevInstance, ptHandshakeBlock->atHsk[ulCell].ulValue);

        /* we need to check in DSR which handshake bits have changed */
        iRet = CIFX_TKIT_IRQ_DSR_REQUESTED;
      }
    }
  }
  
  return iRet;
}

/*****************************************************************************/
/*! Process IO Areas for changes / callbacks
*   \param ptChannel      Channel Instance
*   \param ptIoArea       IO Area
*   \param usChangedBits  Bits that have changed since last IRQ
*   \param usUnequalBits  Bits that are unequal between host and netX
*   \param fOutput        !=0 if an output area is processed                 */
/*****************************************************************************/
static void ProcessIOArea(PCHANNELINSTANCE  ptChannel,
                          PIOINSTANCE       ptIoArea,
                          uint16_t          usChangedBits,
                          uint16_t          usUnequalBits,
                          int               fOutput)
{
  uint16_t usBitMask = (uint16_t)(1 << ptIoArea->bHandshakeBit);
  
  if(usChangedBits & usBitMask)
  {
    PFN_NOTIFY_CALLBACK pfnCallback = NULL;
    uint8_t             bIOBitState = DEV_GetIOBitstate(ptChannel, ptIoArea, fOutput);
    
    switch(bIOBitState)
    {
    case RCX_FLAGS_EQUAL:
      if(0 == (usUnequalBits & usBitMask))
        pfnCallback = ptIoArea->pfnCallback;
      break;

    case RCX_FLAGS_NOT_EQUAL:
      if(usUnequalBits & usBitMask)
        pfnCallback = ptIoArea->pfnCallback;
      break;

    case RCX_FLAGS_CLEAR:
      if(0 == (ptChannel->usNetxFlags & usBitMask))
        pfnCallback = ptIoArea->pfnCallback;
      break;

    case RCX_FLAGS_SET:
      if(ptChannel->usNetxFlags & usBitMask)
        pfnCallback = ptIoArea->pfnCallback;
      break;
    }
    
    if(pfnCallback)
      pfnCallback(ptIoArea->ulNotifyEvent, 0, NULL, ptIoArea->pvUser);
  
    OS_SetEvent(ptChannel->ahHandshakeBitEvents[ptIoArea->bHandshakeBit]);
  }
}

/*****************************************************************************/
/*! Deferred interrupt handler
*   \param ptDevInstance Instance the DSR is requested for                   */
/*****************************************************************************/
void cifXTKitDSRHandler(PDEVICEINSTANCE ptDevInstance)
{
  if(!ptDevInstance->fResetActive)
  {
    /* Get actual data buffer index */
    uint32_t              ulChannel        = 0;
    PCHANNELINSTANCE      ptChannel        = &ptDevInstance->tSystemDevice;
    int                   iIrqToDsrBuffer  = 0;
    IRQ_TO_DSR_BUFFER_T*  ptIrqToDsrBuffer = NULL;

#ifdef CIFX_TOOLKIT_ENABLE_DSR_LOCK
    /* Lock against ISR */
    OS_IrqLock(ptDevInstance->pvOSDependent);
#else

    /* ATTENTION: The IrqToDsr Buffer handling implies a "always" higher priority */
    /*            of the ISR function. This does usually happens on physical ISR functions */
    /*            but does not work if the ISR and DSR are handled as a threads! */

#endif

    iIrqToDsrBuffer  = ptDevInstance->iIrqToDsrBuffer;
    ptIrqToDsrBuffer = &ptDevInstance->atIrqToDsrBuffer[iIrqToDsrBuffer];

    if(!ptIrqToDsrBuffer->fValid)
    {
      /* Interrupt did not provide data yet */

#ifdef CIFX_TOOLKIT_ENABLE_DSR_LOCK
      /* Release lock against ISR */
      OS_IrqUnlock(ptDevInstance->pvOSDependent);
#endif

      return;
    } else
    {
      /* Flip data buffer so IRQ uses the other buffer */
      ptDevInstance->iIrqToDsrBuffer ^= 0x01;

      /* Invalidate the buffer, we are now handling */
      ptIrqToDsrBuffer->fValid        = 0;
    }

#ifdef CIFX_TOOLKIT_ENABLE_DSR_LOCK
    /* Release lock against ISR */
    OS_IrqUnlock(ptDevInstance->pvOSDependent);
#endif

    /* Only process rest of flags if NSF_READY is set. This must be done to prevent
       confusion of the toolkit during a system start (xSysdeviceReset) */
    if(ptIrqToDsrBuffer->tHandshakeBuffer.atHsk[0].t8Bit.bNetxFlags & NSF_READY)
    {
      /*--------------------------------------------------------------------*/
      /* Evaluate device synchronistation flags, the flags are fixed 16 Bit */
      /*--------------------------------------------------------------------*/
      uint16_t  usChangeBits;
      uint16_t  usUnequalBits;
      uint32_t  ulBitPos;
      uint16_t  usOldNSyncFlags = ptDevInstance->tSyncData.usNSyncFlags; /* Remember last known netX flags */

      /* Get pointer to the new flag data from ISR */
      NETX_HANDSHAKE_CELL*  ptSyncCell  = &ptIrqToDsrBuffer->tHandshakeBuffer.atHsk[NETX_HSK_SYNCH_FLAG_POS];

      /* Get the actual flags */
      ptDevInstance->tSyncData.usNSyncFlags = LE16_TO_HOST(ptSyncCell->t16Bit.usNetxFlags);

      /* Check which bits have changed since last interrupt from netX side */
      usChangeBits  = usOldNSyncFlags ^ ptDevInstance->tSyncData.usNSyncFlags;
      usUnequalBits = ptDevInstance->tSyncData.usNSyncFlags ^ ptDevInstance->tSyncData.usHSyncFlags;

      /* Signal sync events */
      for(ulBitPos = 0; ulBitPos < NETX_NUM_OF_SYNCH_FLAGS; ++ulBitPos)
      {
        /* There is a valid channel */
        uint16_t         usBitMask = (uint16_t)(1 << ulBitPos);
        uint8_t          bState    = RCX_FLAGS_NOT_EQUAL;
        PCHANNELINSTANCE ptChannel = NULL;

        if (ulBitPos >= ptDevInstance->ulCommChannelCount)
          break;

        ptChannel = (PCHANNELINSTANCE)ptDevInstance->pptCommChannels[ulBitPos];

        /* Handle Sync interrupts */
        if( RCX_SYNC_MODE_HST_CTRL == HWIF_READ8(ptDevInstance, ptChannel->ptCommonStatusBlock->bSyncHskMode))
          bState = RCX_FLAGS_EQUAL;

        if ( usChangeBits & usBitMask)
        {
          int fProcess = 0;

          /* Check which mode to handle */
          /* RCX_FLAGS_NOT_EQUAL corresponds to DEVICE_CONTROLLED */
          if( (bState == RCX_FLAGS_NOT_EQUAL) &&
              (usUnequalBits & usBitMask) )
          {
            fProcess = 1;

          } else if( (bState == RCX_FLAGS_EQUAL) &&
                     (0 == (usUnequalBits & usBitMask)) )
          {
            fProcess = 1;
          }

          if(fProcess)
          {
            /* There is a valid channel */
            PCHANNELINSTANCE ptChannel = (PCHANNELINSTANCE)ptDevInstance->pptCommChannels[ulBitPos];

            /* Check if we have a callback assigned */
            if (ptChannel->tSynch.pfnCallback)
              ptChannel->tSynch.pfnCallback( CIFX_NOTIFY_SYNC, 0, NULL, ptChannel->tSynch.pvUser);

            /* Signal event to allow waiting for sync state without callback */
            if( ptDevInstance->tSyncData.ahSyncBitEvents[ulBitPos])
              OS_SetEvent(ptDevInstance->tSyncData.ahSyncBitEvents[ulBitPos]);
          }
        }
      }

      /*-----------------------------------------------------*/
      /* Evaluate all changed handshake bits on all channels */
      do
      {
        uint16_t usChangedBits;
        uint16_t usUnequalBits;
        uint16_t usOldNetxFlags = ptChannel->usNetxFlags; /* Remember last known netX flags */
        uint32_t ulIdx;

        /* Address the handshake cell */
        NETX_HANDSHAKE_CELL* ptHskCell = &ptIrqToDsrBuffer->tHandshakeBuffer.atHsk[ptChannel->ulBlockID];

        if(ptChannel->bHandshakeWidth == RCX_HANDSHAKE_SIZE_8BIT)
        {
          ptChannel->usNetxFlags = ptHskCell->t8Bit.bNetxFlags;
        } else
        {
          ptChannel->usNetxFlags = LE16_TO_HOST(ptHskCell->t16Bit.usNetxFlags);
        }

        /* Check which bits have changed since last interrupt from netX side */
        usChangedBits = usOldNetxFlags ^ ptChannel->usNetxFlags;
        usUnequalBits = ptChannel->usNetxFlags ^ ptChannel->usHostFlags;

        /*------------------------------------------*/
        /* Process CHANNEL Hardware COS flags       */
        /*------------------------------------------*/
        /* Check if we have a valid channel (not for the bootloader) */
        if(ptChannel->fIsChannel)
        {
          /* Lock flag access */
          OS_EnterLock(ptChannel->pvLock);

          /* Check if the hardware signals new COS flags */
          if((ptChannel->usNetxFlags ^ ptChannel->usHostFlags) & NCF_NETX_COS_CMD)
          {
            /* Read the flags and acknowledge them */
            uint32_t ulNewCOSFlags = LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptChannel->ptCommonStatusBlock->ulCommunicationCOS));

            /* Check if they have changed */
            if(ptChannel->ulDeviceCOSFlags != ulNewCOSFlags)
            {
              ptChannel->ulDeviceCOSFlagsChanged  = ptChannel->ulDeviceCOSFlags ^ ulNewCOSFlags;
              ptChannel->ulDeviceCOSFlags         = ulNewCOSFlags;
            }

            DEV_ToggleBit(ptChannel, HCF_NETX_COS_ACK);
          }

          /* Unlock flag access */
          OS_LeaveLock(ptChannel->pvLock);

        /*------------------------------------------*/
        /* Process SYSTEM DEVICE Hardware COS flags */
        /*------------------------------------------*/
        } else if(ptChannel->fIsSysDevice)
        {
          /* Lock flag access */
          OS_EnterLock(ptChannel->pvLock);

          /* Check if the hardware signals new system COS flags */
          if((ptChannel->usNetxFlags ^ ptChannel->usHostFlags) & NSF_NETX_COS_CMD)
          {
            /* Read the flags and acknowledge them */
            NETX_SYSTEM_CHANNEL* ptSyschannel   = (NETX_SYSTEM_CHANNEL*)ptChannel->pbDPMChannelStart;
            uint32_t             ulNewCOSFlags  = LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptSyschannel->tSystemState.ulSystemCOS));

            /* Read the flags and acknowledge them */
            if(ptChannel->ulDeviceCOSFlags != ulNewCOSFlags)
            {
              ptChannel->ulDeviceCOSFlagsChanged  = ptChannel->ulDeviceCOSFlags ^ ulNewCOSFlags;
              ptChannel->ulDeviceCOSFlags         = ulNewCOSFlags;
            }

            DEV_ToggleBit(ptChannel, HSF_NETX_COS_ACK);
          }

          /* Unlock flag access */
          OS_LeaveLock(ptChannel->pvLock);
        }

        /*---------------------------------------------------*/
        /* Process our own COS flags (Write them to device)  */
        /*---------------------------------------------------*/
        /* Do not process for the system channel */
        if ( !ptChannel->fIsSysDevice)
        {
          /* Lock flag access */
          OS_EnterLock(ptChannel->pvLock);

          if(!((ptChannel->usNetxFlags ^ ptChannel->usHostFlags) & NCF_HOST_COS_ACK))
          {
            /* COS flags are equal */
            if ( LE32_TO_HOST(HWIF_READ32(ptDevInstance, ptChannel->ptControlBlock->ulApplicationCOS)) != ptChannel->ulHostCOSFlags)
            {
              /* Update COS flags */
              HWIF_WRITE32(ptDevInstance, ptChannel->ptControlBlock->ulApplicationCOS, HOST_TO_LE32(ptChannel->ulHostCOSFlags));

              /* Signal new COS flags */
              DEV_ToggleBit(ptChannel, HCF_HOST_COS_CMD);

              /* Remove all enable flags from the local COS flags */
              ptChannel->ulHostCOSFlags &= ~(RCX_APP_COS_BUS_ON_ENABLE | RCX_APP_COS_INITIALIZATION_ENABLE | RCX_APP_COS_LOCK_CONFIGURATION_ENABLE);
            }
          }

          /* Unlock flag access */
          OS_LeaveLock(ptChannel->pvLock);

          /* Check COM Flag */
          if(usChangedBits & NCF_COMMUNICATING)
            OS_SetEvent(ptChannel->ahHandshakeBitEvents[NCF_COMMUNICATING_BIT_NO]);

          /* Check COS Flag */
          if(usChangedBits & NCF_NETX_COS_CMD)
            OS_SetEvent(ptChannel->ahHandshakeBitEvents[NCF_NETX_COS_CMD_BIT_NO]);

          if(usChangedBits & NCF_HOST_COS_ACK)
            OS_SetEvent(ptChannel->ahHandshakeBitEvents[NCF_HOST_COS_ACK_BIT_NO]);

        } else
        {
          /* This is a system channel */

          /* Check COS Flag */
          if(usChangedBits & NSF_NETX_COS_CMD)
            OS_SetEvent(ptChannel->ahHandshakeBitEvents[NSF_NETX_COS_CMD_BIT_NO]);

          if(usChangedBits & NSF_HOST_COS_ACK)
            OS_SetEvent(ptChannel->ahHandshakeBitEvents[NSF_HOST_COS_ACK_BIT_NO]);
        }

        /*-----------------------------------------------------------------------*/
        /* Handling is now done per known block, to be able to process callbacks */
        /*-----------------------------------------------------------------------*/

        /* Check for COM State Changes */
        if(usChangedBits & NCF_COMMUNICATING)
        { 
          /* check if notification is registerd */
          if (NULL != ptChannel->tComState.pfnCallback)
          {
            CIFX_NOTIFY_COM_STATE_T tData;

            tData.ulComState = (ptChannel->usNetxFlags & NCF_COMMUNICATING);

            ptChannel->tComState.pfnCallback( CIFX_NOTIFY_COM_STATE,
                                              sizeof(tData),
                                              &tData,
                                              ptChannel->tComState.pvUser);
          }
        }

        /* Check IO - Input Areas */
        for(ulIdx = 0; ulIdx < ptChannel->ulIOInputAreas; ++ulIdx)
        {
          ProcessIOArea(ptChannel,
                        ptChannel->pptIOInputAreas[ulIdx],
                        usChangedBits,
                        usUnequalBits,
                        0);
        }

        /* Check IO - Output Areas */
        for(ulIdx = 0; ulIdx < ptChannel->ulIOOutputAreas; ++ulIdx)
        {
          ProcessIOArea(ptChannel,
                        ptChannel->pptIOOutputAreas[ulIdx],
                        usChangedBits,
                        usUnequalBits,
                        1);
        }

        /* Check Receive Mailbox */
        if(usChangedBits & ptChannel->tRecvMbx.ulRecvACKBitmask)
        {
          if( (usUnequalBits & ptChannel->tRecvMbx.ulRecvACKBitmask) &&
              (NULL != ptChannel->tRecvMbx.pfnCallback) )
          {
            CIFX_NOTIFY_RX_MBX_FULL_DATA_T tRxData;

            tRxData.ulRecvCount = ptChannel->tRecvMbx.ptRecvMailboxStart->usWaitingPackages;

            ptChannel->tRecvMbx.pfnCallback(CIFX_NOTIFY_RX_MBX_FULL,
                                            sizeof(tRxData),
                                            &tRxData,
                                            ptChannel->tRecvMbx.pvUser);
          }
          OS_SetEvent(ptChannel->ahHandshakeBitEvents[ptChannel->tRecvMbx.bRecvACKBitoffset]);
        }

        /* Check Send Mailbox */
        if(usChangedBits & ptChannel->tSendMbx.ulSendCMDBitmask)
        {
          if( (0    == (usUnequalBits & ptChannel->tSendMbx.ulSendCMDBitmask)) &&
              (NULL != ptChannel->tSendMbx.pfnCallback) )
          {
            CIFX_NOTIFY_TX_MBX_EMPTY_DATA_T tTxData;

            tTxData.ulMaxSendCount = ptChannel->tSendMbx.ptSendMailboxStart->usPackagesAccepted;

            ptChannel->tSendMbx.pfnCallback(CIFX_NOTIFY_TX_MBX_EMPTY,
                                            sizeof(tTxData),
                                            &tTxData,
                                            ptChannel->tSendMbx.pvUser);
          }
          OS_SetEvent(ptChannel->ahHandshakeBitEvents[ptChannel->tSendMbx.bSendCMDBitoffset]);
        }

        /* Next channel */
        if(ulChannel < ptDevInstance->ulCommChannelCount)
          ptChannel = ptDevInstance->pptCommChannels[ulChannel];

      } while(ulChannel++ <= ptDevInstance->ulCommChannelCount);
    }
  }
}

/*****************************************************************************/
/*!  \}                                                                      */
/*****************************************************************************/
