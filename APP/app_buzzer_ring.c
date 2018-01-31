/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_buzzer_ring.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/11/20
 * Description        : This file contains the software implementation for the
 *                      buzzer ring control unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/11/20 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#include "app_buzzer_ring.h"
#include "trace.h"
#include <string.h>
#include "bell.h"



typedef struct
{
    u32 tone:16;
    u32 ring_len:16;
}buzzer_ring_msg;



static OS_EVENT*        g_buzzer_ring_queue;
static OS_STK           buzzer_ring_task_stk[BUZZER_RING_TASK_STK_SIZE];
static void*            g_buzzer_ring_QueueTbl[5];

static void app_buzzer_ring_task(void *p_arg);




void init_buzzer_ring_task(void)
{
    INT8U os_err;

    g_buzzer_ring_queue = (OS_EVENT*)OSQCreate(g_buzzer_ring_QueueTbl, 5);
    assert_param(g_buzzer_ring_queue);

    bell_init();
    os_err = OSTaskCreateExt((void (*)(void *)) app_buzzer_ring_task,
                            (void 		 * ) 0,
                            (OS_STK		 * )&buzzer_ring_task_stk[BUZZER_RING_TASK_STK_SIZE - 1],
                            (INT8U		   ) BUZZER_RING_TASK_PRIO,
                            (INT16U		   ) BUZZER_RING_TASK_PRIO,
                            (OS_STK		 * )&buzzer_ring_task_stk[0],
                            (INT32U		   ) BUZZER_RING_TASK_STK_SIZE,
                            (void 		 * ) 0,
                            (INT16U		   )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(BUZZER_RING_TASK_PRIO, (INT8U *)"buzzer ring", &os_err);
}


/**
  * @brief: BUZZER_RING_WARNING_TONE
  * @note:         ring             stop              ring             stop
  *         | <--- 50ms ---> | <--- 200ms ---> | <--- 50ms ---> | <--- 700ms ----> |
  *         | <----------------------------- 1 second ---------------------------> |
  */
static void buzzer_warn_ring(void)
{
    bell_on();
    OSTimeDlyHMSM(0, 0, 0, 50);
    bell_off();
    OSTimeDlyHMSM(0, 0, 0, 200);
    bell_on();
    OSTimeDlyHMSM(0, 0, 0, 50);
    bell_off();
    OSTimeDlyHMSM(0, 0, 0, 700);
}


/**
  * @brief: BUZZER_RING_ERROR_TONE
  * @note:          ring               stop
  *         | <--- 1000ms ---> | <--- 1000ms ---> |
  *         | <----------- 2 second ------------> |
  */
static void buzzer_error_ring(void)
{
    bell_on();
    OSTimeDlyHMSM(0, 0, 1, 0);
    bell_off();
    OSTimeDlyHMSM(0, 0, 1, 0);
}


/**
  * @brief: BUZZER_RING_TONE
  * @note:         ring
  *         | <--- 100ms ---> |
  */
static void buzzer_tone_ring(void)
{
    bell_on();
    OSTimeDlyHMSM(0, 0, 0, 100);
    bell_off();
}



static void app_buzzer_ring_task(void *p_arg)
{
    INT8U           err;
    void*           p_msg;
    buzzer_ring_msg vl_buzzer_ring_msg;
    u32             ring_len;
    OS_Q_DATA       q_data;
    int             i;

    APP_TRACE("buzzer ring task start...\r\n");

    for (;;)
    {
tone_break_here:
        p_msg = OSQPend(g_buzzer_ring_queue, 10, &err);
        
        if (OS_ERR_NONE == err)
        {
            // do only one warning message
            OSQFlush(g_buzzer_ring_queue);

            // new message
            memcpy(&vl_buzzer_ring_msg, &p_msg, sizeof p_msg);
            ring_len        = vl_buzzer_ring_msg.ring_len;

            switch (vl_buzzer_ring_msg.tone)
            {
                case BUZZER_RING_TONE:
                    buzzer_tone_ring();
                    break;
                case BUZZER_RING_WARNING_TONE:
                    if (ring_len == 0)              // always ring...
                    {
                        for (;;)
                        {
                            err = OSQQuery(g_buzzer_ring_queue, &q_data);
                            if (err == OS_ERR_NONE)
                            {
                                // new message receive, stop tone and break to do new tone
                                if (q_data.OSNMsgs)
                                    goto tone_break_here;
                            }
                            buzzer_warn_ring();
                        }
                    }
                    else                            // ring x seconds...
                    {
                        for (i = 0; i < ring_len; i++)
                        {
                            err = OSQQuery(g_buzzer_ring_queue, &q_data);
                            if (err == OS_ERR_NONE)
                            {
                                // new message receive, stop tone and break to do new tone
                                if (q_data.OSNMsgs)
                                    goto tone_break_here;
                            }
                            buzzer_warn_ring();
                        }
                    }
                    break;
                case BUZZER_RING_ERROR_TONE:
                    if (ring_len == 0)              // always ring...
                    {
                        for (;;)
                        {
                            err = OSQQuery(g_buzzer_ring_queue, &q_data);
                            if (err == OS_ERR_NONE)
                            {
                                // new message receive, stop tone and break to do new tone
                                if (q_data.OSNMsgs)
                                    goto tone_break_here;
                            }
                            buzzer_error_ring();
                        }
                    }
                    else
                    {
                        for (i = 0; i < ring_len; i++)
                        {
                            err = OSQQuery(g_buzzer_ring_queue, &q_data);
                            if (err == OS_ERR_NONE)
                            {
                                // new message receive, stop tone and break to do new tone
                                if (q_data.OSNMsgs)
                                    goto tone_break_here;
                            }
                            buzzer_error_ring();
                        }
                    }
                    break;
                case BUZZER_RING_STOP:
                    bell_off();
                    break;
            }
        }
        else if (OS_ERR_TIMEOUT == err)
        {
        }
        else
        {
            APP_TRACE("buzzer ring Queue pend error!\r\n");
        }
    }
}


/**
  * @brief: set buzzer ring tone
  * @param: tone, warning, error
  * @param: ring_time_length, 0 ----> always
  *                           x ----> ring x(second)
  */
void set_buzzer_ring_tone(buzzer_ring_type tone, u16 ring_time_length)
{
    void* p_msg;
    buzzer_ring_msg  vl_buzzer_ring_msg;

    vl_buzzer_ring_msg.tone = tone;
    vl_buzzer_ring_msg.ring_len = ring_time_length;

    memcpy(&p_msg, &vl_buzzer_ring_msg, sizeof vl_buzzer_ring_msg);
    OSQPostFront(g_buzzer_ring_queue, p_msg);
}








