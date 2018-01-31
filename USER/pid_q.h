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

#ifndef __PID_Q_H__
#define __PID_Q_H__

#include "stm32f2xx.h"
#include "pid.h"


void PIDInit_Q(PIDType index);
void PIDSetPoint_Q(PIDType index, int setpoint);
void PIDSetKp_Q(PIDType index, float dKpp);
void PIDSetKi_Q(PIDType index, float dKii);
void PIDSetKd_Q(PIDType index, float dKdd);
float IncPIDCalc_Q(PIDType index, int NextPoint);









#endif /* __PID_Q_H__ */




