/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_control_board_usart.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/17
 * Description        : This file contains the define of code struct(usart)
 *                      in ARM0.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/16 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _STRUCT_CONTROL_BOARD_USART_H_
#define _STRUCT_CONTROL_BOARD_USART_H_

#include "stdint.h"

/************ usart common struct ***********/
// UART Frame
typedef struct UART_FRAME_STRU
{
    uint8_t module_flag;    /* byte0 */
    uint8_t event_type;     /* byte1 */
    uint8_t param1;         /* byte2 */
    uint8_t param2;         /* byte3 */
    uint8_t param3;         /* byte4 */    
    uint8_t param4;         /* byte5 */
    uint8_t param5;         /* byte6 */
    uint8_t param6;         /* byte7 */
} UART_FRAME;


/************* usart send ******************/
// Power-on test
typedef __packed struct POWERON_TEST_STRU
{
    uint8_t cover_ret:1;            /* 0 machine cover test result:0-fail,1-success */
    uint8_t weep_ret:1;             /* 1 weeping test */
    uint8_t machinePC_ret:1;        /* 2 PC test */
    uint8_t machine_power_ret:1;    /* 3 macihine power test */
    uint8_t power_control_ret:1;    /* 4 power control test */
    uint8_t slot_pos_ret:1;         /* 5 slot position test */
    uint8_t valve_ret:1;            /* 6 valve test */
} POWERON_TEST;

// cycle back param ( pressure and collected volume) in runtime
typedef struct UART_CYCLE_BACK_STRU
{
    uint16_t draw_speed;        /* current draw speed unit:mL/min */
    uint16_t back_speed;        /* current back speed uint:mL/min */
    uint16_t ac_speed;          /* current ac speed uint:mL/min */
    uint16_t draw_pressure;     /* Runtime draw pressure unit:mmHg*/
    uint16_t back_pressure;     /* Runtime feedback pressure unit:-mmHg*/
    uint16_t plt_volume;        /* collected plt volume unit:mL */
    uint16_t plasma_volume;     /* collected plasma volume unit:mL */
    uint16_t rbc_volume;        /* collected rbc volume unit:mL */
} UART_CYCLE_BACK;

// run final result
typedef struct UART_FINAL_BACK_STRU
{
    uint16_t ac_volume;         /* use total ac volume */
    uint16_t draw_volume;       /* draw total volume */
    uint16_t plt_volume;        /* take actually plt volume */
    uint16_t plasma_volume;     /* take actually plasma volume */
    uint16_t rbc_volume;        /* take actually rbc volume */
    uint16_t saline_volume;     /* use total saline volume */
} UART_FINAL_BACK;

typedef struct UART_ERROR_CODE_STRU
{
    uint8_t flow_ret;           /* 0x61 always */
    uint8_t event_type;         /* 0x06 always */
    uint8_t error_type;         /* 0x01:advice infor,0x02:alert infor,0x03:alarm infor */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
} UART_ERROR_CODE;

/************* usart recv ******************/
// current program parameter from pc
typedef struct CUR_PROGRAM_PARAM_STRU
{
    uint16_t plt_pre_volume;        /* plt volume need to take unit:mL */
    uint16_t plasma_pre_volume;     /* plasma volume need to take unit:mL */
    uint16_t rbc_pre_volume;        /* rbc volume need to take unit:mL */
    uint16_t draw_max_speed;        /* max draw speed uint:0.1mL/min*/
    uint16_t back_max_speed;        /* max feedback speed 0.1mL/min*/
    uint16_t ac_max_speed;          /* max ac speed unit:0.1mL/min/L TBV*/
    uint16_t draw_pressure_limit;   /* draw pressure upper limit */
    uint16_t back_pressure_limit;   /* back pressure lower limit */
    uint16_t rate_plt;              /* Draw/AC rate when take plt unit:0.01 */
    uint16_t rate_rbc;              /* Draw/AC rate when take rbc unit:0.01 */
    uint16_t centrifuge_speed;      /* centrifuge speed */
    uint16_t plt_speed;             /* plt flow speed unit:0.1mL/min */
    uint8_t  tbv_volume;             /* tbv volume uint: 0.1L */
    uint8_t  runtime_max;           /* max run time */
    uint8_t  bool_back_switch;      /* use plasma help feedback switch */
    uint8_t  bool_speedup_switch;   /* if speed up in first ten minute */
} CUR_PROGRAM_PARAM;

#endif

