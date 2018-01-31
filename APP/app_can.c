/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_can.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/08
 * Description        : This file contains the software implementation for the
 *                      can module app unit
 *******************************************************************************
 * History:
 * 2013/08/08 v1.0
 *******************************************************************************/

#include "app_can.h"
#if defined(USE_STM32_CAN_DEBUG)
#if !defined(USE_UART3_TRACE_FTR)
#error "must define UART3 to trace CAN debug message!"
#endif /* USE_UART3_TRACE_FTR */

#include "can.h"
#include "trace.h"
#include <string.h>


/* CAN BUS recv data buffer size */
#define CAN_RECV_DATA_BUF_SIZE      2048u
#define CAN_RECV_DATA_BUF_LEN       sizeof(CanRxMsg)

/* can bus recv data */
static OS_EVENT   *can_recv_Queue;
static void       *can_recv_QueueTbl[CAN_RECV_DATA_BUF_SIZE];
static uint8_t     can_data_buf[CAN_RECV_DATA_BUF_SIZE][CAN_RECV_DATA_BUF_LEN];
static OS_MEM*     p_can_mem;

static OS_STK      can_task_stk[CAN_TASK_STK_SIZE];


static void can_app_task(void *p_arg);


void init_can_app_task(void)
{
    INT8U os_err;

    /* can task, for debug ***************************************************/
    os_err = OSTaskCreateExt((void (*)(void *)) can_app_task,
                            (void		   * ) 0,
                            (OS_STK 	   * )&can_task_stk[CAN_TASK_STK_SIZE - 1],
                            (INT8U			 ) CAN_TASK_PRIO,
                            (INT16U 		 ) CAN_TASK_PRIO,
                            (OS_STK 	   * )&can_task_stk[0],
                            (INT32U 		 ) CAN_TASK_STK_SIZE,
                            (void		   * ) 0,
                            (INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(CAN_TASK_PRIO, (INT8U *)"CAN BUS", &os_err);
}


static void can_app_task(void *p_arg)
{
	uint8_t   data_len;
	uint8_t   i;
	CanRxMsg *RxMessage;
	INT8U     err;
	uint32_t  can_id;
	uint8_t   can_ide_buf[4];
	uint8_t   can_rtr_buf[5];
	uint8_t   vl_buf[32];

	(void)p_arg;

	init_trace3();
	// test
	//OSTimeDlyHMSM(0, 0, 5, 0);
	APP_TRACE3("============== CAN TRACE ==============\r\n\r\n");

	// init can queue
	can_recv_Queue = OSQCreate(&can_recv_QueueTbl[0], CAN_RECV_DATA_BUF_SIZE);
	assert_param(can_recv_Queue);

	// init can mem
	p_can_mem = OSMemCreate(can_data_buf, CAN_RECV_DATA_BUF_SIZE, CAN_RECV_DATA_BUF_LEN, &err);
	assert_param(OS_ERR_NONE == err);

	// init CAN1
	CAN_Config(USER_CAN_1);

	for (;;)
	{
		data_len = 0;
		RxMessage = (CanRxMsg*)OSQPend(can_recv_Queue, 0, &err);
		if (RxMessage)
		{
			switch (RxMessage->IDE)
			{
				case CAN_Id_Standard:
					can_id = RxMessage->StdId;
					memcpy(can_ide_buf, "STD", 4);
					break;
				case CAN_Id_Extended:
					can_id = RxMessage->ExtId;
					memcpy(can_ide_buf, "EXD", 4);
					break;
				default:
					assert_param(0);
					break;
			}

			switch (RxMessage->RTR)
			{
				case CAN_RTR_Data:
					memset(vl_buf, 0x00, sizeof(vl_buf));
					// get CAN frame data
					for (i = 0; i < RxMessage->DLC; i++)
					{
						data_len += snprintf((char*)vl_buf + data_len,
											 sizeof(vl_buf) - data_len,
											 "%.2x ", RxMessage->Data[i]);
					}
					memcpy(can_rtr_buf, "DATA", 5);
					break;
				case CAN_RTR_Remote:
					vl_buf[0] = 0;
					memcpy(can_rtr_buf, "REMO", 5);
					break;
				default:
					assert_param(0);
					break;
			}

			//
			// Display CAN Frame
			// STD/EXD  DATA/REMOTE  CANID  DLC  DATA[...]
			//
			TICK_TRACE3(" %s %s %8x %d %s\r\n",
						can_ide_buf,
						can_rtr_buf,
						can_id,
						RxMessage->DLC,
						vl_buf);

			/* release memory */
			OSMemPut(p_can_mem, (void*)RxMessage);
		}
		else
		{
			assert_param(0);
		}
	}

}



/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/******************************************************************************/
/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg         RxMessage;
	INT8U            err;
	void            *p_mem;

	OSIntEnter();

	if (CAN_GetFlagStatus(CAN1, CAN_FLAG_FMP0) != RESET)
	{
		memset(&RxMessage, 0x00, sizeof(RxMessage));
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		/* get memory */
		p_mem = OSMemGet(p_can_mem, &err);
		if (p_mem)
		{
			memcpy(p_mem, (void*)&RxMessage, sizeof(RxMessage));
			/* Send data to user task */
			OSQPost(can_recv_Queue, (void *)p_mem);
		}
		else
		{
			TRACE3("OSMemGet err\r\n");
		}
	}

	/* Error Warning Flag */
	if (CAN_GetFlagStatus(CAN1, CAN_FLAG_EWG) != RESET)
	{
		TRACE3("*** CAN1 CAN_FLAG_EWG---Error Warning Flag---\r\n");
		CAN_ClearITPendingBit(CAN1, CAN_IT_EWG);
	}

	/* Error Passive Flag */
	if (CAN_GetFlagStatus(CAN1, CAN_FLAG_EPV) != RESET)
	{
		TRACE3("*** CAN1 CAN_FLAG_EPV---Error Passive Flag---\r\n");
		CAN_ClearITPendingBit(CAN1, CAN_IT_EPV);
	}

	/* Bus-Off Flag */
	if (CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF) != RESET)
	{
		TRACE3("*** CAN1 CAN_FLAG_BOF---Bus-Off Flag---\r\n");
		CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
	}

	/* Last error code Flag */
	if (CAN_GetFlagStatus(CAN1, CAN_FLAG_LEC) != RESET)
	{
		TRACE3("*** CAN1 CAN_FLAG_LEC---Last error code Flag[%.2x]---\r\n", CAN_GetLastErrorCode(CAN1));
		CAN_ClearITPendingBit(CAN1, CAN_IT_LEC);
	}

	OSIntExit();
}



#endif /* USE_STM32_CAN_DEBUG */


