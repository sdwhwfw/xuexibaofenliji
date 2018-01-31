/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cmd_send.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/31
 * Description        : This file contains the software implementation for the
 *                      uart/TPDO send task of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/31 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_cmd_send.h"
#include "trace.h"
#include "app_control_board.h"
#include "app_comX100.h"
#include "app_control_common_function.h"
#include "app_control_monitor.h"
#include "app_usart_dma_route.h"
#include "app_control_pump.h"
#include "app_control_global_state.h"

#define _DEBUG_COMX         0

/***************************UART SEND TASK*************************************************/
static OS_EVENT *                   g_uart_send_sem_event;
static OS_EVENT *                   g_uart_ipc_mbox_event;
u8                                  g_uart_cmd_data[UART_FRAME_ALL_SIZE];
static OS_STK                       uart_send_task_stk[CONTROL_UART_SEND_TASK_STK_SIZE];
static void                         uart_ipc_send_task(void *p_arg);
/******************************************************************************************/

/***************************TPDO ARM1 TASK*************************************************/
static OS_EVENT *                   g_arm1_send_sem_event;
static OS_EVENT *                   g_arm1_tpdo_mbox_event;
u8                                  g_arm1_cmd_data[ARM_TPDO_MAX_SIZE];
static OS_STK                       tpdo_arm1_task_stk[CONTROL_ARM1_TPDO_TASK_STK_SIZE];
static void                         tpdo_arm1_send_task(void *p_arg);
/******************************************************************************************/

/***************************TPDO ARM2 TASK*************************************************/
static OS_EVENT *                   g_arm2_send_sem_event;
static OS_EVENT *                   g_arm2_tpdo_mbox_event;
u8                                  g_arm2_cmd_data[ARM_TPDO_MAX_SIZE];
static OS_STK                       tpdo_arm2_task_stk[CONTROL_ARM2_TPDO_TASK_STK_SIZE];
static void                         tpdo_arm2_send_task(void *p_arg);
/******************************************************************************************/

/***************************TPDO ARM3 TASK*************************************************/
static OS_EVENT *                   g_arm3_send_sem_event;
static OS_EVENT *                   g_arm3_tpdo_mbox_event;
u8                                  g_arm3_cmd_data[ARM_TPDO_MAX_SIZE];
static OS_STK                       tpdo_arm3_task_stk[CONTROL_ARM3_TPDO_TASK_STK_SIZE];
static void                         tpdo_arm3_send_task(void *p_arg);
/******************************************************************************************/

void  control_init_send_task(void)
{
    INT8U err;

    /*******Create uart ipc send task *********************************/
	err = OSTaskCreateExt((void (*)(void *)) uart_ipc_send_task,
						  (void		   * ) 0,
						  (OS_STK 	   * )&uart_send_task_stk[CONTROL_UART_SEND_TASK_STK_SIZE - 1],
						  (INT8U         ) CONTROL_UART_SEND_TASK_PRIO,
						  (INT16U 		 ) CONTROL_UART_SEND_TASK_PRIO,
						  (OS_STK 	   * )&uart_send_task_stk[0],
						  (INT32U 		 ) CONTROL_UART_SEND_TASK_STK_SIZE,
						  (void		   * ) 0,
						  (INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == err);
	OSTaskNameSet(CONTROL_UART_SEND_TASK_PRIO, (INT8U *)"IPC send", &err);

    /*******Create tpdo arm1 task *********************************/
	err = OSTaskCreateExt((void (*)(void *)) tpdo_arm1_send_task,
						  (void		   * ) 0,
						  (OS_STK 	   * )&tpdo_arm1_task_stk[CONTROL_ARM1_TPDO_TASK_STK_SIZE - 1],
						  (INT8U         ) CONTROL_ARM1_TPDO_TASK_PRIO,
						  (INT16U 		 ) CONTROL_ARM1_TPDO_TASK_PRIO,
						  (OS_STK 	   * )&tpdo_arm1_task_stk[0],
						  (INT32U 		 ) CONTROL_ARM1_TPDO_TASK_STK_SIZE,
						  (void		   * ) 0,
						  (INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == err);
	OSTaskNameSet(CONTROL_ARM1_TPDO_TASK_PRIO, (INT8U *)"ARM1 tpdo", &err);

     /*******Create tpdo arm2 task *********************************/
	err = OSTaskCreateExt((void (*)(void *)) tpdo_arm2_send_task,
						  (void		   * ) 0,
						  (OS_STK 	   * )&tpdo_arm2_task_stk[CONTROL_ARM2_TPDO_TASK_STK_SIZE - 1],
						  (INT8U         ) CONTROL_ARM2_TPDO_TASK_PRIO,
						  (INT16U 		 ) CONTROL_ARM2_TPDO_TASK_PRIO,
						  (OS_STK 	   * )&tpdo_arm2_task_stk[0],
						  (INT32U 		 ) CONTROL_ARM2_TPDO_TASK_STK_SIZE,
						  (void		   * ) 0,
						  (INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == err);
	OSTaskNameSet(CONTROL_ARM2_TPDO_TASK_PRIO, (INT8U *)"ARM2 tpdo", &err);

     /*******Create tpdo arm3 task *********************************/
	err = OSTaskCreateExt((void (*)(void *)) tpdo_arm3_send_task,
						  (void		   * ) 0,
						  (OS_STK 	   * )&tpdo_arm3_task_stk[CONTROL_ARM3_TPDO_TASK_STK_SIZE - 1],
						  (INT8U         ) CONTROL_ARM3_TPDO_TASK_PRIO,
						  (INT16U 		 ) CONTROL_ARM3_TPDO_TASK_PRIO,
						  (OS_STK 	   * )&tpdo_arm3_task_stk[0],
						  (INT32U 		 ) CONTROL_ARM3_TPDO_TASK_STK_SIZE,
						  (void		   * ) 0,
						  (INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == err);
	OSTaskNameSet(CONTROL_ARM3_TPDO_TASK_PRIO, (INT8U *)"ARM3 tpdo", &err);

}

static void uart_ipc_send_task(void *p_arg)
{
    INT8U err;
    u8 i, l_send_ret;
    u8 *p_data;
    OS_FLAGS l_flag;
    INNER_ERROR_MSG l_errMsg;

    g_uart_send_sem_event = OSSemCreate(1);
    assert_param(g_uart_send_sem_event);
    g_uart_acknak_flag = OSFlagCreate(0x00, &err);  /* 0x01:ACK; 0x02:NAK */
    assert_param(g_uart_acknak_flag);
    g_uart_ipc_mbox_event = OSMboxCreate(g_uart_cmd_data);
    assert_param(g_uart_ipc_mbox_event);    

    for (;;)
    {
        p_data = (u8*)OSMboxPend(g_uart_ipc_mbox_event, 0, &err);
        assert_param(OS_ERR_NONE == err);

        if (judge_array_null(p_data, UART_FRAME_ALL_SIZE))
        {// first time pend return

            APP_TRACE("judge_array_null()\r\n");
            // add by wenquan
            err = OSSemPost(g_uart_send_sem_event);
            assert_param(OS_ERR_NONE == err);
            
            continue;
        }
        //DumpData(p_data, UART_FRAME_ALL_SIZE);  // debug
        
        for (i=0; i<NAK_REPEAT_COUNT; i++)
        {
            send_data_packet(COM3, p_data, UART_FRAME_ALL_SIZE);
            
            l_flag = OSFlagPend(g_uart_acknak_flag, 0x03, OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME, ACK_WAIT_TIMEOUT, &err);
            assert_param(OS_ERR_NONE == err || OS_ERR_TIMEOUT == err);


            
            if (FLAG_ACK_BIT == l_flag)
            {// ACK
                l_send_ret = 0;
                break;
            }
            else
            {// NAK or timeout
                l_send_ret = 1;
            }
        }
        if (0 == l_send_ret)
        {// uart send OK
            err = OSSemPost(g_uart_send_sem_event);
            assert_param(OS_ERR_NONE == err);
        }
        else
        {// uart send error
            //APP_TRACE("UART send error!\r\n");
            err = OSSemPost(g_uart_send_sem_event);
            assert_param(OS_ERR_NONE == err);
            // triggle stop program
            l_errMsg.source.all = ERR_SOURCE_UART;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_STOP_PROGRAM;
            l_errMsg.errcode1 = 0x10;
            l_errMsg.errcode2 = 0x00;
            l_errMsg = l_errMsg;
            post_error_msg(l_errMsg);
        }
    }
}

static void tpdo_arm1_send_task(void *p_arg)
{
    INT8U err;
    u8 i, l_send_ret, l_tpdo_offset, l_tpdo_num;
    u8 *p_data;
    OS_FLAGS l_flag;
    INNER_ERROR_MSG l_errMsg;
#if _DEBUG_COMX
    u8 j;
#endif

    g_arm1_send_sem_event = OSSemCreate(1);
    assert_param(g_arm1_send_sem_event);
    g_arm1_acknak_flag = OSFlagCreate(0x00, &err);  /* 0x01:ACK; 0x02:NAK */
    assert_param(g_arm1_acknak_flag);
    g_arm1_tpdo_mbox_event = OSMboxCreate(g_arm1_cmd_data);
    assert_param(g_arm1_tpdo_mbox_event);

    for (;;)
    {
        p_data = (u8*)OSMboxPend(g_arm1_tpdo_mbox_event, 0, &err);
        assert_param(OS_ERR_NONE == err);

        if (judge_array_null(p_data, sizeof(p_data)))
        {
            continue;
        }
        
        l_tpdo_offset = p_data[0];
        l_tpdo_num = p_data[1];

        for (i=0; i<NAK_REPEAT_COUNT; i++)
        {
#if defined(_COMX100_MODULE_FTR_)
#if _DEBUG_COMX
            for (j=0; j<l_tpdo_num; j++)
            {
                comX100_send_packet(p_data+2+j*8, l_tpdo_offset+j, 1);
//                OSTimeDly(10);
            }
#else
            comX100_send_packet(p_data+2, l_tpdo_offset, l_tpdo_num);
#endif
#endif

            if(l_tpdo_offset == 3)//TPDO3 need wait ACK
            {
                l_flag = OSFlagPend(g_arm1_acknak_flag, 0x03, OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME, ACK_WAIT_TIMEOUT, &err);
                assert_param(OS_ERR_NONE == err || OS_ERR_TIMEOUT == err);
                
                if (FLAG_ACK_BIT == l_flag && OS_ERR_TIMEOUT != err)
                {// ACK
                    l_send_ret = 0;
                    APP_TRACE("ACK\r\n");
                    break;
                }
                else
                {// NAK or timeout
                    // indicate the frame is resend
                    arm_resend_cmd(p_data, l_tpdo_num);
                    l_send_ret = 1;
                }
            }
            else//no NEED wait ACK
            {

            }
            
        }
        
        if (0 == l_send_ret)
        {// arm1 send OK
            if (3==l_tpdo_offset && COMM_MODULE_FLAG==p_data[3] && 0x00==p_data[6])
            {// ARM1 comm Init OK!
                g_comm_sync_state.arm1 = 0x01;
            }
            err = OSSemPost(g_arm1_send_sem_event);
            assert_param(OS_ERR_NONE == err);
            APP_TRACE("0 == l_send_ret OSLockNesting = %d\r\n",OSLockNesting);
        }
        else
        {// arm1 send error
            APP_TRACE("ARM1 TPDO send error!\r\n");
            err = OSSemPost(g_arm1_send_sem_event);
            assert_param(OS_ERR_NONE == err);
            APP_TRACE("else  OSLockNesting = %d\r\n",OSLockNesting);

            
            // triggle stop program
            l_errMsg.source.all = ERR_SOURCE_CANOPEN;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_STOP_PROGRAM;
            l_errMsg.errcode1 = 0x12;
            l_errMsg.errcode2 = 0x00;
            l_errMsg = l_errMsg;
            post_error_msg(l_errMsg);
        }
    }
}

static void tpdo_arm2_send_task(void *p_arg)
{
    INT8U err;
    u8 i, l_send_ret, l_tpdo_offset, l_tpdo_num;
    u8 *p_data;
    OS_FLAGS l_flag;
    INNER_ERROR_MSG l_errMsg;
#if _DEBUG_COMX
    u8 j;
#endif

    g_arm2_send_sem_event = OSSemCreate(1);
    assert_param(g_arm2_send_sem_event);
    g_arm2_acknak_flag = OSFlagCreate(0x00, &err);  /* 0x01:ACK; 0x02:NAK */
    assert_param(g_arm2_acknak_flag);
    g_arm2_tpdo_mbox_event = OSMboxCreate(g_arm2_cmd_data);
    assert_param(g_arm2_tpdo_mbox_event);

    for (;;)
    {
        p_data = (u8*)OSMboxPend(g_arm2_tpdo_mbox_event, 0, &err);
        assert_param(OS_ERR_NONE == err);

        if (judge_array_null(p_data, sizeof(p_data)))
        {
            continue;
        }
        
        l_tpdo_offset = p_data[0];
        l_tpdo_num = p_data[1];

        for (i=0; i<NAK_REPEAT_COUNT; i++)
        {
#if defined(_COMX100_MODULE_FTR_)
#if _DEBUG_COMX
            for (j=0; j<l_tpdo_num; j++)
            {
                comX100_send_packet(p_data+2+j*8, l_tpdo_offset+j, 1);
//                OSTimeDly(10);
            }
#else
            comX100_send_packet(p_data+2, l_tpdo_offset, l_tpdo_num);
#endif
#endif

            if(l_tpdo_offset == 7)//offset = 7 TPDO3 need judge ack or nak
            {
                l_flag = OSFlagPend(g_arm2_acknak_flag, 0x03, OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME, ACK_WAIT_TIMEOUT, &err);
                assert_param(OS_ERR_NONE == err || OS_ERR_TIMEOUT == err);
                if (FLAG_ACK_BIT == l_flag)
                {// ACK
                    l_send_ret = 0;
                    break;
                }
                else
                {// NAK or timeout
                    // indicate the frame is resend
                    arm_resend_cmd(p_data, l_tpdo_num);
                    l_send_ret = 1;
                }
            }
            else
            {

            }

        }
        if (0 == l_send_ret)
        {// arm2 send OK
            if (7==l_tpdo_offset && COMM_MODULE_FLAG==p_data[3] && 0x00==p_data[6])
            {// ARM2 comm Init OK!
                g_comm_sync_state.arm2 = 0x01;
            }
            err = OSSemPost(g_arm2_send_sem_event);
            assert_param(OS_ERR_NONE == err);
        }
        else
        {// arm2 send error
            APP_TRACE("ARM2 TPDO send error!\r\n");
            err = OSSemPost(g_arm2_send_sem_event);
            assert_param(OS_ERR_NONE == err);
            // triggle stop program
            l_errMsg.source.all = ERR_SOURCE_CANOPEN;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_STOP_PROGRAM;
            l_errMsg.errcode1 = 0x14;
            l_errMsg.errcode2 = 0x00;
            l_errMsg = l_errMsg;
            post_error_msg(l_errMsg);           
        }
    }
}

static void tpdo_arm3_send_task(void *p_arg)
{
    INT8U err;
    u8 i, l_send_ret, l_tpdo_offset, l_tpdo_num;
    u8 *p_data;
    OS_FLAGS l_flag;
    INNER_ERROR_MSG l_errMsg;

    g_arm3_send_sem_event = OSSemCreate(1);
    assert_param(g_arm3_send_sem_event);
    g_arm3_acknak_flag = OSFlagCreate(0x00, &err);  /* 0x01:ACK; 0x02:NAK */
    assert_param(g_arm3_acknak_flag);
    g_arm3_tpdo_mbox_event = OSMboxCreate(g_arm3_cmd_data);
    assert_param(g_arm3_tpdo_mbox_event);

    for (;;)
    {
        p_data = (u8*)OSMboxPend(g_arm3_tpdo_mbox_event, 0, &err);
        assert_param(OS_ERR_NONE == err);
        
        if (judge_array_null(p_data, sizeof(p_data)))
        {
            continue;
        }
        
        l_tpdo_offset = p_data[0];
        l_tpdo_num = p_data[1];

        for (i=0; i<NAK_REPEAT_COUNT; i++)
        {
#if defined(_COMX100_MODULE_FTR_)
            comX100_send_packet(p_data+2, l_tpdo_offset, l_tpdo_num);
#endif

            if(l_tpdo_offset == 11)//TPDO3 offset = 11 need judge ack or nak
            {
                l_flag = OSFlagPend(g_arm3_acknak_flag, 0x03, OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME, ACK_WAIT_TIMEOUT, &err);
                assert_param(OS_ERR_NONE == err || OS_ERR_TIMEOUT == err);
                
                if (FLAG_ACK_BIT == l_flag)
                {// ACK
                    l_send_ret = 0;
                    break;
                }
                else
                {// NAK or timeout
                    // indicate the frame is resend
                    arm_resend_cmd(p_data, l_tpdo_num);
                    l_send_ret = 1;
                }
            }
            else
            {

            }
        }
        
        if (0 == l_send_ret)
        {// arm3 send OK
            if (11==l_tpdo_offset && COMM_MODULE_FLAG==p_data[3] && 0x00==p_data[6])
            {// ARM3 comm Init OK!
                g_comm_sync_state.arm3 = 0x01;
            }
            err = OSSemPost(g_arm3_send_sem_event);
            assert_param(OS_ERR_NONE == err);
        }
        else
        {// arm3 send error
            APP_TRACE("ARM3 TPDO send error!\r\n");
            err = OSSemPost(g_arm3_send_sem_event);
            assert_param(OS_ERR_NONE == err);
            // triggle stop program
            l_errMsg.source.all = ERR_SOURCE_CANOPEN;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_STOP_PROGRAM;
            l_errMsg.errcode1 = 0x16;
            l_errMsg.errcode2 = 0x00;
            l_errMsg = l_errMsg;
            post_error_msg(l_errMsg);
        }

    }
}


uint8_t uart_send_frame(UART_FRAME text_frame)
{
    u8 l_p_cmd[UART_FRAME_ALL_SIZE];
    u8 l_crcData = {0};
    INT8U err;

    // get uart send sem
    OSSemPend(g_uart_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);
    
    l_p_cmd[0] = STX;
    l_p_cmd[1] = get_current_fn(IPC_DEST, FN_CREATE);
    /* Text evaluate */
    l_p_cmd[2] = text_frame.module_flag;
    l_p_cmd[3] = text_frame.event_type;
    l_p_cmd[4] = text_frame.param1;
    l_p_cmd[5] = text_frame.param2;
    l_p_cmd[6] = text_frame.param3;
    l_p_cmd[7] = text_frame.param4;
    l_p_cmd[8] = text_frame.param5;
    l_p_cmd[9] = text_frame.param6;
    
    l_crcData = uart_crc_generate_func(l_p_cmd, UART_FRAME_ALL_SIZE);
    l_p_cmd[10] = l_crcData;
    l_p_cmd[11] = ETX;
    l_p_cmd[12] = LF;

    memcpy(g_uart_cmd_data, l_p_cmd, UART_FRAME_ALL_SIZE);
    // post uart send frame
    err = OSMboxPost(g_uart_ipc_mbox_event, (void *)g_uart_cmd_data);
    assert_param(OS_ERR_NONE ==err);

    return 0;
}

uint8_t uart_answer_cmd(uint8_t ACKorNAK)
{
    static u8 s_nak_count = 0;
    INT8U err;
    u8 l_answer[1] = {0};
    u8 l_send_ret;
    INNER_ERROR_MSG l_errMsg;

    // get uart send sem
    OSSemPend(g_uart_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    l_answer[0] = ACKorNAK;
    l_send_ret = send_data_packet(COM3, l_answer, 1);
    
    TRACE("uart_answer_cmd();\r\n");
    
    if (ACK == ACKorNAK)
    {
        s_nak_count = 0;
    }
    else
    {
        if (++s_nak_count == NAK_REPEAT_COUNT)
        {// uart recv error
            s_nak_count = 0;
            // triggle stop program    
            APP_TRACE("uart_answer_cmd Error!\r\n");
            l_errMsg.source.all = ERR_SOURCE_UART;
            l_errMsg.taskNo = arm0_get_cur_prio();
            l_errMsg.action = ERROR_STOP_PROGRAM;
            l_errMsg.errcode1 = 0x11;
            l_errMsg.errcode2 = 0x00;
            l_errMsg = l_errMsg;
            post_error_msg(l_errMsg);
        }
    }

    // release uart send sem
    err = OSSemPost(g_uart_send_sem_event);
    assert_param(OS_ERR_NONE == err);
    
    return l_send_ret;
}

uint8_t arm1_send_frame(uint8_t *p_data, uint8_t tpdo_offset, uint8_t tpdo_num)
{
    INT8U err;
    u8 l_fn;
  
    // get arm1 send sem
    
    OSSemPend(g_arm1_send_sem_event, 0, &err);
    
    assert_param(OS_ERR_NONE == err);

    if (1==tpdo_num)
    {
        l_fn = p_data[0];
    }
    else if (3==tpdo_num)
    {
        l_fn = p_data[16];
    }
    set_cmd_fn(SEND_FRAME_FN, ARM1_DEST, l_fn); // record the send fn

    g_arm1_cmd_data[0] = tpdo_offset;
    g_arm1_cmd_data[1] = tpdo_num;
    memcpy(&g_arm1_cmd_data[2], p_data, tpdo_num*8);

    // post arm1 send tpdo
    err = OSMboxPost(g_arm1_tpdo_mbox_event, (void *)g_arm1_cmd_data);
    assert_param(OS_ERR_NONE == err);

    return 0;
}

uint8_t arm1_answer_cmd(uint8_t arm1_fn, uint8_t ACKorNAK)
{
    INT8U err;
    ARM_ANSWER_CMD l_answer_tpdo;
    u8 l_answer_block[8] = {0};

    // get arm1 send sem
    OSSemPend(g_arm1_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    memset(&l_answer_tpdo, 0x00, sizeof(l_answer_tpdo));
    l_answer_tpdo.fn = arm1_fn&0x0F;
    l_answer_tpdo.reserved5 = arm1_fn>>4;
    l_answer_tpdo.answer = ACKorNAK;
    memcpy(l_answer_block, &l_answer_tpdo, sizeof(l_answer_tpdo));
    // send tpdo
    comX100_send_packet(l_answer_block, 4, 1);
    
    // release arm1 send sem
    err = OSSemPost(g_arm1_send_sem_event);
    assert_param(OS_ERR_NONE == err);

    APP_TRACE("arm1_answer_cmd OSLockNesting = %d\r\n",OSLockNesting);
    return 0;
}


uint8_t arm2_send_frame(uint8_t *p_data, uint8_t tpdo_offset, uint8_t tpdo_num)
{
    INT8U err;
    u8 l_fn;
    
    // get arm2 send sem
    OSSemPend(g_arm2_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    if (1==tpdo_num)
    {
       l_fn = p_data[0];
    }
    else if (3==tpdo_num)
    {
       l_fn = p_data[16];
    }
    set_cmd_fn(SEND_FRAME_FN, ARM2_DEST, l_fn); // record the send fn
       

    g_arm2_cmd_data[0] = tpdo_offset;
    g_arm2_cmd_data[1] = tpdo_num;
    memcpy(&g_arm2_cmd_data[2], p_data, tpdo_num*8);

    // post arm2 send tpdo
    err = OSMboxPost(g_arm2_tpdo_mbox_event, (void *)g_arm2_cmd_data);
    assert_param(OS_ERR_NONE == err);

    return 0;
}

uint8_t arm2_answer_cmd(uint8_t arm2_fn, uint8_t ACKorNAK)
{
    INT8U err;
    ARM_ANSWER_CMD l_answer_tpdo;
    u8 l_answer_block[8] = {0};

    // get arm1 send sem
    OSSemPend(g_arm2_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    memset(&l_answer_tpdo, 0x00, sizeof(l_answer_tpdo));
    l_answer_tpdo.fn = arm2_fn&0x0F;
    l_answer_tpdo.answer = ACKorNAK;
    l_answer_tpdo.reserved5 = arm2_fn>>4;
    memcpy(l_answer_block, &l_answer_tpdo, sizeof(l_answer_tpdo));
    // send tpdo
    comX100_send_packet(l_answer_block, 8, 1);
    
    // release arm1 send sem
    err = OSSemPost(g_arm2_send_sem_event);
    assert_param(OS_ERR_NONE == err);
    return 0;
}


uint8_t arm3_send_frame(uint8_t *p_data, uint8_t tpdo_offset, uint8_t tpdo_num)
{
    INT8U err;
    u8 l_fn;

    // get arm3 send sem
    OSSemPend(g_arm3_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    if (1==tpdo_num)
    {
       l_fn = p_data[0];
    }
    else if (3==tpdo_num)
    {
       l_fn = p_data[16];
    }
    set_cmd_fn(SEND_FRAME_FN, ARM3_DEST, l_fn); // record the send fn
        

    g_arm3_cmd_data[0] = tpdo_offset;
    g_arm3_cmd_data[1] = tpdo_num;
    memcpy(&g_arm3_cmd_data[2], p_data, tpdo_num*8);

    // post arm3 send tpdo
    err = OSMboxPost(g_arm3_tpdo_mbox_event, (void *)g_arm3_cmd_data);
    assert_param(OS_ERR_NONE == err);

    return 0;
}

uint8_t arm3_answer_cmd(uint8_t arm3_fn, uint8_t ACKorNAK)
{
    INT8U err;
    ARM_ANSWER_CMD l_answer_tpdo;
    u8 l_answer_block[8] = {0};

    // get arm1 send sem
    OSSemPend(g_arm3_send_sem_event, 0, &err);
    assert_param(OS_ERR_NONE == err);

    memset(&l_answer_tpdo, 0x00, sizeof(l_answer_tpdo));
    l_answer_tpdo.fn = arm3_fn&0x0f;
    l_answer_tpdo.answer = ACKorNAK;
    l_answer_tpdo.reserved5 = arm3_fn>>4;
    memcpy(l_answer_block, &l_answer_tpdo, sizeof(l_answer_tpdo));
    // send tpdo
    comX100_send_packet(l_answer_block, 12, 1);
    
    // release arm1 send sem
    err = OSSemPost(g_arm3_send_sem_event);
    assert_param(OS_ERR_NONE == err);
    return 0;
}

