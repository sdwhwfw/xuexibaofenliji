/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_key_press.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/5
 * Description        : This file contains the software implementation for the
 *                      pause/resume key and stop key
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/05 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_KEY_PRESS_
#define _APP_CONTROL_KEY_PRESS_

// pause period = 10*(100ms)
#define PAUSE_PERIOD                10
#define PAUSE_ONE_MINUTE            30
#define PAUSE_THREE_MINUTE          180
#define PAUSE_TEN_MINUTE            600

#define KEY_PRESS_STOP              0x01
#define KEY_PRESS_PAUSE_RESUME      0x02

#define KEY_PRESS_DELAY_TIME        20 /* unit: ms */

/*
 * Function: control_init_key_press
 * Description: Init the key press func
 * Param: None
 * Return: None
*/
void control_init_key_press(void);

/*
 * Function: stop_pause_status
 * Description: when in pause status, the program is resume or stop
 * Param: bResume,1-RESUME,0-STOP
 * Return: None
*/
void stop_pause_status(uint32_t bResume);

#endif

