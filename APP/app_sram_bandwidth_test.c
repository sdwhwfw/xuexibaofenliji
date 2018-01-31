/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sram_bandwidth_test.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/16
 * Description        : This file contains the software implementation for the
 *                      sram bandwidth test task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/16 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#include "app_sram_bandwidth_test.h"
#include "trace.h"
#include "sram.h"


#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)

static OS_STK sram_bandwidth_test_task_stk[SRAM_BANDWIDTH_TASK_STK_SIZE];

#define SRAM_BANDWIDTH_SIZE   0x1000

// test SRAM buffer size 4*4KB = 16KB
static u32 sram_test_buffer[SRAM_BANDWIDTH_SIZE];

static void sram_bandwidth_test_task(void *p_arg);


/*
 * @brief: call this function to init callback function
 * @param: p_fun, callback function pointer
 */
void init_sram_bandwidth_test(void)
{
	INT8U os_err;

	os_err = OSTaskCreateExt((void (*)(void *)) sram_bandwidth_test_task,
							(void		   * ) 0,
							(OS_STK 	   * )&sram_bandwidth_test_task_stk[SRAM_BANDWIDTH_TASK_STK_SIZE - 1],
							(INT8U			 ) SRAM_BANDWIDTH_TASK_PRIO,
							(INT16U 		 ) SRAM_BANDWIDTH_TASK_PRIO,
							(OS_STK 	   * )&sram_bandwidth_test_task_stk[0],
							(INT32U 		 ) SRAM_BANDWIDTH_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(SRAM_BANDWIDTH_TASK_PRIO, (INT8U *)"sram bandwidth", &os_err);
}



static void sram_bandwidth_test_task(void *p_arg)
{
    u32 i, j;
    u32 os_tick1, os_tick2, os_tick;
    u32 read_data;
    double speed;

    APP_TRACE("sram_bandwidth_test_task start...\r\n");

    APP_TRACE("internal SRAM test...\r\n");

    /* WRITE TEST */
    APP_TRACE("===== WRITE ---> start address[0x%x], size[0x%x] ====\r\n", sram_test_buffer, SRAM_BANDWIDTH_SIZE * 4);

    os_tick1 = OSTimeGet();
    for (j = 0; j < 10000; j++)
    {
        for (i = 0; i < SRAM_BANDWIDTH_SIZE; i++)
        {
            sram_test_buffer[i] = i;
        }
    }

    os_tick2 = OSTimeGet();
    os_tick  = os_tick2 - os_tick1;
    speed = (SRAM_BANDWIDTH_SIZE * 4.0 * 10000.0 / (os_tick * 1024.0)) * 1000.0;
    
    APP_TRACE("os_tick   = %d\r\n", os_tick);
    APP_TRACE("===== WRITE BANDWIDTH = %dKB/s =====\r\n", (u32)speed);

    /* READ TEST */
    APP_TRACE("===== READ ---> start address[0x%x], size[0x%x] ====\r\n", sram_test_buffer, SRAM_BANDWIDTH_SIZE * 4);
    
    os_tick1 = OSTimeGet();
    for (j = 0; j < 10000; j++)
    {
        for (i = 0; i < SRAM_BANDWIDTH_SIZE; i++)
        {
            read_data = sram_test_buffer[i];
        }
    }

    os_tick2 = OSTimeGet();
    os_tick  = os_tick2 - os_tick1;
    speed = (SRAM_BANDWIDTH_SIZE * 4.0 * 10000.0 / (os_tick * 1024.0)) * 1000.0;
    
    APP_TRACE("os_tick   = %d\r\n", os_tick);
    APP_TRACE("===== READ BANDWIDTH = %dKB/s =====\r\n", (u32)speed);

    APP_TRACE("\r\nexternal SRAM test...\r\n");

    /* WRITE TEST */
    APP_TRACE("===== WRITE ====\r\n");
    read_data = get_fsmc_sram_bandwidth(1);
    APP_TRACE("===== WRITE BANDWIDTH = %dKB/s =====\r\n", read_data);
    /* READ TEST */
    APP_TRACE("===== READ ====\r\n");
    read_data = get_fsmc_sram_bandwidth(0);
    APP_TRACE("===== READ BANDWIDTH = %dKB/s =====\r\n", read_data);
}






#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */


