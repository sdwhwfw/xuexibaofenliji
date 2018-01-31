/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_monitor3.c
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      monitor task of salve_board3
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#include "app_monitor3.h"
#include "app_slave_board3.h"
#include "app_pdo_callback3.h"
#include "datafilter_variance.h"
#include "app_task_err.h"

#include "trace.h"

#define PUMP_START            0x01/*±ÃÆô¶¯*/


/*motor direction*/
#define MOTOR_DIR_CLOCKWISE    0x01
#define MOTOR_DIR_UNCLOCKWISE  0x02
#define MOTOR_DIR_STOP         0x00

/*module_indicate*/
#define MODULE_INDICATE_MOTOR   0x01
#define MODULE_INDICATE_MAGNET  0x02

/*motor errcode*/
#define ERR_NORMAL          0x00 /*Õý³£*/
#define ERR_FAILSPEED       0x01 /*ÉýËÙÊ§°Ü*/
#define ERR_OVERSPEED       0x02 /*³¬ËÙ*/
#define ERR_DIRECTION       0x03 /*·½Ïò´íÎó*/
#define ERR_TIMEOUT         0x04 /*³¬Ê±*/
#define ERR_SHAKE           0x05 /*¶¶¶¯*/
#define ERR_ENCODER         0x07 /*±àÂëÆ÷¹ÊÕÏ*/

#define ERR_CASSETTE_LOCATION 0x01;/*¿¨Ï»Î»ÖÃ´íÎó*/


static u8 motor_dir_translate(u8 motor,PumpSet1 pump_set1);



void salve3_monitor_task(void)
{
    u8 tmr_err = 0,OSflag,err;
   // u8 test_buf[8] = {0,0x01,0x66,0,0,0,0,0};//for test little endion
  //  u8 test_buf[8] = {0x01,0x0,0x00,0x66,0,0,0,0};//for test big endion
  // u8 test_buf[8] = {0xa5,0x01,0x00,0x00,0x00,0x00,0x00,0x00};//for test flag pend

	ControlOrder control_order_compare;
	PumpSet1     pump_set1_compare;
	PumpState1   pump_state1_compare;
    SensorState sensor_state_compare;

    DATAFILTER_VARIANCE datafilter_centrifuge_motor     = DATAFILTER_VARIANCE_DEFAULT;
    DATAFILTER_VARIANCE datafilter_cassette_motor       = DATAFILTER_VARIANCE_DEFAULT;

	APP_TRACE("slave3 monitor task ready...\r\n");

    //RPDO3_callback(test_buf,8);//for test
	memset(&motor_errcode,0x00,sizeof(PumpValveOrder));/*set all errcode to zero*/

    datafilter_centrifuge_motor.ExceptionValve = pump_set1_compare.centrifuge_motor_speed_limit;/*ãÐÖµ 100 rpm*/
    datafilter_centrifuge_motor.VarianceThread = 5;  /*Îó²î ¡À5*/
    datafilter_centrifuge_motor.init(&datafilter_centrifuge_motor);

    datafilter_cassette_motor.ExceptionValve = pump_set1_compare.cassette_motor_speed_limit;/*ãÐÖµ 100 rpm*/
    datafilter_cassette_motor.VarianceThread = 5;  /*Îó²î ¡À5*/
    datafilter_cassette_motor.init(&datafilter_cassette_motor);

    OSflag = OSFlagPend(init_order_flag,0x02,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
	assert_param(OSflag);
    APP_TRACE("monitor_task OSFlagPend \r\n");

	while (1)
	{
		memcpy(&control_order_compare,&control_order_r,sizeof(ControlOrder));
		memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));
		memcpy(&pump_state1_compare,&pump_state1,sizeof(PumpState1));
		memcpy(&sensor_state_compare,&sensor_state,sizeof(SensorState));

		/*********************monitor centrifuge_motor speed and direction ***********************/
		if((control_order_compare.module_indicate == MODULE_INDICATE_MOTOR)
			&&(control_order_compare.order_indicate.bit.centrifugeo_or_electromagnet == PUMP_START))

		{

            datafilter_centrifuge_motor.DataInput = pump_state1_compare.centrifuge_motor_current_speed;
            datafilter_centrifuge_motor.update(&datafilter_centrifuge_motor);//

			/********************check centrifuge_motor direction***********************/
			if(pump_state1_compare.motor_current_dir.bit.centrifuge_motor != motor_dir_translate(0x01,pump_set1_compare))
			{
                if(flag_tmr_centrifuge_motor_dir == 0)/*must be enter once*/
                {
                    flag_tmr_centrifuge_motor_dir = 1;
                    OSTmrStart(tmr_centrifuge_motor_direction,&tmr_err);
                }
			}

			/*********************check wheather fail to setspeed ***********************/
            else if(datafilter_centrifuge_motor.Output == 0x20)
            {
                //do something
                motor_errcode.motor_errcode1.bit.centrifuge_motor = ERR_FAILSPEED;

            }
			/********************check wheather overspeed***********************/
            else if(datafilter_centrifuge_motor.Output == 0x10)
            {
                //do something
                motor_errcode.motor_errcode1.bit.centrifuge_motor = ERR_OVERSPEED;

            }
            /********************check wheather shake***********************/
            else if(datafilter_centrifuge_motor.Output == 0x30)
            {
                //do something
                motor_errcode.motor_errcode1.bit.centrifuge_motor = ERR_SHAKE;

            }
            else  /*normal*/
                motor_errcode.motor_errcode1.bit.centrifuge_motor = ERR_NORMAL;

		}

		/********************check  cassette location***********************/
		if(control_order_compare.module_indicate == MODULE_INDICATE_MOTOR)
		{
            if(control_order_compare.order_indicate.bit.cassette_motor != 0x03)//no ignore
            {
                if(control_order_compare.order_indicate.bit.cassette_motor != \
                    sensor_state_compare.sensor.bit.cassette_location)
        		{
                    if(flag_tmr_cassette_location == 0)/*must be enter once*/
                    {
                        flag_tmr_cassette_location = 1;
                        OSTmrStart(tmr_cassette_location,&tmr_err);
                    }
        		}
                else
                {
                    motor_errcode.motor_errcode1.bit.cassette_location = ERR_NORMAL;
                }
            }

        }

		/*********************monitor magnet  ***********************/
		if(control_order_compare.module_indicate == MODULE_INDICATE_MAGNET)//magnet
		{

		}
        //for test
        // send_task_errcode(MONITOR_TASK,TASK_WAR ,0xcc);

		OSTimeDlyHMSM(0, 0, 0, 20);/*10ms min frequence is 100hz*/
		// do something
		//RPDO3_callback(test_buf,8);//for test

	}
}

/*
 * Function   : tmr_centrifuge_motor_dir_callback
 * Description: call back fonction for centrifuge_motor dir timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_centrifuge_motor_dir_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_dir_centrifuge_motor = 0;
    PumpSet1     pump_set1_compare;

    current_dir_centrifuge_motor = pump_state1.motor_current_dir.bit.centrifuge_motor;
    memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));

    flag_tmr_centrifuge_motor_dir = 0;
    if(current_dir_centrifuge_motor != motor_dir_translate(0x01,pump_set1_compare))
    {
        motor_errcode.motor_errcode1.bit.centrifuge_motor = ERR_DIRECTION;
    }

	TRACE("[%8d] centrifuge_motor_dir_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_centrifuge_motor_direction,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}
/*
 * Function   : tmr_cassette_location_callback
 * Description: call back fonction for cassette_location timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_cassette_location_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_cassette_location = 0;
    ControlOrder     control_order_compare;

    current_cassette_location = sensor_state.sensor.bit.cassette_location;
    memcpy(&control_order_compare,&control_order_r,sizeof(ControlOrder));

    flag_tmr_cassette_location = 0;
    if(current_cassette_location != control_order_compare.order_indicate.bit.cassette_motor)
    {
        motor_errcode.motor_errcode1.bit.cassette_location = ERR_CASSETTE_LOCATION;
    }
	TRACE("[%8d] cassette_location_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_cassette_location,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}


static u8 motor_dir_translate(u8 motor,PumpSet1 pump_set1)
{
	switch(motor)
	{
	case 0x01://centrifuge_motor
		if(pump_set1.motor_direction.bit.centrifuge_motor == 0x01)
		{
			return MOTOR_DIR_CLOCKWISE;/*Ë³Ê±Õë*/
		}
		else
		{
			if(pump_set1.motor_direction.bit.centrifuge_motor == 0x00)
			{
				return MOTOR_DIR_UNCLOCKWISE;/*ÄæÊ±Õë*/
			}
			else
			{
				return MOTOR_DIR_STOP;//stop
			}
		}
		//break;
	case 0x02://cassette_motor
		if(pump_set1.motor_direction.bit.cassette_motor == 0x01)
		{
			return MOTOR_DIR_CLOCKWISE;/*Ë³Ê±Õë*/
		}
		else
		{
			if(pump_set1.motor_direction.bit.cassette_motor == 0x00)
			{
				return MOTOR_DIR_UNCLOCKWISE;/*ÄæÊ±Õë*/
			}
			else
			{
				return MOTOR_DIR_STOP;//stop
			}
		}

		//break;
	default:
		break;

	}
	return 0x00;

}


/*
 * Function   : slave3_show_parameter_table
 * Description: show parameter table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave3_show_parameter_table()
{
    /*motor*/
   if(control_order_r.module_indicate == 0x01)//motor
    {
        /*centrifuge_motor*/
        APP_TRACE("centrifuge_motor order                  =  ");
        switch(control_order_r.order_indicate.bit.centrifugeo_or_electromagnet)
        {
            case 0x01://start
                APP_TRACE("START\r\n");
                break;
            case 0x00://stop
                APP_TRACE("STOP\r\n");
                break;
            case 0x02://init
                APP_TRACE("INIT\r\n");
                break;
            case 0x03://ignore
                APP_TRACE("IGNORE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;
        }

        APP_TRACE("centrifuge_motor set speed              = %4d.%1d  rpm\r\n",
                                                            pump_set1.centrifuge_motor_speed_limit/10,
                                                            pump_set1.centrifuge_motor_speed_limit%10);
        APP_TRACE("centrifuge_motor current speed          = %4d.%1d  rpm\r\n",
                                                            pump_state1.centrifuge_motor_current_speed/10,
                                                            pump_state1.centrifuge_motor_current_speed%10);
        APP_TRACE("centrifuge_motor set distance           = %4d.%1d  r\r\n",
                                                            pump_set2.centrifuge_motor_need_distance/10,
                                                            pump_set2.centrifuge_motor_need_distance%10);
        APP_TRACE("centrifuge_motor current distance       = %4d.%1d  r\r\n",
                                                            pump_state2.centrifuge_motor_moved_distance/10,
                                                            pump_state2.centrifuge_motor_moved_distance%10);
        APP_TRACE("centrifuge_motor set direction          = ");
        switch(pump_set1.motor_direction.bit.centrifuge_motor)
        {
            case 0x01:/*Ë³Ê±Õë*/
                APP_TRACE("CLOCKWISE\r\n");
                break;
            case 0x00:/*ÄæÊ±Õë*/
                APP_TRACE("ANTICLOCKWISE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;
        }
        APP_TRACE("centrifuge_motor current direction       = ");
        switch(pump_state1.motor_current_dir.bit.centrifuge_motor)
        {
            case 0x01:/*Ë³Ê±Õë*/
                APP_TRACE("CLOCKWISE\r\n");
                break;
            case 0x02:/*ÄæÊ±Õë*/
                APP_TRACE("ANTICLOCKWISE\r\n");
                break;
            case 0x00:/*¾²Ö¹*/
                APP_TRACE("STOP\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;

        }
        APP_TRACE("\r\n");
    }


}

/*
 * Function   : slave3_show_sensor_table
 * Description: show sensor table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave3_show_sensor_table()
{
    u8  sign;
    u16 pressure;

    /*RBC weeping_detector*/
    APP_TRACE("weeping_detector           = %5d \r\n",sensor_state.weeping_detector);
    APP_TRACE("\r\n");

    /*for blood_collect_preasure */
    sign     = (sensor_state.centrifuge_motor_preasure>>13)&0x01;
    pressure = (sensor_state.centrifuge_motor_preasure&0x1fff); //12:0

    if(sign == 0x00)//+
    {
      APP_TRACE("centrifuge_motor_preasure  = +%4d mmHG\r\n",pressure);
    }
    else
    {
      APP_TRACE("centrifuge_motor_preasure  = -%4d mmHG\r\n",pressure);
    }

    /*door sensor*/




}


/*
 * Function   : slave3_show_error_table
 * Description: show error table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave3_show_error_table()
{
    /*err_code of centrifuge_motor*/
    switch(motor_errcode.motor_errcode1.bit.centrifuge_motor)
    {
        case 0x00:
            APP_TRACE("centrifuge_motor is normal\r\n");
            break;
        case 0x01:
            APP_TRACE("centrifuge_motor is failspeed\r\n");
            break;
        case 0x02:
            APP_TRACE("centrifuge_motor is overspeed\r\n");
            break;
        case 0x03:
            APP_TRACE("centrifuge_motor is err direction\r\n");
            break;
        case 0x04:
            APP_TRACE("centrifuge_motor is timeout\r\n");
            break;
        default:
            APP_TRACE("the errcode of centrifuge_motor is unknow!\r\n");
            break;
    }

    /*err_code of cassette_motor*/
    switch(motor_errcode.motor_errcode1.bit.cassette_location)
    {
        case 0x00:
            APP_TRACE("cassette_location  is right\r\n");
            break;
        case 0x01:
            APP_TRACE("cassette_location  is err\r\n");
            break;
        default:
            APP_TRACE("the errcode of cassette_location is unknow!\r\n");
            break;
    }

}


