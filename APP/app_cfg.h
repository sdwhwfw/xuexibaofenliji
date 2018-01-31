/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cfg.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/03/22
 * Description        : This file contains the software implementation for the
 *                      app configuration unit
 *******************************************************************************
 * History:
 * DATE       | VER      | AUTOR      | Description
 * 2013/03/22 | v1.0     |            | initial released
 * 2013/08/13 | v0.0.2.0 |            | comX100 module support interrupt mode
 * 2013/08/27 | v0.0.3.0 |            | bug fix and add some feature
 * 2013/09/23 | v0.0.4.0 | Bruce.zhu  | Merge source code from WQ
 * 2013/10/18 | v0.0.4.0 | ZhangQ     | Add ControlBoard Task feature
 *******************************************************************************/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#define FIRMWARE_VERSION            "0.0.4.0"

#if defined(CONTROL_MAIN_BOARD)
#define BOARD_VERSION               "CONTROL BOARD 0.1"
#elif defined(SLAVE_BOARD_1)
#define BOARD_VERSION               "SLAVE BOARD[1] 0.1"
#elif defined(SLAVE_BOARD_2)
#define BOARD_VERSION               "SLAVE BOARD[2] 0.1"
#elif defined(SLAVE_BOARD_3)
#define BOARD_VERSION               "SLAVE BOARD[3] 0.1"
#endif /* BOARD_VERSION */


/* APP_TRACE3 mutex priority */
#define APP_TRACE3_MUTEX_PRIO            5u
/* APP_TRACE mutex priority */
#define APP_TRACE_MUTEX_PRIO             6u
/* timer task priority */
#define OS_TASK_TMR_PRIO                 40u


//==============================================
// define your task priority and stack here...
//==============================================

/*################################################*/
/* TASK MACRO USE FOR ALL BOARDS                  */
/*################################################*/

/* SRAM bandwidth test task proiority */
#define SRAM_BANDWIDTH_TASK_PRIO         7u
#define SRAM_BANDWIDTH_TASK_STK_SIZE     128u

/* app start task priority */
#define STARTUP_TASK_PRIO                9u
/* app start task stack size */
#define STARTUP_TASK_STK_SIZE            128u

/* SHELL task priority */
#define SHELL_TASK_PRIO                  58u
/* SHELL task stack size */
#define SHELL_TASK_STK_SIZE              256u

/* LED task */
#define LED_TASK_PRIO                    59u
#define LED_TASK_STK_SIZE                128u

/* WATCHDOG */
#define WDG_TASK_PRIO                    60u
#define WDG_TASK_STK_SIZE                128u

/* USART DMA ROUTE TASK */
#define USART_DMA_ROUTE_RX_TASK_PRIO     14u
#define USART_DMA_ROUTE_RX_TASK_STK_SIZE 256u
#define USART_DMA_ROUTE_TX_TASK_PRIO     15u
#define USART_DMA_ROUTE_TX_TASK_STK_SIZE 256u

/* SENSOR TASK */
#define SENSOR_TASK_PRIO                  16u //for debug
#define SENSOR_TASK_STK_SIZE              256u



/* DC COOLING FAN TASK */
#define DC_COOLIING_FAN_TASK_PRIO        45u
#define DC_COOLIING_FAN_TASK_STK_SIZE    128u

/* PCA8538 COG TASK for debug DC motor */
#define PCA8538_COG_TASK_PRIO            46u
#define PCA8538_COG_TASK_STK_SIZE        256u

/* buzzer ring task */
#define BUZZER_RING_TASK_PRIO            47u
#define BUZZER_RING_TASK_STK_SIZE        128u


#define CONTROL_USER_TMR_TASK_PRIO          55u
#define CONTROL_USER_TMR_TASK_STK_SIZE      128u



/*################################################*/
/* CONTORL MAIN BOARD MACRO DEFINITION            */
/*################################################*/
#if defined(CONTROL_MAIN_BOARD)

#define CONTROL_BOARD_TASK_PRIO          10u
#define CONTROL_BOARD_TASK_STK_SIZE      256u

#if defined(_COMX100_MODULE_FTR_)
/* comX100 module interrupt service thread priority */
#define COMX100_ISR_TASK_PRIO            11u
/* comX100 module interrupt service stack size */
#define COMX100_ISR_TASK_STK_SIZE        512u
/* COMX100 task priority */
#define COMX100_TASK_PRIO                12u
/* COMX100 task stack size */
#define COMX100_TASK_STK_SIZE            2048u
#endif /* _COMX100_MODULE_FTR_ */

#define CONTROL_UART_SEND_TASK_PRIO         23u
#define CONTROL_UART_SEND_TASK_STK_SIZE     512u

#define CONTROL_ARM1_TPDO_TASK_PRIO         24u
#define CONTROL_ARM1_TPDO_TASK_STK_SIZE     512u

#define CONTROL_ARM2_TPDO_TASK_PRIO         25u
#define CONTROL_ARM2_TPDO_TASK_STK_SIZE     512u

#define CONTROL_ARM3_TPDO_TASK_PRIO         26u
#define CONTROL_ARM3_TPDO_TASK_STK_SIZE     512u

#define CONTROL_MONITOR_TASK_PRIO           28u
#define CONTROL_MONITOR_TASK_STK_SIZE       512u

#define CONTROL_ADJUST_TASK_PRIO            29u
#define CONTROL_ADJUST_TASK_STK_SIZE        512u

#define CONTROL_FEEDBACK_TASK_PRIO          31u
#define CONTROL_FEEDBACK_TASK_STK_SIZE      512u

//#define CONTROL_FLOW_TASK_PRIO              32u
#define CONTROL_FLOW_TASK_PRIO              32u
#define CONTROL_FLOW_TASK_STK_SIZE          512u

#define CONTROL_TIMING_BACK_TASK_PRIO       33u
#define CONTROL_TIMING_BACK_TASK_STK_SIZE   512u

#define COTNROL_CALI_PUMP_TASK_PRIO         35u
#define CONTROL_CALI_PUMP_TASK_STK_SIZE     512u


#define TROUBLE_SHOOTING_TASK_PRIO          36u
#define TROUBLE_SHOOTING_TASK_STK_SIZE      256u


#if defined(USE_FATFS_FTR)
/* FS task priority */
#define FS_TASK_PRIO                     50u
/* FS task stack size */
#define FS_TASK_STK_SIZE                 256u
#endif /* USE_FATFS_FTR */

#if defined(USE_STM32_CAN_DEBUG)
/* SHELL task priority */
#define CAN_TASK_PRIO                    57u
/* SHELL task stack size */
#define CAN_TASK_STK_SIZE                128u
#endif /* USE_STM32_CAN_DEBUG */

#endif /* CONTROL_MAIN_BOARD */


/*################################################*/
/* SLAVE BOARD 1 MACRO DEFINITION                 */
/*################################################*/
#if defined(SLAVE_BOARD_1)
#define SLAVE_BOARD1_TASK_PRIO           22u
#define SLAVE_BOARD1_TASK_STK_SIZE       512u

#define MODBUS_ANALYSE_TASK_PRIO         21u
#define MODBUS_ANALYSE_TASK_STK_SIZE     256u

#define PUMP_CONTROL_TASK_PRIO           24u
#define PUMP_CONTROL_TASK_STK_SIZE       256u

#define DC_MOTOR_TASK_PRIO               25u
#define DC_MOTOR_TASK_STK_SIZE           256u

#define DC_MOTOR_SEND_TASK_PRIO          26u
#define DC_MOTOR_SEND_TASK_STK_SIZE      256u

#define TPDO_TASK_PRIO                   27u
#define TPDO_TASK_STK_SIZE               256u


#define SENSOR_HANDLE_TASK_PRIO          28u
#define SENSOR_HANDLE_TASK_STK_SIZE      256u


#endif /* SLAVE_BOARD_1 */


/*################################################*/
/* SLAVE BOARD 2 MACRO DEFINITION                 */
/*################################################*/
#if defined(SLAVE_BOARD_2)
#define SLAVE_BOARD2_TASK_PRIO           22u
#define SLAVE_BOARD2_TASK_STK_SIZE       512u

#define MODBUS_ANALYSE_TASK_PRIO         21u
#define MODBUS_ANALYSE_TASK_STK_SIZE     256u

#define PUMP_CONTROL_TASK_PRIO           24u //for debug
#define PUMP_CONTROL_TASK_STK_SIZE       256u

#define DC_MOTOR_TASK_PRIO               25u
#define DC_MOTOR_TASK_STK_SIZE           256u

#define DC_MOTOR_SEND_TASK_PRIO          26u
#define DC_MOTOR_SEND_TASK_STK_SIZE      256u

#define TPDO_TASK_PRIO                   27u
#define TPDO_TASK_STK_SIZE               256u

#define SENSOR_HANDLE_TASK_PRIO          28u
#define SENSOR_HANDLE_TASK_STK_SIZE      256u

#define DA_OUTPUT_TASK_PRIO              30u
#define DA_OUTPUT_TASK_STK_SIZE          128u

#define VALVE_CONTROL_TASK_PRIO          34u //for debug
#define VALVE_CONTROL_TASK_STK_SIZE      256u


#endif /* SLAVE_BOARD_2 */


/*################################################*/
/* SLAVE BOARD 3 MACRO DEFINITION                 */
/*################################################*/
#if defined(SLAVE_BOARD_3)
#define SLAVE_BOARD3_TASK_PRIO           22u
#define SLAVE_BOARD3_TASK_STK_SIZE       512u

#define MODBUS_ANALYSE_TASK_PRIO         21u
#define MODBUS_ANALYSE_TASK_STK_SIZE     256u

#define PUMP_CONTROL_TASK_PRIO           24u
#define PUMP_CONTROL_TASK_STK_SIZE       256u

#define DC_MOTOR_TASK_PRIO               25u
#define DC_MOTOR_TASK_STK_SIZE           256u

#define DC_MOTOR_SEND_TASK_PRIO          26u
#define DC_MOTOR_SEND_TASK_STK_SIZE      256u

#define TPDO_TASK_PRIO                   27u
#define TPDO_TASK_STK_SIZE               256u

#define SENSOR_HANDLE_TASK_PRIO          28u
#define SENSOR_HANDLE_TASK_STK_SIZE      256u

#define CENTRIFUGE_CONTROL_TASK_PRIO      23u  
#define CENTRIFUGE_CONTROL_TASK_STK_SIZE  256u

#define CASSETTE_CONTROL_TASK_PRIO        29u
#define CASSETTE_CONTROL_TASK_STK_SIZE    256u

#define CENTRIFUGE_MONITOR_TASK_PRIO      30u
#define CENTRIFUGE_MONITOR_TASK_STK_SIZE  256u

#define MAGNET_CONTROL_TASK_PRIO          31u
#define MAGNET_CONTROL_TASK_STK_SIZE      128u


#endif /* SLAVE_BOARD_3 */




//==============================================
// define interrupt priority here...
//----------------------------------------------
// NVIC_PriorityGroup_2
// 2 bits for pre-emption priority
// 2 bits for subpriority
//==============================================
#define INPUT_CAPTURE_PREEPRIOROTY      0x03
#define INPUT_CAPTURE_SUBPRIOROTY       0x03

#define PWM_CAPTURE_PREEPRIOROTY        0x03
#define PWM_CAPTURE_SUBPRIOROTY         0x03

#define UART1_PREEPRIOROTY           0x02
#define UART1_SUBPRIOROTY            0x01

#define UART2_PREEPRIOROTY           0x02
#define UART2_SUBPRIOROTY            0x01

#define UART3_PREEPRIOROTY           0x02
#define UART3_SUBPRIOROTY            0x01

#define UART4_PREEPRIOROTY           0x02
#define UART4_SUBPRIOROTY            0x01

#define UART5_PREEPRIOROTY           0x02
#define UART5_SUBPRIOROTY            0x01

#define UART6_PREEPRIOROTY           0x02
#define UART6_SUBPRIOROTY            0x01

#define DMA1_STREAM0_PRE_PRI         0x01
#define DMA1_STREAM0_SUB_PRI         0x01

#define DMA1_STREAM1_PRE_PRI         0x01
#define DMA1_STREAM1_SUB_PRI         0x01

#define DMA1_STREAM2_PRE_PRI         0x01
#define DMA1_STREAM2_SUB_PRI         0x01

#define DMA1_STREAM3_PRE_PRI         0x01
#define DMA1_STREAM3_SUB_PRI         0x01

#define DMA1_STREAM4_PRE_PRI         0x01
#define DMA1_STREAM4_SUB_PRI         0x01

#define DMA1_STREAM5_PRE_PRI         0x01
#define DMA1_STREAM5_SUB_PRI         0x01

#define DMA1_STREAM6_PRE_PRI         0x01
#define DMA1_STREAM6_SUB_PRI         0x01

#define DMA1_STREAM7_PRE_PRI         0x01
#define DMA1_STREAM7_SUB_PRI         0x01

#define DMA2_STREAM0_PRE_PRI         0x01
#define DMA2_STREAM0_SUB_PRI         0x01

#define DMA2_STREAM1_PRE_PRI         0x01
#define DMA2_STREAM1_SUB_PRI         0x01

#define DMA2_STREAM2_PRE_PRI         0x01
#define DMA2_STREAM2_SUB_PRI         0x01

#define DMA2_STREAM3_PRE_PRI         0x01
#define DMA2_STREAM3_SUB_PRI         0x01

#define DMA2_STREAM4_PRE_PRI         0x01
#define DMA2_STREAM4_SUB_PRI         0x01

#define DMA2_STREAM5_PRE_PRI         0x01
#define DMA2_STREAM5_SUB_PRI         0x01

#define DMA2_STREAM6_PRE_PRI         0x01
#define DMA2_STREAM6_SUB_PRI         0x01

#define DMA2_STREAM7_PRE_PRI         0x01
#define DMA2_STREAM7_SUB_PRI         0x01

#define USER_TMR_PRE_PRI             0X01
#define USER_TMR_SUB_PRI             0x02

#define AC_BUBBL_PRE_PRI             0x02
#define AC_BUBBL_SUB_PRI             0x02

#define LIQUIDLEVEL_HIGH_PRE_PRI     0x02
#define LIQUIDLEVEL_HIGH_SUB_PRI     0x03

#define LIQUIDLEVEL_LOW_PRE_PRI      0x02
#define LIQUIDLEVEL_LOW_SUB_PRI      0x03

#define ACPUMP_HALL_PRE_PRI          0x03
#define ACPUMP_HALL_SUB_PRI          0X02

#define RETRANSFUSIONPUMP_HALL_PRE_PRI  0x03
#define RETRANSFUSIONPUMP_HALL_SUB_PRI  0X02

#define PLASMAPUMP_HALL_PRE_PRI      0X03     
#define PLASMAPUMP_HALL_SUB_PRI      0X02

#define PLTPUMP_HALL_PRE_PRI         0x03
#define PLTPUMP_HALL_SUB_PRI         0x02

#define DRAWBLOODPUMP_HALL_PRE_PRI   0x03
#define DRAWBLOODPUMP_HALL_SUB_PRI   0x02

#define PLTVALVE_OPTICAL2_PRE_PRI    0x03
#define PLTVALVE_OPTICAL2_SUB_PRI    0x01

#define PLAVALVE_OPTICAL1_PRE_PRI    0x03
#define PLAVALVE_OPTICAL1_SUB_PRI    0x01

#define PLAVALVE_OPTICAL2_PRE_PRI    0x03
#define PLAVALVE_OPTICAL2_SUB_PRI    0x01

#define PLAVALVE_OPTICAL3_PRE_PRI    0x03
#define PLAVALVE_OPTICAL3_SUB_PRI    0x01

#define RBCVALVE_OPTICAL1_PRE_PRI    0x03
#define RBCVALVE_OPTICAL1_SUB_PRI    0x01

#define RBCVALVE_OPTICAL2_PRE_PRI    0x03
#define RBCVALVE_OPTICAL2_SUB_PRI    0x01

#define RBCVALVE_OPTICAL3_PRE_PRI    0x03
#define RBCVALVE_OPTICAL3_SUB_PRI    0x01

#define PLTVALVE_OPTICAL3_PRE_PRI    0x03
#define PLTVALVE_OPTICAL3_SUB_PRI    0x01

#define PLTVALVE_OPTICAL1_PRE_PRI    0x03
#define PLTVALVE_OPTICAL1_SUB_PRI    0x01

#define KASEDO_OPTICAL1_PRE_PRI      0x03
#define KASEDO_OPTICAL1_SUB_PRI      0X01

#define KASEDO_OPTICAL2_PRE_PRI      0x03
#define KASEDO_OPTICAL2_SUB_PRI      0X01


#if defined(CONTROL_MAIN_BOARD)
#define STOP_KEY_PRE_PRI            0x01
#define STOP_KEY_SUB_PRI            0x01

#define PAUSE_KEY_PRE_PRI           0x01
#define PAUSE_KEY_SUB_PRI           0x02



#endif


#endif /* __APP_CFG_H__ */

