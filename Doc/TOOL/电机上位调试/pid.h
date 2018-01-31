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



typedef struct PID{
	double Command;     //����ָ��
	double Proportion;  //����ϵ��
	double Integral;    //����ϵ��
	double Derivative;  //΢��ϵ��
	double preErr;      //ǰһ�����
	double sumErr;      //����ۻ�
}PID;

typedef struct motor{
	double lastY;
	double preY;
	double lastU;
	double preU;
}motor;


class CPID
{
public:
	CPID();
	~CPID();
	double PIDCale(PID *p, double feedback);
	void PIDInit(PID *p);
	void motorInit(motor *m);
	double motorCal(motor *m, double u);
};









#endif /* __PID_H__ */


