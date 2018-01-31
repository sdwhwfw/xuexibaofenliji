/*=====================================================================================
 File name:        RMP_CNTL.H

 Originator:	Digital Control Systems Group
			Texas Instruments

 Description: Header file containing constants, data type definitions, and
 function prototypes for the RMPCNTL module.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/
#ifndef _RMP_CNTL_H_
#define _RMP_CNTL_H_

#include "stm32f2xx.h"



typedef enum
{
    V_OPERATION_START           = 0,
    V_OPERATION_ACCELERATE      = 1,
    V_OPERATION_CONSTANT        = 2,
    V_OPERATION_DECELERATION    = 3,
    V_OPERATION_DONE            = 4
}velocity_operation_mode;


typedef struct
{
    // parameter
    float   k_ramp_value;

    // Variable
    u32     count;
    float   step;
}ramp_slope_t;


typedef struct
{
    // Parameter
    s32                     target_position;        // Target input, encoder pulse
    ramp_slope_t            acceleration_slope;     // acceleration velocity slope
    ramp_slope_t            deceleration_slope;     // deceleration velocity slope
    ramp_slope_t            constant_sp;            // constant velocity slope

    float                   max_speed;              // Maximum speed, pulse every second

    // Variable
    velocity_operation_mode ramp_control_state;
    u32                     count;

    // Output
    double                  SetpointValue;      // Output: Target output
} RMPCNTL;


typedef RMPCNTL *RMPCNTL_handle;


/*------------------------------------------------------------------------------
 * Prototypes for the functions in RMP_CNTL.C
 *------------------------------------------------------------------------------*/
void rmp_cntl_calc(RMPCNTL_handle);




#endif /* _RMP_CNTL_H_ */


