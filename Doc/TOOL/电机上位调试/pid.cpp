//
// pid.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "pid.h"


CPID::CPID()
{
	
}

CPID::~CPID()
{
	
}



double CPID::PIDCale(PID *p, double feedback)
{
	double dErr, Err;

	Err = p->Command - feedback;  //当前误差
	p->sumErr += Err;             //误差累加
	dErr = Err - p->preErr;       //误差微分
	p->preErr = Err;
	/* 比例项, 微分项, 积分项 */
	return ( p->Proportion * Err + p->Derivative * dErr + p->Integral * p->sumErr );
}


void CPID::PIDInit(PID *p)
{
	memset(p, 0x00, sizeof(PID));  //初始化
}


void CPID::motorInit(motor *m)
{
	memset(m, 0x00, sizeof(motor));
}

double CPID::motorCal(motor *m, double u)
{
	//二阶系统
	double y = 1.9753 * m->lastY - 0.9753 * m->preY + 0.00003284 * u + 0.00006568 * m->lastU + 0.00003284 * m->preU;

	m->preY  = m->lastY;
	m->lastY = y;
	m->preU  = m->lastU;
	m->lastU = u;
	return y;
}