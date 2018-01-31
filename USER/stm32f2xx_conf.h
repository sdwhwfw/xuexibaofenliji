/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/stm32f2xx_conf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_CONF_H
#define __STM32F2xx_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm32f2xx_adc.h"
#include "stm32f2xx_can.h"
#include "stm32f2xx_crc.h"
#include "stm32f2xx_cryp.h"
#include "stm32f2xx_dac.h"
#include "stm32f2xx_dbgmcu.h"
#include "stm32f2xx_dcmi.h"
#include "stm32f2xx_dma.h"
#include "stm32f2xx_exti.h"
#include "stm32f2xx_flash.h"
#include "stm32f2xx_fsmc.h"
#include "stm32f2xx_hash.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_i2c.h"
#include "stm32f2xx_iwdg.h"
#include "stm32f2xx_pwr.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_rng.h"
#include "stm32f2xx_rtc.h"
#include "stm32f2xx_sdio.h"
#include "stm32f2xx_spi.h"
#include "stm32f2xx_syscfg.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_usart.h"
#include "stm32f2xx_wwdg.h"
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* If an external clock source is used, then the value of the following define
   should be set to the value of the external clock source, else, if no external
   clock is used, keep this define commented */
/*#define I2S_EXTERNAL_CLOCK_VAL   12288000 */ /* Value of the external clock in Hz */


/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */

#define USE_FULL_ASSERT    1

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed.
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */



/*
 *===================================================
 * define our MACRO here...
 *===================================================
 */

// for debug
#define _DEBUG


//
// main control board with uCOS-II
// This board use comX100 module for CANOpen master
//
#if defined(CONTROL_MAIN_BOARD)

    // ------------------------ use uCOS-II ------------------------
    #define _USE_UCOS_FTR_

    // ---------- use comX100 module for CANOpen master ------------
    #define _COMX100_MODULE_FTR_
    // ------------------------ Toolkit ----------------------------
    #define CIFX_TOOLKIT_HWIF               /* Toolkit */

    // ------ use STM32 CAN to get CANBUS data from CANOpen --------
    //#define USE_STM32_CAN_DEBUG

    // debug comX100 module with CAN and output CAN bus data to UART3
    //#define USE_UART3_TRACE_FTR

    // -------------------- SPI FLASH feature ----------------------
    //#define USE_SPI_FLASH_AT45DB_FTR

    // ---------------------- FATFS feature ------------------------
    //#define USE_FATFS_FTR

    // --------------------- SRAM definition -----------------------
    // if you want to USE sram you need to uncomment the MACRO
    //#define USE_ISSI_SRAM_FTR

    // ------------ SRAM bandwidth test task definition ------------
    //#define USE_SRAM_BANDWIDTH_TEST_FTR

    // ------------------ RTC and BKPSRAM feature ------------------
    #define USE_RTC_BKP_FTR

    // ----------------------- buzzer feature ----------------------
    #define USE_BUZZER_FTR

    // ----------------------- PWM test cmd ------------------------
    //#define USE_PWM_TEST_FTR

    // ---------- debug message output serial definition -----------
    // Now, we support USART1 or USART3 for debug
    #define USE_USART1_TO_DEBUG
    //#define USE_USART3_TO_DEBUG

    // -------- USART for DMA roure task feature definition --------
    // !!! Be care with DEUBG USART num !!!
    //#define USE_USART1_DMA_FTR
    //#define USE_USART2_DMA_FTR
    #define USE_USART3_DMA_FTR
    //#define USE_UART4_DMA_FTR
    //#define USE_UART5_DMA_FTR
    //#define USE_USART6_DMA_FTR

	// --------------------- Communication test cmd ----------------
	// UART COMM test cmd
    #define USE_UART_COMM_TEST_FTR
    // TPDO SEND test cmd
    #define USE_TPDO_SEND_TEST_FTR
    // Control pump cmd
    #define USE_CONTROL_PUMP_FTR
    // control centrifuge cmd
    #define USE_CONTROL_CENTRIFUGE_FTR
    //add by Big.Man for valve move together
    #define CTRL_TURN_VALVE_FTR

    // ---------------- which board do you use? --------------------
    // only one board can be selected
    //#define USE_OPEN_207Z_BOARD
    //#define USE_OPEN_207V_BOARD
    //#define USE_OPEN_207Z_MINI_BOARD
    #define USE_ARM0_REV_0_1

#endif /* CONTROL_MAIN_BOARD */

//
// slave board 1 with uCOS-II
//
#if defined(SLAVE_BOARD_1)
    #define _USE_UCOS_FTR_                  /* use uCOS-II */
    #define _NETX_MODBUS_FTR_               /* use NETX50 module for CANopen slave */

    // ---------- debug message output serial definition -----------
    // Now, we support USART1 or USART3 for debug
  //  #define USE_USART1_TO_DEBUG     //for board v 0.1
    #define USE_USART3_TO_DEBUG   //for board v 0.2

    // -------- USART for DMA roure task feature definition --------
    // Be care with DEUBG USART num!!!
    #define USE_USART1_DMA_FTR     //for board v 0.2
    //#define USE_USART2_DMA_FTR
   // #define USE_USART3_DMA_FTR   //for board v 0.1
    //#define USE_UART4_DMA_FTR
    //#define USE_UART5_DMA_FTR
    //#define USE_USART6_DMA_FTR


    // ---------------- which board do you use? --------------------
    // only one board can be selected
    //#define USE_OPEN_207Z_BOARD
    //#define USE_OPEN_207V_BOARD
    //#define USE_OPEN_207Z_MINI_BOARD
    //#define USE_ARM1_REV_0_1
    #define USE_ARM1_REV_0_2

    // ---------- use shell cmd to read parameter table ------------
    #define USE_PARA_TABLE_FTR

    // ----------------------- PWM test cmd ------------------------
    //#define USE_PWM_TEST_FTR

    // ----------------------- buzzer feature ----------------------
    #define USE_BUZZER_FTR

    // -------------------- DC Motor shell cmd ---------------------
    #define USE_DC_MOTOR_SHELL_CMD_FTR

    // ----------------- DC cooling fan shell cmd ------------------
    //#define USE_DC_COOLING_FAN_CMD_FTR

    // ----------------- PCA8538 COG shell cmd ------------------
    //#define USE_PCA8538_COG_DEBUG_FTR

    // ------------------------ USB DEBUG --------------------------
    // You should add USB souce to your project
    // if you want to USE USB for communication
    //#define USE_USB_TO_DEBUG_DC_MOTOR       /* use USB to DEBUG */
    #define USE_USB_OTG_FS                  /* put message to PC with USB */
    #define USE_OPEN207V_EVAL               /* for VCP */

#endif /* SLAVE_BOARD_1 */

//
// slave board 2 with uCOS-II
//
#if defined(SLAVE_BOARD_2)
	#define _USE_UCOS_FTR_                  /* use uCOS-II */
	#define _NETX_MODBUS_FTR_               /* use NETIC module for CANopen slave */

    // ---------- debug message output serial definition -----------
    // Now, we support USART1 or USART3 for debug
   // #define USE_USART1_TO_DEBUG
    #define USE_USART3_TO_DEBUG

    // -------- USART for DMA roure task feature definition --------
    // Be care with DEUBG USART num!!!
    #define USE_USART1_DMA_FTR
    //#define USE_USART2_DMA_FTR
   // #define USE_USART3_DMA_FTR
    //#define USE_UART4_DMA_FTR
    //#define USE_UART5_DMA_FTR
    //#define USE_USART6_DMA_FTR

    // ---------------- which board do you use? --------------------
    // only one board can be selected
    //#define USE_OPEN_207Z_BOARD
    //#define USE_OPEN_207V_BOARD
    //#define USE_OPEN_207Z_MINI_BOARD
    #define USE_ARM2_REV_0_1

    // ----------------------- PWM test cmd ------------------------
    //#define USE_PWM_TEST_FTR

    // ----------------------- buzzer feature ----------------------
    #define USE_BUZZER_FTR

    // -------------------- DC Motor shell cmd ---------------------
    #define USE_DC_MOTOR_SHELL_CMD_FTR

    /*use shell cmd to read parameter table*/
    #define USE_PARA_TABLE_FTR


    #define USE_VALVE_CMD_FTR

    

#endif /* CONTROL_SLAVE_BOARD_2 */

//
// slave board 3 with uCOS-II
//
#if defined(SLAVE_BOARD_3)

    // ------------------------ use uCOS-II ------------------------
    #define _USE_UCOS_FTR_

    // ------------ use NETIC module for CANopen slave -------------
    //#define _NETX_MODBUS_FTR_

    // ------------ use shell cmd to read parameter table ----------
    #define USE_PARA_TABLE_FTR

    // ----------------------- buzzer feature ----------------------
    #define USE_BUZZER_FTR
    // centrifuge shell cmd ----------------------
    #define USE_CENT_CMD_FTR
    //electromaget shell cmd ----------------------
    #define USE_MAGET_CMD_FTR

    #define USE_CASSETTE_CMD_FTR
    
	// ---------- debug message output serial definition -----------
	// Now, we support USART1 or USART3 for debug
    //#define USE_USART1_TO_DEBUG
    #define USE_USART3_TO_DEBUG

    // -------- USART for DMA roure task feature definition --------
    // Be care with DEUBG USART num!!!
    #define USE_USART1_DMA_FTR
    #define USE_USART2_DMA_FTR
    //#define USE_USART3_DMA_FTR
    //#define USE_UART4_DMA_FTR
    //#define USE_UART5_DMA_FTR
    //#define USE_USART6_DMA_FTR

    // ----------------- DC cooling fan shell cmd ------------------
    //#define USE_DC_COOLING_FAN_CMD_FTR

    // ---------------- which board do you use? --------------------
    // only one board can be selected
    //#define USE_OPEN_207Z_BOARD
    //#define USE_OPEN_207V_BOARD
    //#define USE_OPEN_207Z_MINI_BOARD
    #define USE_ARM3_REV_0_1

#endif /* CONTROL_SLAVE_BOARD_3 */


//
// slave board 2 with no uCOS-II
// This board use USB module to output large data to PC for debug purpose
// USB Speed: ~ 1Mbps
//
#if defined(NO_RTOS_BOARD)
	#define _MOTOR_CONTROL_FTR_             /* control motor with no OS */
	#define USE_USB_OTG_FS                  /* put message to PC with USB */
	#define USE_OPEN207V_EVAL               /* for VCP */
#endif /* CONTROL_SLAVE_BOARD_2 */

#include "app_cfg.h"
#include "ucos_ii.h"


#endif /* __STM32F2xx_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
