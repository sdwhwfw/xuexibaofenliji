/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_board2.c
 * Author             : su
 * Date First Issued  : 2013/11/24
 * Description        : This file contains the software implementation for the
 *                      sensor unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/24 | v1.0  |            | initial released, move code from app.c
 *******************************************************************************/

#include "app_sensor_board2.h"
#include "gpio.h"
#include "adc_dac.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#include "app_slave_board2.h"
#include "struct_slave_board2.h"
#include "stm32f2xx_adc.h"
#include "exti_board2.h"
#include "app_dc_motor.h"
#include "trace.h"
#include "app_da_output.h"

/* Private variables ---------------------------------------------------------*/
#define ADC1_DR_Address    ((u32)0x4001204C)
#define RBC_LEAKAGE 0x01
#define RBC_NOT_LEAKAGE 0x00
#define RBC_LEAKAGE_THRESHOLD  1250 //少量红细胞通过时的ad值
#define RBC_AIR_THRESHOLD  	   910  //空气通过时的ad值

uint16_t RBC_detector_status;
uint8_t PLT_pump_current_direction=0;
uint8_t plasma_pump_current_direction=0;
u16 PLT_pump_current_speed=0;
u16 plasma_pump_current_speed=0;
u8 arm3_recv_fun(u8 *p_data, u16 len);

static OS_STK sensor_task_stk[SENSOR_TASK_STK_SIZE];

static void sensor_task(void *p_arg);
static uint16_t read_RBC_detector_status(void);
static uint8_t read_PLT_pump_current_direction(void);
static uint8_t read_plasma_pump_current_direction(void);
static u16 read_PLT_pump_current_speed(void);
static u16 read_plasma_pump_current_speed(void);

void init_sensor_task(void)
{
    INT8U os_err;
	/* sensor task ************************************************************/

    os_err = OSTaskCreateExt((void (*)(void *)) sensor_task,
                        (void          * ) 0,
                        (OS_STK        * )&sensor_task_stk[SENSOR_TASK_STK_SIZE - 1],
                        (INT8U           ) SENSOR_TASK_PRIO,
                        (INT16U          ) SENSOR_TASK_PRIO,
                        (OS_STK        * )&sensor_task_stk[0],
                        (INT32U          ) SENSOR_TASK_STK_SIZE,
                        (void          * )0,
                        (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(SENSOR_TASK_PRIO, (INT8U *)"sensor", &os_err);
}

/*force sensor initialization*/

static void sensor_task(void *p_arg)
{
    /* Configure ADC1 Channel 15 (PC5) as analog input ****************************/
    ADC1_GPIO_CONFIG(ADC1_CHANNAL15);
    /* Configure ADC1 mode and DMA channal , ADC1,DMA2,channal0,Stream0************/
    ADC1_DMA_Config();
    /*Enables the ADC1 software start conversion. *********************************/
    ADC_SoftwareStartConv(ADC1);
    /*sensor external interrupt */
    user_timer_init(1000);
    arm2_exti_gpio_init();
    init_arm2sensor_tmr();
    APP_TRACE("enter sensor task!\r\n");

    for (;;)
    {

        /*PLT pump current direction of rotation*/
        pump_state1_handle.pump_current_dir.bit.PLT_pump = read_PLT_pump_current_direction();

        /*plasma pump current direction of rotation*/
        pump_state1_handle.pump_current_dir.bit.plasma_pump = read_plasma_pump_current_direction();

        /*PLT pump current speed*/
        pump_state1_handle.PLT_pump_current_speed = read_PLT_pump_current_speed();

        /*plasma pump current speed*/
        pump_state1_handle.plasma_pump_current_speed = read_plasma_pump_current_speed() ;

        /*PLT pump current distance*/
         switch(read_pump_distance_flag(PLT_PUMP))
        {
            case 0x01://total
                 //send total_distance and its flag
                 pump_state2_handle.PLT_pump_moved_distance = PLT_pump_total_distance;
                 pump_state2_handle.single_or_total.bit.PLT_pump = 0x01;
            break;
            case 0x00://single
                pump_state2_handle.PLT_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM1));
                pump_state2_handle.single_or_total.bit.PLT_pump = 0x00;
            break;
            case 0x03://clear total
                pump_state2_handle.PLT_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM1));
                //pump_state2_handle.single_or_total.bit.feedback_pump = 0x00;
                control_order_r.single_or_total.bit.PLT_pump = 0x00;
                PLT_pump_total_distance = 0;
            break;
            default:
                APP_TRACE("undnfined single_or_total order!\r\n");
            break;
        }




        /*plasma pump current distance*/
         switch(read_pump_distance_flag(PLASMA_PUMP))
        {
            case 0x01://total
                 //send total_distance and its flag
                 pump_state2_handle.plasma_pump_moved_distance = plasma_pump_total_distance;
                 pump_state2_handle.single_or_total.bit.plasma_pump=0x01;
            break;
            case 0x00://single
                pump_state2_handle.plasma_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM2));
                pump_state2_handle.single_or_total.bit.plasma_pump=0x00;
            break;
            case 0x03://clear total
                pump_state2_handle.plasma_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM2));
                //pump_state2_handle.single_or_total.bit.feedback_pump = 0x00;
                control_order_r.single_or_total.bit.plasma_pump = 0x00;
                plasma_pump_total_distance = 0;
            break;
            default:
                APP_TRACE("undnfined single_or_total order!\r\n");
            break;
        }

        /*red blood cell leakage detector status*/
        sensor_state_handle.RBC_detector = read_RBC_detector_status() ;
        read_optical_hall_status_arm2();
        /*RBC valve location*/
        sensor_state_handle.valve.bit.RBC_valve_left = arm2_sensor_status.valve.bit.RBC_valve_left;
        sensor_state_handle.valve.bit.RBC_valve_mid = arm2_sensor_status.valve.bit.RBC_valve_mid;
        sensor_state_handle.valve.bit.RBC_valve_right = arm2_sensor_status.valve.bit.RBC_valve_right;
        /*plsama valve location*/
        sensor_state_handle.valve.bit.plsama_valve_left = arm2_sensor_status.valve.bit.plsama_valve_left;
        sensor_state_handle.valve.bit.plsama_valve_mid = arm2_sensor_status.valve.bit.plsama_valve_mid;
        sensor_state_handle.valve.bit.plsama_valve_right = arm2_sensor_status.valve.bit.plsama_valve_right;
        /*platelet(PLT) valve location*/
        sensor_state_handle.valve.bit.PLT_valve_left = arm2_sensor_status.valve.bit.PLT_valve_left;
        sensor_state_handle.valve.bit.PLT_valve_mid = arm2_sensor_status.valve.bit.PLT_valve_mid;
        sensor_state_handle.valve.bit.PLT_valve_right = arm2_sensor_status.valve.bit.PLT_valve_right;
        /*whether the PLT pump finish init*/
        sensor_state_handle.pump_init.bit.PLT_pump_init = arm2_sensor_status.pump_init.bit.PLT_pump_init;

        /*whether the plasma pump finish init*/
        sensor_state_handle.pump_init.bit.plasma_pump_init = arm2_sensor_status.pump_init.bit.plasma_pump_init;
//      APP_TRACE("ADC1_converted_value=%4d\r\n",ADC1_converted_value);
        OSTimeDlyHMSM(0,0,1,10);

        //APP_TRACE("sensor_state_handle.valve.all = %d\r\n",sensor_state_handle.valve.all);//add by wq
    }
}

// TODO:for debug 20150619

void swap(u16 *p1, u16 *p2)
{
	int temp;
	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}
u16 get_max_three_value_average()
{
	u8 i,j;
	u16 result[50] = {0};

	memcpy((void *)result,(const void *)(&ADC1_convert_values[30]),50);
	for(i = 0;i<50;i++)
	{
		for(j = 0;j < 50-i-1;j++)
		{
			if(result[j] > result[j+1])
				swap(&result[j],&result[j+1]);
		}
	}
	//APP_TRACE("result[47] = %d,result[48] = %d,result[49] = %d\r\n",result[47],result[48],result[49]);
	return (result[49]+result[48]+result[47])/3;
}


static uint16_t read_RBC_detector_status(void)
{
	 u16 average_value = 0;
    //APP_TRACE("ADC1_convert_value = %d\r\n",ADC1_convert_value);
    //绿光时判断是否红细胞泄露，因为红光对泄露不敏感
    if(red_or_green_flag)
    {
    	average_value = get_max_three_value_average();
		//APP_TRACE("average_value = %d\r\n",average_value);
	    if( average_value < RBC_LEAKAGE_THRESHOLD)//for debug,need test
	        RBC_detector_status=RBC_LEAKAGE;
	    else
	        RBC_detector_status=RBC_NOT_LEAKAGE;
    }
    return RBC_detector_status;
}


// TODO:for debug 20150619 end

static uint8_t read_PLT_pump_current_direction(void)
{
    if(get_dc_motor_sp(MOTOR_NUM1)==0)
        PLT_pump_current_direction=0;
    else if(get_dc_motor_sp(MOTOR_NUM1)<0)
            PLT_pump_current_direction=2;//Bit76：10
        else
            PLT_pump_current_direction=1;//Bit76：01
    return  PLT_pump_current_direction;
}

static uint8_t read_plasma_pump_current_direction(void)
{
    if(get_dc_motor_sp(MOTOR_NUM2)==0)
        plasma_pump_current_direction=0;
    else if(get_dc_motor_sp(MOTOR_NUM2)<0)
            plasma_pump_current_direction=2;//Bit54：10
        else
            plasma_pump_current_direction=1;//Bit54：01
    return   plasma_pump_current_direction;
}


static u16 read_PLT_pump_current_speed(void)
{
    if(get_dc_motor_sp(MOTOR_NUM1)<0)
        PLT_pump_current_speed = (u16)(-get_dc_motor_sp(MOTOR_NUM1));
    else
        PLT_pump_current_speed = (u16)get_dc_motor_sp(MOTOR_NUM1);
    return PLT_pump_current_speed;
}

static u16 read_plasma_pump_current_speed(void)
{
    if(get_dc_motor_sp(MOTOR_NUM2)<0)
        plasma_pump_current_speed = (u16)(-get_dc_motor_sp(MOTOR_NUM2));
    else
        plasma_pump_current_speed = (u16)get_dc_motor_sp(MOTOR_NUM2);
    return plasma_pump_current_speed ;
}

