/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_task_err.c
 * Author             : WQ
 * Date First Issued  : 2013/11/14
 * Description        : This file contains the software implementation for the
 *                      task err
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/14 | v1.0  | WQ         | initial released
 *******************************************************************************/

#include "app_task_err.h"
#include "trace.h"

#ifdef SLAVE_BOARD_1
#include "app_slave_board1.h"
#include "struct_slave_board1.h"
#endif
#ifdef SLAVE_BOARD_2
#include "app_slave_board2.h"
#include "struct_slave_board2.h"
#endif
#ifdef SLAVE_BOARD_3
#include "app_slave_board3.h"
#include "struct_slave_board3.h"
#endif

static OS_STK TPDO_task_stk[TPDO_TASK_STK_SIZE];
static void TPDO_task(void *p_arg);

#define ACK_TIMEOUT     500 //1000 ticks
#define TIMEOUT_REPEAT  3   //

/*
 * @brief: Send task errcode to arm0
 * @param: task_num: must be 1~15
 * @param: level:    TASK_WAR or TASK_ERR
 * @param: err_code: must be 0~255
 * @retval: void
 */
void send_task_errcode(task_number task_num,task_errcode_level level ,uint8_t err_code)
{
    uint8_t err,ret;
    OS_CPU_SR cpu_sr;
    ControlOrder_t l_task_errcode;
    static uint8_t task_errcode_fn;

    assert_param(IS_TASK_NUM(task_num));
    assert_param(IS_ERR_LEVEL(level));
    assert_param(IS_ERR_CODE(err_code));

    OS_ENTER_CRITICAL();
    task_errcode_fn = (task_errcode_fn+1)&0x07;
    l_task_errcode.fn = task_errcode_fn;
    OS_EXIT_CRITICAL();

    l_task_errcode.module_indicate = 0x01;
    if(level == TASK_WAR)
        l_task_errcode.order_indicate = TASK_WAR;//
    else
        l_task_errcode.order_indicate = TASK_ERR;//

    l_task_errcode.number.bit.num_board = 0x01;
    l_task_errcode.number.bit.num_task = task_num;
    l_task_errcode.errcode = err_code;

    //TRACE("Send fn = %d\r\n",l_task_errcode.fn);

     TPDO_mem_ptr = (u8*)OSMemGet(TPDO_mem_order,&err);

     if(OS_ERR_MEM_NO_FREE_BLKS == err)
     {
         TRACE("task errcode Mem is FULL!\r\n ");
     }
     else if(TPDO_mem_ptr != 0)
     {
         memcpy(TPDO_mem_ptr,&l_task_errcode,sizeof(ControlOrder_t));
         ret = OSQPost(TPDO_q_send, TPDO_mem_ptr);
        if(OS_ERR_Q_FULL == ret)
        {
            TRACE("task errcode Queue id FULL!\r\n");
        }
     }

}




void init_TPDO_task()
{
    INT8U os_err;

	/* pump control task ***************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) TPDO_task,
							(void		   * ) 0,
							(OS_STK 	   * )& TPDO_task_stk[TPDO_TASK_STK_SIZE - 1],
							(INT8U			 ) TPDO_TASK_PRIO,
							(INT16U 		 ) TPDO_TASK_PRIO,
							(OS_STK 	   * )& TPDO_task_stk[0],
							(INT32U 		 ) TPDO_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(TPDO_TASK_PRIO, (INT8U *)"TPDO TASK", &os_err);

}

static void TPDO_task(void *p_arg)
{
    uint8_t err,OSflag,i;
    uint8_t flag_ack = 0;
    u8* TPDO_order_ptr;
    OS_Q_DATA tmp;

    APP_TRACE("TPDO_TASK\r\n");

    while(1)
    {

        TPDO_order_ptr = (u8*)OSQPend(TPDO_q_send,0,&err);
       // OSQQuery(TPDO_q_send,&tmp);
        assert_param(TPDO_order_ptr);
       // TICK_TRACE("PEND Queue num = 0x%d\r\n",tmp.OSNMsgs);
		memcpy(&task_errcode,TPDO_order_ptr,sizeof(ControlOrder_t));//send
        OSMemPut(TPDO_mem_order,(void*)TPDO_order_ptr);
        TICK_TRACE("put TPDO_order_ptr = 0x%p write fn = %d\r\n",TPDO_order_ptr,task_errcode.fn);
        for(i = 0;i<TIMEOUT_REPEAT;i++)
        {
            //TICK_TRACE("ENTER PEND \r\n");
            OSflag = OSFlagPend(task_errcode_acknak_flag,0x03,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,ACK_TIMEOUT,&err);
           // TICK_TRACE("OUT   PEND\r\n");

           switch(OSflag)
            {
                case 0x00://timeout or err
                    if(err == OS_ERR_TIMEOUT)
                    {
                        task_errcode.continue_retransmit.bit.retransmit++;//resend
                        task_errcode.continue_retransmit.bit.retransmit &= 0x03;

                        APP_TRACE("OSFlagPend TIMEOUT!\r\n");
                    }
                    else
                    {
                        APP_TRACE("OSFlagPend ERR!\r\n");
                    }
                    break;
                case 0x01://ACK
                    flag_ack = 1;
                    APP_TRACE("Receive ACK FROM ARM0! fn = %d\r\n",control_order_sync_r.fn);
                    break;
                case 0x02://NAK

                    task_errcode.continue_retransmit.bit.retransmit++;
                    task_errcode.continue_retransmit.bit.retransmit &= 0x03;

                    APP_TRACE("Receive NAK from ARM0!\r\n");
                    break;
                default:
                    APP_TRACE("OSFlagPend return err!\r\n");
                    break;

            }
            if(flag_ack == 1)
            {
                flag_ack = 0;
                break;//out for()
            }
            else
            {
                //timeout three times do what???
            }
        }
    }
}






