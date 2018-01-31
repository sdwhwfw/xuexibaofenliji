/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_usart_dma_route.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/26
 * Description        : This file contains the software implementation for the
 *                      usart dma routing data task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/26 | v1.0  | Bruce.zhu  | initial released
 * 2013/10/15 | v1.1  | Bruce.zhu  | move init code
 * 2013/11/01 | v1.2  | Bruce.zhu  | add code to support USART1 or USART3
 *                                   dma route task.
 * 2013/12/13 | v1.3  | Bruce.zhu  | add support for all usart dma send and recv
 *******************************************************************************/
#ifndef _APP_USART_DMA_ROUTE_C_
#define _APP_USART_DMA_ROUTE_C_

#include "app_usart_dma_route.h"
#include "trace.h"
#include "usart.h"
#include <string.h>


//--------------------- FOR USART TX ------------------------------
static OS_STK     usart_dma_route_tx_task_stk[USART_DMA_ROUTE_TX_TASK_STK_SIZE];
static OS_EVENT*  g_usart_tx_dma_queue;
static void*      g_usart_rx_QueueTbl[8];
static OS_EVENT*  g_usart_tx_dma_sem[COM_NUM];
static void*      p_backup_mem_block[COM_NUM];
//-----------------------------------------------------------------

//--------------------- FOR USART RX ------------------------------
static OS_STK     usart_dma_route_rx_task_stk[USART_DMA_ROUTE_RX_TASK_STK_SIZE];
#define           USART_DMA_RECV_SIZE        128
#define           USART_DMA_RECV_BUF_NUM     16

// last 4 bytes: 2bytes store packet length, 
//               2bytes for usart number
#define           USART_DMA_RECV_BUF_SIZE    (USART_DMA_RECV_SIZE + 4)


static OS_EVENT*  g_usart_rx_dma_event;
static void*      g_usart_recv_QueueTbl[8];
//-----------------------------------------------------------------

//------------------ Memory block for RX and TX -------------------
static u8         g_dma_data_buf[USART_DMA_RECV_BUF_NUM][USART_DMA_RECV_BUF_SIZE];
static OS_MEM*    g_dma_mem;
static void*      g_mem_block[COM_NUM];
//-----------------------------------------------------------------

static p_usart_dma_recv p_usart_dma_recv_fun[COM_NUM] = {0};
static u8               g_usart_dma_ready_flag = 0;

static void usart_dma_send_task(void* p_arg);
static void usart_dma_route_task(void *p_arg);

void init_usart_dma_route_task(void)
{
    INT8U os_err;
    int i;

    // create tx queue
    g_usart_tx_dma_queue = (OS_EVENT*)OSQCreate(g_usart_rx_QueueTbl, 8);
    assert_param(g_usart_tx_dma_queue);

    // create dma tx semaphore
    for (i = 0; i < COM_NUM; i++)
    {
        g_usart_tx_dma_sem[i] = (OS_EVENT*)OSSemCreate(1);
        assert_param(g_usart_tx_dma_sem[i]);
    }

    g_usart_rx_dma_event = (OS_EVENT*)OSQCreate(&g_usart_recv_QueueTbl[0], 8);
    assert_param(g_usart_rx_dma_event);

    // init USART DMA memory
    g_dma_mem = OSMemCreate(g_dma_data_buf, USART_DMA_RECV_BUF_NUM, USART_DMA_RECV_BUF_SIZE, &os_err);
    assert_param(OS_ERR_NONE == os_err);

    os_err = OSTaskCreateExt((void (*)(void *)) usart_dma_route_task,
                            (void          * ) 0,
                            (OS_STK        * )&usart_dma_route_rx_task_stk[USART_DMA_ROUTE_RX_TASK_STK_SIZE - 1],
                            (INT8U           ) USART_DMA_ROUTE_RX_TASK_PRIO,
                            (INT16U          ) USART_DMA_ROUTE_RX_TASK_PRIO,
                            (OS_STK        * )&usart_dma_route_rx_task_stk[0],
                            (INT32U          ) USART_DMA_ROUTE_RX_TASK_STK_SIZE,
                            (void          * ) 0,
                            (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(USART_DMA_ROUTE_RX_TASK_PRIO, (INT8U *)"usart dma route", &os_err);

}


/*
 * @brief: call this function to init callback function
 * @param: uart_num, COM1 - COM5
 * @param: p_fun, callback function pointer
 * @param: even_flag, PARITY_NONE ---> 0
 *                    PARITY_EVEN ---> 1
 *                    PARITY_ODD  ---> 2
 */
void init_usart_with_recv_callback(uart_num_def_t uart_num, p_usart_dma_recv p_fun, u32 baud_rate, parity_mode even_flag)
{
    INT8U err;

    // check parameter
    assert_param(IS_USART_COM_NUM(uart_num));
    assert_param(p_fun);
    assert_param(IS_USART_COM_PARITY(even_flag));

    p_usart_dma_recv_fun[uart_num] = p_fun;

    /*
    g_mem_block[uart_num] = OSMemGet(g_dma_mem, &err);
    assert_param(g_mem_block[uart_num]);
    
    reload_usart_dma_rx(uart_num, (u8*)g_mem_block[uart_num], USART_DMA_RECV_SIZE);
    */


    /* init USART with DMA feature */
    switch (even_flag)
    {
        case PARITY_NONE:
            init_usart(uart_num, baud_rate, 8, PARITY_NONE, USART_MODE_DMA);
            break;
        case PARITY_EVEN:
        case PARITY_ODD:
            init_usart(uart_num, baud_rate, 9, even_flag, USART_MODE_DMA);//8+1
            break;
    }

    // get memory to reload the first DMA rx
    
   g_mem_block[uart_num] = OSMemGet(g_dma_mem, &err);
   assert_param(g_mem_block[uart_num]);

   reload_usart_dma_rx(uart_num, (u8*)g_mem_block[uart_num], USART_DMA_RECV_SIZE);
   //////why to reload??????
     
}


/**
 * @brief: Use DMA send cmd data, so it may not send complete after
 *         this function return.
 * @param: p_cmd, pointer to your send data
 * @param: cmd_len, send cmd data length,
 *                  please don't send more than USART_DMA_RECV_SIZE bytes.
 * @retval 0 ------> success
 *         other --> error
 */
u8 send_data_packet(uart_num_def_t uart, u8* p_cmd, u16 cmd_len)
{
    INT8U err;
    u8*   p_mem_block;
    u16*  p_data_value;
    u16*  p_usart_num_value;

    assert_param(IS_USART_COM_NUM(uart));

    // check route task ready
    if (g_usart_dma_ready_flag == 0)
        return 1;

    if (cmd_len >= USART_DMA_RECV_SIZE)
        return 1;

    p_mem_block = OSMemGet(g_dma_mem, &err);
    if (p_mem_block)
    {
        memcpy(p_mem_block, p_cmd, cmd_len);

        // data packet length
        p_data_value  = (u16*)(p_mem_block + USART_DMA_RECV_SIZE);
        *p_data_value = cmd_len;

        // usart number
        p_usart_num_value  = (u16*)(p_mem_block + USART_DMA_RECV_SIZE + 2);
        *p_usart_num_value = (u16)uart;

        // send this memory block to task
        OSQPost(g_usart_tx_dma_queue, (void*)p_mem_block);
    }
    else
    {
        APP_TRACE("send_data_packet get memory error!\r\n");
        return 1;
    }

    return 0;
}


/*
 * @brief: This task is used for packet store with FIFO
 *         and reload USART DMA tx
 */
static void usart_dma_send_task(void* p_arg)
{
    INT8U err;
    u8*   p_tx_data;
    u16   tx_data_len;
    uart_num_def_t uart_index;

    APP_TRACE("USART with DMA send task ready...\r\n");

    // fix bug: set usart dma route task ready flag
    g_usart_dma_ready_flag = 1;

    for (;;)
    {
        // wait tx data packet
        p_tx_data = (u8*)OSQPend(g_usart_tx_dma_queue, 0, &err);
        tx_data_len = *(u16*)(&p_tx_data[USART_DMA_RECV_SIZE]);
        uart_index  = (uart_num_def_t)*(u16*)(&p_tx_data[USART_DMA_RECV_SIZE + 2]);

        // Here, we must wait dma tx complete or timeout
        OSSemPend(g_usart_tx_dma_sem[uart_index], 2000, &err);
        if (OS_ERR_NONE == err)
        {
            // store the memory block pointer
            // free in DMA_Stream_IRQHandler
            p_backup_mem_block[uart_index] = (void*)p_tx_data;

            reload_usart_dma_tx(uart_index, (u8*)p_tx_data, tx_data_len);
        }
        else
        {
            // DMA send time out, free this memory block
            OSMemPut(g_dma_mem, (void*)p_tx_data);
            p_backup_mem_block[uart_index] = 0;
            APP_TRACE("Warning: DMA send timeout[UART%], some error maybe happened\r\n", uart_index);
        }
    }
}


/*
 * do dma reload job for usart rx
 *
 * Here, we use USART3. 115200, 1 stop bit, no parity, DMA mode
 * You can change it to other USART easily
 *
 */
static void usart_dma_route_task(void *p_arg)
{
    INT8U err;
    u8*   p_recv_data;
    u16   rx_data_len;
    u16   rx_uart_number;

    /* Create USART send task */
    err = OSTaskCreateExt((void (*)(void *)) usart_dma_send_task,
                            (void       * ) 0,
                            (OS_STK     * )&usart_dma_route_tx_task_stk[USART_DMA_ROUTE_TX_TASK_STK_SIZE - 1],
                            (INT8U        ) USART_DMA_ROUTE_TX_TASK_PRIO,
                            (INT16U       ) USART_DMA_ROUTE_TX_TASK_PRIO,
                            (OS_STK     * )&usart_dma_route_tx_task_stk[0],
                            (INT32U       ) USART_DMA_ROUTE_TX_TASK_STK_SIZE,
                            (void       * ) 0,
                            (INT16U       )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == err);
    OSTaskNameSet(USART_DMA_ROUTE_TX_TASK_PRIO, (INT8U *)"usart dma send", &err);

    APP_TRACE("USART with DMA ready to recv...\r\n");
    for (;;)
    {
        p_recv_data = (u8*)OSQPend(g_usart_rx_dma_event, 0, &err);
        assert_param(OS_ERR_NONE == err);

        rx_data_len    = *(u16*)(&p_recv_data[USART_DMA_RECV_SIZE]);
        rx_uart_number = *(u16*)(&p_recv_data[USART_DMA_RECV_SIZE + 2]);

        // callback user defined function
        if (p_usart_dma_recv_fun[rx_uart_number])
        {
            p_usart_dma_recv_fun[rx_uart_number](p_recv_data, rx_data_len);
        }

        // free gobal memory
        OSMemPut(g_dma_mem, (void*)p_recv_data);
    }

}




/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/******************************************************************************/

/**
  * Note: See STM32F2xx RU V5.pdf page 173
  *
  * ----- USART1 -----
  * RX DMA Stream: DMA2_STREAM2
  * TX DMA Stream: DMA2_STREAM7
  *
  * ----- USART2 -----
  * RX DMA Stream: DMA1_STREAM5
  * TX DMA Stream: DMA1_STREAM6
  *
  * ----- USART3 -----
  * RX DMA Stream: DMA1_STREAM1
  * TX DMA Stream: DMA1_STREAM3
  *
  * ----- USART4 -----
  * RX DMA Stream: DMA1_STREAM2
  * TX DMA Stream: DMA1_STREAM4
  *
  * ----- USART5 -----
  * RX DMA Stream: DMA1_STREAM0
  * TX DMA Stream: DMA1_STREAM7
  *
  * ----- USART6 -----
  * RX DMA Stream: DMA2_STREAM1
  * TX DMA Stream: DMA2_STREAM6
  *
  *
  */

#if defined(USE_USART1_DMA_FTR)
/*
 * DMA2 Stream 2
 * RX
 */
void DMA2_Stream2_IRQHandler(void)
{
    u16   data_recv;
    INT8U err;
    u8*   p_data;
    u16*  P_data_value;
    u16*  p_uart_index;

    OSIntEnter();
    if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2) != RESET)
    {
        data_recv = USART_DMA_RECV_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);

        if (data_recv)
        {
            // data packet length
            p_data        = (u8*)g_mem_block[COM1];
            P_data_value  = (u16*)(p_data + USART_DMA_RECV_SIZE);
            *P_data_value = data_recv;
            // save uart number
            p_uart_index  = (u16*)(p_data + USART_DMA_RECV_SIZE + 2);
            *p_uart_index = (u16)COM1;

            // send this memory block to task
            OSQPost(g_usart_rx_dma_event, (void*)g_mem_block[COM1]);
        }
        else
        {
            // free memory
            OSMemPut(g_dma_mem, (void*)g_mem_block[COM1]);
        }

        // get memory
        g_mem_block[COM1] = OSMemGet(g_dma_mem, &err);
        if (g_mem_block[COM1] == 0)
        {
            TRACE("* DMA2-2 GET MEMORY ERROR! Can't reload next rx dma\r\n");
            DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
            OSIntExit();
            return;
        }

        // reload next usart dma rx
        reload_usart_dma_rx(COM1, (u8*)g_mem_block[COM1], USART_DMA_RECV_SIZE);

        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
    }
    OSIntExit();

}


/*
 * DMA2 Stream 7
 * TX
 */
void DMA2_Stream7_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
    {
        // free memory
        if (p_backup_mem_block[COM1])
            OSMemPut(g_dma_mem, (void*)p_backup_mem_block[COM1]);

        OSSemPost(g_usart_tx_dma_sem[COM1]);
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
    }
    OSIntExit();
}


void USART1_IRQHandler(void)
{
    OSIntEnter();
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        if (DMA_GetCmdStatus(DMA2_Stream2) == ENABLE)
        {
            /*
             * Disable the DMA Rx Stream and wait DMA operation complete
             * until DMA2_Stream2_IRQHandler() interrupt
             */
            DMA_Cmd(DMA2_Stream2, DISABLE);
        }

        /* clear the IDLE flag */
        USART_ReceiveData(USART1);
    }
    OSIntExit();
}

#endif /* USE_USART1_DMA_FTR */


#if defined(USE_USART2_DMA_FTR)
/*
 * ----- USART2 -----
 * RX DMA Stream: DMA1_STREAM5
 * TX DMA Stream: DMA1_STREAM6
 */
void DMA1_Stream5_IRQHandler(void)
{
    u16   data_recv;
    INT8U err;
    u8*   p_data;
    u16*  P_data_value;
    u16*  p_uart_index;

    OSIntEnter();
    if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5) != RESET)
    {
        data_recv = USART_DMA_RECV_SIZE - DMA_GetCurrDataCounter(DMA1_Stream5);

        if (data_recv)
        {
            // save data packet length
            p_data        = (u8*)g_mem_block[COM2];
            P_data_value  = (u16*)(p_data + USART_DMA_RECV_SIZE);
            *P_data_value = data_recv;
            // save uart number
            p_uart_index  = (u16*)(p_data + USART_DMA_RECV_SIZE + 2);
            *p_uart_index = (u16)COM2;

            // send this memory block to task
            OSQPost(g_usart_rx_dma_event, (void*)g_mem_block[COM2]);
        }
        else
        {
            // free memory
            OSMemPut(g_dma_mem, (void*)g_mem_block[COM2]);
        }

        // get memory
        g_mem_block[COM2] = OSMemGet(g_dma_mem, &err);
        if (g_mem_block[COM2] == 0)
        {
            TRACE("* DMA1-5 GET MEMORY ERROR! Can't reload next rx dma\r\n");
            DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
            OSIntExit();
            return;
        }

        // reload next usart dma rx
        reload_usart_dma_rx(COM2, (u8*)g_mem_block[COM2], USART_DMA_RECV_SIZE);

        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
    }
    OSIntExit();

}

/*
 * DMA1 Stream 6
 * TX
 */
void DMA1_Stream6_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) != RESET)
    {
        // free memory
        if (p_backup_mem_block[COM2])
            OSMemPut(g_dma_mem, (void*)p_backup_mem_block[COM2]);

        OSSemPost(g_usart_tx_dma_sem[COM2]);
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
    }
    OSIntExit();
}


void USART2_IRQHandler(void)
{
    OSIntEnter();
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        if (DMA_GetCmdStatus(DMA1_Stream5) == ENABLE)
        {
            /*
             * Disable the DMA Rx Stream and wait DMA operation complete
             * until DMA interrupt
             */
            DMA_Cmd(DMA1_Stream5, DISABLE);
        }

        /* clear the IDLE flag */
        USART_ReceiveData(USART2);
    }
    OSIntExit();
}

#endif /* USE_USART2_DMA_FTR */


#if defined(USE_USART3_DMA_FTR)
/*
 * DMA1 Stream 1
 * RX
 */
void DMA1_Stream1_IRQHandler(void)
{
    u16   data_recv;
    INT8U err;
    u8*   p_data;
    u16*  P_data_value;
    u16*  p_uart_index;

    OSIntEnter();
    if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1) != RESET)
    {
        data_recv = USART_DMA_RECV_SIZE - DMA_GetCurrDataCounter(DMA1_Stream1);

        // store recv data length
        if (data_recv)
        {
            p_data        = (u8*)g_mem_block[COM3];
            P_data_value  = (u16*)(p_data + USART_DMA_RECV_SIZE);
            *P_data_value = data_recv;
            // save uart number
            p_uart_index  = (u16*)(p_data + USART_DMA_RECV_SIZE + 2);
            *p_uart_index = (u16)COM3;

            // send this memory block to task
            OSQPost(g_usart_rx_dma_event, (void*)g_mem_block[COM3]);
        }
        else
        {
            // free memory
            OSMemPut(g_dma_mem, (void*)g_mem_block[COM3]);
        }

        // get memory
        g_mem_block[COM3] = OSMemGet(g_dma_mem, &err);
        if (g_mem_block[COM3] == 0)
        {
            TRACE("DMA1-1 GET MEMORY ERROR!\r\n");
            DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
            OSIntExit();
            return;
        }

        // reload next usart dma rx
        reload_usart_dma_rx(COM3, (u8*)g_mem_block[COM3], USART_DMA_RECV_SIZE);

        DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
    }
    OSIntExit();
}


/*
 * DMA1 Stream 3
 * TX
 */
void DMA1_Stream3_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) != RESET)
    {
        // free memory
        if (p_backup_mem_block[COM3])
            OSMemPut(g_dma_mem, (void*)p_backup_mem_block[COM3]);

        OSSemPost(g_usart_tx_dma_sem[COM3]);
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
    }
    OSIntExit();
}


void USART3_IRQHandler(void)
{
    OSIntEnter();
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        if (DMA_GetCmdStatus(DMA1_Stream1) == ENABLE)
        {
            /*
             * Disable the DMA Rx Stream and wait DMA operation complete
             * until DMA1_Stream1_IRQHandler() interrupt
             */
            DMA_Cmd(DMA1_Stream1, DISABLE);
        }

        /* clear the IDLE flag */
        USART_ReceiveData(USART3);
    }
    OSIntExit();
}

#endif /* USE_USART3_DMA_FTR */


#if defined(USE_UART4_DMA_FTR)
/*
 * ----- USART4 -----
 * RX DMA Stream: DMA1_STREAM2
 * TX DMA Stream: DMA1_STREAM4
 */
void DMA1_Stream2_IRQHandler(void)
{
    u16   data_recv;
    INT8U err;
    u8*   p_data;
    u16*  P_data_value;
    u16*  p_uart_index;

    OSIntEnter();
    if (DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2) != RESET)
    {
        data_recv = USART_DMA_RECV_SIZE - DMA_GetCurrDataCounter(DMA1_Stream2);

        // store recv data length
        if (data_recv)
        {
            p_data        = (u8*)g_mem_block[COM4];
            P_data_value  = (u16*)(p_data + USART_DMA_RECV_SIZE);
            *P_data_value = data_recv;
            // save uart number
            p_uart_index  = (u16*)(p_data + USART_DMA_RECV_SIZE + 2);
            *p_uart_index = (u16)COM4;

            // send this memory block to task
            OSQPost(g_usart_rx_dma_event, (void*)g_mem_block[COM4]);
        }
        else
        {
            // free memory
            OSMemPut(g_dma_mem, (void*)g_mem_block[COM4]);
        }

        // get memory
        g_mem_block[COM4] = OSMemGet(g_dma_mem, &err);
        if (g_mem_block[COM4] == 0)
        {
            TRACE("* DMA1-2 GET MEMORY ERROR!\r\n");
            DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
            OSIntExit();
            return;
        }

        // reload next usart dma rx
        reload_usart_dma_rx(COM4, (u8*)g_mem_block[COM4], USART_DMA_RECV_SIZE);

        DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
    }
    OSIntExit();
}


/*
 * DMA1 Stream 4
 * TX
 */
void DMA1_Stream4_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) != RESET)
    {
        // free memory
        if (p_backup_mem_block[COM4])
            OSMemPut(g_dma_mem, (void*)p_backup_mem_block[COM4]);

        OSSemPost(g_usart_tx_dma_sem[COM4]);
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
    }
    OSIntExit();
}


void UART4_IRQHandler(void)
{
    OSIntEnter();
    if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
    {
        if (DMA_GetCmdStatus(DMA1_Stream2) == ENABLE)
        {
            /*
             * Disable the DMA Rx Stream and wait DMA operation complete
             * until DMA1_Stream1_IRQHandler() interrupt
             */
            DMA_Cmd(DMA1_Stream2, DISABLE);
        }

        /* clear the IDLE flag */
        USART_ReceiveData(UART4);
    }
    OSIntExit();
}

#endif /* USE_UART4_DMA_FTR */


#if defined(USE_UART5_DMA_FTR)
/*
 * ----- USART5 -----
 * RX DMA Stream: DMA1_STREAM0
 * TX DMA Stream: DMA1_STREAM7
 */
void DMA1_Stream0_IRQHandler(void)
{
    u16   data_recv;
    INT8U err;
    u8*   p_data;
    u16*  P_data_value;
    u16*  p_uart_index;

    OSIntEnter();
    if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0) != RESET)
    {
        data_recv = USART_DMA_RECV_SIZE - DMA_GetCurrDataCounter(DMA1_Stream0);

        // store recv data length
        if (data_recv)
        {
            p_data        = (u8*)g_mem_block[COM5];
            P_data_value  = (u16*)(p_data + USART_DMA_RECV_SIZE);
            *P_data_value = data_recv;
            // save uart number
            p_uart_index  = (u16*)(p_data + USART_DMA_RECV_SIZE + 2);
            *p_uart_index = (u16)COM5;

            // send this memory block to task
            OSQPost(g_usart_rx_dma_event, (void*)g_mem_block[COM5]);
        }
        else
        {
            // free memory
            OSMemPut(g_dma_mem, (void*)g_mem_block[COM5]);
        }

        // get memory
        g_mem_block[COM5] = OSMemGet(g_dma_mem, &err);
        if (g_mem_block[COM5] == 0)
        {
            TRACE("* DMA1-0 GET MEMORY ERROR!\r\n");
            DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
            OSIntExit();
            return;
        }

        // reload next usart dma rx
        reload_usart_dma_rx(COM5, (u8*)g_mem_block[COM5], USART_DMA_RECV_SIZE);

        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
    }
    OSIntExit();
}


/*
 * DMA1 Stream 7
 * TX
 */
void DMA1_Stream7_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_Stream7, DMA_IT_TCIF7) != RESET)
    {
        // free memory
        if (p_backup_mem_block[COM5])
            OSMemPut(g_dma_mem, (void*)p_backup_mem_block[COM5]);

        OSSemPost(g_usart_tx_dma_sem[COM5]);
        DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
    }
    OSIntExit();
}


void UART5_IRQHandler(void)
{
    OSIntEnter();
    if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)
    {
        if (DMA_GetCmdStatus(DMA1_Stream0) == ENABLE)
        {
            /*
             * Disable the DMA Rx Stream and wait DMA operation complete
             * until DMA1_Stream1_IRQHandler() interrupt
             */
            DMA_Cmd(DMA1_Stream0, DISABLE);
        }

        /* clear the IDLE flag */
        USART_ReceiveData(UART5);
    }
    OSIntExit();
}

#endif /* USE_UART5_DMA_FTR */


#if defined(USE_USART6_DMA_FTR)
/*
 * ----- USART6 -----
 * RX DMA Stream: DMA2_STREAM1
 * TX DMA Stream: DMA2_STREAM6
 */
void DMA2_Stream1_IRQHandler(void)
{
    u16   data_recv;
    INT8U err;
    u8*   p_data;
    u16*  P_data_value;
    u16*  p_uart_index;

    OSIntEnter();
    if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) != RESET)
    {
        data_recv = USART_DMA_RECV_SIZE - DMA_GetCurrDataCounter(DMA2_Stream1);

        // store recv data length
        if (data_recv)
        {
            p_data        = (u8*)g_mem_block[COM6];
            P_data_value  = (u16*)(p_data + USART_DMA_RECV_SIZE);
            *P_data_value = data_recv;
            // save uart number
            p_uart_index  = (u16*)(p_data + USART_DMA_RECV_SIZE + 2);
            *p_uart_index = (u16)COM6;

            // send this memory block to task
            OSQPost(g_usart_rx_dma_event, (void*)g_mem_block[COM6]);
        }
        else
        {
            // free memory
            OSMemPut(g_dma_mem, (void*)g_mem_block[COM6]);
        }

        // get memory
        g_mem_block[COM6] = OSMemGet(g_dma_mem, &err);
        if (g_mem_block[COM6] == 0)
        {
            TRACE("* DMA2-1 GET MEMORY ERROR!\r\n");
            DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
            OSIntExit();
            return;
        }

        // reload next usart dma rx
        reload_usart_dma_rx(COM6, (u8*)g_mem_block[COM6], USART_DMA_RECV_SIZE);

        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
    }
    OSIntExit();
}


/*
 * DMA2 Stream 6
 * TX
 */
void DMA2_Stream6_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6) != RESET)
    {
        // free memory
        if (p_backup_mem_block[COM6])
            OSMemPut(g_dma_mem, (void*)p_backup_mem_block[COM6]);

        OSSemPost(g_usart_tx_dma_sem[COM6]);
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
    }
    OSIntExit();
}


void USART6_IRQHandler(void)
{
    OSIntEnter();
    if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
    {
        if (DMA_GetCmdStatus(DMA2_Stream1) == ENABLE)
        {
            /*
             * Disable the DMA Rx Stream and wait DMA operation complete
             * until DMA1_Stream1_IRQHandler() interrupt
             */
            DMA_Cmd(DMA2_Stream1, DISABLE);
        }

        /* clear the IDLE flag */
        USART_ReceiveData(USART6);
    }
    OSIntExit();
}

#endif /* USE_USART6_DMA_FTR */


#endif /* _APP_USART_DMA_ROUTE_C_ */

