/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_buzzer_ring.h
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
#ifndef _APP_BUZZER_RING_TASK_H_
#define _APP_BUZZER_RING_TASK_H_

#include "stm32f2xx.h"


typedef enum
{
    BUZZER_RING_TONE,               // prompt tone
    BUZZER_RING_WARNING_TONE,       // Warn tone
    BUZZER_RING_ERROR_TONE,         // Error tone
    BUZZER_RING_STOP                // Stop buzzer ring
}buzzer_ring_type;


void init_buzzer_ring_task(void);

void set_buzzer_ring_tone(buzzer_ring_type tone, u16 ring_time_length);



#endif /* _APP_BUZZER_RING_TASK_H_ */

