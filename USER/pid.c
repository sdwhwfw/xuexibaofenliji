/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : pid.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/14
 * Description        : This file contains the software implementation for the
 *                      pid unit
 *******************************************************************************
 * History:
 * 2013/05/14 v1.0
 * 2013/05/31 V1.1 add 3 pid support
 *******************************************************************************/

#include "pid.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct PID
{
    float   SetPoint;           // �趨Ŀ�� Desired Value
    float   SumError;			// ����ۼ�

    float   Proportion;         // �������� Proportional Const
    float   Integral;           // ���ֳ��� Integral Const
    float   Derivative;         // ΢�ֳ��� Derivative Const

    float   LastError;          // Error[-1]
    float   PrevError;          // Error[-2]
} PID;


/* Private variables ---------------------------------------------------------*/
static PID	 sPID[PID_NUM];
static PID	 *sptr = sPID;



//=============================================================
// ----Function: IncPIDInit()
// ------Syntax: void IncPIDInit(void);
// -Description: Initialization PID parameter.
// -------Notes:
// --parameters: none
// -----returns: none
//=============================================================
void PIDInit(PIDType index)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    sptr[index].LastError  = 0;			//Error[-1]
    sptr[index].PrevError  = 0;			//Error[-2]
    sptr[index].Proportion = 0;			//�������� Proportional Const
    sptr[index].Integral   = 0;			//���ֳ��� Integral Const
    sptr[index].Derivative = 0;			//΢�ֳ��� Derivative Const
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
void PIDSetPoint(PIDType index, float setpoint)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    sptr[index].SetPoint = setpoint;
}

int PIDGetSetpoint(PIDType index)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    return(sptr[index].SetPoint);
}

//=============================================================
// ----Function: IncPIDKp()
// ------Syntax: void IncPIDKp(double dKp);
// -Description: Set PID Proportion coefficient
// -------Notes:
// --parameters: Proportion Const
// -----returns: none
//=============================================================
void PIDSetKp(PIDType index, float dKpp)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    sptr[index].Proportion = dKpp;
}

//===================================//
// Get Proportion
//===================================//
float PIDGetKp(PIDType index)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    return(sptr[index].Proportion);
}

//=============================================================
// ----Function: IncPIDKi()
// ------Syntax: void IncPIDKi(double dKi);
// -Description: Set PID Integral coefficient
// -------Notes:
// --parameters: Integral Const
// -----returns: none
//=============================================================
void PIDSetKi(PIDType index, float dKii)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    sptr[index].Integral = dKii;
}

//===================================//
// Get Integral
//===================================//
float PIDGetKi(PIDType index)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    return(sptr[index].Integral);
}

//=============================================================
// ----Function: IncPIDKd()
// ------Syntax: void IncPIDKd(double dKd);
// -Description: Set PID Derivative coefficient
// -------Notes:
// --parameters: Derivative Const
// -----returns: none
//=============================================================
void PIDSetKd(PIDType index, float dKdd)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    sptr[index].Derivative = dKdd;
}

//===================================//
// Get Derivative
//===================================//
float PIDGetKd(PIDType index)
{
    assert_param(IS_PID_NUMBER_TYPE(index));

    return(sptr[index].Derivative);
}


//=============================================================
// ----Function: IncPIDCalc()
// ------Syntax: int IncPIDCalc(unsigned int NextPoint);
// -Description: Increment Digital PID calculate
// -------Notes: Basic Increment Digital PID
// --parameters: Next Point
// -----returns: increase controls parameter
//=============================================================
float IncPIDCalc(PIDType index, float NextPoint)
{
    register float iError, iIncpid;

    iError = sptr[index].SetPoint - NextPoint;

    iIncpid = sptr[index].Proportion * iError                   //E[0]
              - sptr[index].Integral   * sptr[index].LastError  //E[-1]
              + sptr[index].Derivative * sptr[index].PrevError; //E[-2]

    sptr[index].PrevError = sptr[index].LastError;
    sptr[index].LastError = iError;

    return(iIncpid);
}


//=============================================================
// ----Function: LocPIDCalc()
// ------Syntax: unsigned int locPIDCalc(unsigned int NextPoint);
// -Description: Location Digital PID calculate
// -------Notes: Basic Location Digital PID
// --parameters: Next Point
// -----returns: Location controls parameter
//=============================================================
float LocPIDCalc(PIDType index, int NextPoint)
{
    float iError, dError;

    iError                = sptr[index].SetPoint - NextPoint;      	    // ƫ��
    sptr[index].SumError += iError;                                     // ����
    dError                = iError - sptr[index].LastError;             // ��ǰ΢��
    sptr[index].LastError = iError;

    return(sptr[index].Proportion * iError           	                // ������
         + sptr[index].Integral * sptr[index].SumError                  // ������
         + sptr[index].Derivative * dError);
}


//=============================================//
//	*END*
//=============================================//




