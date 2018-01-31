/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_handle3.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the software implementation for the
 *                      sensor handle
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/19 | v1.0  | WQ         | initial released
 *******************************************************************************/

#include "app_sensor_handle3.h"
#include "trace.h"
#include "struct_slave_board3.h"
#include "app_slave_board3.h"

static OS_STK sensor_handle_task_stk[SENSOR_HANDLE_TASK_STK_SIZE];

static void sensor_handle_task(void *p_arg);

static motor_direction motor_dir_handle(void);
static uint16_t motor_speed_handle(void);


static sensor_status weeping_detector_handle(void);
static uint16_t centrifuge_motor_preesure_handle(void);
static door_hall_status door_hall_handle(void);
static cassette_location_status cassette_location_handle(void);
static sensor_status centrifuge_motor_state_handle(void);

static sensor_status voltage_70V_handle(void);
static sensor_status voltage_s16_handle(void);
static sensor_status voltage_s19_handle(void);
static sensor_status voltage_p12V_handle(void);
static sensor_status voltage_n12V_handle(void);
static sensor_status voltage_5V_handle(void);
static sensor_status voltage_24V_handle(void);



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
        write_motor_direction(motor_dir_handle());
        write_motor_speed(motor_speed_handle());
        write_weeping_detector(weeping_detector_handle());
        write_centrifuge_motor_preesure(centrifuge_motor_preesure_handle());
        write_door_hall(door_hall_handle());
        write_cassette_location(cassette_location_handle());
        write_centrifuge_motor_state(centrifuge_motor_state_handle());
        write_voltage_status(STATUS_70V,voltage_70V_handle());
        write_voltage_status(STATUS_24V,voltage_s16_handle());
        write_voltage_status(STATUS_S16,voltage_s19_handle());
        write_voltage_status(STATUS_S19,voltage_p12V_handle());
        write_voltage_status(STATUS_P_12,voltage_n12V_handle());
        write_voltage_status(STATUS_N_12,voltage_5V_handle());
        write_voltage_status(STATUS_5V,voltage_24V_handle());
        OSTimeDlyHMSM(0, 0, 0, 50);
       // APP_TRACE("sensor_handle_task!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    }
}

static motor_direction motor_dir_handle()
{
    uint8_t ret;
    ret = pump_state1_handle.motor_current_dir.bit.centrifuge_motor;
    return (motor_direction)ret;
}

static uint16_t motor_speed_handle()
{
    uint16_t tmp;
    tmp = pump_state1_handle.centrifuge_motor_current_speed;
    return tmp;
}


/*
 * Function: weeping_detector_handle
 * Param:  None
 * Return: 0x00:normal
           0x01:abnormal
*/
static sensor_status weeping_detector_handle()
{
    uint16_t ret;
    ret = sensor_state_handle.weeping_detector;
    return (sensor_status)ret;
}

/*
 * Function: centrifuge_motor_preesure_handle
 * Param:  None
 * Return: preesure
*/

static uint16_t centrifuge_motor_preesure_handle()
{
    uint16_t temp_preesure;
    temp_preesure = sensor_state_handle.centrifuge_motor_preasure;
    return temp_preesure;
}

/*
 * Function: door_hall_handle
 * Param:  None
 * Return: 0x00:nosignal
           0x01:signal
*/
static door_hall_status door_hall_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.sensor.bit.door_hoare;
    return (door_hall_status)ret;
}

/*
 * Function: cassette_location_handle
 * Param:  None
 * Return: 0x00:unknow
           0x01:up
           0x02:down
*/
static cassette_location_status cassette_location_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.sensor.bit.cassette_location;
    return (cassette_location_status)ret;
}

/*
 * Function: centrifuge_motor_state_handle
 * Param:  None
 * Return: 0x00:normal
           0x01:unnormal
*/
static sensor_status centrifuge_motor_state_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.centrifuge_motor_state;
    return (sensor_status)ret;
}
/*
 * Function: voltage_status_handle
 * Param:  None
 * Return: 0x00:normal
           0x01:unnormal
*/
static sensor_status voltage_70V_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_70V;
    return (sensor_status)ret;
}
static sensor_status voltage_24V_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_24V;
    return (sensor_status)ret;
}
static sensor_status voltage_s16_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_S16;
    return (sensor_status)ret;
}
static sensor_status voltage_s19_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_S19;
    return (sensor_status)ret;
}
static sensor_status voltage_p12V_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_positive_12V;
    return (sensor_status)ret;
}
static sensor_status voltage_n12V_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_negative_12V;
    return (sensor_status)ret;
}
static sensor_status voltage_5V_handle()
{
    uint8_t ret;
    ret = motor_errcode_handle.errcode_voltage.bit.volage_5V;
    return (sensor_status)ret;
}