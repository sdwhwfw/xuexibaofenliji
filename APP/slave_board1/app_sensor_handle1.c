/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_handle1.c
 * Author             : WQ
 * Date First Issued  : 2013/11/19
 * Description        : This file contains the software implementation for the
 *                      sensor handle
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/19 | v1.0  | WQ         | initial released
 *******************************************************************************/

#include "app_sensor_handle1.h"
#include "trace.h"
#include "struct_slave_board1.h"
#include "app_slave_board1.h"


static OS_STK sensor_handle_task_stk[SENSOR_HANDLE_TASK_STK_SIZE];

static void sensor_handle_task(void *p_arg);


static pump_direction draw_pump_dir_handle(void);
static pump_direction AC_pump_dir_handle(void);
static pump_direction feedback_pump_dir_handle(void);
static uint16_t draw_pump_speed_handle(void);
static uint16_t AC_pump_speed_handle(void);
static uint16_t feedback_pump_speed_handle(void);

static uint16_t draw_pump_distance_handle(void);
static uint16_t AC_pump_distance_handle(void);
static uint16_t feedback_pump_distance_handle(void);





static uint16_t blood_collect_preesure_handle(void);
static AC_bubble_status AC_bubble_handle(void);
static level_status high_level_handle(void);
static level_status low_level_handle(void);
static sensor_status feedback_pump_state_handle(void);
static pump_init_status draw_pump_init_handle(void);
static pump_init_status AC_pump_init_handle(void);
static pump_init_status feedback_pump_init_handle(void);





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


    OSTmrStart(tmr_AC_bubble,&err);//AC bubble timer start

    while(1)
    {
        write_pump_direction(DRAW_PUMP,draw_pump_dir_handle());
        write_pump_direction(AC_PUMP,AC_pump_dir_handle());
        write_pump_direction(FEEDBACK_PUMP,feedback_pump_dir_handle());

        write_pump_speed(DRAW_PUMP,draw_pump_speed_handle());
        write_pump_speed(AC_PUMP,AC_pump_speed_handle());
        write_pump_speed(FEEDBACK_PUMP,feedback_pump_speed_handle());

        write_pump_moved_distance(DRAW_PUMP,draw_pump_distance_handle());
        write_pump_moved_distance(AC_PUMP,AC_pump_distance_handle());
        write_pump_moved_distance(FEEDBACK_PUMP,feedback_pump_distance_handle());

        write_blood_collect_preesure(blood_collect_preesure_handle());


        write_feedback_pump_state(feedback_pump_state_handle());
        write_AC_bubble(AC_bubble_handle());

        write_hige_level(high_level_handle());
        write_low_level(low_level_handle());

        write_draw_pump_init(draw_pump_init_handle());
        write_AC_pump_init(AC_pump_init_handle());
        write_feedback_pump_init(feedback_pump_init_handle());

        OSTimeDlyHMSM(0, 0, 0, 10);
       // APP_TRACE("sensor_handle_task!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");

    }


}


static pump_direction draw_pump_dir_handle()
{
    uint8_t ret;
    ret = pump_state1_handle.pump_current_dir.bit.draw_pump;

    return (pump_direction)ret;
}

static pump_direction AC_pump_dir_handle()
{
    uint8_t ret;
    ret = pump_state1_handle.pump_current_dir.bit.AC_pump;

    return (pump_direction)ret;
}

static pump_direction feedback_pump_dir_handle()
{
    uint8_t ret;
    ret = pump_state1_handle.pump_current_dir.bit.feed_back_pump;

    return (pump_direction)ret;
}

static uint16_t draw_pump_speed_handle()
{
    uint16_t tmp;
    tmp = pump_state1_handle.draw_pump_current_speed;

    return tmp;
}
static uint16_t AC_pump_speed_handle()
{
    uint16_t tmp;
    tmp = pump_state1_handle.AC_pump_current_speed;

    return tmp;
}

static uint16_t feedback_pump_speed_handle()
{
    uint16_t tmp;
    tmp = pump_state1_handle.feed_back_pump_current_speed;

    return tmp;
}

static uint16_t draw_pump_distance_handle()
{
    uint16_t tmp;
    tmp = pump_state2_handle.draw_pump_moved_distance;

    return tmp;
}

static uint16_t AC_pump_distance_handle()
{
    uint16_t tmp;
    tmp = pump_state2_handle.AC_pump_moved_distance;

    return tmp;
}

static uint16_t feedback_pump_distance_handle()
{
    uint16_t tmp;
    tmp = pump_state2_handle.feed_back_pump_moved_distance;

    return tmp;
}




/*
 * Function: blood_collect_preesure_handle
 * Param:  None
 * Return: preesure
*/
static uint16_t blood_collect_preesure_handle()
{
    uint16_t temp_preesure;
    temp_preesure = sensor_state_handle.blood_collect_preasure;

    return temp_preesure;
}
/*
 * Function: feedback_pump_state_handle
 * Param:  None
 * Return: 0x00:normal
           0x01:unnormal
*/
static sensor_status feedback_pump_state_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.feedback_pump_state;

    return (sensor_status)ret;
}


/*
 * Function: AC_bubble_handle
 * Param:  None
 * Return: 0x00:no bubble
           0x01:have bubble
*/
static AC_bubble_status AC_bubble_handle()
{
    uint8_t ret = 0x00;

    if(flag_tmr_AC_bubble == 0x01)//100ms
    {
        flag_tmr_AC_bubble = 0x00;
       if(sensor_state_handle.liquid_level.bit.AC_bubble == 1)
        {
            //clear
            // TODO:clear
            //clear_ac_bubble_status();
            ret = 0x01;//have bubble
        }

       //TRACE("AC_bubble_handle\r\n");
    }

    return (AC_bubble_status)ret;
}

/*
 * Function: high_level_handle
 * Param:  None
 * Return: 0x00:high_level unreach
           0x01:high_level reach
*/
static level_status high_level_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.liquid_level.bit.high_level;
    return (level_status)ret;
}
/*
 * Function: low_level_handle
 * Param:  None
 * Return: 0x00:level unreach
           0x01:level reach
*/
static level_status low_level_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.liquid_level.bit.low_level;
    return (level_status)ret;
}

/*
 * Function: draw_pump_init_handle
 * Param:  None
 * Return: 0x00:init unfinished
           0x01:init finished
*/
static pump_init_status draw_pump_init_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.pump_init.bit.draw_pump_init;
    return (pump_init_status)ret;
}
/*
 * Function: AC_pump_init_handle
 * Param:  None
 * Return: 0x00:init unfinished
           0x01:init finished
*/
static pump_init_status AC_pump_init_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.pump_init.bit.AC_pump_init;
    return (pump_init_status)ret;
}
/*
 * Function: feedback_pump_init_handle
 * Param:  None
 * Return: 0x00:init unfinished
           0x01:init finished
*/
static pump_init_status feedback_pump_init_handle()
{
    uint8_t ret;
    ret = sensor_state_handle.pump_init.bit.feed_back_pump_init;
    return (pump_init_status)ret;
}




/*
 * Function   : tmr_AC_bubble_callback
 * Description: call back fonction for AC bubble timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/


void tmr_AC_bubble_callback(OS_TMR *ptmr, void *p_arg)
{
    u8 tmr_err = 0;

    flag_tmr_AC_bubble = 0x01;

	//TRACE("AC bubble _timeout!\r\n");
    //do something
}

