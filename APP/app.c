/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/07/20
 * Description        : This file contains the software implementation for the
 *                      app unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/07/20 | v1.0  | Bruce.zhu  | initial released
 * 2013/09/23 | v1.1  | Bruce.zhu  | Reorganization all the task
 * 2013/10/16 | v1.2  | Bruce.zhu  | add _DEBUG macro to close watchdog task
 *******************************************************************************/
#ifndef __APP_C__
#define __APP_C__

#include "app.h"

#include "trace.h"
#include "app_shell.h"
#include "app_led.h"
#include "app_watchdog.h"
#include "app_dc_motor.h"
#include "app_dc_cooling_fan.h"

#ifdef USE_BUZZER_FTR
#include "app_buzzer_ring.h"
#endif /* USE_BUZZER_FTR */

#if defined(CONTROL_MAIN_BOARD)
#include "app_control_board.h"
#elif defined(SLAVE_BOARD_1)
#include "app_slave_board1.h"
#elif defined(SLAVE_BOARD_2)
#include "app_slave_board2.h"
#elif defined(SLAVE_BOARD_3)
#include "app_slave_board3.h"
#endif


/* Private variables ---------------------------------------------------------*/
static OS_STK startup_task_stk[STARTUP_TASK_STK_SIZE];


/* Private function prototypes -----------------------------------------------*/
static void app_start_task(void *p_arg);

/**
  * Print firmware version through UART1
  *
  * Note: UART1 must be initialized before call this function
  */
static void show_version_info()
{
    u8 vl_buf[128];

    APP_TRACE("\r\n====================================================\r\n");
    snprintf((char*)vl_buf, sizeof(vl_buf), "* uC/OS-II Version: [%d]", OSVersion());
    APP_TRACE("%-51s*\r\n", vl_buf);
    snprintf((char*)vl_buf, sizeof(vl_buf), "* Firmware Version: [%s]", FIRMWARE_VERSION);
    APP_TRACE("%-51s*\r\n", vl_buf);
    snprintf((char*)vl_buf, sizeof(vl_buf), "* Hardware Version: [%s]", BOARD_VERSION);
    APP_TRACE("%-51s*\r\n", vl_buf);
    snprintf((char*)vl_buf, sizeof(vl_buf), "* Created Date    : %s/%s", __DATE__, __TIME__);
    APP_TRACE("%-51s*\r\n", vl_buf);
    snprintf((char*)vl_buf, sizeof(vl_buf), "*");
    APP_TRACE("%-51s*\r\n", vl_buf);
    snprintf((char*)vl_buf, sizeof(vl_buf), "* (C) COPYRIGHT 2013 VINY");
    APP_TRACE("%-51s*\r\n", vl_buf);
    APP_TRACE("====================================================\r\n\r\n");
}

#if defined(USE_RTC_BKP_FTR)
static void read_rtc_data(void)
{
    u32 rtc_ret;
    u8  sec, minute, hour;
    u32 weekday, date, month, year;
    u32 vbat_V, vbat_mV;

    init_VBAT_measure();
    rtc_ret = rtc_init();
    if (rtc_ret)
    {
        APP_TRACE("Warning: RTC is first used!\r\n");
    }
    get_rtc_time(&sec, &minute, &hour);
    get_rtc_date(&weekday, &date, &month, &year);
    APP_TRACE("RTC time: %d-%d-%d %d:%d:%d\r\n", year, month, date, hour, minute, sec);

    // delay some time to wait VBAT measure
    OSTimeDlyHMSM(0, 0, 0, 100);
    get_vbat_value(&vbat_V, &vbat_mV);
    APP_TRACE("VBAT: %d.%dV\r\n", vbat_V, vbat_mV);
}
#endif /*USE_RTC_BKP_FTR*/



void init_app_start_task(void)
{
    INT8U  os_err = 0;
    os_err = OSTaskCreateExt((void (*)(void *)) app_start_task,
                            (void 		 * ) 0,
                            (OS_STK		 * )&startup_task_stk[STARTUP_TASK_STK_SIZE-1],
                            (INT8U		   ) STARTUP_TASK_PRIO,
                            (INT16U		   ) STARTUP_TASK_PRIO,
                            (OS_STK		 * )&startup_task_stk[0],
                            (INT32U		   ) STARTUP_TASK_STK_SIZE,
                            (void 		 * ) 0,
                            (INT16U		   )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(STARTUP_TASK_PRIO, (INT8U *)"app task", &os_err);
}


/**
  * APP start fucntion, all other app task should do init in this function,
  * this task will sleep after it return.[R14 = OS_TaskReturn]
  */
static void app_start_task(void *p_arg)
{
    bsp_init();

    /* Determine CPU capacity. */
    OSStatInit();

    init_trace();
    show_version_info();

#if defined(USE_RTC_BKP_FTR)
    read_rtc_data();
#endif /* USE_RTC_BKP_FTR */

#if defined(CONTROL_MAIN_BOARD)
    init_control_board_task();
#elif defined(SLAVE_BOARD_1)
    init_slave_board1_task();
#elif defined(SLAVE_BOARD_2)
    init_slave_board2_task();
#elif defined(SLAVE_BOARD_3)
    init_slave_board3_task();
#else
	#error "MUST define one board at least!!!"
#endif /* CONTROL_MAIN_BOARD */

    init_shell_task();
    init_led_task();
    

#ifdef USE_BUZZER_FTR
    init_buzzer_ring_task();
    set_buzzer_ring_tone(BUZZER_RING_TONE, 0);
#endif /* USE_BUZZER_FTR */

#if !defined(_DEBUG)
    init_watchdog_task();
#endif /* _DEBUG */

}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    OS_TCB tcb;
    INT8U  ret;

    TRACE("==> [%d] - [%s]\r\n", line, file);

    if (OSRunning == OS_TRUE)
    {
        ret = OSTaskQuery(OS_PRIO_SELF, &tcb);
        if (OS_ERR_NONE == ret)
        {
            TRACE("=====>> TASK: %s, PRIO: %d <<=====\r\n", tcb.OSTCBTaskName, tcb.OSTCBPrio);
        }
        else
        {
            TRACE("maybe ISR called\r\n");
        }
    }

    /* Infinite loop */
    while (1)
    {
    }
}

#endif /* USE_FULL_ASSERT */


#endif /* __APP_C__ */

