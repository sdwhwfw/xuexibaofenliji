/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_valve_control.h
 * Author             : su
 * Date First Issued  : 2013/11/26
 * Description        : This file contains the software implementation for the
 *                      valve control unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/26 | v1.0  | su         | initial released
 *******************************************************************************/

#ifndef _APP_VALVE_CONTROL_H_
#define _APP_VALVE_CONTROL_H_

#include "stm32f2xx.h"
#include "app_slave_board2.h"
#include "string.h"
#include "pwm.h"

/*
 *PLT            0X1000
 *PLA            0X2000
 *RBC            0X4000
 *START          0X0100
 *_FROM_LEFT     0X0010
 *_FROM_MIDDLE   0X0020
 *_FROM_RIGHT    0X0040
 *_FROM_UNKNOW   0X0080
 *_TO_LEFT       0X0001
 *_TO_MIDDLE     0X0002
 *_TO_RIGHT      0X0004
 *
 */

/*PLT valve */
#define PLT_START_FROM_LEFT_TO_MIDDLE    0X1112
#define PLT_START_FROM_LEFT_TO_RIGHT     0X1114
#define PLT_START_FROM_MIDDLE_TO_LEFT    0X1121
#define PLT_START_FROM_MIDDLE_TO_RIGHT   0X1124
#define PLT_START_FROM_RIGHT_TO_MIDDLE   0X1142
#define PLT_START_FROM_RIGHT_TO_LEFT     0x1141
#define PLT_START_FROM_UNKNOW_TO_LEFT    0X1181
#define PLT_START_FROM_UNKNOW_TO_MIDDLE  0X1182
#define PLT_START_FROM_UNKNOW_TO_RIGHT   0X1184


/*PLA valve*/
#define PLA_START_FROM_LEFT_TO_MIDDLE    0X2112
#define PLA_START_FROM_LEFT_TO_RIGHT     0X2114
#define PLA_START_FROM_MIDDLE_TO_LEFT    0X2121
#define PLA_START_FROM_MIDDLE_TO_RIGHT   0X2124
#define PLA_START_FROM_RIGHT_TO_MIDDLE   0X2142
#define PLA_START_FROM_RIGHT_TO_LEFT     0X2141
#define PLA_START_FROM_UNKNOW_TO_LEFT    0X2181
#define PLA_START_FROM_UNKNOW_TO_MIDDLE  0X2182
#define PLA_START_FROM_UNKNOW_TO_RIGHT   0X2184

/*RBC valve */
#define RBC_START_FROM_LEFT_TO_MIDDLE    0X4112
#define RBC_START_FROM_LEFT_TO_RIGHT     0X4114
#define RBC_START_FROM_MIDDLE_TO_LEFT    0X4121
#define RBC_START_FROM_MIDDLE_TO_RIGHT   0X4124
#define RBC_START_FROM_RIGHT_TO_MIDDLE   0X4142
#define RBC_START_FROM_RIGHT_TO_LEFT     0X4141
#define RBC_START_FROM_UNKNOW_TO_LEFT    0X4181
#define RBC_START_FROM_UNKNOW_TO_MIDDLE  0X4182
#define RBC_START_FROM_UNKNOW_TO_RIGHT   0X4184


#define VALVE_TIMEOUT                0X64

typedef enum
{
    PLAVALVE,//0 xuejiang
    PLTVALVE,//  1  xuexiaoban
    RBCVALVE,//   2
} ValveType;

#define IS_VALVE_AND_CASSETTE_TYPE(TYPE)  (((TYPE) == PLAVALVE) || \
                                        ((TYPE) == PLTVALVE) || \
                                        ((TYPE) == RBCVALVE))


typedef enum
{
    forward,
    reverse
} ValveDirection;

/*init the value control task*/
void init_valve_control_task(void);

/*
 * @brief: start valves
 * @param: x_valve, select which valve to start
 *                  PLAVALVE
 *                  PLTVALVE
 *                  RBCVALVE
 * @param: direction select forward or reverse
 *         forward  0
 *         reverse  1
 * @retval: void
 */

/*start value*/
void start_valve(ValveType x_valve,ValveDirection direction);

/*
 * @brief: stop valves
 * @param: valve, select which valve to stop
 *                  PLAVALVE
 *                  PLTVALVE
 *                  RBCVALVE
 * @retval: void
 */

/*stop value*/
void stop_valve(ValveType valve);


#endif /* _APP_VALVE_CONTROL_H_ */

