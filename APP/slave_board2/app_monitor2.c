/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_monitor2.c
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      monitor task of salve_board2
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#include "app_monitor2.h"
#include "app_slave_board2.h"
#include "app_pdo_callback2.h"
#include "datafilter_variance.h"
#include "app_task_err.h"
#include "struct_slave_board2.h"
#include "trace.h"
#include "exti_board2.h" //for debug jiulong 2014 08 15

#define PUMP_START            0x01/*±√∆Ù∂Ø*/


/*valve location*/
/*#define VALVE_LEFT            0x00
#define VALVE_MID             0x01
#define VALVE_RIGHT           0x02
*/

/*pump direction*/
#define PUMP_DIR_CLOCKWISE    0x01
#define PUMP_DIR_UNCLOCKWISE  0x02
#define PUMP_DIR_STOP         0x00

/*module_indicate*/
#define MODULE_INDICATE_PUMP   0x01
#define MODULE_INDICATE_VALVE  0x02




/*valve errcode*/
#define ERR_VALVE           0x03;/*∑ßŒª÷√¥ÌŒÛ 11*/

/*pump errcode*/
#define ERR_NORMAL          0x00 /*’˝≥£*/
#define ERR_FAILSPEED       0x01 /*…˝ÀŸ ß∞‹*/
#define ERR_OVERSPEED       0x02 /*≥¨ÀŸ*/
#define ERR_DIRECTION       0x03 /*∑ΩœÚ¥ÌŒÛ*/
#define ERR_TIMEOUT         0x04 /*≥¨ ±*/
#define ERR_SHAKE           0x05 /*∂∂∂Ø*/
#define ERR_ENCODER         0x07 /*±‡¬Î∆˜π ’œ*/


static u8 pump_dir_translate(u8 pump,PumpSet1 pump_set1);
u8 valve_location_translate(u8 valve,SensorState valve_state);



void salve2_monitor_task(void)
{
    u8 tmr_err = 0,OSflag,err;
  //  u8 test_buf[8] = {0,0x02,0x66,0,0,0,0,0};//for test little endion
  //  u8 test_buf[8] = {0x02,0x00,0x00,0x66,0,0,0,0};//for test little endion
  //u8 test_buf[8] = {0xa5,0x01,0x00,0x00,0x00,0x00,0x00,0x00};//for test flag pend

	ControlOrder control_order_compare;
	PumpSet1     pump_set1_compare;
	PumpState1   pump_state1_compare;
	SensorState  sensor_state_compare;

    DATAFILTER_VARIANCE datafilter_plasma_pump    = DATAFILTER_VARIANCE_DEFAULT;
    DATAFILTER_VARIANCE datafilter_PLT_pump       = DATAFILTER_VARIANCE_DEFAULT;


	APP_TRACE("slave2 monitor task ready...\r\n");
	//RPDO3_callback(test_buf,8);//for test
	memset(&pump_errcode,0x00,sizeof(PumpValveOrder));/*set all errcode to zero*/

    //datafilter_plasma_pump.ExceptionValve = pump_set1_compare.plasma_pump_speed_limit;/*„–÷µ 100 rpm*/
    datafilter_plasma_pump.VarianceThread = 5;  /*10 ŒÛ≤Ó °¿10*/
    datafilter_plasma_pump.init(&datafilter_plasma_pump);

    //datafilter_PLT_pump.ExceptionValve = pump_set1_compare.PLT_pump_speed_limit;/*„–÷µ 100 rpm*/
    datafilter_PLT_pump.VarianceThread = 5;  /*10 ŒÛ≤Ó °¿10*/
    datafilter_PLT_pump.init(&datafilter_PLT_pump);

    OSflag = OSFlagPend(init_order_flag,0x02,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
	assert_param(OSflag);
    APP_TRACE("monitor_task OSFlagPend \r\n");


	while (1)
	{
		memcpy(&control_order_compare,&control_order_r,sizeof(ControlOrder));
		memcpy(&sensor_state_compare,&sensor_state,sizeof(SensorState));
		memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));
		memcpy(&pump_state1_compare,&pump_state1,sizeof(PumpState1));

		/*********************monitor plasma_pump speed and direction ***********************/
		if((control_order_compare.module_indicate == MODULE_INDICATE_PUMP)
			&&(control_order_compare.order_indicate.bit.plasma_valve_or_pump == PUMP_START))
		{
            datafilter_plasma_pump.ExceptionValve = pump_set1_compare.plasma_pump_speed_limit/100;/*„–÷µ 100 rpm*/
            datafilter_plasma_pump.DataInput = pump_state1_compare.plasma_pump_current_speed/100;

           // APP_TRACE("%d / %d\r\n",datafilter_plasma_pump.ExceptionValve,datafilter_plasma_pump.DataInput );

            datafilter_plasma_pump.update(&datafilter_plasma_pump);//

			/********************check pump direction***********************/
            if(pump_state1_compare.pump_current_dir.bit.plasma_pump != pump_dir_translate(0x02,pump_set1_compare))
			{
                if(flag_tmr_plasma_pump_dir == 0)/*must be enter once*/
                {
                    flag_tmr_plasma_pump_dir = 1;
                    OSTmrStart(tmr_plasma_pump_direction,&tmr_err);
                }
			}
			/*********************check wheather fail to setspeed ***********************/
            else if(datafilter_plasma_pump.Output == 0x20)
            {
                //do something
                pump_errcode.pump_errcode1.bit.plasma_pump = ERR_FAILSPEED;
                // TODO: read hall of pump
                if(0)//if hall speed is normal && encoder speed is zero
                    pump_errcode.pump_errcode1.bit.plasma_pump = ERR_ENCODER;

            }
			/********************check wheather overspeed***********************/
            else if(datafilter_plasma_pump.Output == 0x10)
            {
                //do something
                pump_errcode.pump_errcode1.bit.plasma_pump = ERR_OVERSPEED;

            }
			/********************check wheather shake***********************/
            else if(datafilter_plasma_pump.Output == 0x30)
            {
                //do something
                pump_errcode.pump_errcode1.bit.plasma_pump = ERR_SHAKE;

            }
            else if(flag_tmr_plasma_pump_timeout != 1)/*no timeout*/
                pump_errcode.pump_errcode1.bit.plasma_pump = ERR_NORMAL;

		}

		/*********************monitor PLT_pump speed and direction ***********************/
		if((control_order_compare.module_indicate == MODULE_INDICATE_PUMP)
			&&(control_order_compare.order_indicate.bit.PLT_valve_or_pump == PUMP_START))
		{
            datafilter_PLT_pump.ExceptionValve = pump_set1_compare.PLT_pump_speed_limit/100;/*„–÷µ 100 rpm*/
            datafilter_PLT_pump.DataInput = pump_state1_compare.PLT_pump_current_speed/100;
            datafilter_PLT_pump.update(&datafilter_PLT_pump);//

			/********************check pump direction***********************/
            if(pump_state1_compare.pump_current_dir.bit.PLT_pump != pump_dir_translate(0x01,pump_set1_compare))
			{
                if(flag_tmr_PLT_pump_dir == 0)/*must be enter once*/
                {
                    flag_tmr_PLT_pump_dir = 1;
                    OSTmrStart(tmr_PLT_pump_direction,&tmr_err);
                }
			}
			/*********************check wheather fail to setspeed ***********************/
            else if(datafilter_PLT_pump.Output == 0x20)
            {
                //do something
                pump_errcode.pump_errcode1.bit.PLT_pump = ERR_FAILSPEED;
                // TODO: read hall of pump
                if(0)//if hall speed is normal && encoder speed is zero
                    pump_errcode.pump_errcode1.bit.PLT_pump = ERR_ENCODER;

            }
			/********************check wheather overspeed***********************/
            else if(datafilter_PLT_pump.Output == 0x10)
            {
                //do something
                pump_errcode.pump_errcode1.bit.PLT_pump = ERR_OVERSPEED;

            }
			/********************check wheather shake***********************/
            else if(datafilter_PLT_pump.Output == 0x30)
            {
                //do something
                pump_errcode.pump_errcode1.bit.PLT_pump = ERR_SHAKE;

            }
            else if(flag_tmr_plasma_pump_timeout != 1)/*no timeout*/
                pump_errcode.pump_errcode1.bit.PLT_pump = ERR_NORMAL;

		}


		/********************check plsama_valve location***********************/
		if(control_order_compare.module_indicate == MODULE_INDICATE_VALVE)
		{
			if(valve_location_translate(0x01,sensor_state_compare) != control_order_compare.order_indicate.bit.plasma_valve_or_pump)
    		{
                if(flag_tmr_plasma_valve_location == 0)/*must be enter once*/
                {
                    flag_tmr_plasma_valve_location = 1;
                    OSTmrStart(tmr_plasma_valve_location,&tmr_err);
                }
    		}
            else
            {
                pump_errcode.valve_errcode1.bit.plasma_valve = ERR_NORMAL;
            }
        }


		/********************check PLT_valve location***********************/
		if(control_order_compare.module_indicate == MODULE_INDICATE_VALVE)
		{
			if(valve_location_translate(0x02,sensor_state_compare) != control_order_compare.order_indicate.bit.PLT_valve_or_pump)
    		{
                if(flag_tmr_PLT_valve_location == 0)/*must be enter once*/
                {
                    flag_tmr_PLT_valve_location = 1;
                    OSTmrStart(tmr_PLT_valve_location,&tmr_err);
                }
    		}
            else
            {
                pump_errcode.valve_errcode1.bit.PLT_valve = ERR_NORMAL;
            }
        }

		/********************check RBC_valve location***********************/
		if(control_order_compare.module_indicate == MODULE_INDICATE_VALVE)
		{
			if(valve_location_translate(0x03,sensor_state_compare) != control_order_compare.order_indicate.bit.RBC_valve)
    		{
                if(flag_tmr_RBC_valve_location == 0)/*must be enter once*/
                {
                    flag_tmr_RBC_valve_location = 1;
                    OSTmrStart(tmr_RBC_valve_location,&tmr_err);
                }
    		}
            else
            {
                pump_errcode.valve_errcode1.bit.RBC_valve = ERR_NORMAL;
            }
        }

        //for test
         //send_task_errcode(MONITOR_TASK,TASK_WAR ,0xbb);

		OSTimeDlyHMSM(0, 0, 0, 20);/*10ms min frequence is 100hz*/

	}
}

/*
 * Function   : tmr_PLT_pump_timeout_callback
 * Description: call back fonction for PLT pump timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_PLT_pump_timeout_callback(OS_TMR *ptmr, void *p_arg)
{
    u8 tmr_err = 0;
    flag_tmr_PLT_pump_timeout = 1;
    pump_errcode.pump_errcode1.bit.PLT_pump = ERR_TIMEOUT;
    TRACE("[%8d] PLT_pump_timeout!\r\n",OSTimeGet());

    OSTmrStop(tmr_PLT_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}
/*
 * Function   : tmr_plasma_pump_timeout_callback
 * Description: call back fonction for PLA pump timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_plasma_pump_timeout_callback(OS_TMR *ptmr, void *p_arg)
{
    u8  tmr_err = 0;
    flag_tmr_plasma_pump_timeout = 1;
    pump_errcode.pump_errcode1.bit.plasma_pump = ERR_TIMEOUT;
    TRACE("[%8d] plasma_pump_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_plasma_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}

/*
 * Function   : tmr_plasma_pump_dir_callback
 * Description: call back fonction for PLA pump dir timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_plasma_pump_dir_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_dir_plasma_pump = 0;
    PumpSet1     pump_set1_compare;
    if(control_order_r.order_indicate.bit.plasma_valve_or_pump == PUMP_START)
    {
        current_dir_plasma_pump = pump_state1.pump_current_dir.bit.plasma_pump;
        memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));

        flag_tmr_plasma_pump_dir = 0;
        if(current_dir_plasma_pump != pump_dir_translate(0x02,pump_set1_compare))
        {
            pump_errcode.pump_errcode1.bit.plasma_pump = ERR_DIRECTION;
        }
    }
    TRACE("[%8d] plasma_pump_dir_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_plasma_pump_direction,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}
/*
 * Function   : tmr_PLT_pump_dir_callback
 * Description: call back fonction for PLT pump dir timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_PLT_pump_dir_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_dir_PLT_pump = 0;
    PumpSet1     pump_set1_compare;

    if(control_order_r.order_indicate.bit.PLT_valve_or_pump == PUMP_START)
    {
        current_dir_PLT_pump = pump_state1.pump_current_dir.bit.PLT_pump;
        memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));

        flag_tmr_PLT_pump_dir = 0;
        if(current_dir_PLT_pump != pump_dir_translate(0x01,pump_set1_compare))
        {
            pump_errcode.pump_errcode1.bit.PLT_pump = ERR_DIRECTION;
        }
    }
    TRACE("[%8d] LPT_pump_dir_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_PLT_pump_direction,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}
/*
 * Function   : tmr_plasma_valve_location_callback
 * Description: call back fonction for PLA valve location timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_plasma_valve_location_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           set_location_plasma_valve = 0;
	SensorState  sensor_state_compare;

    if(control_order_r.module_indicate == MODULE_INDICATE_VALVE)
    {
        set_location_plasma_valve = control_order_r.order_indicate.bit.plasma_valve_or_pump;
        memcpy(&sensor_state_compare,&sensor_state,sizeof(ControlOrder));

        flag_tmr_plasma_valve_location = 0;
        if(valve_location_translate(0x01,sensor_state_compare) != set_location_plasma_valve)
        {
            pump_errcode.valve_errcode1.bit.plasma_valve = ERR_VALVE;
        }
        else
        {
            pump_errcode.valve_errcode1.bit.plasma_valve = ERR_NORMAL;
        }
    }
    TRACE("[%8d] plasma_valve_location_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_plasma_valve_location,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}
/*
 * Function   : tmr_PLT_valve_location_callback
 * Description: call back fonction for PLT valve location timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_PLT_valve_location_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           set_location_PLT_valve;
	SensorState  sensor_state_compare;

    if(control_order_r.module_indicate == MODULE_INDICATE_VALVE)
    {
        set_location_PLT_valve = control_order_r.order_indicate.bit.PLT_valve_or_pump;
        memcpy(&sensor_state_compare,&sensor_state,sizeof(ControlOrder));

        flag_tmr_PLT_valve_location = 0;
        if(valve_location_translate(0x02,sensor_state_compare) != set_location_PLT_valve)
        {
            pump_errcode.valve_errcode1.bit.PLT_valve = ERR_VALVE;
        }
        else
        {
            pump_errcode.valve_errcode1.bit.PLT_valve = ERR_NORMAL;
        }
    }
    TRACE("[%8d] PLT_valve_location_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_PLT_valve_location,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}
/*
 * Function   : tmr_RBC_valve_location_callback
 * Description: call back fonction for RBC valve location timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_RBC_valve_location_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           set_location_RBC_valve = 0;
	SensorState  sensor_state_compare;

    if(control_order_r.module_indicate == MODULE_INDICATE_VALVE)
    {
        set_location_RBC_valve = control_order_r.order_indicate.bit.RBC_valve;
        memcpy(&sensor_state_compare,&sensor_state,sizeof(ControlOrder));

        flag_tmr_RBC_valve_location = 0;
        if(valve_location_translate(0x03,sensor_state_compare) != set_location_RBC_valve)
        {
            pump_errcode.valve_errcode1.bit.RBC_valve = ERR_VALVE;
        }
        else
        {
            pump_errcode.valve_errcode1.bit.RBC_valve = ERR_NORMAL;
        }
    }
    TRACE("[%8d] RBC_valve_location_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_RBC_valve_location,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}

static u8 pump_dir_translate(u8 pump,PumpSet1 pump_set1)
{
	switch(pump)
	{
	case 0x01://PLT_pump
    	if((control_order_r.module_indicate == MODULE_INDICATE_PUMP)
			&&(control_order_r.order_indicate.bit.plasma_valve_or_pump == PUMP_START))
    	{
    		if(pump_set1.pump_direction.bit.PLT_pump == 0x01)
    		{
    			return PUMP_DIR_CLOCKWISE;/*À≥ ±’Î*/
    		}
    		else
    		{
    			if(pump_set1.pump_direction.bit.PLT_pump == 0x00)
    			{
    				return PUMP_DIR_UNCLOCKWISE;/*ƒÊ ±’Î*/
    			}
    		}
    	}
        else
		{
			return PUMP_DIR_STOP;//stop
		}
		//break;
	case 0x02://plasma_pump
    	if((control_order_r.module_indicate == MODULE_INDICATE_PUMP)
		&&(control_order_r.order_indicate.bit.plasma_valve_or_pump == PUMP_START))
    	{
    		if(pump_set1.pump_direction.bit.plasma_pump == 0x01)
    		{
    			return PUMP_DIR_CLOCKWISE;/*À≥ ±’Î*/
    		}
    		else
    		{
    			if(pump_set1.pump_direction.bit.plasma_pump == 0x00)
    			{
    				return PUMP_DIR_UNCLOCKWISE;/*ƒÊ ±’Î*/
    			}

    		}
    	}
		else
		{
			return PUMP_DIR_STOP;//stop
		}
		//break;
	default:
		break;

	}
	return 0x00;

}




u8 valve_location_translate(u8 valve,SensorState valve_state)
{
    uint8_t ret;

    switch(valve)
	{
	case 0x01://plsama_valve
		if(valve_state.valve.bit.plsama_valve_left == 0x01)
		{
			ret = LEFT_VALVE;
		}
		else
		{
			if(valve_state.valve.bit.plsama_valve_mid == 0x01)
			{
				ret = MID_VALVE;
			}
			else
			{
				if(valve_state.valve.bit.plsama_valve_right == 0x01)
				{
					ret = RIGHT_VALVE;
				}
                else
					ret = UNKNOW_VALVE;
			}
		}
		break;
	case 0x02://PLT_valve
		if(valve_state.valve.bit.PLT_valve_left == 0x01)
		{
			ret = LEFT_VALVE;
		}
		else
		{
			if(valve_state.valve.bit.PLT_valve_mid == 0x01)
			{
				ret = MID_VALVE;
			}
			else
			{
				if(valve_state.valve.bit.PLT_valve_right == 0x01)
				{
					ret = RIGHT_VALVE;
				}
                else
                    ret = UNKNOW_VALVE;
			}
		}
		break;
	case 0x03://RBC_valve
		if(valve_state.valve.bit.RBC_valve_left == 0x01)
		{
			ret = LEFT_VALVE;
		}
		else
		{
			if(valve_state.valve.bit.RBC_valve_mid == 0x01)
			{
				ret = MID_VALVE;
			}
			else
			{
				if(valve_state.valve.bit.RBC_valve_right == 0x01)
				{
					ret = RIGHT_VALVE;
				}
                else
                    ret = UNKNOW_VALVE;
			}
		}
		break;
	default:
		break;

	}
	return ret;
}

/*
 * Function   : slave2_show_parameter_table
 * Description: show parameter table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave2_show_parameter_table()
{
    /*pump*/
    if(control_order_r.module_indicate == 0x01)//pump
    {
        /*PLT_pump*/
        APP_TRACE("PLT_pump order                  =  ");
        switch(control_order_r.order_indicate.bit.PLT_valve_or_pump)
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

        APP_TRACE("PLT_pump set speed              =  %3d.%-2d rpm\r\n",
                                                        pump_set1.PLT_pump_speed_limit/100,
                                                        pump_set1.PLT_pump_speed_limit%100);
        APP_TRACE("PLT_pump current speed          =  %3d.%-2d rpm\r\n",
                                                        pump_state1.PLT_pump_current_speed/100,
                                                        pump_state1.PLT_pump_current_speed%100);
        APP_TRACE("PLT_pump set distance           = %4d.%1d  r\r\n",
                                                        pump_set2.PLT_pump_need_distance/10,
                                                        pump_set2.PLT_pump_need_distance%10);
        APP_TRACE("PLT_pump current distance       = %4d.%1d  r\r\n",
                                                        pump_state2.PLT_pump_moved_distance/10,
                                                        pump_state2.PLT_pump_moved_distance%10);
        APP_TRACE("PLT_pump set direction          = ");
        switch(pump_set1.pump_direction.bit.PLT_pump)
        {
            case 0x01:/*À≥ ±’Î*/
                APP_TRACE("CLOCKWISE\r\n");
                break;
            case 0x00:/*ƒÊ ±’Î*/
                APP_TRACE("ANTICLOCKWISE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;
        }
        APP_TRACE("PLT_pump current direction      = ");
        switch(pump_state1.pump_current_dir.bit.PLT_pump)
        {
            case 0x01:/*À≥ ±’Î*/
                APP_TRACE("CLOCKWISE\r\n");
                break;
            case 0x02:/*ƒÊ ±’Î*/
                APP_TRACE("ANTICLOCKWISE\r\n");
                break;
            case 0x00:/*æ≤÷π*/
                APP_TRACE("STOP\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;

        }
        APP_TRACE("\r\n");

       /*plasma_pump*/
        APP_TRACE("plasma_pump order               =  ");
        switch(control_order_r.order_indicate.bit.plasma_valve_or_pump)
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

        APP_TRACE("plasma_pump set speed           =  %3d.%-2d rpm\r\n",
                                                        pump_set1.plasma_pump_speed_limit/100,
                                                        pump_set1.plasma_pump_speed_limit%100);
        APP_TRACE("plasma_pump current speed       =  %3d.%-2d rpm\r\n",
                                                        pump_state1.plasma_pump_current_speed/100,
                                                        pump_state1.plasma_pump_current_speed%100);
        APP_TRACE("plasma_pump set distance        = %4d.%1d  r\r\n",
                                                        pump_set2.plasma_pump_need_distance/10,
                                                        pump_set2.plasma_pump_need_distance%10);
        APP_TRACE("plasma_pump current distance    = %4d.%1d  r\r\n",
                                                        pump_state2.plasma_pump_moved_distance/10,
                                                        pump_state2.plasma_pump_moved_distance%10);
        APP_TRACE("plasma_pump set direction       = ");
        switch(pump_set1.pump_direction.bit.plasma_pump)
        {
            case 0x01:/*À≥ ±’Î*/
                APP_TRACE("CLOCKWISE\r\n");
                break;
            case 0x00:/*ƒÊ ±’Î*/
                APP_TRACE("ANTICLOCKWISE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;
        }
        APP_TRACE("plasma_pump current direction   = ");
        switch(pump_state1.pump_current_dir.bit.plasma_pump)
        {
            case 0x01:/*À≥ ±’Î*/
                APP_TRACE("CLOCKWISE\r\n");
                break;
            case 0x02:/*ƒÊ ±’Î*/
                APP_TRACE("ANTICLOCKWISE\r\n");
                break;
            case 0x00:/*æ≤÷π*/
                APP_TRACE("STOP\r\n");
                break;
            default:
                APP_TRACE("UNKNOW\r\n");
                break;

        }
        APP_TRACE("\r\n");

    }
    /*valve*/
    if(control_order_r.module_indicate == 0x02)//valve
    {
        switch(control_order_r.order_indicate.bit.PLT_valve_or_pump)
        {
            case 0x00:
                APP_TRACE("PLT_valve set location    is LEFT\r\n");
                break;
            case 0x01:
                APP_TRACE("PLT_valve set location    is MID\r\n");
                break;
            case 0x02:
                APP_TRACE("PLT_valve set location    is RIGHT\r\n");
                break;
            case 0x03:
                APP_TRACE("PLT_valve set location    is IGNORE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW!\r\n");
                break;
        }
        switch(control_order_r.order_indicate.bit.plasma_valve_or_pump)
        {
            case 0x00:
                APP_TRACE("plasma_valve set location is LEFT\r\n");
                break;
            case 0x01:
                APP_TRACE("plasma_valve set location is MID\r\n");
                break;
            case 0x02:
                APP_TRACE("plasma_valve set location is RIGHT\r\n");
                break;
            case 0x03:
                APP_TRACE("plasma_valve set location is IGNORE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW!\r\n");
                break;
        }
        switch(control_order_r.order_indicate.bit.RBC_valve)
        {
            case 0x00:
                APP_TRACE("RBC_valve set location    is LEFT\r\n");
                break;
            case 0x01:
                APP_TRACE("RBC_valve set location    is MID\r\n");
                break;
            case 0x02:
                APP_TRACE("RBC_valve set location    is RIGHT\r\n");
                break;
            case 0x03:
                APP_TRACE("RBC_valve set location    is IGNORE\r\n");
                break;
            default:
                APP_TRACE("UNKNOW!\r\n");
                break;
        }

    }
    else
    {
        APP_TRACE("control_order_r.module_indicate = 0x%x\r\n",control_order_r.module_indicate);
    }
    APP_TRACE("\r\n");

}

/*
 * Function   : slave2_show_sensor_table
 * Description: show sensor table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave2_show_sensor_table()
{
    /*RBC detector*/
    APP_TRACE("RBC_detector           = %5d \r\n",sensor_state.RBC_detector);
    APP_TRACE("\r\n");

    
    APP_TRACE("sensor_state_handle.valve.all = %x\r\n",sensor_state_handle.valve.all);//add by wq
    APP_TRACE("       sensor_state.valve.all = %x\r\n",sensor_state.valve.all);//add by wq
    APP_TRACE("arm2_sensor_status.valve.all = %x\r\n",arm2_sensor_status.valve.all);
    
    APP_TRACE("arm2_sensor_status.valve.bit.RBC_valve_left = %d\r\n",arm2_sensor_status.valve.bit.RBC_valve_left);
    APP_TRACE("arm2_sensor_status.valve.bit.RBC_valve_mid = %d\r\n",arm2_sensor_status.valve.bit.RBC_valve_mid);
    APP_TRACE("arm2_sensor_status.valve.RBC_valve_right= %d\r\n",arm2_sensor_status.valve.bit.RBC_valve_right);
    APP_TRACE("arm2_sensor_status.valve.plsama_valve_left = %d\r\n",arm2_sensor_status.valve.bit.plsama_valve_left);
    APP_TRACE("arm2_sensor_status.valve.plsama_valve_mid = %d\r\n",arm2_sensor_status.valve.bit.plsama_valve_mid);
    APP_TRACE("arm2_sensor_status.valve.plsama_valve_right = %d\r\n",arm2_sensor_status.valve.bit.plsama_valve_right);
    APP_TRACE("arm2_sensor_status.valve.PLT_valve_left = %d\r\n",arm2_sensor_status.valve.bit.PLT_valve_left);
    APP_TRACE("arm2_sensor_status.valve.PLT_valve_mid = %d\r\n",arm2_sensor_status.valve.bit.PLT_valve_mid);
    APP_TRACE("arm2_sensor_status.valve.PLT_valve_right = %d\r\n",arm2_sensor_status.valve.bit.PLT_valve_right);
    /*plsama_valve*/
    if(sensor_state.valve.bit.plsama_valve_left)
        APP_TRACE("plsama_valve_left  is triggered \r\n");
    else
        APP_TRACE("plsama_valve_left  isn't triggered \r\n");
    if(sensor_state.valve.bit.plsama_valve_mid)
        APP_TRACE("plsama_valve_mid   is triggered \r\n");
    else
        APP_TRACE("plsama_valve_mid   isn't triggered \r\n");
    if(sensor_state.valve.bit.plsama_valve_right)
        APP_TRACE("plsama_valve_right is triggered \r\n");
    else
        APP_TRACE("plsama_valve_right isn't triggered \r\n");

    /*PLT_valve*/
    if(sensor_state.valve.bit.PLT_valve_left)
        APP_TRACE("PLT_valve_left     is triggered \r\n");
    else
        APP_TRACE("PLT_valve_left     isn't triggered \r\n");
    if(sensor_state.valve.bit.PLT_valve_mid)
        APP_TRACE("PLT_valve_mid      is triggered \r\n");
    else
        APP_TRACE("PLT_valve_mid      isn't triggered \r\n");
    if(sensor_state.valve.bit.PLT_valve_right)
        APP_TRACE("PLT_valve_right    is triggered \r\n");
    else
        APP_TRACE("PLT_valve_right    isn't triggered \r\n");

    /*RBC_valve*/
    if(sensor_state.valve.bit.RBC_valve_left)
        APP_TRACE("RBC_valve_left     is triggered \r\n");
    else
        APP_TRACE("RBC_valve_left     isn't triggered \r\n");
    if(sensor_state.valve.bit.RBC_valve_mid)
        APP_TRACE("RBC_valve_mid      is triggered \r\n");
    else
        APP_TRACE("RBC_valve_mid      isn't triggered \r\n");
    if(sensor_state.valve.bit.RBC_valve_right)
        APP_TRACE("RBC_valve_right    is triggered \r\n");
    else
        APP_TRACE("RBC_valve_right    isn't triggered \r\n");
    APP_TRACE("\r\n");

    /*for hoare sensor */
    if(sensor_state.pump_init.bit.plasma_pump_init)
    {
      APP_TRACE("plasma_pump_init    is finished!\r\n");
    }
    else
    {
      APP_TRACE("plasma_pump_init    is unfinished!\r\n");
    }
    if(sensor_state.pump_init.bit.PLT_pump_init)
    {
      APP_TRACE("PLT_pump_init       is finished!\r\n");
    }
    else
    {
      APP_TRACE("PLT_pump_init       is unfinished!\r\n");
    }
    APP_TRACE("\r\n");

}

/*
 * Function   : slave2_show_error_table
 * Description: show error table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave2_show_error_table()
{
    /*err_code of plasma_pump*/
    switch(pump_errcode.pump_errcode1.bit.plasma_pump)
    {
        case 0x00:
            APP_TRACE("plasma_pump  is normal\r\n");
            break;
        case 0x01:
            APP_TRACE("plasma_pump  is failspeed\r\n");
            break;
        case 0x02:
            APP_TRACE("plasma_pump  is overspeed\r\n");
            break;
        case 0x03:
            APP_TRACE("plasma_pump  is err direction\r\n");
            break;
        case 0x04:
            APP_TRACE("plasma_pump  is timeout\r\n");
            break;
        case 0x05:
            APP_TRACE("plasma_pump  is shake\r\n");
            break;
        case 0x07:
            APP_TRACE("plasma_pump  is encoder err\r\n");
            break;
        default:
            APP_TRACE("the errcode of plasma_pump is unknow!\r\n");
            break;
    }

    /*err_code of PLT_pump*/
    switch(pump_errcode.pump_errcode1.bit.PLT_pump)
    {
        case 0x00:
            APP_TRACE("PLT_pump     is normal\r\n");
            break;
        case 0x01:
            APP_TRACE("PLT_pump     is failspeed\r\n");
            break;
        case 0x02:
            APP_TRACE("PLT_pump     is overspeed\r\n");
            break;
        case 0x03:
            APP_TRACE("PLT_pump     is err direction\r\n");
            break;
        case 0x04:
            APP_TRACE("PLT_pump     is timeout\r\n");
            break;
        case 0x05:
            APP_TRACE("PLT_pump     is shake\r\n");
            break;
        case 0x07:
            APP_TRACE("PLT_pump     is encoder err\r\n");
            break;
        default:
            APP_TRACE("the errcode of PLT_pump is unknow!\r\n");
            break;
    }

    /*err_code of plasma_valve*/
    switch(pump_errcode.valve_errcode1.bit.plasma_valve)
    {
        case 0x00:
            APP_TRACE("plasma_valve is normal\r\n");
            break;
        case 0x03:
            APP_TRACE("plasma_valve is error\r\n");
            break;
        default:
            APP_TRACE("the errcode of plasma_valve is unknow!\r\n");
            break;
    }
    /*err_code of RBC_valve*/
    switch(pump_errcode.valve_errcode1.bit.RBC_valve)
    {
        case 0x00:
            APP_TRACE("RBC_valve    is normal\r\n");
            break;
        case 0x03:
            APP_TRACE("RBC_valve    is error\r\n");
            break;
        default:
            APP_TRACE("the errcode of RBC_valve is unknow!\r\n");
            break;
    }
    /*err_code of PLT_valve*/
    switch(pump_errcode.valve_errcode1.bit.PLT_valve)
    {
        case 0x00:
            APP_TRACE("PLT_valve    is normal\r\n");
            break;
        case 0x03:
            APP_TRACE("PLT_valve    is error\r\n");
            break;
        default:
            APP_TRACE("the errcode of PLT_valve is unknow!\r\n");
            break;
    }

}


