/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : encoder.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/07
 * Description        : This file contains the software implementation for the
 *                      encoder unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/05/07 | v1.0  |            | initial released
 * 2013/05/31 | v1.1  | Bruce.zhu  | add three timer for encoder
 * 2013/11/04 | v1.2  | Bruce.zhu  | change framework to support multiple
 *                                   configurations for encoder timer
 *******************************************************************************/
#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32f2xx.h"


typedef enum {
	ENCODER_NUM1,
	ENCODER_NUM2,
	ENCODER_NUM3,
	ENCODER_NUM
} EncoderType;

#define IS_ENCODER_TYPE(TYPE) (((TYPE) == ENCODER_NUM1) || \
                                ((TYPE) == ENCODER_NUM2) || \
                                ((TYPE) == ENCODER_NUM3))

// number of pulses per revolution
#define ENCODER_PPR             (u16)(512)
//#define ENCODER_PPR             (u16)(401)



/* Exported functions ------------------------------------------------------- */
void QET_configuration(EncoderType encoder);

void Encoder1_Configuration(void);
void Encoder2_Configuration(void);
void Encoder3_Configuration(void);
void encoder_reset(EncoderType encoder);
float get_dc_motor_speed(EncoderType encoder, u16 time_interval, s32* distance);
s32 get_current_dc_motor_speed(EncoderType encoder);
u32 get_encoder_count(EncoderType encoder);




#endif /* __ENCODER_H__ */

