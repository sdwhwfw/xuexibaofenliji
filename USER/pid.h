/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : pid.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/14
 * Description        : This file contains the software implementation for the
 *                      pid unit
 *******************************************************************************
 * History:
 * 2013/05/14 v1.0
 *******************************************************************************/

#ifndef __PID_H__
#define __PID_H__

#include "stm32f2xx.h"

typedef enum {
	PID_NUM1,
	PID_NUM2,
	PID_NUM3,
	PID_NUM
} PIDType;


#define IS_PID_NUMBER_TYPE(TYPE) (((TYPE) == PID_NUM1) || \
                                  ((TYPE) == PID_NUM2) || \
                                  ((TYPE) == PID_NUM3))


void PIDInit(PIDType index);

void PIDSetPoint(PIDType index, float setpoint);
int PIDGetSetpoint(PIDType index);

void PIDSetKp(PIDType index, float dKpp);
float PIDGetKp(PIDType index);

void PIDSetKi(PIDType index, float dKii);
float PIDGetKi(PIDType index);

void PIDSetKd(PIDType index, float dKdd);
float PIDGetKd(PIDType index);

float IncPIDCalc(PIDType index, float NextPoint);
float LocPIDCalc(PIDType index, int NextPoint);



#endif /* __PID_H__ */

