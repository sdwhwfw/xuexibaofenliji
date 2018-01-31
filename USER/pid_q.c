/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : pid_q.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/12/20
 * Description        : This file contains the software implementation for the
 *                      pid IQ format unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/12/20 | v1.0  | Bruce.zhu  | initial released, move code from pid.c
 *******************************************************************************/

#include "pid_q.h"
#include "IQmathLib.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct q_PID
{
	_iq  SetPoint; 			//  设定目标 Desired Value
	_iq  SumError;			//	误差累计

	_iq  Proportion;        //  比例常数 Proportional Const
	_iq  Integral;          //  积分常数 Integral Const
	_iq  Derivative;        //  微分常数 Derivative Const

	_iq  LastError;         //  Error[-1]
	_iq  PrevError;         //  Error[-2]
} q_PID;


/* Private variables ---------------------------------------------------------*/
static q_PID    sPID[PID_NUM];
static q_PID	*sptr = sPID;

//============================================================= 
// ----Function: IncPIDInit() 
// ------Syntax: void IncPIDInit(void);
// -Description: Initialization PID parameter.
// -------Notes: 
// --parameters: none
// -----returns: none
//=============================================================
void PIDInit_Q(PIDType index)
{
    sptr[index].LastError  = 0;			//Error[-1]
    sptr[index].PrevError  = 0;			//Error[-2]
	sptr[index].Proportion = 0;			//比例常数 Proportional Const
    sptr[index].Integral   = 0;			//积分常数 Integral Const
    sptr[index].Derivative = 0;			//微分常数 Derivative Const
    sptr[index].SetPoint   = 0;
	sptr[index].SumError   = 0;
}


//============================================================= 
// ----Function: IncPIDSetPoint() 
// ------Syntax: void IncPIDSetPoint(unsigned int setpoint);
// -Description: Set PID Desired Value
// -------Notes: 
// --parameters: Desired Value
// -----returns: none
//=============================================================
void PIDSetPoint_Q(PIDType index, int setpoint)
{
	sptr[index].SetPoint = _IQ(setpoint);
}


//============================================================= 
// ----Function: IncPIDKp() 
// ------Syntax: void IncPIDKp(double dKp);
// -Description: Set PID Proportion coefficient
// -------Notes: 
// --parameters: Proportion Const
// -----returns: none
//=============================================================
void PIDSetKp_Q(PIDType index, float dKpp)
{
	sptr[index].Proportion = _IQ(dKpp);
}


//============================================================= 
// ----Function: IncPIDKi() 
// ------Syntax: void IncPIDKi(double dKi);
// -Description: Set PID Integral coefficient
// -------Notes: 
// --parameters: Integral Const
// -----returns: none
//=============================================================
void PIDSetKi_Q(PIDType index, float dKii)
{
	sptr[index].Integral = _IQ(dKii);
}


//============================================================= 
// ----Function: IncPIDKd() 
// ------Syntax: void IncPIDKd(double dKd);
// -Description: Set PID Derivative coefficient
// -------Notes: 
// --parameters: Derivative Const
// -----returns: none
//=============================================================
void PIDSetKd_Q(PIDType index, float dKdd)
{
	sptr[index].Derivative = _IQ(dKdd);
}


//============================================================= 
// ----Function: IncPIDCalc() 
// ------Syntax: int IncPIDCalc(unsigned int NextPoint);
// -Description: Increment Digital PID calculate
// -------Notes: Basic Increment Digital PID
// --parameters: Next Point
// -----returns: increase controls parameter
//=============================================================
float IncPIDCalc_Q(PIDType index, int NextPoint)
{
	register _iq iError, iIncpid;

	iError = sptr[index].SetPoint - _IQ(NextPoint);

	iIncpid = _IQmpy(sptr[index].Proportion, iError)                    //E[0]
            - _IQmpy(sptr[index].Integral, sptr[index].LastError)       //E[-1]
            + _IQmpy(sptr[index].Derivative, sptr[index].PrevError);    //E[-2]

	sptr[index].PrevError = sptr[index].LastError;
	sptr[index].LastError = iError;

	return(_IQtoF(iIncpid));
}


//=============================================//
//	*END*
//=============================================//




