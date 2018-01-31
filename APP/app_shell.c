/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_shell.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/08/01
 * Description        : This file contains the software implementation for the
 *                      app shell task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/08/01 | v1.0  |            | initial released
 * 2013/08/26 | v1.1  | Bruce.zhu  | add SPI FLASH related shell cmd
 * 2013/08/30 | v1.2  | Bruce.zhu  | add up and down key feature to
 *                                 | load saved cmds
 * 2013/09/13 | v1.3  | Bruce.zhu  | add FATFS test cmd: autotest writetest read
 *                                 | and so on
 * 2013/10/17 | v1.4  | Bruce.zhu  | add sram auto test cmd
 * 2013/11/08 | v1.5  | Bruce.zhu  | add DC motor test cmd
 *******************************************************************************/

#include "app_shell.h"
#include "shell.h"
#include "trace.h"
#include "ucos_ii.h"
#include "spi_flash.h"
#include "app_fs.h"
#include "sram.h"
#include "pwm.h"



#if defined(SLAVE_BOARD_1)
#include "app_monitor1.h"
#endif

#if defined(SLAVE_BOARD_2)
#include "app_monitor2.h"
#endif

#if defined(SLAVE_BOARD_3)
#include "app_monitor3.h"
#include "exti_board3.h"

#endif


#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)
#include "app_sram_bandwidth_test.h"
#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */

#if defined(USE_RTC_BKP_FTR)
#include "rtc.h"
#endif /* USE_RTC_BKP_FTR */

#if defined(USE_UART_COMM_TEST_FTR)
#include "struct_control_board_usart.h"
#include "app_cmd_send.h"
#endif /* USE_UART_COMM_TEST_FTR */

#if defined(USE_TPDO_SEND_TEST_FTR)
#include "app_comX100.h"
#include "app_control_feedback.h"
#endif /* USE_TPDO_SEND_TEST_FTR */


#ifdef USE_DC_MOTOR_SHELL_CMD_FTR
#include "encoder.h"
#include "app_dc_motor.h"
#endif /* USE_DC_MOTOR_SHELL_CMD_FTR */

#ifdef USE_DC_COOLING_FAN_CMD_FTR
#include "app_dc_cooling_fan.h"
#endif /* USE_DC_COOLING_FAN_CMD_FTR */

#ifdef USE_PCA8538_COG_DEBUG_FTR
#include "app_pca8538_cog.h"
#endif /* USE_PCA8538_COG_DEBUG_FTR */

#if defined(USE_CONTROL_PUMP_FTR) || defined(USE_CONTROL_CENTRIFUGE_FTR)
#include "app_control_pump.h"
#include "app_control_board.h"
#endif

#ifdef USE_VALVE_CMD_FTR
#include "app_valve_control.h"
#endif

#ifdef USE_CASSETTE_CMD_FTR
#include "app_cassette.h"
#endif

#ifdef USE_CENT_CMD_FTR
#include "app_centrifuge_control.h"
#endif

#ifdef USE_MAGET_CMD_FTR
#include "app_electromagnet.h"
#endif

//add by Big.Man
#ifdef CTRL_TURN_VALVE_FTR
#include"app_control_pump.h"
#endif



#define  SHELL_HEADER       "app_"

#define CMD_SAVE_BUF_COUNT  5
#define CMD_BUF_SIZE        64

/* Private variable -------------------------------------------------------- */
static OS_STK shell_task_stk[SHELL_TASK_STK_SIZE];

static void* uart_array_msg[CMD_BUF_SIZE];
static OS_EVENT* g_uart1_queue;



static u8 g_cmd_save_buf[CMD_SAVE_BUF_COUNT][CMD_BUF_SIZE];
static u8 g_cmd_save_num;
static u8 g_cmd_pos;

static void shell_task(void *p_arg);
static CPU_INT16S App_TestShellCmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
static CPU_INT16S get_system_version(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
static CPU_INT16S cmd_list(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
static CPU_INT16S get_ucos_status( CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
static CPU_INT16S reset_system(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
static u32 get_uart_data(u8* p_buf, u32 len, u8 cmd_input_end, u8 display_flag);

#if defined(USE_SPI_FLASH_AT45DB_FTR)
static CPU_INT16S spi_flash_Cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_SPI_FLASH_AT45DB_FTR */

#if defined(USE_FATFS_FTR)
static CPU_INT16S fatfs_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_FATFS_FTR */

#if defined(USE_ISSI_SRAM_FTR)
static CPU_INT16S sram_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_ISSI_SRAM_FTR */

#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)
static CPU_INT16S sram_bandwidth_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */

#if defined(USE_RTC_BKP_FTR)
static CPU_INT16S rtc_bkpsram_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_RTC_BKP_FTR */

#if defined(USE_PWM_TEST_FTR)
static CPU_INT16S pwm_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_PWM_TEST_FTR */
#if defined(USE_PARA_TABLE_FTR)
static CPU_INT16S para_table_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_PARA_TABLE_FTR */

#ifdef USE_UART_COMM_TEST_FTR
static CPU_INT16S uart_send_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_UART_COMM_TEST_FTR */

#ifdef USE_TPDO_SEND_TEST_FTR
static CPU_INT16S tpdo_send_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif

#ifdef USE_DC_MOTOR_SHELL_CMD_FTR
static CPU_INT16S dc_motor_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_DC_MOTOR_SHELL_CMD_FTR */

#ifdef USE_DC_COOLING_FAN_CMD_FTR
static CPU_INT16S dc_cooling_fan_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_DC_COOLING_FAN_CMD_FTR */

#ifdef USE_PCA8538_COG_DEBUG_FTR
static CPU_INT16S PCA8538_COG_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif /* USE_PCA8538_COG_DEBUG_FTR */

#ifdef USE_CONTROL_PUMP_FTR
static CPU_INT16S ctrl_pump_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif

#ifdef USE_VALVE_CMD_FTR
static CPU_INT16S valve_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif


#ifdef USE_CASSETTE_CMD_FTR
static CPU_INT16S cassette_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif

#ifdef USE_CENT_CMD_FTR
static CPU_INT16S centrifuge_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif

#ifdef USE_MAGET_CMD_FTR
static CPU_INT16S electromaget_test_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif

#ifdef USE_CONTROL_CENTRIFUGE_FTR
static CPU_INT16S ctrl_centri_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif

//add by Big.Man
#ifdef CTRL_TURN_VALVE_FTR
static CPU_INT16S ctrl_turn_valve_cmd(CPU_INT16U, CPU_CHAR**, SHELL_OUT_FNCT, SHELL_CMD_PARAM*);
#endif



static SHELL_CMD App_ShellAppCmdTbl[] = {
	// you can follow this command to create your own commands...
	{SHELL_HEADER"test"        ,App_TestShellCmd     },
	// show system version information on the console
	{SHELL_HEADER"version"     ,get_system_version   },
	// show all the commands on the console
	{SHELL_HEADER"list"        ,cmd_list             },
	// get ucos task status
	{SHELL_HEADER"top"         ,get_ucos_status      },
	// request a system reset
	{SHELL_HEADER"restart"     ,reset_system         },

#if defined(USE_SPI_FLASH_AT45DB_FTR)
	// SPI FLASH cmd
	{SHELL_HEADER"flash"       ,spi_flash_Cmd        },
#endif /* USE_SPI_FLASH_AT45DB_FTR */

#if defined(USE_FATFS_FTR)
	{SHELL_HEADER"fatfs"       ,fatfs_cmd            },
#endif /* USE_FATFS_FTR */

#if defined(USE_ISSI_SRAM_FTR)
	// sram test cmd
	{SHELL_HEADER"sram"        ,sram_cmd             },
#endif /* USE_ISSI_SRAM_FTR */

#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)
    {SHELL_HEADER"bandwidth"   ,sram_bandwidth_cmd   },
#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */

#if defined(USE_RTC_BKP_FTR)
	// RTC and BKPSRAM
	{SHELL_HEADER"rtc"         ,rtc_bkpsram_cmd      },
#endif /* USE_RTC_BKP_FTR */

#if defined(USE_PWM_TEST_FTR)
    // PWM test cmd, use TIM1 to output PWM signal
    {SHELL_HEADER"pwm"         ,pwm_cmd              },
#endif /* USE_PWM_TEST_FTR */

#if defined(USE_PARA_TABLE_FTR)
    // PARAMETER TABLE
    {SHELL_HEADER"para"        ,para_table_cmd       },
#endif /* USE_PARA_TABLE_FTR */

#ifdef USE_UART_COMM_TEST_FTR
    {SHELL_HEADER"uartsend"    ,uart_send_test_cmd   },
#endif
#ifdef USE_TPDO_SEND_TEST_FTR
    {SHELL_HEADER"tpdo"        ,tpdo_send_test_cmd   },
#endif
#ifdef USE_CONTROL_PUMP_FTR
    {SHELL_HEADER"pump", ctrl_pump_cmd},
#endif
#ifdef USE_CONTROL_CENTRIFUGE_FTR
    {SHELL_HEADER"centrifuge", ctrl_centri_cmd},
#endif
#ifdef USE_DC_MOTOR_SHELL_CMD_FTR
    {SHELL_HEADER"dc"          ,dc_motor_test_cmd    },
#endif /* USE_DC_MOTOR_SHELL_CMD_FTR */

#ifdef USE_DC_COOLING_FAN_CMD_FTR
    {SHELL_HEADER"fan"         ,dc_cooling_fan_cmd   },
#endif /* USE_DC_COOLING_FAN_CMD_FTR */

#ifdef USE_PCA8538_COG_DEBUG_FTR
    {SHELL_HEADER"cog"         ,PCA8538_COG_cmd      },
#endif /* USE_PCA8538_COG_DEBUG_FTR */

#ifdef USE_VALVE_CMD_FTR
    {SHELL_HEADER"valve"    ,valve_test_cmd      },
#endif /* USE_VALVE_CMD_FTR */

#ifdef USE_CASSETTE_CMD_FTR
    {SHELL_HEADER"cassette"    ,cassette_test_cmd      },
#endif /* USE_CASSETTE_CMD_FTR */


#ifdef USE_CENT_CMD_FTR
    {SHELL_HEADER"cent"    ,centrifuge_test_cmd      },
#endif /* USE_CENT_CMD_FTR */

#ifdef USE_MAGET_CMD_FTR
    {SHELL_HEADER"mag"    ,electromaget_test_cmd      },
#endif /* USE_MAGET_CMD_FTR */

#ifdef CTRL_TURN_VALVE_FTR
        {SHELL_HEADER"3valve"    ,ctrl_turn_valve_cmd      },
#endif /* CTRL_TURN_VALVE_FTR */

	// Don't delete this command line!!!
	{0                         ,0                    }
};



/**
  * show version information on the console.
  */
static CPU_INT16S get_system_version( CPU_INT16U i,
											CPU_CHAR  **p_p,
											SHELL_OUT_FNCT p_f,
											SHELL_CMD_PARAM* p)
{
	u8 vl_buf[128];
	u32 version = (u32)__STM32F2XX_STDPERIPH_VERSION;
    u8* dev_board;

#if defined(USE_OPEN_207Z_BOARD)
    dev_board = "OPEN_207Z_BOARD";
#elif defined(USE_OPEN_207V_BOARD)
    dev_board = "OPEN_207V_BOARD";
#elif defined(USE_OPEN_207Z_MINI_BOARD)
    dev_board = "OPEN_207Z_MINI_BOARD";
#elif defined(USE_ARM0_REV_0_1)
    dev_board = "ARM0_REV_0_1";
#elif defined(USE_ARM1_REV_0_1)
    dev_board = "ARM1_REV_0_1";
#elif defined(USE_ARM1_REV_0_2)
    dev_board = "ARM1_REV_0_2";
#elif defined(USE_ARM2_REV_0_1)
    dev_board = "ARM2_REV_0_1";
#elif defined(USE_ARM3_REV_0_1)
    dev_board = "ARM3_REV_0_1";
#else
    dev_board = "UNKNOWN";
#endif

	APP_TRACE("+---------------- system_version ----------------+\r\n");
	snprintf((char*)vl_buf, sizeof(vl_buf), "+ firmware version: %s", FIRMWARE_VERSION);
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+ hardware version: %s", BOARD_VERSION);
	APP_TRACE("%-49s+\r\n", vl_buf);
    snprintf((char*)vl_buf, sizeof(vl_buf), "+ devboard version: %s", dev_board);
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+ shell version   : %d", SHELL_VERSION);
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+ uC/OS-II version: %d", OSVersion());
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+ ST LIB version  : %x", version);
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+ build           : %s/%s" , __DATE__, __TIME__);
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+");
	APP_TRACE("%-49s+\r\n", vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "+          (C) COPYRIGHT 2013 VINY");
	APP_TRACE("%-49s+\r\n", vl_buf);
	APP_TRACE("+------------------------------------------------+\r\n");

	return 0;
}


/**
  * This is an example of shell module
  *
  */
static CPU_INT16S App_TestShellCmd( CPU_INT16U i,
										  CPU_CHAR  **p_p,
										  SHELL_OUT_FNCT p_f,
										  SHELL_CMD_PARAM* p)
{
	APP_TRACE("App_TestShellCmd\r\n");

	return 0;
}



/**
  * (argc, argv, pout_fnct, pcmd_param)
  * escape character information:
  * -----------------------------------------------------
  * http://baike.baidu.com/view/73.htm
  * http://blog.sina.com.cn/s/blog_5a237c2b0100dcz3.html
  * http://wenku.baidu.com/view/e5dfabc74028915f804dc2ef.html
  * -----------------------------------------------------
  * format-string:
  * %[flags][width].[precision] [{h|l|i|232|264}] type
  */
static void show_os_run_information(void)
{
	u16      i, j = 0;
	INT8U    err;
	OS_TCB   task_tcb;

	// Clear Screen CLS, 0x0c, \f
	APP_TRACE("\f");

	//APP_TRACE("\033[1;m%s\033[0;m\r\n", "Hello world");

	APP_TRACE(" %-4s %-16s%5s%6s%10s%11s\r\n", "NUM", "TASK", "PRIO", "USAGE", "CtxSwCtr", "Stack used");
    APP_TRACE("-------------------------------------------------------\r\n");
	for (i = 0; i <= OS_LOWEST_PRIO; i++)
	{
		err = OSTaskQuery(i, &task_tcb);
		if (OS_ERR_NONE == err)
		{
            j++;
			// task    usage	stack size	  stack used
			APP_TRACE(" [%2d] %-16s%5d%6d%10d [%4d,%4d]\r\n",
			            j,
                        task_tcb.OSTCBTaskName,
                        task_tcb.OSTCBPrio,
                        OSCPUUsage,
                        task_tcb.OSTCBCtxSwCtr,
                        task_tcb.OSTCBStkSize,
                        task_tcb.OSTCBStkUsed);
		}
	}

}



static CPU_INT16S get_ucos_status( CPU_INT16U argc,
										CPU_CHAR  **argv,
										SHELL_OUT_FNCT pout_fnct,
										SHELL_CMD_PARAM* pcmd_param)
{
	INT8U  err;
	u8     cmd;
	INT32U tick_time;
	u32    hour, minute, second, millisecond;

	switch (argc)
	{
		case 1:
			show_os_run_information();                          // show uCOS-II run inforamtion only once
			break;
		case 2:
            if (strcmp("-h", argv[1]) == 0)
			{
				APP_TRACE("top usage:\r\n"
						  "                  get uCOS-II run information\r\n"
						  "-a                get uCOS-II run information every second\r\n"
						  "-t                get uCOS-II run time\r\n"
						  "-c                get CPU Usage\r\n"
						  "-h                get help information\r\n");
			}
			else if (strcmp("-a", argv[1]) == 0)                // show uCOS-II information every second except \0x03 receive
			{
				for(;;)
				{
					show_os_run_information();
					cmd = (u8)(u32)OSQPend(g_uart1_queue, 1000, &err);
					if (0x03 == cmd)
					{
						// ctrl+c
						break;
					}
				}
			}
			else if (strcmp("-c", argv[1]) == 0)                // get CPU usage
			{
                APP_TRACE("OSIdleCtrMax = %d, OSCPUUsage = %d, OSIdleCtr = %d\r\n", OSIdleCtrMax, OSCPUUsage, OSIdleCtr);
			}
			else if (strcmp("-t", argv[1]) == 0)
			{
				APP_TRACE("uCOS-II run time: ");
				tick_time = OSTimeGet();
				hour   = (u32)(tick_time/(1000*3600));
				tick_time -= hour*(1000*3600);
				minute = (u32)(tick_time/(1000*60));
				tick_time -= minute*(1000*60);
				second = (u32)(tick_time/1000);
				tick_time -= second*1000;
				millisecond = tick_time;
				APP_TRACE("%.2d:%.2d:%.2d.%.3d\r\n", hour, minute, second, millisecond);
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		default:
			return SHELL_EXEC_ERR;
	}

	return 0;
}


/**
  * request a system reset
  */
static CPU_INT16S reset_system(  CPU_INT16U argc,
									CPU_CHAR  **argv,
									SHELL_OUT_FNCT pout_fnct,
									SHELL_CMD_PARAM* pcmd_param)
{
	APP_TRACE("System reset...\r\n");
	NVIC_SystemReset();

	return 0;
}



#if defined(USE_SPI_FLASH_AT45DB_FTR)
/**
  * flash -e [PAGE]     : erase whole flash chip if no PAGE param
  *       -r  PAGE      : read PAGE content
  *       -w  PAGE      : write test data to flash page[0x00, 0x01, 0x02..., 0xFF]
  *       -i            : read SPI FLASH chip id
  *       ...
  */
static CPU_INT16S spi_flash_Cmd(CPU_INT16U argc,
									CPU_CHAR  **argv,
									SHELL_OUT_FNCT pout_fnct,
									SHELL_CMD_PARAM* pcmd_param)
{
	static u16 flash_page_size = FLASH_PAGE_SIZE;
	u8* p_data_buf;
	u16 i;
	u32 page_add;
	u32 byte_add;
	u32 flash_id;

	switch (argc)
	{
		case 2:
			if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("flash usage:\r\n"
						  "-i                get FLASH id\r\n"
						  "-init             init SPI flash\r\n"
						  "-e [PA]           erase whole chip or spec PAGE\r\n"
						  "-w [PA] [BA byte] write test data to spec PA or assign a byte to BA\r\n"
						  "-r  PA            read PAGE data\r\n"
						  "-h                get help information\r\n");
			}
			else if (strcmp("-e", argv[1]) == 0)                 // chip erase
			{
				APP_TRACE("erase whole chip...");
				chip_erase();
				APP_TRACE("done\r\n");
			}
			else if(strcmp("-i", argv[1]) == 0)                  // read FLASH chip id
			{
				flash_id = read_flash_id();
				APP_TRACE("SPI FLASH ID[0x%.8x]", flash_id);
				switch (flash_id)
				{
					case AT45DB041D:
						APP_TRACE(" -- AT45DB041D [page size 256]\r\n");
						break;
					case AT45DB321D:
						APP_TRACE(" -- AT45DB321D [page size 512]\r\n");
						break;
					default:
						APP_TRACE(" -- UNKNOWN SPI FLASH\r\n");
						break;
				}
			}
			else if(strcmp("-init", argv[1]) == 0)               // init SPI FLASH chip
			{
				APP_TRACE("SPI FLASH init...");
				spi_flash_init();
				APP_TRACE("done\r\n");
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;

		case 3:
			if (strcmp("-r", argv[1]) == 0)                      // read flash page
			{
				page_add = atoi(argv[2]);
				APP_TRACE("read FLASH PAGE[%d]...\r\n", page_add);
				p_data_buf = (u8*)malloc(flash_page_size);
				assert_param(p_data_buf);
				continuous_array_read(page_add, 0, p_data_buf, flash_page_size);
				DumpData(p_data_buf, flash_page_size);
				free(p_data_buf);
			}
			else if (strcmp("-e", argv[1]) == 0)                 // erase select page
			{
				page_add = atoi(argv[2]);
				APP_TRACE("erase FLASH PAGE[%d]...", page_add);
				page_erase(page_add);
				APP_TRACE("done\r\n");
			}
			else if (strcmp("-w", argv[1]) == 0)                 // write test data to FLASH
			{
				page_add = atoi(argv[2]);
				p_data_buf = (u8*)malloc(flash_page_size);
				assert_param(p_data_buf);
				for (i = 0; i < flash_page_size; i++)
				{
					p_data_buf[i] = i;
				}
				APP_TRACE("Write test data to FLASH PAGE[%d]...", page_add);
				flash_buffer_write(1, 0, p_data_buf, flash_page_size);
				program_buf_to_flash(1, page_add);
				free(p_data_buf);
				APP_TRACE("done\r\n");
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		case 5:
			if (strcmp("-w", argv[1]) == 0)                      // write data to specify PAGE and BYTE address
			{
				// flash -w PA BA byte
				// flash -w 2000 4 30
				page_add = atoi(argv[2]);
				byte_add = atoi(argv[3]);
				p_data_buf = (u8*)malloc(flash_page_size);
				assert_param(p_data_buf);
				p_data_buf[0] = (u8)atoi(argv[4]);
				APP_TRACE("Write %.2x to FLASH PAGE[%d] BYTE[%d]...", p_data_buf[0], page_add, byte_add);
				copy_page_to_buffer(1, page_add);
				flash_buffer_write(1, byte_add, p_data_buf, 1);
				program_buf_to_flash(1, page_add);
				free(p_data_buf);
				APP_TRACE("done\r\n");
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		default:
			return SHELL_EXEC_ERR;
	}

	return 0;
}

#endif /* USE_SPI_FLASH_AT45DB_FTR */


#if defined(USE_FATFS_FTR)
/**
  * fatfs -e [PAGE]     : erase whole flash chip if no PAGE param
  *       -r  PAGE      : read PAGE content
  *       -w  PAGE      : write test data to flash page[0x00, 0x01, 0x02..., 0xFF]
  *       -i            : read SPI FLASH chip id
  */
static CPU_INT16S fatfs_cmd( CPU_INT16U argc,
								CPU_CHAR  **argv,
								SHELL_OUT_FNCT pout_fnct,
								SHELL_CMD_PARAM* pcmd_param)
{
	u32 drive_num;
	u32 drive_total;
	u32 drive_free;
	u32 file_len;
	u8  input_buf[128];
	u8  file_name[16];
	u8  ret = 0;
	u32 i = 0, j = 0;
	u8* p_file_buffer;

	switch (argc)
	{
		case 2:
			if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("fatfs usage:\r\n"
						  "-c                Create FATFS\r\n"
						  "-w file_name      Open file for write\r\n"
						  "-writetest        write 512 files to test FATFS\r\n"
						  "-autotest num     test num of times for writetest\r\n"
						  "-r file_name      read file and display the content in the console\r\n"
						  "-i drive num      get total and free drive space\r\n"
						  "-m drive num      mount drive to FATFS\r\n"
						  "-um drive num     unmount FATFS\r\n"
						  "-s path           scan files\r\n"
						  "-h                get help information\r\n");
			}
			else if (strcmp("-c", argv[1]) == 0)                 // Create FATFS
			{
				APP_TRACE("Create FATFS...");
				create_fatfs(0);
				APP_TRACE("done\r\n");
			}
			else if (strcmp("-writetest", argv[1]) == 0)         // write file test
			{
				i = 0;
				while (1)
				{
					APP_TRACE("write file[%d]...", i);
					file_len = snprintf((char*)input_buf, sizeof input_buf, "[%d]Hello, this is Bruce for test file\n", i);
					snprintf((char*)file_name, sizeof file_name, "Bruce%d.txt", i);
					ret = fs_write_file(0, file_name, input_buf, file_len);
					if(ret == 0)
					{
						APP_TRACE("done\r\n");
					}
					else
					{
						APP_TRACE("error[%d]\r\n", ret);
						break;
					}
					i++;
				}
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		case 3:
			if (strcmp("-i", argv[1]) == 0) 				    // get total and free drive space
			{
				drive_num = atoi(argv[2]);
				get_drive_space(drive_num, &drive_total, &drive_free);
				APP_TRACE("Drive Number: %d\r\n", drive_num);
				APP_TRACE("Total       : %dKB\r\n", drive_total);
				APP_TRACE("Available   : %dKB\r\n", drive_free);
				APP_TRACE("Used        : %d%%\r\n", (100 - drive_free*100/drive_total));
			}
			else if (strcmp("-m", argv[1]) == 0)
			{
				APP_TRACE("mount FATFS...");
				drive_num = atoi(argv[2]);
				mount_fatfs(drive_num);
				APP_TRACE("done\r\n");
			}
			else if (strcmp("-um", argv[1]) == 0)
			{
				APP_TRACE("unmount FATFS...");
				drive_num = atoi(argv[2]);
				unmount_fatfs(drive_num);
				APP_TRACE("done\r\n");
			}
			else if (strcmp("-w", argv[1]) == 0)                 // write file test
			{
				APP_TRACE("write file[%s], pleae input data...\r\n", argv[2]);
				file_len = get_uart_data(input_buf, sizeof input_buf, 0x03, 1);
				APP_TRACE("\r\nComplete, write to file[%d]...", file_len);
				ret = fs_write_file(0, (u8*)argv[2], input_buf, file_len);
				if(ret == 0)
				{
					APP_TRACE("done\r\n");
				}
				else
				{
					APP_TRACE("error[%d]\r\n", ret);
				}
			}
			else if (strcmp("-s", argv[1]) == 0)
			{
				APP_TRACE("scan files...\r\n", ret);
				snprintf((char*)input_buf, sizeof input_buf, "%s", argv[2]);
				scan_files((char*)input_buf);
			}
			else if (strcmp("-r", argv[1]) == 0)                 // read file and show content in the console
			{
				ret = get_file_size(0, (u8*)argv[2], &file_len);
				if (ret == 0)
				{
					APP_TRACE("files dump[%d]...\r\n", file_len);
					p_file_buffer = (u8*)malloc(file_len);
					assert_param(p_file_buffer);
					fs_read_file(0, (u8*)argv[2], p_file_buffer, file_len);
					DumpData(p_file_buffer, file_len);
					free((void*)p_file_buffer);
				}
				else
				{
					APP_TRACE("files \"%s\" open error.\r\n", argv[2]);
				}
			}
			else if (strcmp("-autotest", argv[1]) == 0)          // auto write file test
			{
				j = 0;
				while (j < atoi(argv[2]))
				{
					APP_TRACE("========= file write round [%d] =========\r\n", j++);
					i = 0;
					while (1)
					{
						APP_TRACE("write file[%d]...", i);
						file_len = snprintf((char*)input_buf, sizeof input_buf, "[%d]Hello, this is Bruce for test file\n", i);
						snprintf((char*)file_name, sizeof file_name, "Bruce%d.txt", i);
						ret = fs_write_file(0, file_name, input_buf, file_len);
						if(ret == 0)
						{
							APP_TRACE("done\r\n");
						}
						else if (ret == 4)
						{
							APP_TRACE("error[%d]\r\n", ret);
							break;
						}
						else
						{
							APP_TRACE("file system error![%d]\r\n", ret);
							return 0;
						}
						i++;
					}
				}
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		default:
			return SHELL_EXEC_ERR;
	}

	return 0;
}

#endif /* USE_FATFS_FTR */



#if defined(USE_ISSI_SRAM_FTR)

static CPU_INT16S sram_cmd( CPU_INT16U argc,
								CPU_CHAR  **argv,
								SHELL_OUT_FNCT pout_fnct,
								SHELL_CMD_PARAM* pcmd_param)
{
	u8* p_data_buf;
	u32 start_add;
	u32 size;
	u32 ret;
    u8  cmd;
    INT8U err;

	switch (argc)
	{
		case 2:
			if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("sram usage:\r\n"
						  "-t [start][size]  test SRAM, write to start address and read\r\n"
						  "-w [start][size]  get input from usart and write it SRAM\r\n"
						  "-r [start][size]  read SRAM from start address to start + size address\r\n"
						  "-auto             auto test external SRAM\r\n"
						  "-h                get help information\r\n");
			}
            else if (strcmp("-auto", argv[1]) == 0)             // auto test SRAM
            {
                size = 1;
                APP_TRACE("auto test SRAM[press 'ctrl + c' break]...\r\n");
                for (;;)
                {
                    APP_TRACE("route [%4d] start...", size++);
                    for (start_add = 0; start_add < 0x80000; )
                    {
                        ret = fsmc_sram_test(start_add, 0x1000);
                        if (ret)
                        {
                            APP_TRACE("error\r\n");
                            APP_TRACE("address = %d, route = %d\r\n", ret, size);
                            return 0;
                        }

                        cmd = (u8)(u32)OSQPend(g_uart1_queue, 50, &err);
                        if (cmd == 0x03)
                        {
                            return 0;
                        }
                        start_add += 0x1000;
                    }
                    APP_TRACE("done\r\n");
                }
            }
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;

		case 4:
			if (strcmp("-t", argv[1]) == 0)                      // test SRAM automatic
			{
				start_add = atoi(argv[2]);
				size      = atoi(argv[3]);
				if (start_add + size > ISSI_SRAM_SIZE)
				{
					APP_TRACE("start_add + size = %d[0x%x] > SRAM size %d[0x%x]\r\n", start_add + size, start_add + size,
																					  ISSI_SRAM_SIZE, ISSI_SRAM_SIZE);

					return SHELL_EXEC_ERR;
				}
				else
				{
					APP_TRACE("SRAM test [START = %d, size = %d]...\r\n", start_add, size);
					ret = fsmc_sram_test(start_add, size);
					if(ret == 0)
					{
						APP_TRACE("SRAM test OK\r\n");
					}
					else
					{
						APP_TRACE("SRAM test err, address = 0x%x\r\n", ret);
					}
				}
			}
			else if (strcmp("-w", argv[1]) == 0)
			{
				start_add = atoi(argv[2]);
				size      = atoi(argv[3]);

				p_data_buf = (u8*)malloc(size);
				assert_param(p_data_buf);

				APP_TRACE("Please input data[size = %d]...\r\n", size);
				ret = get_uart_data(p_data_buf, size, 0x03, 1);
				APP_TRACE("\r\nBegin to write [address = %d], [size = %d]...", start_add, ret);
				sram_write_byte(p_data_buf, start_add, ret);

				// free memory
				free(p_data_buf);
				APP_TRACE("done\r\n");
			}
			else if (strcmp("-r", argv[1]) == 0)
			{
				start_add = atoi(argv[2]);
				size      = atoi(argv[3]);

				p_data_buf = (u8*)malloc(size);
				assert_param(p_data_buf);

				APP_TRACE("read SRAM [address = %d], [size = %d]...\r\n", start_add, size);
				sram_read_byte(p_data_buf, start_add, size);
				DUMP_DATA(p_data_buf, size);

				// free memory
				free(p_data_buf);
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		default:
			return SHELL_EXEC_ERR;
	}

	return 0;

}
#endif /* USE_ISSI_SRAM_FTR */


#if defined(USE_SRAM_BANDWIDTH_TEST_FTR)
static CPU_INT16S sram_bandwidth_cmd( CPU_INT16U argc,
                                                   CPU_CHAR  **argv,
                                                   SHELL_OUT_FNCT pout_fnct,
                                                   SHELL_CMD_PARAM* pcmd_param)
{
    switch (argc)
    {
        case 2:
            if (strcmp("-h", argv[1]) == 0)                      // help information
            {
                APP_TRACE("bandwidth usage:\r\n"
                          "-i    test internel SRAM bandwidth\r\n"
#if defined(USE_ISSI_SRAM_FTR)
                          "-e    test external SRAM\r\n"
#endif /* USE_ISSI_SRAM_FTR */
                          "-h    get help information\r\n");
            }
            else if (strcmp("-i", argv[1]) == 0)
            {
                init_sram_bandwidth_test();
            }
#if defined(USE_ISSI_SRAM_FTR)
            else if (strcmp("-e", argv[1]) == 0)
            {

            }
#endif /* USE_ISSI_SRAM_FTR */
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        default:
            return SHELL_EXEC_ERR;
    }

    return 0;
}

#endif /* USE_SRAM_BANDWIDTH_TEST_FTR */


#if defined(USE_RTC_BKP_FTR)
static CPU_INT16S rtc_bkpsram_cmd( CPU_INT16U argc,
											 CPU_CHAR  **argv,
											 SHELL_OUT_FNCT pout_fnct,
											 SHELL_CMD_PARAM* pcmd_param)
{
	u32 rtc_ret;
	u32 start_add;
	u32 size;
	u8* p_data_buf;
	u8  sec, minute, hour;
	u32 weekday, date, month, year;
	u32 VBAT_V, VBAT_mV;

	switch (argc)
	{
		case 2:
			if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("rtc usage:\r\n"
						  "-init             init RTC and BKPSRAM\r\n"
						  "-r [start][size]  read BKP SRAM\r\n"
						  "-w [start][size]  write BKP SRAM\r\n"
						  "-read             read RTC date and time\r\n"
						  "-write ...        write RTC writertc [2013 10 10 12 00 00 05]\r\n"
						  "-v                read VBAT value\r\n"
						  "-h                get help information\r\n");
			}
			else if (strcmp("-init", argv[1]) == 0)              // init RTC
			{
				rtc_ret = rtc_init();
				if (rtc_ret)
				{
					APP_TRACE("rtc is first used\r\n");
				}
				else
				{
					APP_TRACE("rtc init OK!\r\n");
				}
			}
			else if (strcmp("-read", argv[1]) == 0)
			{
				get_rtc_time(&sec, &minute, &hour);
				get_rtc_date(&weekday, &date, &month, &year);
				APP_TRACE("RTC time: %d-%d-%d %d:%d:%d\r\n", year, month, date, hour, minute, sec);
			}
			else if (strcmp("-v", argv[1]) == 0)
			{
				get_vbat_value(&VBAT_V, &VBAT_mV);
				APP_TRACE("VBAT: %d.%dV\r\n", VBAT_V, VBAT_mV);
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		case 4:
			if (strcmp("-r", argv[1]) == 0)                      // read backup sram
			{
				start_add = atoi(argv[2]);
				size      = atoi(argv[3]);
				p_data_buf = (u8*)malloc(size);
				assert_param(p_data_buf);

				read_byte_from_bkpsram(start_add, (void*)p_data_buf, size);
				DUMP_DATA(p_data_buf, size);
				// free memory
				free(p_data_buf);
			}
			else if (strcmp("-w", argv[1]) == 0)                 // write backup sram
			{
				start_add = atoi(argv[2]);
				size      = atoi(argv[3]);
				p_data_buf = (u8*)malloc(size);
				assert_param(p_data_buf);

				APP_TRACE("Please input data[size = %d]...\r\n", size);
				size = get_uart_data(p_data_buf, size, 0x03, 1);
				APP_TRACE("\r\nBegin to write [address = %d], [size = %d]...", start_add, size);
				write_byte_to_bkpsram(start_add, (void*)p_data_buf, size);
				// free memory
				free(p_data_buf);
				APP_TRACE("done\r\n");
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		case 9:
			if (strcmp("-write", argv[1]) == 0)                  // set RTC time and date
			{
				year    = atoi(argv[2]);
				month   = atoi(argv[3]);
				date    = atoi(argv[4]);
				hour    = (u8)atoi(argv[5]);
				minute  = (u8)atoi(argv[6]);
				sec     = (u8)atoi(argv[7]);
				weekday = atoi(argv[8]);

				set_rtc_time(sec, minute, hour);
				set_rtc_date(weekday, date, month, year);
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;
		default:
			return SHELL_EXEC_ERR;
	}

	return 0;
}
#endif /* USE_RTC_BKP_FTR */



#if defined(USE_PWM_TEST_FTR)

static CPU_INT16S pwm_cmd( CPU_INT16U argc,
                                  CPU_CHAR  **argv,
                                  SHELL_OUT_FNCT pout_fnct,
                                  SHELL_CMD_PARAM* pcmd_param)
{
    u32 freq;
    u32 duty, duty1, duty2;
    u8  index;

	switch (argc)
	{
		case 2:
			if (strcmp("-h", argv[1]) == 0)                      // help information
            {
				APP_TRACE("pwm usage:\r\n"
                          "-i [index][freq][duty...] init pwm value\r\n"
						  "-init [index]             init pwm to 1KHz 50 60 70 80\r\n"
						  "-h                        get help information\r\n"
						  );
			}
            else if (strcmp("-init", argv[1]) == 0)
            {
                APP_TRACE("Set TIM1 PWM to 50, 60, 70, 80\r\n");
                // default PWM feq 1KHz
                TIM1_8_pwm_config(PWM_TIM1,
                                  20000,
                                  PWM_CHANNEL_1 | PWM_CHANNEL_2 | PWM_CHANNEL_3 | PWM_CHANNEL_4,
                                  50,
                                  60,
                                  70,
                                  80);
                OSTimeDlyHMSM(0, 0, 10, 0);
                APP_TRACE("Set TIM1 PWM to 50, 60, 90, 35\r\n");
                pwm_change(PWM_TIM1, PWM_CHANNEL_1, 1000, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_2, 1000, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_3, 1000, 1);
                pwm_change(PWM_TIM1, PWM_CHANNEL_4, 1000, 1);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        case 3:
            if (strcmp("-init", argv[1]) == 0)
            {
                index = atoi(argv[2]);
                if (index == 1)
                {
                    TIM1_8_pwm_config(PWM_TIM1,
                                        1000,
                                        PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                        50,
                                        60,
                                        0,
                                        0);
                }
                else if (index <= 5)
                {
                    TIM2_5_9_14_pwm_config((pwm_timer_num)index,
                                            1000,
                                            PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                            50,
                                            60,
                                            0,
                                            0);
                }
                else if (index == 8)
                {
                    TIM1_8_pwm_config(PWM_TIM8,
                                        1000,
                                        PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                        50,
                                        60,
                                        0,
                                        0);
                }
                else if (index > 8 && index <=14)
                {
                    index -= 2;
                    TIM2_5_9_14_pwm_config((pwm_timer_num)index,
                                            1000,
                                            PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                            50,
                                            60,
                                            0,
                                            0);
                }
                else
                {
                    return SHELL_EXEC_ERR;
                }
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        case 4:
            if (strcmp("-i", argv[1]) == 0)
            {
                freq = atoi(argv[2]);
                duty = atoi(argv[3]);
                assert_param(duty <= 100);
                TIM1_8_pwm_config(PWM_TIM1,
                                  freq,
                                  PWM_CHANNEL_1 | PWM_CHANNEL_2 | PWM_CHANNEL_3 | PWM_CHANNEL_4,
                                  duty,
                                  60,
                                  70,
                                  80);
            }
            break;
        case 6:
            if (strcmp("-init", argv[1]) == 0)
            {
                index = atoi(argv[2]);
                freq  = atoi(argv[3]);
                duty1 = atoi(argv[4]);
                duty2 = atoi(argv[5]);

                if (duty1 <= 100 && duty2 <= 100)
                {
                    switch (index)
                    {
                        case 1:
                            TIM1_8_pwm_config(PWM_TIM1, freq, PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                           duty1, duty2, 0, 0);
                            APP_TRACE("TIM1 PWM channel 1 and 2 set to[%3d][%3d]\r\n", duty1, duty2);
                            break;
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                            TIM2_5_9_14_pwm_config((pwm_timer_num)(index-1), freq, PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                                duty1, duty2, 0, 0);
                            APP_TRACE("TIM%d PWM channel 1 and 2 set to[%3d][%3d]\r\n", index, duty1, duty2);
                            break;
                        case 8:
                            TIM1_8_pwm_config(PWM_TIM8, freq, PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                                duty1, duty2, 0, 0);
                            APP_TRACE("TIM%d PWM channel 1 and 2 set to[%3d][%3d]\r\n", index, duty1, duty2);
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                            TIM2_5_9_14_pwm_config((pwm_timer_num)(index-3), freq, PWM_CHANNEL_1 | PWM_CHANNEL_2,
                                                duty1, duty2, 0, 0);
                            APP_TRACE("TIM%d PWM channel 1 and 2 set to[%3d][%3d]\r\n", index, duty1, duty2);
                            break;
                        default:
                            APP_TRACE("TIM1-TIM5 and TIM8-TIM14 have PWM output\r\n");
                            break;
                    }
                }
                else
                {
                    APP_TRACE("pwm duty have to set to 0%-100%\r\n");
                    return SHELL_EXEC_ERR;
                }
            }
            break;
        default:
            return SHELL_EXEC_ERR;
	}

    return 0;
}

#endif /* USE_PWM_TEST_FTR */



#if defined(USE_PARA_TABLE_FTR)

static CPU_INT16S para_table_cmd(CPU_INT16U argc,
                                        CPU_CHAR** argv,
                                        SHELL_OUT_FNCT pout_fnct,
                                        SHELL_CMD_PARAM* pcmd_param)
{
	switch (argc)
	{
		case 2:
			if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("para usage:\r\n"
						  "-p                show parameter table\r\n"
                          "-s                show sensor table\r\n"
                          "-e                show err table\r\n"
						  "-h                get help information\r\n");
			}
			else if (strcmp("-p", argv[1]) == 0)              // show parameter table
			{
                #ifdef  SLAVE_BOARD_1
                        slave1_show_parameter_table();
                #endif
                #ifdef  SLAVE_BOARD_2
                        slave2_show_parameter_table();
                #endif
                #ifdef  SLAVE_BOARD_3
                        slave3_show_parameter_table();
                #endif

			}
            else if (strcmp("-s", argv[1]) == 0)              // show sensor table
			{
                #ifdef SLAVE_BOARD_1
                        slave1_show_sensor_table();
                #endif
                #ifdef SLAVE_BOARD_2
                        slave2_show_sensor_table();
                #endif
                #ifdef SLAVE_BOARD_3
                        slave3_show_sensor_table();
                #endif
			}
            else if (strcmp("-e", argv[1]) == 0)              // show err table
			{
                #ifdef SLAVE_BOARD_1
                        slave1_show_error_table();
                #endif
                #ifdef SLAVE_BOARD_2
                        slave2_show_error_table();
                #endif
                #ifdef SLAVE_BOARD_3
                        slave3_show_error_table();
                #endif
			}
			else
			{
				return SHELL_EXEC_ERR;
			}
			break;

		default:
			return SHELL_EXEC_ERR;
	}

    return 0;
}

#endif/*USE_PARA_TABLE_FTR*/

/****************for valve test **************************************/
#if defined(USE_VALVE_CMD_FTR)
static CPU_INT16S valve_test_cmd(CPU_INT16U argc,
                                       CPU_CHAR** argv,
                                       SHELL_OUT_FNCT pout_fnct,
                                       SHELL_CMD_PARAM* pcmd_param)
{
    u8 valve_num,valve_dir;

    switch (argc)
	{
        case 2:
            if(strcmp("-h", argv[1]) == 0)
            {
                APP_TRACE("valve test usage:\r\n"
                          "-start [index][dir]  start valve\r\n"
                          "-stop  [index]       stop valve\r\n"
                          "-h                   get help information\r\n");
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        case 3:
            valve_num = atoi(argv[2]);
            if(strcmp("-stop", argv[1]) == 0)
            {
                stop_valve((ValveType)valve_num);
                APP_TRACE("stop valve[%d]\r\n",valve_num);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        case 4:
            valve_num = atoi(argv[2]);
            valve_dir = atoi(argv[3]);

            if(strcmp("-start", argv[1]) == 0)
            {
                start_valve((ValveType)valve_num,(ValveDirection)valve_dir);
                if(valve_dir == 0x00)
                    APP_TRACE("start valve[%d] clockwise\r\n",valve_num);
                else
                    APP_TRACE("start valve[%d] anticlockwise\r\n",valve_num);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;

        default:
            break;

    }

    return 0;

}

#endif

/****************for cassette test **************************************/
#if defined(USE_CASSETTE_CMD_FTR)
static CPU_INT16S cassette_test_cmd(CPU_INT16U argc,
                                       CPU_CHAR** argv,
                                       SHELL_OUT_FNCT pout_fnct,
                                       SHELL_CMD_PARAM* pcmd_param)
{
    u8 cassette_dir;

    switch (argc)
	{
        case 2:
            if(strcmp("-h", argv[1]) == 0)
            {
                APP_TRACE("cassette test usage:\r\n"
                          "-start [dir]  start cassette\r\n"
                          "-stop         stop cassette\r\n"
                          "-h                   get help information\r\n");


                APP_TRACE(" arm3_sensor_status.sensor.bit.cassette_location  = %d\r\n", arm3_sensor_status.sensor.bit.cassette_location);
                APP_TRACE(" sensor_state_handle.sensor.bit.cassette_location = %d\r\n", sensor_state_handle.sensor.bit.cassette_location);
                APP_TRACE("  sensor_state.sensor.bit.cassette_location       = %d\r\n",  sensor_state.sensor.bit.cassette_location);

            }

            else if(strcmp("-stop", argv[1]) == 0)
            {
                stop_cassette();

                APP_TRACE("stop cassette\r\n");
            }

            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        case 3:
            cassette_dir = atoi(argv[2]);
            if(strcmp("-start", argv[1]) == 0)
            {
                start_cassette((CassetteDirection)cassette_dir);

                if(upward == (CassetteDirection)cassette_dir)
                    APP_TRACE("start cassette up\r\n");
                else
                    APP_TRACE("start cassette down\r\n");
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        default:
            break;

    }

    return 0;

}

#endif
/*USE_CASSETTE_CMD_FTR*/


/****************for centrifuge test jiulong add**************************************/
#if defined(USE_CENT_CMD_FTR)
static CPU_INT16S centrifuge_test_cmd(CPU_INT16U argc,
                                       CPU_CHAR** argv,
                                       SHELL_OUT_FNCT pout_fnct,
                                       SHELL_CMD_PARAM* pcmd_param)
{
    u16 velocity;

    switch (argc)
	{
        case 2:
            if(strcmp("-h", argv[1]) == 0)
            {
                APP_TRACE("centrifuge test usage:\r\n"
                          "-init          init centrifuge\r\n"
                          "-start [speed] start centrifuge\r\n"
                          "-stop          stop centrifuge\r\n"
                          "-h              get help information\r\n");
            }
            else if (strcmp("-stop", argv[1]) == 0)
            {
                stop_centrifuge_motor();
            }
            else if (strcmp("-init", argv[1]) == 0)
            {
                init_centrifuge_motor();
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        case 3:
            velocity = atoi(argv[2]);
            if(strcmp("-start", argv[1]) == 0)
            {
                start_centrifuge_motor(velocity);
                APP_TRACE("centrifuge velocity [%d]\r\n",velocity);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        default:
            break;

    }

    return 0;

}

#endif

/****************for electromaget test jiulong add**************************************/

#if defined(USE_MAGET_CMD_FTR)
static CPU_INT16S electromaget_test_cmd(CPU_INT16U argc,
                                       CPU_CHAR** argv,
                                       SHELL_OUT_FNCT pout_fnct,
                                       SHELL_CMD_PARAM* pcmd_param)
{
   switch (argc)
	{
        case 2:
            if(strcmp("-h", argv[1]) == 0)
            {
                APP_TRACE("electromaget test usage:\r\n"
                          "-lock           lock electromaget\r\n"
                          "-unlock         unlock electromaget\r\n"
                          "-h              get help information\r\n");
            }
            else if (strcmp("-unlock", argv[1]) == 0)
            {
                unlock_magnet();
            }
            else if (strcmp("-lock", argv[1]) == 0)
            {
                lock_magnet();
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        default:
            break;

    }

    return 0;

}

#endif

/********************************************************************************/
/******************************ZhangQ Test Shell Area****************************/
#ifdef USE_UART_COMM_TEST_FTR
static CPU_INT16S uart_send_test_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{
    UART_FRAME l_frame;
    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("uartsend usage:\r\n"
						  "-f                Enter 8 cmd send to PC\r\n"
                          "-a                Enter ACK or NAK send to PC\r\n"
						  "-h                get help information\r\n");
			}
            break;
        }
        case 3:
        {
            if (strcmp("-a", argv[1]) == 0)
            {
                if (strcmp("ACK", argv[2]) == 0)
                {
                    uart_answer_cmd(ACK);
                }
                else if (strcmp("NAK", argv[2]) == 0)
                {
                    uart_answer_cmd(NAK);
                }
            }
            break;
        }
        case 10:
        {
            if (strcmp("-f", argv[1]) == 0)
            {
                l_frame.module_flag = (u8)strtol(argv[2], NULL, 16);
                l_frame.event_type = (u8)strtol(argv[3], NULL, 16);
                l_frame.param1 = (u8)strtol(argv[4], NULL, 16);
                l_frame.param2 = (u8)strtol(argv[5], NULL, 16);
                l_frame.param3 = (u8)strtol(argv[6], NULL, 16);
                l_frame.param4 = (u8)strtol(argv[7], NULL, 16);
                l_frame.param5 = (u8)strtol(argv[8], NULL, 16);
                l_frame.param6 = (u8)strtol(argv[9], NULL, 16);
                uart_send_frame(l_frame);
            }
        }
        default:
            break;

    }
    return 0;
}
#endif

#ifdef USE_TPDO_SEND_TEST_FTR
static CPU_INT16S tpdo_send_test_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{
    u8 l_offset, i;
    u8 l_tpdo[8] = {0};

    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("tpdo send usage:\r\n"
						  "-t [offset] [tpdo]   Enter TPDO(8bytes) send to Slave\r\n"
						  "-h                   get help information\r\n");


                APP_TRACE("g_arm3_rpdo3_data.sensor.bit.cassette_up_location = %d\r\n",g_arm3_rpdo3_data.sensor.bit.cassette_up_location);
                APP_TRACE("g_arm3_rpdo3_data.sensor.bit.cassette_down_location = %d\r\n",g_arm3_rpdo3_data.sensor.bit.cassette_down_location);
			}
            else if(strcmp("-unload", argv[1]) == 0)
            {
                unload_canal();
                APP_TRACE("unload canal\r\n");
            }
            break;
        }
        case 11:
            if (strcmp("-t", argv[1]) == 0)
            {
                l_offset = (u8)atoi(argv[2]);

                for (i=0; i<8; i++)
                {
                    l_tpdo[i] = (u8)strtol(argv[i+3], NULL, 16);
                }
                if (comX100_send_packet(l_tpdo, l_offset, 1))
                {
                    APP_TRACE("TPDO Send error!\r\n");
                }
            }
            break;
        default:
            break;
    }
    return 0;
}
#endif

#ifdef USE_CONTROL_PUMP_FTR
static CPU_INT16S ctrl_pump_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{
    u8                  l_cmdType = 0; /*0:clear, 1:get 2:stop, 3: speed*/
    u8                  l_arg_pump;
    u8                  l_arg_dir;
    u16                 l_arg_speed;
    u16                 l_arg_distance;

    u8                  i;
    u8                  l_arg_feedback_dir;
    u16                 l_pArg_speed[5];
    u16                 l_pArg_distance[5];
    CTRL_PUMP_TYPE      l_pump_type;
    CTRL_PUMP_DISTANCE  l_distance;
    CTRL_PUMP_SPEED     l_speed;
    CTRL_PUMP_COUNT     l_count;

    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                     // help information
			{
				APP_TRACE("pump send usage:\r\n"
						  "-i [No][dir][speed][distance] Enter pump control param.\r\n"
						  "-speed [No]                   Get the speed of pump.\r\n"
						  "-s [No]                       Stop pump.\r\n"
						  "-d [No]                       Get the pump's total distance.\r\n"
						  "-c [No]                       clear the pump's total distance.\r\n"
						  "-a [No][feedbackDir][DrawSp][ACSp][FeedbackSp][PlaSp][PltSp]\r\n"
						  "   [DrawDis][ACDis][FeedbackDis][PlaDis][PltDis]\r\n"
						  "                              Enter all param of the pumps\r\n"
						  "-h                            get help information\r\n");
			}
            break;

        }
        case 6:
        {
            if (strcmp("-i", argv[1]) == 0)
            {
                init_pump_param(&l_pump_type, &l_speed, &l_distance, &l_count);
                l_arg_speed = (u16)atoi(argv[4]);
                l_arg_distance = (u16)atoi(argv[5]);
                l_arg_dir = (u8)atoi(argv[3]);
                switch ((u8)atoi(argv[2]))
                {
                    case 1:
                    {// draw pump
                        l_pump_type.draw_pump = 0x01;
                        l_speed.draw_speed = l_arg_speed;
                        l_distance.draw_distance = l_arg_distance;
                        break;
                    }
                    case 2:
                    {// AC pump
                        l_pump_type.ac_pump = 0x01;
                        l_speed.ac_speed = l_arg_speed;
                        l_distance.ac_distance = l_arg_distance;
                        break;
                    }
                    case 3:
                    {// feedback pump
                        l_pump_type.feedBack_pump = 0x01;
                        l_pump_type.feedBack_dir = l_arg_dir;
                        l_speed.feedBack_speed = l_arg_speed;
                        l_distance.feedBack_distance = l_arg_distance;
                        break;
                    }
                    case 4:
                    {// pla pump
                        l_pump_type.pla_pump = 0x01;
                        l_speed.pla_speed = l_arg_speed;
                        l_distance.pla_distance = l_arg_distance;
                        break;
                    }
                    case 5:
                    {// plt pump
                        l_pump_type.plt_pump = 0x01;
                        l_speed.plt_speed = l_arg_speed;
                        l_distance.plt_distance = l_arg_distance;
                        break;
                    }
                    default:
                        break;
                }

                if (ctrl_start_pump(l_pump_type, l_speed, l_distance, l_count))
                {
                    APP_TRACE("start pump error!\r\n");
                }

                if (l_distance.ac_distance || l_distance.draw_distance || l_distance.feedBack_distance
                    || l_distance.pla_distance || l_distance.plt_distance)
                {
                    if (wait_pump_end(l_pump_type, l_speed, l_distance))
                    {
                        APP_TRACE("wait pump end error!\r\n");
                    }
                    if (ctrl_stop_pump(l_pump_type, l_count))
                    {
                        APP_TRACE("stop pump error!\r\n");
                    }
                }
            }
            break;
        }
        case 3:
        {
            l_arg_pump = (u8)strtol(argv[2], NULL ,16);
            if (strcmp("-d", argv[1]) == 0
                || strcmp("-c", argv[1]) == 0
                || strcmp("-s", argv[1]) == 0
                || strcmp("-speed", argv[1]) == 0)
            {
                if (strcmp("-c", argv[1]) == 0)
                {
                    l_cmdType = 0;
                }
                else if (strcmp("-d", argv[1]) == 0)
                {
                    l_cmdType = 1;
                }
                else if (strcmp("-s", argv[1]) == 0)
                {
                    l_cmdType = 2;
                }
                else
                {
                    l_cmdType = 3;
                }

                init_pump_param(&l_pump_type, &l_speed, &l_distance, &l_count);
                if (l_arg_pump & 0x01)
                {
                    l_pump_type.draw_pump = 0x01;
                }
                if (l_arg_pump & 0x02)
                {
                    l_pump_type.ac_pump = 0x01;
                }
                if (l_arg_pump & 0x04)
                {
                    l_pump_type.feedBack_pump = 0x01;
                }
                if (l_arg_pump & 0x08)
                {
                    l_pump_type.pla_pump = 0x01;
                }
                if (l_arg_pump & 0x10)
                {
                    l_pump_type.plt_pump = 0x01;
                }

                if (0 == l_cmdType)
                {
                    clear_pump_total_distance(l_pump_type);
                    APP_TRACE("clear pump total distance.\r\n");
                }
                else if (1==l_cmdType)
                {
                    l_distance = get_pump_total_distance(l_pump_type);
                    if (l_distance.draw_distance)
                    {
                        APP_TRACE("get draw pump total distance=[%d].\r\n", l_distance.draw_distance);
                    }
                    if (l_distance.ac_distance)
                    {
                        APP_TRACE("get ac pump total distance=[%d].\r\n", l_distance.ac_distance);
                    }
                    if (l_distance.feedBack_distance)
                    {
                        APP_TRACE("get feedBack pump total distance=[%d].\r\n", l_distance.feedBack_distance);
                    }
                    if (l_distance.pla_distance)
                    {
                        APP_TRACE("get pla pump total distance=[%d].\r\n", l_distance.pla_distance);
                    }
                    if (l_distance.plt_distance)
                    {
                        APP_TRACE("get plt pump total distance=[%d].\r\n", l_distance.plt_distance);
                    }
                }
                else if (2==l_cmdType)
                {
                    if (ctrl_stop_pump(l_pump_type, l_count))
                    {
                        APP_TRACE("stop pump error!\r\n");
                    }
                }
                else if (3==l_cmdType)
                {
                    if (l_pump_type.draw_pump)
                    {
                        APP_TRACE("draw pump speed=[%d].\r\n", g_arm1_rpdo1_data.draw_pump_current_speed);
                    }
                    if (l_pump_type.ac_pump)
                    {
                        APP_TRACE("AC pump speed=[%d].\r\n", g_arm1_rpdo1_data.AC_pump_current_speed);
                    }
                    if (l_pump_type.feedBack_pump)
                    {
                        APP_TRACE("feedback pump speed=[%d].\r\n", g_arm1_rpdo1_data.feed_back_pump_current_speed);
                    }
                    if (l_pump_type.pla_pump)
                    {
                        APP_TRACE("pla pump speed=[%d].\r\n", g_arm2_rpdo1_data.plasma_pump_current_speed);
                    }
                    if (l_pump_type.plt_pump)
                    {
                        APP_TRACE("plt pump speed=[%d].\r\n", g_arm2_rpdo1_data.PLT_pump_current_speed);
                    }
                }
            }

        }
        case 14:
        {
            if (strcmp("-a", argv[1]) == 0)
            {
                init_pump_param(&l_pump_type, &l_speed, &l_distance, &l_count);
                l_arg_pump = (u8)strtol(argv[2], NULL, 16);
                l_arg_feedback_dir = (u8)atoi(argv[3]);
                for (i=0; i<5; i++)
                {
                    l_pArg_speed[i] = (u16)atoi(argv[4+i]);
                    l_pArg_distance[i] = (u16)atoi(argv[9+i]);
                }

                if (l_arg_pump & 0x01)
                {
                    l_pump_type.draw_pump = 0x01;
                    l_speed.draw_speed = l_pArg_speed[0];
                    l_distance.draw_distance = l_pArg_distance[0];
                }
                if (l_arg_pump & 0x02)
                {
                    l_pump_type.ac_pump = 0x01;
                    l_speed.ac_speed = l_pArg_speed[1];
                    l_distance.ac_distance = l_pArg_distance[1];
                }
                if (l_arg_pump & 0x04)
                {
                    l_pump_type.feedBack_pump = 0x01;
                    l_speed.feedBack_speed = l_pArg_speed[2];
                    l_distance.feedBack_distance = l_pArg_distance[2];
                    l_pump_type.feedBack_dir = l_arg_feedback_dir;
                }
                if (l_arg_pump & 0x08)
                {
                    l_pump_type.pla_pump = 0x01;
                    l_speed.pla_speed = l_pArg_speed[3];
                    l_distance.pla_distance = l_pArg_distance[3];
                }
                if (l_arg_pump & 0x10)
                {
                    l_pump_type.plt_pump = 0x01;
                    l_speed.plt_speed = l_pArg_speed[4];
                    l_distance.plt_distance = l_pArg_distance[4];
                }
                if (ctrl_start_pump(l_pump_type, l_speed, l_distance, l_count))
                {
                    APP_TRACE("start pump error!\r\n");
                }
                if (wait_pump_end(l_pump_type, l_speed, l_distance))
                {
                    APP_TRACE("wait pump end error!\r\n");
                }
                if (l_distance.ac_distance>0 || l_distance.draw_distance>0 || l_distance.feedBack_distance>0
                    || l_distance.pla_distance>0 || l_distance.plt_distance>0)
                {
                    if (ctrl_stop_pump(l_pump_type, l_count))
                    {
                        APP_TRACE("stop pump error!\r\n");
                    }
                }
            }

            break;
        }
        default:
            break;
    }

    return 0;
}
#endif

#ifdef USE_CONTROL_CENTRIFUGE_FTR
static CPU_INT16S ctrl_centri_cmd(CPU_INT16U argc,
                                         CPU_CHAR** argv,
                                         SHELL_OUT_FNCT pout_fnct,
                                         SHELL_CMD_PARAM* pcmd_param)
{
    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                     // help information
    		{
    			APP_TRACE("centrifuge send usage:\r\n"
                          "-speed[i]            Enter the centrifuge speed.\r\n"
                          "-stop                Stop the centrifuge.\r\n"
                          "-h                   get help information\r\n"
    					  );
            }
            else if (strcmp("-stop", argv[1]) == 0)
            {
                ctrl_stop_centrifuge();
            }
            break;
        }
        case 3:
        {
            if (strcmp("-speed", argv[1])==0)
            {
                ctrl_start_centrifuge((u16)atoi(argv[2])*10);
            }
            break;
        }
    }

    return 0;
}
#endif




#ifdef CTRL_TURN_VALVE_FTR
static CPU_INT16S ctrl_turn_valve_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{

    CTRL_VALVE_TYPE l_type;
    CTRL_VALVE_DIR l_dir;

    memset(&l_type, 0x07, sizeof(l_type));

    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                     // help information
    		{
    			APP_TRACE("ctrl_turn_valve_cmd usage:\r\n"
                          "-set [dir]   set 3 valve dir.  [0=>left] [1=>mid] [2=>right]\r\n"
                          "-h           get help information\r\n"
    					  );      
                
            }

        }

        case 3:
        {
            if (strcmp("-set", argv[1])==0)
            {
                switch(atoi(argv[2]))
                {
                    case 0://left

                        l_dir.rbc_valve = VALVE_DIR_LEFT;//00
                        l_dir.pla_valve = VALVE_DIR_LEFT;
                        l_dir.plt_valve = VALVE_DIR_LEFT;
                        ctrl_turn_valve(l_type, l_dir);
                        // all valve turn left
                         APP_TRACE("3 valve TO LEFT\r\n");
                    break;
                    
                    case 1://mid
                        l_dir.rbc_valve = VALVE_DIR_MIDDLE;//00
                        l_dir.pla_valve = VALVE_DIR_MIDDLE;
                        l_dir.plt_valve = VALVE_DIR_MIDDLE;
                        ctrl_turn_valve(l_type, l_dir);
                        // all valve turn mid
                         APP_TRACE("3 valve TO  MID\r\n");
                    break;
                    
                    case 2://right
                        l_dir.rbc_valve = VALVE_DIR_RIGHT;//00
                        l_dir.pla_valve = VALVE_DIR_RIGHT;
                        l_dir.plt_valve = VALVE_DIR_RIGHT;
                        ctrl_turn_valve(l_type, l_dir);
                        // all valve turn right
                         APP_TRACE("3 valve TO IGHT\r\n");
                    break;

                }
            
            }
            break;
        }
    }

    return 0;

}

#endif








/********************************************************************************/



#ifdef USE_DC_MOTOR_SHELL_CMD_FTR
static CPU_INT16S dc_motor_test_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{
    u32 motor_num;
    s32 sp1, sp2, sp3;
    u32 dis;
    u32 pwm_freq, pid_interval_time;
    static u32 kp, p, i, d;         // save those parameter in memory
    u32 dc_motor_adjust_value[3];
    u32 current_pwm[3], max_pwm[3];
    dc_motor_PID_param_t pid;
    u32 distance;

    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                      // help information
            {
                APP_TRACE("dc motor usage:\r\n"
                          "-init     [pwm f][pid f]   init DC motor module(PWM and encoder)\r\n"
                          "-readpid  [index]          read pid parameter from internel flash\r\n"
                          "-pid      [index][PID]     set PID param\r\n"
                          "-pidsave  [index][PID]     save PID param to internal flash\r\n"
                          "-set      [index][sp][dis] set DC motor speed\r\n"
                          "-start    [index]          start DC motor\r\n"
                          "-stop     [index]          stop DC motor\r\n"
                          "-sp       [index]          get DC motor speed\r\n"
                          "-distance [index]          get DC motor distance(1/10)\r\n"
                          "-h                         get help information\r\n");
            }
            else if (strcmp("-sp", argv[1]) == 0)
            {
                dc_motor_adjust_value[0] = get_adjust_pwm_percent(MOTOR_NUM1, &current_pwm[0], &max_pwm[0]);
                dc_motor_adjust_value[1] = get_adjust_pwm_percent(MOTOR_NUM2, &current_pwm[1], &max_pwm[1]);
                dc_motor_adjust_value[2] = get_adjust_pwm_percent(MOTOR_NUM3, &current_pwm[2], &max_pwm[2]);
                sp1 = get_dc_motor_sp(MOTOR_NUM1);
                sp2 = get_dc_motor_sp(MOTOR_NUM2);
                sp3 = get_dc_motor_sp(MOTOR_NUM3);
                APP_TRACE("DC motor speed and PWM\r\n");
                APP_TRACE("[0] [%5d]RPM, PWM = %3d[%4d,%4d]\r\n", sp1, dc_motor_adjust_value[0], current_pwm[0], max_pwm[0]);
                APP_TRACE("[1] [%5d]RPM, PWM = %3d[%4d,%4d]\r\n", sp2, dc_motor_adjust_value[1], current_pwm[1], max_pwm[1]);
                APP_TRACE("[2] [%5d]RPM, PWM = %3d[%4d,%4d]\r\n", sp3, dc_motor_adjust_value[2], current_pwm[2], max_pwm[2]);
            }
            else if (strcmp("-distance", argv[1]) == 0)
            {
                distance = get_dc_motor_current_distance(MOTOR_NUM1);
                APP_TRACE("motor1 distance = %d\r\n", distance);
                distance = get_dc_motor_current_distance(MOTOR_NUM2);
                APP_TRACE("motor2 distance = %d\r\n", distance);
                distance = get_dc_motor_current_distance(MOTOR_NUM3);
                APP_TRACE("motor3 distance = %d\r\n", distance);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        }

        case 3:
        {
            motor_num = atoi(argv[2]) - 1;
            if (!IS_DC_MOTOR_NUM_TYPE(motor_num))
            {
                APP_TRACE("error: index can only be set as 1,2,3\r\n");
                return SHELL_EXEC_ERR;
            }

            if (strcmp("-start", argv[1]) == 0)
            {
                APP_TRACE("start DC motor [%d]\r\n", motor_num);
                start_dc_motor((MotorType)motor_num);
            }
            else if (strcmp("-stop", argv[1]) == 0)
            {
                APP_TRACE("stop DC motor [%d]\r\n", motor_num);
                stop_dc_motor((MotorType)motor_num);
            }
            else if (strcmp("-sp", argv[1]) == 0)
            {
                if (IS_ENCODER_TYPE(motor_num))
                {
                    sp1 = 0;//get_current_dc_motor_speed((EncoderType)motor_num);
                    APP_TRACE("DC motor[%d] speed = %d RPM\r\n", motor_num, sp1);
                }
                else
                {
                    APP_TRACE("DC motor [%d] error!\r\n", motor_num);
                }
            }
            else if (strcmp("-readpid", argv[1]) == 0)          // read pid parameter from internel flash
            {
                pid.index = motor_num;
                get_pid_param_from_internal_flash(&pid);
                APP_TRACE("motor[%d] PID param: %d, %d, %d, %d\r\n", pid.index + 1, pid.kp, pid.kpp, pid.kpi, pid.kpd);
            }
            else if (strcmp("-pidsave", argv[1]) == 0)
            {
                pid.index = motor_num;
                pid.kp  = kp;
                pid.kpp = p;
                pid.kpi = i;
                pid.kpd = d;
                APP_TRACE("save motor[%d] PID param: %d, %d, %d, %d to internel flash...",
                            pid.index+1,
                            pid.kp,
                            pid.kpp,
                            pid.kpi,
                            pid.kpd);

                if (write_pid_param_to_internal_flash(&pid) == 0)
                {
                    APP_TRACE("done\r\n");
                }
                else
                {
                    APP_TRACE("error\r\n");
                }
            }
            else if (strcmp("-distance", argv[1]) == 0)
            {
                distance = get_dc_motor_current_distance((MotorType)motor_num);
                APP_TRACE("motor%d distance = %d\r\n", motor_num+1, distance);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        }

        case 4:
        {
            // -init [pwm][pid] frequency
            if (strcmp("-init", argv[1]) == 0)                   //init DC motor module
            {
                APP_TRACE("init DC motor module...\r\n");
                pwm_freq          = atoi(argv[2]);
                pid_interval_time = atoi(argv[3]);
                motor_module_init(pwm_freq, pid_interval_time);
            }
            else if (strcmp("-set", argv[1]) == 0)
            {
                // -set [index][sp] set DC motor speed
                motor_num = atoi(argv[2]) - 1;
                sp1       = atoi(argv[3]);
                if (!IS_DC_MOTOR_NUM_TYPE(motor_num))
                {
                    APP_TRACE("error: index can only be set as 1,2,3\r\n");
                    return SHELL_EXEC_ERR;
                }

                APP_TRACE("set DC motor[%d] speed to [%5dRPM]\r\n", motor_num+1, sp1);
                set_motor_speed((MotorType)motor_num, sp1, 0);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }

            break;
        }

        case 5:
        {
            motor_num = atoi(argv[2]) - 1;
            sp1       = atoi(argv[3]);
            dis       = atoi(argv[4]);
            if (!IS_DC_MOTOR_NUM_TYPE(motor_num))
            {
                APP_TRACE("error: index can only be set as 1,2,3\r\n");
                return SHELL_EXEC_ERR;
            }

            // -set [index][sp][dis]
            if (strcmp("-set", argv[1]) == 0)
            {
                APP_TRACE("set DC motor[%d] speed to [%5dRPM] distance = %d\r\n", motor_num+1, sp1, dis);
                set_motor_speed((MotorType)motor_num, sp1, dis);
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
        }
        break;

        case 7:
        {
            motor_num = atoi(argv[2]) - 1;
            if (!IS_DC_MOTOR_NUM_TYPE(motor_num))
            {
                APP_TRACE("error: index can only be set as 1,2,3\r\n");
                return SHELL_EXEC_ERR;
            }

            // -pid [index][PID]   set DC motor PID parameter
            if (strcmp("-pid", argv[1]) == 0)
            {
                kp = atoi(argv[3]);
                p  = atoi(argv[4]);
                i  = atoi(argv[5]);
                d  = atoi(argv[6]);
                //example 100, 9000, 1000, 0
                APP_TRACE("set motor%d PID[%d][%d][%d][%d]\r\n", motor_num+1, kp, p, i, d);
                dc_motor_pid_param_config((MotorType)motor_num, kp, p, i, d);
            }
            else if (strcmp("-pidsave", argv[1]) == 0)
            {
                kp = atoi(argv[3]);
                p  = atoi(argv[4]);
                i  = atoi(argv[5]);
                d  = atoi(argv[6]);
                //example 100, 9000, 1000, 0
                APP_TRACE("save motor%d PID[%d][%d][%d][%d] to internel flash...", motor_num+1, kp, p, i, d);
                pid.index = motor_num;
                pid.kp  = kp;
                pid.kpp = p;
                pid.kpi = i;
                pid.kpd = d;
                write_pid_param_to_internal_flash(&pid);
                APP_TRACE("done\r\n");
            }
            else
            {
                return SHELL_EXEC_ERR;
            }
            break;
        }

        default:
            return SHELL_EXEC_ERR;

    }
    return 0;
}


#endif /* USE_DC_MOTOR_SHELL_CMD_FTR */



#ifdef USE_DC_COOLING_FAN_CMD_FTR
static CPU_INT16S dc_cooling_fan_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{
    u32 index;
    u32 speed;
    int i;

    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                      // help information
			{
				APP_TRACE("fan usage:\r\n"
                          "-sp [index]   get DC motor speed\r\n"
						  "-h            get help information\r\n");
			}
            else if (strcmp("-sp", argv[1]) == 0)
            {
                for (i = 0; i < DC_COOLING_FAN_NUM; i++)
                {
                    speed = get_dc_cooling_fan_speed((dc_cooling_fan_type)i);
                    APP_TRACE("FAN[%d] speed = %d\r\n", i + 1, speed);
                }
            }
            break;
        }

        case 3:
            if (strcmp("-sp", argv[1]) == 0)
            {
                index = atoi(argv[2]);
                if (IS_DC_COOLING_FAN_NUM_TYPE(index))
                {
                    speed = get_dc_cooling_fan_speed((dc_cooling_fan_type)index);
                    APP_TRACE("FAN[%d] speed = %d\r\n", index + 1, speed);
                }
                else
                {
                    return SHELL_EXEC_ERR;
                }
            }
            break;

        default:
            return SHELL_EXEC_ERR;
    }

    return 0;
}

#endif /* USE_DC_COOLING_FAN_CMD_FTR */


#ifdef USE_PCA8538_COG_DEBUG_FTR
static CPU_INT16S PCA8538_COG_cmd(CPU_INT16U argc,
                                               CPU_CHAR** argv,
                                               SHELL_OUT_FNCT pout_fnct,
                                               SHELL_CMD_PARAM* pcmd_param)
{
    switch (argc)
    {
        case 2:
        {
            if (strcmp("-h", argv[1]) == 0)                      // help information
            {
                APP_TRACE("cog usage:\r\n"
                          "-auto   AUTO test PCA8538\r\n"
                          "-h      get help information\r\n");
            }
            else if (strcmp("-auto", argv[1]) == 0)
            {
                PCA8538_AUTO_test();
            }
            break;
        }

        default:
            return SHELL_EXEC_ERR;
    }

    return 0;
}

#endif /* USE_PCA8538_COG_DEBUG_FTR */



/**
  * show all support commands
  *
  */
static CPU_INT16S cmd_list(CPU_INT16U i, CPU_CHAR  **p_p, SHELL_OUT_FNCT p_f, SHELL_CMD_PARAM* p)
{
	SHELL_MODULE_CMD* p_module_cmd;
	SHELL_CMD*        p_cmd;
	uint32_t          cmd_pos = 0;

	APP_TRACE("------------ support commands ------------\r\n");
	p_module_cmd = Shell_ModuleCmdUsedPoolPtr;
	while(p_module_cmd != 0)
	{
		cmd_pos = 0;
		p_cmd = p_module_cmd->CmdTblPtr;
		while(p_cmd[cmd_pos].Name)
		{
			APP_TRACE("%s\r\n", p_cmd[cmd_pos].Name);
			cmd_pos++;
		}

		p_module_cmd = p_module_cmd->NextModuleCmdPtr;
	}

	return 0;
}



void app_cmd_tbl_add (void)
{
	SHELL_ERR err;

	APP_TRACE("Adding app shell command table ... ");
	Shell_CmdTblAdd("app", App_ShellAppCmdTbl, &err);
	if (err == SHELL_ERR_NONE) {
		APP_TRACE("done.\n\r");
	} else {
		APP_TRACE("failed.\n\r");
	}
}

/**
  * @brief: Get data from uart input
  *
  * @param: cmd_input_end  eg. 0x03(CTRL+C) or 0x0D(Enter)
  * @param: p_buf          buffer to store the receive data
  * @param: len            the receive buffer size
  * @param: display_flag   1: display the receive data in console
  *                        0: don't dispaly
  */
static u32 get_uart_data(u8* p_buf, u32 len, u8 cmd_input_end, u8 display_flag)
{
	INT8U     err;
	u8        cmd;
	u32       cmd_len = 0;

	for(;;)
	{
		cmd = (u8)(u32)OSQPend(g_uart1_queue, 0, &err);
		assert_param(OS_ERR_NONE == err);

		if (cmd == cmd_input_end)
		{
			p_buf[cmd_len] = 0;
			break;
		}
		else if (cmd == '\b')
		{
			if(cmd_len > 0)
			{
				cmd_len--;
				APP_TRACE("%c", '\b');
			}
		}
		else
		{
			if (cmd_len == len - 1)       // buffer is full
			{
				p_buf[cmd_len] = cmd;
				break;
			}
			else                          // buffer not full store the data
			{
				if (display_flag)
				{
					APP_TRACE("%c", cmd);
				}
				p_buf[cmd_len++] = cmd;
			}
		}
	}

	return cmd_len + 1;
}


/**
  * UART will send a event when it receive a char
  *
  */
static void init_uart_event()
{
	g_uart1_queue = OSQCreate(uart_array_msg, CMD_BUF_SIZE);
	assert_param(g_uart1_queue);

	memset(g_cmd_save_buf, 0x00, sizeof g_cmd_save_buf);
	g_cmd_save_num = 0;
}


static void save_cmd_to_buf(u8* p_buf)
{
	u8 i = 0;

	if (g_cmd_save_num < CMD_SAVE_BUF_COUNT)
	{
		strncpy((char*)g_cmd_save_buf[g_cmd_save_num++], (const char*)p_buf, CMD_BUF_SIZE);
	}
	else
	{
		// buffer is full and we need discard the first one
		// NOTE: g_cmd_save_num = CMD_SAVE_BUF_COUNT
		//
		for (i = 0; i < g_cmd_save_num - 2; i++)
		{
			memcpy(g_cmd_save_buf[i], g_cmd_save_buf[i + 1], CMD_BUF_SIZE);
		}
		// save the latest buffer
		strncpy((char*)g_cmd_save_buf[g_cmd_save_num - 1], (const char*)p_buf, CMD_BUF_SIZE);
	}
}


static void delete_saved_cmd(u8 index)
{
	u8 i;

	if (index < g_cmd_save_num)
	{
		for (i = index; i < g_cmd_save_num - 1; i++)
		{
			memcpy(g_cmd_save_buf[i], g_cmd_save_buf[i + 1], CMD_BUF_SIZE);
		}

		g_cmd_save_num--;
	}
}

static void exec_cmd(u8* p_buf)
{
	SHELL_ERR shell_err;
	u8        cmd_buf[CMD_BUF_SIZE + 10];

	snprintf((char*)cmd_buf, sizeof(cmd_buf), "%s%s", SHELL_HEADER, p_buf);
	Shell_Exec((CPU_CHAR*)cmd_buf, (SHELL_OUT_FNCT)0, (SHELL_CMD_PARAM*)0, &shell_err);
	switch(shell_err)
	{
		case SHELL_ERR_NONE:
			break;
		case SHELL_ERR_NULL_PTR:
			APP_TRACE("Error, NULL pointer passed.");
			break;
		case SHELL_ERR_CMD_NOT_FOUND:
			APP_TRACE("Error, command NOT found.");
			break;
		case SHELL_ERR_CMD_SEARCH:
			APP_TRACE("Error, searching command.");
			break;
		case SHELL_ERR_ARG_TBL_FULL:
			APP_TRACE("Error, too many arguments.");
			break;
		case SHELL_ERR_CMD_EXEC:
			APP_TRACE("Error, executing command.");
			break;
		default:
			break;
	}

	delete_saved_cmd(g_cmd_save_num - g_cmd_pos);
	// save this cmd data
	save_cmd_to_buf(p_buf);
	// reset up or down position
	g_cmd_pos = 0;

}


// ESC
// 1b 5b 41  --- up
// 1b 5b 42  --- down
// 1b 5b 43  --- right
// 1b 5b 44  --- left
static void do_esc_cmd(u8 cmd_len)
{
	u8   cmd_buf[CMD_BUF_SIZE];
	u8   i;
	OS_CPU_SR cpu_sr;

	get_uart_data(cmd_buf, 2, 0x00, 0);

	if (cmd_buf[0] == 0x5b)
	{
		if (cmd_buf[1] == 0x41) 		// up
		{
			// buffer have saved cmds
			if (g_cmd_save_num)
			{
				if (g_cmd_save_num - g_cmd_pos > 0)
				{
					// clear the cmd
					for (i = 0; i < cmd_len; i++)
					{
						APP_TRACE("\b");
					}

					OS_ENTER_CRITICAL();
					for (i = 0; i < strlen((const char*)g_cmd_save_buf[g_cmd_save_num - 1 - g_cmd_pos]); i++)
					{
						OSQPost(g_uart1_queue, (void *)(g_cmd_save_buf[g_cmd_save_num - 1 - g_cmd_pos][i]));
					}
					OS_EXIT_CRITICAL();

					g_cmd_pos++;
				}
				else
				{
				}

			}
			else
			{
				// do nothing when no cmd in buffer
			}
		}
		else if (cmd_buf[1] == 0x42)	// down
		{
			if (g_cmd_pos)
			{
				// clear the cmd
				for (i = 0; i < cmd_len; i++)
				{
					APP_TRACE("\b");
				}

				OS_ENTER_CRITICAL();
				for (i = 0; i < strlen((const char*)g_cmd_save_buf[g_cmd_save_num - g_cmd_pos]); i++)
				{
					OSQPost(g_uart1_queue, (void *)(g_cmd_save_buf[g_cmd_save_num - g_cmd_pos][i]));
				}
				OS_EXIT_CRITICAL();

				g_cmd_pos--;
			}
		}
		else if (cmd_buf[1] == 0x43)	// right
		{

		}
		else if (cmd_buf[1] == 0x44)	// left
		{

		}
		else
		{
			// no care about other cmd data
		}
	}
	else
	{
		// no care about other cmd data
	}

}


void init_shell_task(void)
{
    INT8U  os_err = 0;

	/* shell module task ***************************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) shell_task,
							(void		   * ) 0,
							(OS_STK 	   * )&shell_task_stk[SHELL_TASK_STK_SIZE - 1],
							(INT8U			 ) SHELL_TASK_PRIO,
							(INT16U 		 ) SHELL_TASK_PRIO,
							(OS_STK 	   * )&shell_task_stk[0],
							(INT32U 		 ) SHELL_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(SHELL_TASK_PRIO, (INT8U *)"shell", &os_err);
}


static void shell_task(void *p_arg)
{
	uint8_t   cmd_buf[CMD_BUF_SIZE];
	uint8_t   cmd_len = 0;
	uint8_t   cmd;
	INT8U     err;
	uint8_t   active_flag = 0;

	Shell_Init();
	app_cmd_tbl_add();
	init_uart_event();

	OSTimeDlyHMSM(0, 0, 0, 200);
	APP_TRACE("\r\n>>>>> Type Enter key to active shell console. <<<<<");
    APP_TRACE("\r\n>>>>>   Type CTRL+C to exit shell console.    <<<<<\r\n");

	while (1)
	{
		cmd = (uint8_t)(uint32_t)OSQPend(g_uart1_queue, 0, &err);
		assert_param(OS_ERR_NONE == err);

		// check shell module active
		if(active_flag == 0)
		{
			if (cmd == 0x0d)
			{
				APP_TRACE("\r\nVINY>");
				active_flag = 1;
			}
			else
			{
				APP_TRACE("Type Enter key to active this shell console...\r\n");
			}

			continue;
		}

		switch (cmd)
		{
			// ESC
			case 0x1b:
				do_esc_cmd(cmd_len);
				cmd_len = 0;
				break;

			// backspace
			case '\b':
				if(cmd_len > 0)
				{
					cmd_len--;
					APP_TRACE("%c", '\b');
				}
				break;

            // CTRL + C ----> exit shell console
            case 0x03:
                active_flag = 0;
                APP_TRACE("\r\nExit shell console.\r\n");
                cmd_len = 0;
                break;

			case 0x0d:
				if(cmd_len == 0)
				{
					APP_TRACE("\r\nVINY>");
					break;
				}
				else
				{
					cmd_buf[cmd_len] = 0;
					APP_TRACE("\r\n");

					// exec cmd
					exec_cmd(cmd_buf);

					APP_TRACE("\r\nVINY>");
					cmd_len = 0;
				}
				break;

			default:
				if (cmd_len >= 64)
				{
					APP_TRACE("\r\nError, cmd is too long.\r\n");
					APP_TRACE("VINY>");
					cmd_len = 0;
				}
				else
				{
					cmd_buf[cmd_len++] = cmd;
					APP_TRACE("%c", cmd);
				}
				break;
		}
	}
}



/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/******************************************************************************/
#if defined(USE_USART1_TO_DEBUG)
void USART1_IRQHandler(void)
{
	uint16_t ch;

	OSIntEnter();
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART1);
		OSQPost(g_uart1_queue, (void *)ch);
	}
	OSIntExit();
}

#elif defined(USE_USART3_TO_DEBUG)

void USART3_IRQHandler(void)
{
	uint16_t ch;

	OSIntEnter();
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART3);
		OSQPost(g_uart1_queue, (void *)ch);
	}
	OSIntExit();
}

#endif



