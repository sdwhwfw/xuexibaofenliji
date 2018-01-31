
/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cassette.h
 * Author             : su
 * Date First Issued  : 2013/11/26
 * Description        : This file contains the software implementation for the
 *                      cassette control unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/26 | v1.0  | su         | initial released
 *******************************************************************************/

#ifndef _APP_CASSETTE_H_
#define _APP_CASSETTE_H_

#include "stm32f2xx.h"
#include "app_slave_board3.h"
#include "trace.h"
#include "pwm.h"
#include "string.h"

/*
 *CASSETTE       0X8000
 *START          0X0100 
 *_FROM_UP     0X0010
 *_FROM_DOWN   0X0020
 *_FROM_UNKNOW   0X0040
 *_TO_UP       0X0001
 *_TO_DOWN     0X0002
 *
 */

/*CASSETTE*/
#define CASSETTE_START_FROM_UP_TO_DOWN          0X8112
#define CASSETTE_START_FROM_DOWN_TO_UP          0X8121
#define CASSETTE_START_FROM_UNKNOW_TO_DOWN      0X8142
#define CASSETTE_START_FROM_UNKNOW_TO_UP        0X8141

typedef enum
{
    upward,
    downward
} CassetteDirection;

/*init the value control task*/
void init_cassette_control_task(void);
/*stop cassette*/
void stop_cassette(void);
/*start cassette*/
void start_cassette(CassetteDirection direction);

#endif

