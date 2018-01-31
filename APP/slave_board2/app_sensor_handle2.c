/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_handle2.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the software implementation for the
 *                      sensor handle
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/19 | v1.0  | WQ         | initial released
 *******************************************************************************/

#include "app_sensor_handle2.h"
#include "trace.h"
#include "struct_slave_board2.h"
#include "app_slave_board2.h"

static OS_STK sensor_handle_task_stk[SENSOR_HANDLE_TASK_STK_SIZE];

static void sensor_handle_task(void *p_arg);



static pump_direction PLT_pump_dir_handle(void);
static pump_direction plasma_pump_dir_handle(void);

static uint16_t PLT_pump_speed_handle(void);
static uint16_t plasma_pump_speed_handle(void);

static uint16_t PLT_pump_distance_handle(void);
static uint16_t plasma_pump_distance_handle(void);



static RBC_detector RBC_detector_handle(void);
static valve_location_type RBC_valve_handle(void);
static valve_location_type plasma_valve_handle(void);
static valve_location_type PLT_valve_handle(void);
static pump_init_status PLT_pump_init_handle(void);
static pump_init_status plasma_pump_init_handle(void);


void init_sensor_handle_task()
{
    INT8U os_err;

	/* pump control task ***************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) sensor_handle_task,
							(void		   * ) 0,
							(OS_STK 	   * )& sensor_handle_task_stk[SENSOR_HANDLE_TASK_STK_SIZE - 1],
							(INT8U			 ) SENSOR_HANDLE_TASK_PRIO,
							(INT16U 		 ) SENSOR_HANDLE_TASK_PRIO,
							(OS_STK 	   * )& sensor_handle_task_stk[0],
							(INT32U 		 ) SENSOR_HANDLE_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(SENSOR_HANDLE_TASK_PRIO, (INT8U *)"SENSOR_HANDLE", &os_err);

}

static void sensor_handle_task(void *p_arg)
{
    uint8_t OSflag,err;

    APP_TRACE("sensor_handle_task!\r\n");

    OSflag = OSFlagPend(init_order_flag,0x01,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
	assert_param(OSflag);
    APP_TRACE("sensor_handle_task OSFlagPend \r\n");

    while(1)
    {
        write_pump_direction(PLT_PUMP,PLT_pump_dir_handle());
        write_pump_direction(PLASMA_PUMP,plasma_pump_dir_handle());

        write_pump_speed(PLT_PUMP,PLT_pump_speed_handle());
        write_pump_speed(PLASMA_PUMP,plasma_pump_speed_handle());

        write_pump_moved_distance(PLT_PUMP,PLT_pump_distance_handle());
        write_pump_moved_distance(PLASMA_PUMP,plasma_pump_distance_handle());


        write_RBC_detector(RBC_detector_handle());

        write_valve_location(RBC_VALVE,RBC_valve_handle());
        write_valve_location(PLASMA_VALVE,plasma_valve_handle());
        write_valve_location(PLT_VALVE,PLT_valve_handle());

        write_PLT_pump_init(PLT_pump_init_handle());
        write_plasma_pump_init(plasma_pump_init_handle());

        OSTimeDlyHMSM(0, 0, 0, 50);
        //APP_TRACE("sensor_handle_task!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    }
}


static pump_direction PLT_pump_dir_handle()
{
    uint8_t ret;
    ret = pump_state1_handle.pump_current_dir.bit.PLT_pump;

    return (pump_direction)ret;
}

static pump_direction plasma_pump_dir_handle()
{
    uint8_t ret;
    ret = pump_state1_handle.pump_current_dir.bit.plasma_pump;

    return (pump_direction)ret;
}

static uint16_t PLT_pump_speed_handle()
{
    uint16_t tmp;
    tmp = pump_state1_handle.PLT_pump_current_speed;

    return tmp;
}

static uint16_t plasma_pump_speed_handle()
{
    uint16_t tmp;
    tmp = pump_state1_handle.plasma_pump_current_speed;

    return tmp;
}

static uint16_t PLT_pump_distance_handle()
{
    uint16_t tmp;
    tmp = pump_state2_handle.PLT_pump_moved_distance;

    return tmp;
}

static uint16_t plasma_pump_distance_handle()
{
    uint16_t tmp;
    tmp = pump_state2_handle.plasma_pump_moved_distance;

    return tmp;
}






/*
 * Function: RBC_detector_handle
 * Param:  None
 * Return: 0x00:unoverflow
           0x01:overflow
*/
static RBC_detector RBC_detector_handle()
{
    uint16_t detector;
    detector = sensor_state_handle.RBC_detector;

    return (RBC_detector)detector;
}


/*
 * Function: RBC_valve_handle
 * Param:  None
 * Return: 0x00:LEFT_VALVE
           0x01:MID_VALVE
           0x02:RIGHT_VALVE
*/
static valve_location_type RBC_valve_handle()
{
    valve_location_type location ;
    if(sensor_state_handle.valve.bit.RBC_valve_left == 0x01)
    {
        location = LEFT_VALVE;//0x00
    }
    else if(sensor_state_handle.valve.bit.RBC_valve_mid == 0x01)
    {
        location = MID_VALVE;//0x01
    }
    else if(sensor_state_handle.valve.bit.RBC_valve_right == 0x01)
    {
        location = RIGHT_VALVE;//0x02
    }
    else
        location = UNKNOW_VALVE;
    return (valve_location_type)location;
}
/*
 * Function: plasma_valve_handle
 * Param:  None
 * Return: 0x00:LEFT_VALVE
           0x01:MID_VALVE
           0x02:RIGHT_VALVE
*/
static valve_location_type plasma_valve_handle()
{
    valve_location_type location;
    if(sensor_state_handle.valve.bit.plsama_valve_left == 0x01)
    {
        location = LEFT_VALVE;//0x00
    }
    else if(sensor_state_handle.valve.bit.plsama_valve_mid == 0x01)
    {
        location = MID_VALVE;
    }
    else if(sensor_state_handle.valve.bit.plsama_valve_right == 0x01)
    {
        location = RIGHT_VALVE;
    }
    else
        location = UNKNOW_VALVE;

    return location;
}

/*
 * Function: PLT_valve_handle
 * Param:  None
 * Return: 0x00:LEFT_VALVE
           0x01:MID_VALVE
           0x02:RIGHT_VALVE
*/
static valve_location_type PLT_valve_handle()
{
    valve_location_type location;
    if(sensor_state_handle.valve.bit.PLT_valve_left == 0x01)
    {
        location = LEFT_VALVE;
    }
    else if(sensor_state_handle.valve.bit.PLT_valve_mid == 0x01)
    {
        location = MID_VALVE;
    }
    else if(sensor_state_handle.valve.bit.PLT_valve_right == 0x01)
    {
        location = RIGHT_VALVE;
    }
    else
        location = UNKNOW_VALVE;

    return location;
}

/*
 * Function: PLT_pump_init_handle
 * Param:  None
 * Return: 0x00:init unfinished
           0x01:init finished
*/
static pump_init_status PLT_pump_init_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.pump_init.bit.PLT_pump_init;
    return (pump_init_status)ret;
}

/*
 * Function: plasma_pump_init_handle
 * Param:  None
 * Return: 0x00:init unfinished
           0x01:init finished
*/
static pump_init_status plasma_pump_init_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.pump_init.bit.plasma_pump_init;
    return (pump_init_status)ret;
}

