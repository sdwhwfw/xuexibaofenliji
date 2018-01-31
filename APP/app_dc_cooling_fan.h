/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_dc_cooling_fan.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/11/13
 * Description        : This file contains the software implementation for the
 *                      DC cooling fan unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/11/13 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#ifndef _APP_DC_COOLING_FAN_H_
#define _APP_DC_COOLING_FAN_H_

#include "stm32f2xx.h"


typedef enum
{
    DC_COOLING_FAN_1,
    DC_COOLING_FAN_2,
    DC_COOLING_FAN_3,
    DC_COOLING_FAN_4,
    DC_COOLING_FAN_NUM,
}dc_cooling_fan_type;


#define IS_DC_COOLING_FAN_NUM_TYPE(TYPE)    (((TYPE) == DC_COOLING_FAN_1) || \
                                            ((TYPE) == DC_COOLING_FAN_2) || \
                                            ((TYPE) == DC_COOLING_FAN_3) || \
                                            ((TYPE) == DC_COOLING_FAN_4))

void init_dc_cooling_fan(void);
u32 get_dc_cooling_fan_speed(dc_cooling_fan_type index);







#endif /* _APP_DC_COOLING_FAN_H_ */

