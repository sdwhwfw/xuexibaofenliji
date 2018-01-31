/*=====================================================================================
 File name:        RMP_CNTL.C

 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Ramp control

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "rmp_cntl.h"
#include "trace.h"


void rmp_cntl_calc(RMPCNTL *v)
{
    float mini_step;

    switch (v->ramp_control_state)
    {
        case V_OPERATION_START:
            v->SetpointValue = 0.0;
            v->count = 0;
            break;

        case V_OPERATION_ACCELERATE:                    // acceleration stage
            if (v->count < v->acceleration_slope.count)
            {
                mini_step = (2*v->count++ + 1) * v->acceleration_slope.step/2.0;
                v->SetpointValue += mini_step;

                if (v->count + 1 >= v->acceleration_slope.count)
                {
                    v->count = 0;
                    if (v->constant_sp.count)
                    {
                        v->ramp_control_state = V_OPERATION_CONSTANT;
                    }
                    else
                    {
                        v->ramp_control_state = V_OPERATION_DECELERATION;
                    }
                }
            }
            else
            {
                v->count = 0;
                if (v->constant_sp.count)
                {
                    v->ramp_control_state = V_OPERATION_CONSTANT;
                }
                else
                {
                    v->ramp_control_state = V_OPERATION_DECELERATION;
                }
            }
            break;

        case V_OPERATION_CONSTANT:                      // constant speed stage
            if (v->count++ < v->constant_sp.count)
            {
                v->SetpointValue += v->constant_sp.step;
                //TRACE("C[%d] step=%d,SetpointValue=%d\r\n", v->count - 1, (s32)(v->constant_sp.step), (s32)(v->SetpointValue));
                if (v->count + 1 >= v->constant_sp.count)
                {
                    v->count = 0;
                    v->ramp_control_state = V_OPERATION_DECELERATION;
                }
            }
            else
            {
                v->count = 0;
                v->ramp_control_state = V_OPERATION_DECELERATION;
            }
            break;

        case V_OPERATION_DECELERATION:                  // deceleration stage
            if (v->count < v->deceleration_slope.count)
            {
                mini_step = v->constant_sp.step + (2*v->count++ + 1) * v->deceleration_slope.step/2.0;
                v->SetpointValue += mini_step;
                //TRACE("D mini_step=%d,SetpointValue=%d\r\n", (s32)mini_step, (s32)(v->SetpointValue));
                if (v->count + 1 >= v->deceleration_slope.count)
                {
                    v->ramp_control_state = V_OPERATION_DONE;
                }
            }
            else
            {
                v->ramp_control_state = V_OPERATION_DONE;
            }
            break;

        case V_OPERATION_DONE:
            v->count = 0;
            break;

        default:
            break;
    }

}






