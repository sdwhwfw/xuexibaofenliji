/*=====================================================================================
 File name:        PID_REG3.C

 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  PID controller with anti-windup

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "pid_reg3.h"

void pid_reg3_calc(PIDREG3 *v)
{
    // Compute the error
    v->Err = v->Ref - v->Fdb;

    // Compute the proportional output
    v->Up = v->Kp*v->Err;

    // Compute the integral output
    v->Ui = v->Ui + v->Ki*v->Up + v->Kc*v->SatErr;

    // Compute the derivative output
    v->Ud = v->Kd*(v->Up - v->Up1);

    // Compute the pre-saturated output
    v->OutPreSat = v->Up + v->Ui + v->Ud;

#if 0
    // Saturate the output
    if (v->OutPreSat > v->OutMax)
        v->Out =  v->OutMax;
    else if (v->OutPreSat < v->OutMin)
        v->Out =  v->OutMin;
    else
        v->Out = v->OutPreSat;

    // Compute the saturate difference
    v->SatErr = v->Out - v->OutPreSat;
#else
    v->Out = v->OutPreSat;
#endif

    // Update the previous proportional output
    v->Up1 = v->Up;
}


