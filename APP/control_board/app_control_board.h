/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_board.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/26
 * Description        : This file contains the software implementation for the
 *                      control board task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/26 | v1.0  | Bruce.zhu  | initial released
 * 2013/10/18 | v1.1  | ZhangQ     | modify, add event and internal status
 *******************************************************************************/

#ifndef _APP_CONTROL_BOARD_H_
#define _APP_CONTROL_BOARD_H_

#include "stm32f2xx.h"
#include "struct_control_board_canopen.h"
#include "struct_control_board_usart.h"
#include "string.h"

#ifndef _APP_CONTROL_BOARD_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

#define SLAVE_BORAD_NUM             0X03
#define CONTROL_RPDO_SIZE           0X08

#define FLAG_ACK_BIT                0x01
#define FLAG_NAK_BIT                0x02

// ring separating belt press indicate blood is full of the belt
//#define SEPARATE_BELT_FULL      1000u
#define SEPARATE_BELT_FULL      100u  //change by Big.Man

// ring separating belt press indicate blood is empty
//#define SEPARATE_BELT_EMPTY     10u

#define SEPARATE_BELT_EMPTY     20u //change by Big.Man




// define the feedback container's volume
#define FEEDBACK_CONTAINER_VOLUME   50u     /* 50mL */

void init_control_board_task(void);

/************** USART Event *************************/
GLOBAL OS_EVENT *       g_uart_poweron_test_event;      /* Press Poweron Test event */
GLOBAL OS_EVENT *       g_uart_suit_load_event;         /* Press Load suit event */
GLOBAL OS_EVENT *       g_uart_begin_suit_test_event;   /* Press test suit event */
GLOBAL OS_EVENT *       g_uart_ac_priming_event;        /* Press AC priming event */
GLOBAL OS_EVENT *       g_uart_connect_donor_event;     /* Press connect to donor */
GLOBAL OS_EVENT *       g_uart_start_collect_event;     /* Press start collect */
GLOBAL OS_EVENT *       g_uart_reset_machine_event;     /* Press reset machine */
GLOBAL OS_EVENT *       g_uart_stop_run_event;          /* Press stop run */
GLOBAL OS_EVENT *       g_uart_pause_run_event;         /* uart pause program reserved */
GLOBAL OS_EVENT *       g_uart_continue_run_event;      /* Press continue program */
GLOBAL OS_EVENT *       g_uart_mute_event;              /* Press Mute */
GLOBAL OS_EVENT *       g_uart_unload_event;            /* Press unload the canal */
GLOBAL OS_EVENT *       g_uart_show_result;             /* Press show final result */

GLOBAL OS_FLAG_GRP *    g_uart_acknak_flag;


/************** CANOpen Event **********************/
GLOBAL OS_EVENT *       g_canopen_centri_full_event;    /* centrifuge belt full */
GLOBAL OS_EVENT *       g_canopen_centri_empty_event;   /* centrifuge belt empty */
GLOBAL OS_EVENT *       g_canopen_upper_liquid_event;   /* upper liquid ok */
GLOBAL OS_EVENT *       g_canopen_lower_liquid_event;   /* lower liquid ok */


GLOBAL OS_FLAG_GRP *    g_arm1_acknak_flag;
GLOBAL OS_FLAG_GRP *    g_arm2_acknak_flag;
GLOBAL OS_FLAG_GRP *    g_arm3_acknak_flag;

/************** RUN Status ********************/
typedef	enum RUN_STATUS_ENUM
{
    STATUS_STOP,                         /* 0 STOP status */
    STATUS_PAUSE_IN_ONE,                 /* 1 PAUSE in one minute */
    STATUS_PAUSE_ONE_THREE,              /* 2 PASUE between one to three minutes */
    STATUS_PAUSE_THREE_TEN,              /* 3 PAUSE betwen three to ten minutes */
    STATUS_PAUSE_AFTER_TEN,              /* 4 PAUSE after ten minutes */
    STATUS_COMM_OK,                      /* 5 communication status ok */
    STATUS_POWERON,                      /* 6 POWERON test status */
    STATUS_SUIT_LOADING,                 /* 7 load canal suit */
    STATUS_SUIT_CHECKING,                /* 8 suit check */
    STATUS_AC_PRIMING,                   /* 9 ac priming */
    STATUS_BLOOD_PRIMING,                /* 10 blood priming */
    STATUS_COLLECT_DRAW,                 /* 11 blood collect when draw */
    STATUS_COLLECT_BACK,                 /* 12 blood collect when back */ 
    STATUS_PLATELET_COLLECTING,          /* 13 plasma collect */
    STATUS_BLOOD_FEEDBACK,               /* 14 feedback status */
    STATUS_CANAL_UNLOADING,              /* 15 unload canal suit */
    STATUS_UNEXCEPTION = 0xEE           /*  16 unexception status */
} RUN_STATUS;

GLOBAL __IO RUN_STATUS          g_run_status;
GLOBAL __IO RUN_STATUS          g_pause_status;

GLOBAL OS_TMR*                  g_tmr_pause;
GLOBAL OS_EVENT *               g_feedback_event;

// TODO: check the 5 pressure status
typedef enum CANAL_TEST_PRESSURE_EUM
{
    TEST_HIGH_PRESSURE = 310,   /* positive pressure */
    TEST_FIRST_PRESSURE = 200,  /* positive pressure */
    TEST_SECOND_PRESSURE = 100, /* positive pressure */
    TEST_LOW_PRESSURE = 250,    /* negative pressure */
    //TEST_LOW_PRESSURE = 80+100,    /* negative pressure */
    TEST_NORMAL_PRESSURE = 0,    /* zero pressure */
}CANAL_TEST_PRESSURE_EUM;

GLOBAL OS_EVENT *       g_test_canal_pressure_event;

GLOBAL OS_EVENT *       g_feedback_pressure_immediate;      /* send pressure to IPC event */


// communication state
typedef __packed struct COMM_SYNC_STATE_STRU
{
    uint8_t uart:1;
    uint8_t arm1:1;
    uint8_t arm2:1;
    uint8_t arm3:1;
    uint8_t reserved:4;
} COMM_SYNC_STATE;
GLOBAL COMM_SYNC_STATE      g_comm_sync_state;

typedef enum CENTRI_SPEED_ENUM
{
    CENTRI_SLOW_SPEED   = 1000,     /* 1000rpm */
    CENTRI_SPEED_ONE = 200,         /* 200rpm */
    CENTRI_SPEED_TWO = 2000,        /* 2000rpm */
    CENTRI_NORMAL_SPEED = 3000      /* 3000rpm */
} CENTRI_SPEED;
GLOBAL uint16_t             g_centri_speed;

// The ratio flow speed parse to rotation speed, mL/r
typedef __packed struct RATIO_FLOW_TO_ROTATION_STRU
{
    float ratio_ac;
    float ratio_draw;
    float ratio_feedback;
    float ratio_pla;
    float ratio_plt;
} RATIO_FLOW_TO_ROTATION;
GLOBAL RATIO_FLOW_TO_ROTATION       g_ratio_parse;

/************** RPDO Table ********************/
GLOBAL ARM1_RPDO1       g_arm1_rpdo1_data;
GLOBAL ARM1_RPDO2       g_arm1_rpdo2_data;
GLOBAL ARM1_RPDO3       g_arm1_rpdo3_data;
GLOBAL ARM1_RPDO6       g_arm1_rpdo6_data;
GLOBAL ARM2_RPDO1       g_arm2_rpdo1_data;
GLOBAL ARM2_RPDO2       g_arm2_rpdo2_data;
GLOBAL ARM2_RPDO3       g_arm2_rpdo3_data;
GLOBAL ARM2_RPDO6       g_arm2_rpdo6_data;
GLOBAL ARM3_RPDO1       g_arm3_rpdo1_data;
GLOBAL ARM3_RPDO2       g_arm3_rpdo2_data;
GLOBAL ARM3_RPDO3       g_arm3_rpdo3_data;
GLOBAL ARM3_RPDO6       g_arm3_rpdo6_data;

/************** Uart Table ********************/
// send data
GLOBAL POWERON_TEST         g_poweron_status;
GLOBAL UART_CYCLE_BACK      g_uart_cycle_back;
GLOBAL UART_FINAL_BACK      g_uart_final_back;
GLOBAL UART_ERROR_CODE      g_uart_error_code;
// recv data
GLOBAL CUR_PROGRAM_PARAM    g_cur_program_param;

// distance pump need to turn
typedef struct 
{
    uint16_t ac_distance;
    uint16_t draw_distance;
    uint16_t feedBack_distance;
    uint16_t plt_distance;
    uint16_t pla_distance;
} TOTAL_DISTANCE_RETURN;


GLOBAL volatile TOTAL_DISTANCE_RETURN g_total_distance_return;
GLOBAL volatile TOTAL_DISTANCE_RETURN g_total_distance_calc;



void control_board_task(void *p_arg);


#undef GLOBAL

#endif /* _APP_CONTROL_BOARD_H_ */


