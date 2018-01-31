/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_monitor1.c
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      monitor task of salve_board1
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/
#include "app_monitor1.h"
#include "app_slave_board1.h"
#include "app_pdo_callback1.h"
#include "datafilter_variance.h"
#include "app_task_err.h"
#include "trace.h"


#define PUMP_START            0x01/*±ÃÆô¶¯*/

/*pump direction*/
#define PUMP_DIR_CLOCKWISE    0x01
#define PUMP_DIR_UNCLOCKWISE  0x02
#define PUMP_DIR_STOP         0x00

/*pump errcode*/
#define ERR_NORMAL          0x00 /*Õý³£*/
#define ERR_FAILSPEED       0x01 /*ÉýËÙÊ§°Ü*/
#define ERR_OVERSPEED       0x02 /*³¬ËÙ*/
#define ERR_DIRECTION       0x03 /*·½Ïò´íÎó*/
#define ERR_TIMEOUT         0x04 /*³¬Ê±*/
#define ERR_SHAKE           0x05 /*¶¶¶¯*/
#define ERR_ENCODER         0x07 /*±àÂëÆ÷¹ÊÕÏ*/




static u8 pump_dir_translate(u8 pump,PumpSet1 pump_set1);



void salve1_monitor_task(void)
{
    u8 tmr_err,OSflag,err;
 //   u8 test_buf[8] = {0,0x01,0x66,0,0,0,0,0};//for test little endion
  //  u8 test_buf[8] = {0x01,0x00,0x00,0x66,0,0,0x01,0x02};//for test big endion
 //  u8 test_buf[8] = {0xa5,0x01,0x00,0x00,0x00,0x00,0x00,0x00};//for test flag pend

	ControlOrder control_order_compare;
	PumpSet1     pump_set1_compare;
	PumpState1   pump_state1_compare;


    DATAFILTER_VARIANCE datafilter_draw_pump     = DATAFILTER_VARIANCE_DEFAULT;

    DATAFILTER_VARIANCE datafilter_AC_pump       = DATAFILTER_VARIANCE_DEFAULT;

    DATAFILTER_VARIANCE datafilter_feedback_pump = DATAFILTER_VARIANCE_DEFAULT;

    //need delay some time wait other task running to judge
   // OSTimeDlyHMSM(0, 0, 1, 0);

	APP_TRACE("slave1 monitor task ready...\r\n");
	memset(&pump_errcode,0x00,sizeof(PumpValveOrder));/*set all errcode to zero*/

	//RPDO3_callback(test_buf,8);//for test



  //  datafilter_draw_pump.ExceptionValve = pump_set1_compare.draw_pump_speed_limit;/*ãÐÖµ 100 rpm*/
    datafilter_draw_pump.VarianceThread = 5;  /*10 Îó²î ¡À10*/
    datafilter_draw_pump.init(&datafilter_draw_pump);

   // datafilter_AC_pump.ExceptionValve = pump_set1_compare.AC_pump_speed_limit;/*ãÐÖµ 100 rpm*/
    datafilter_AC_pump.VarianceThread = 5;  /*10 Îó²î ¡À10*/
    datafilter_AC_pump.init(&datafilter_AC_pump);


  //  datafilter_feedback_pump.ExceptionValve = pump_set1_compare.feed_back_pump_speed_limit;/*ãÐÖµ 100 rpm*/
    datafilter_feedback_pump.VarianceThread = 5;  /*10 Îó²î ¡À10*/
    datafilter_feedback_pump.init(&datafilter_feedback_pump);


    OSflag = OSFlagPend(init_order_flag,0x02,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0x00,&err);
	assert_param(OSflag);
    APP_TRACE("monitor_task OSFlagPend \r\n");

	while (1)
	{
		memcpy(&control_order_compare, &control_order_r, sizeof(ControlOrder));
		memcpy(&pump_set1_compare, &pump_set1, sizeof(PumpSet1));
		memcpy(&pump_state1_compare, &pump_state1, sizeof(PumpState1));

		/*********************monitor draw_pump speed and direction ***********************/
// add by wq ( mask draw pump encoder interface err )
#if 0      
		if(control_order_compare.order_indicate.bit.draw_pump == PUMP_START)
		{
            datafilter_draw_pump.ExceptionValve = pump_set1_compare.draw_pump_speed_limit/100;/*ãÐÖµ 100 rpm*/
            datafilter_draw_pump.DataInput = pump_state1_compare.draw_pump_current_speed/100;
            datafilter_draw_pump.update(&datafilter_draw_pump);//

			/********************check pump direction***********************/
           // APP_TRACE("%d %d \r\n",pump_state1_compare.pump_current_dir.bit.draw_pump ,pump_dir_translate(0x01,pump_set1_compare));
            if(pump_state1_compare.pump_current_dir.bit.draw_pump != pump_dir_translate(0x01,pump_set1_compare))
			{
                if(flag_tmr_draw_pump_dir == 0)/*must be enter once*/
                {
                    flag_tmr_draw_pump_dir = 1;
                    OSTmrStart(tmr_draw_pump_direction,&tmr_err);
                }
			}
			/*********************check wheather fail to setspeed ***********************/
            else if(datafilter_draw_pump.Output == 0x20)
            {
                //do something
                pump_errcode.pump_errcode1.bit.draw_pump = ERR_FAILSPEED;

                // TODO: read hall of pump
                if(0)//if hall speed is normal && encoder speed is zero
                    pump_errcode.pump_errcode1.bit.draw_pump = ERR_ENCODER;//encoder is err
            }
			/********************check wheather overspeed***********************/
            else if(datafilter_draw_pump.Output == 0x10)
            {
                //do something
                pump_errcode.pump_errcode1.bit.draw_pump = ERR_OVERSPEED;

            }
            /********************check wheather shake***********************/
            else if(datafilter_draw_pump.Output == 0x30)
            {
                //do something
                pump_errcode.pump_errcode1.bit.draw_pump = ERR_SHAKE;

            }
            else if(flag_tmr_draw_pump_timeout != 1)/*no timeout*/
                pump_errcode.pump_errcode1.bit.draw_pump = ERR_NORMAL;

		}
#endif
		/*********************monitor AC_pump speed and direction ***********************/
		if(control_order_compare.order_indicate.bit.AC_pump == PUMP_START)
		{
            datafilter_AC_pump.ExceptionValve = pump_set1_compare.AC_pump_speed_limit/100;/*ãÐÖµ */
           // APP_TRACE("ExceptionValve = %d \r\n",datafilter_AC_pump.ExceptionValve);
            datafilter_AC_pump.DataInput = pump_state1_compare.AC_pump_current_speed/100;

           // APP_TRACE(" %d  %d \r\n",datafilter_AC_pump.ExceptionValve,datafilter_AC_pump.DataInput);
            datafilter_AC_pump.update(&datafilter_AC_pump);//

			/********************check pump direction***********************/

           // APP_TRACE("%d %d \r\n",pump_state1_compare.pump_current_dir.bit.AC_pump ,pump_dir_translate(0x03,pump_set1_compare));
            if(pump_state1_compare.pump_current_dir.bit.AC_pump != pump_dir_translate(0x03,pump_set1_compare))
			{
                if(flag_tmr_AC_pump_dir == 0)/*must be enter once*/
                  {
                      flag_tmr_AC_pump_dir = 1;
                      OSTmrStart(tmr_AC_pump_direction,&tmr_err);
                  }
			}
			/*********************check wheather fail to setspeed ***********************/
            else if(datafilter_AC_pump.Output == 0x20)
            {
                //do something
                //pump_errcode.pump_errcode1.bit.AC_pump = ERR_FAILSPEED;//mask by wq
                // TODO: read hall of pump

               // APP_TRACE("AC pump speed is fail\r\n");
                if(0)//if hall speed is normal && encoder speed is zero
                    pump_errcode.pump_errcode1.bit.AC_pump = ERR_ENCODER;//encoder is err

            }
			/********************check wheather overspeed***********************/
            else if(datafilter_AC_pump.Output == 0x10)
            {
                //do something
                pump_errcode.pump_errcode1.bit.AC_pump = ERR_OVERSPEED;

            }
            /********************check wheather shake***********************/
            else if(datafilter_AC_pump.Output == 0x30)
            {
                //do something
                pump_errcode.pump_errcode1.bit.AC_pump = ERR_SHAKE;

            }
            else if(flag_tmr_AC_pump_timeout != 1)
                pump_errcode.pump_errcode1.bit.AC_pump = ERR_NORMAL;

		}



		/*********************monitor feed_back_pump speed and direction ***********************/
		if(control_order_compare.order_indicate.bit.feed_back_pump == PUMP_START)
		{
            datafilter_feedback_pump.ExceptionValve = pump_set1_compare.feed_back_pump_speed_limit/100;/*ãÐÖµ 100 rpm*/
            datafilter_feedback_pump.DataInput = pump_state1_compare.feed_back_pump_current_speed/100;
            datafilter_feedback_pump.update(&datafilter_feedback_pump);//

			/********************check pump direction***********************/
            if(pump_state1_compare.pump_current_dir.bit.feed_back_pump != pump_dir_translate(0x02,pump_set1_compare))
			{
			  //  APP_TRACE("pump_state1_compare.pump_current_dir.bit.feed_back_pump = %d\r\n",pump_state1_compare.pump_current_dir.bit.feed_back_pump);
			 //   APP_TRACE("pump_dir_translate(0x02,pump_set1_compare)              = %d\r\n",pump_dir_translate(0x02,pump_set1_compare));
                
                if(flag_tmr_feedback_pump_dir == 0)/*must be enter once*/
                  {
                      flag_tmr_feedback_pump_dir = 1;
                      OSTmrStart(tmr_feedback_pump_direction,&tmr_err);
                  }
			}
			/*********************check wheather fail to setspeed ***********************/
            else if(datafilter_feedback_pump.Output == 0x20)
            {
                //do something
                pump_errcode.pump_errcode2.bit.feedback_pump = ERR_FAILSPEED;
                // TODO: read hall of pump
                if(0)//if hall speed is normal && encoder speed is zero
                    pump_errcode.pump_errcode2.bit.feedback_pump = ERR_ENCODER;

            }
			/********************check wheather overspeed***********************/
            else if(datafilter_feedback_pump.Output == 0x10)
            {
                //do something
                pump_errcode.pump_errcode2.bit.feedback_pump = ERR_OVERSPEED;

            }
			/********************check wheather shake***********************/
            else if(datafilter_feedback_pump.Output == 0x30)
            {
                //do something
                pump_errcode.pump_errcode2.bit.feedback_pump = ERR_SHAKE;

            }
            else if(flag_tmr_feedback_pump_timeout != 1)
                pump_errcode.pump_errcode2.bit.feedback_pump = ERR_NORMAL;

		}

        //for test
       // send_task_errcode(MONITOR_TASK,TASK_WAR ,0xaa);
     //   pump_errcode.pump_errcode1.bit.AC_pump = 0x02;

		OSTimeDlyHMSM(0, 0, 0, 10);/*10ms min frequence is 100hz*/
		// do something

    //    pump_errcode.pump_errcode1.bit.AC_pump = 0x03;

	}
}

/*
 * Function   : tmr_draw_pump_timeout_callback
 * Description: call back fonction for draw pump timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/
void tmr_draw_pump_timeout_callback(OS_TMR *ptmr, void *p_arg)
{
    u8 tmr_err = 0;
    flag_tmr_draw_pump_timeout = 1;

    pump_errcode.pump_errcode1.bit.draw_pump = ERR_TIMEOUT;
    TRACE("[%8d] draw_pump_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_draw_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something

}
/*
 * Function   : tmr_AC_pump_timeout_callback
 * Description: call back fonction for AC pump timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_AC_pump_timeout_callback(OS_TMR *ptmr, void *p_arg)
{
    u8 tmr_err = 0;
    flag_tmr_AC_pump_timeout = 1;
    pump_errcode.pump_errcode1.bit.AC_pump = ERR_TIMEOUT;
    TRACE("[%8d] AC_pump_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_AC_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}
/*
 * Function   : tmr_feedback_pump_timeout_callback
 * Description: call back fonction for feedback pump timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_feedback_pump_timeout_callback(OS_TMR *ptmr, void *p_arg)
{
    u8 tmr_err = 0;
    flag_tmr_feedback_pump_timeout = 1;
    pump_errcode.pump_errcode2.bit.feedback_pump = ERR_TIMEOUT;
	TRACE("[%8d] feedback_pump_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_feedback_pump_timeout,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}

/*
 * Function   : tmr_draw_pump_dir_callback
 * Description: call back fonction for feedback pump dir timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_draw_pump_dir_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_dir_draw_pump = 0;
    PumpSet1     pump_set1_compare;

    if(control_order_r.order_indicate.bit.draw_pump == PUMP_START)
    {
        current_dir_draw_pump = pump_state1.pump_current_dir.bit.draw_pump;
        memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));

        flag_tmr_draw_pump_dir = 0;
        if(current_dir_draw_pump != pump_dir_translate(0x01,pump_set1_compare))
        {
            pump_errcode.pump_errcode1.bit.draw_pump = ERR_DIRECTION;
        }
    }
	TRACE("[%8d] draw_pump_dir_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_draw_pump_direction,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}
/*
 * Function   : tmr_AC_pump_dir_callback
 * Description: call back fonction for AC pump dir timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/

void tmr_AC_pump_dir_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_dir_AC_pump = 0;
    PumpSet1     pump_set1_compare;

    if(control_order_r.order_indicate.bit.AC_pump == PUMP_START)
    {
        current_dir_AC_pump = pump_state1.pump_current_dir.bit.AC_pump;
        memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));

        TRACE("[%8d] 111AC_pump_dir_timeout!\r\n",OSTimeGet());
        flag_tmr_AC_pump_dir = 0;
        
        TRACE("current_dir_AC_pump                   = %d\r\n",current_dir_AC_pump);
        TRACE("pump_dir_translate(0x03,pump_set1_compare) = %d\r\n",pump_dir_translate(0x03,pump_set1_compare));
        
        if(current_dir_AC_pump != pump_dir_translate(0x03,pump_set1_compare))
        {
            pump_errcode.pump_errcode1.bit.AC_pump = ERR_DIRECTION;

            TRACE("[%8d] 222AC_pump_dir_timeout!\r\n",OSTimeGet());
        }

    }
	TRACE("[%8d] 333AC_pump_dir_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_AC_pump_direction,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}

/*
 * Function   : tmr_feedback_pump_dir_callback
 * Description: call back fonction for feedback pump dir timeout
 * Param      : ptmr => timer
 *            : p_arg => para
 * Return     : void
*/


void tmr_feedback_pump_dir_callback(OS_TMR *ptmr, void *p_arg)
{
    u8           tmr_err = 0;
    u8           current_dir_feedback_pump = 0;
    PumpSet1     pump_set1_compare;

    if(control_order_r.order_indicate.bit.feed_back_pump == PUMP_START)
    {
        current_dir_feedback_pump = pump_state1.pump_current_dir.bit.feed_back_pump;
        memcpy(&pump_set1_compare,&pump_set1,sizeof(PumpSet1));

        flag_tmr_feedback_pump_dir = 0;
        
        TRACE("current_dir_feedback_pump                   = %d\r\n",current_dir_feedback_pump);
        TRACE("2pump_dir_translate(0x02,pump_set1_compare) = %d\r\n",pump_dir_translate(0x02,pump_set1_compare));
        
        if(current_dir_feedback_pump != pump_dir_translate(0x02,pump_set1_compare))
        {
            pump_errcode.pump_errcode2.bit.feedback_pump = ERR_DIRECTION;
        }
    }
	TRACE("[%8d] feedback_pump_dir_timeout!\r\n",OSTimeGet());
    OSTmrStop(tmr_feedback_pump_direction,OS_TMR_OPT_NONE,(void*)0,&tmr_err);
    //do something
}


static u8 pump_dir_translate(u8 pump,PumpSet1 pump_set1)
{
	switch(pump)
	{
	case 0x01://draw_pump
	    if(control_order_r.order_indicate.bit.draw_pump == PUMP_START)
	    {
    		if(pump_set1.pump_direction.bit.draw_pump == 0x01)
    		{
    			return PUMP_DIR_CLOCKWISE;/*Ë³Ê±Õë*/
    		}
    		else
    		{
    			if(pump_set1.pump_direction.bit.draw_pump == 0x00)
    			{
    				return PUMP_DIR_UNCLOCKWISE;/*ÄæÊ±Õë*/
    			}
    		}
	    }
		else
		{
			return PUMP_DIR_STOP;//stop
		}
		//break;
	case 0x02://feed_back_pump
	    if(control_order_r.order_indicate.bit.feed_back_pump == PUMP_START)
	    {
    		if(pump_set1.pump_direction.bit.feed_back_pump == 0x01)
    		{
    			return PUMP_DIR_CLOCKWISE;/*Ë³Ê±Õë*/
    		}
    		else
    		{
    			if(pump_set1.pump_direction.bit.feed_back_pump == 0x00)
    			{
    				return PUMP_DIR_UNCLOCKWISE;/*ÄæÊ±Õë*/
    			}

    		}
	    }
		else
		{
			return PUMP_DIR_STOP;//stop
		}
		//break;
	case 0x03://AC_pump

        if(control_order_r.order_indicate.bit.AC_pump == PUMP_START)
        {
    		if(pump_set1.pump_direction.bit.AC_pump == 0x01)
    		{
    			return PUMP_DIR_CLOCKWISE;/*Ë³Ê±Õë*/
    		}
    		else
    		{
    			if(pump_set1.pump_direction.bit.AC_pump == 0x00)
    			{
    				return PUMP_DIR_UNCLOCKWISE;/*ÄæÊ±Õë*/
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


/*
 * Function   : slave1_show_parameter_table
 * Description: show parameter table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave1_show_parameter_table()
{
    /*draw_pump*/
    APP_TRACE("draw_pump order                  =  ");
    switch(control_order_r.order_indicate.bit.draw_pump)
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

    APP_TRACE("draw_pump set speed              =  %3d.%-2d rpm\r\n",
                                                    pump_set1.draw_pump_speed_limit/100,
                                                    pump_set1.draw_pump_speed_limit%100);
    APP_TRACE("draw_pump current speed          =  %3d.%-2d rpm\r\n",
                                                    pump_state1.draw_pump_current_speed/100,
                                                    pump_state1.draw_pump_current_speed%100);
    APP_TRACE("draw_pump set distance           = %4d.%1d  r\r\n",
                                                    pump_set2.draw_pump_need_distance/10,
                                                    pump_set2.draw_pump_need_distance%10);
    APP_TRACE("draw_pump current distance       = %4d.%1d  r\r\n",
                                                    pump_state2.draw_pump_moved_distance/10,
                                                    pump_state2.draw_pump_moved_distance%10);
    APP_TRACE("draw_pump set direction          = ");
    switch(pump_set1.pump_direction.bit.draw_pump)
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
    APP_TRACE("draw_pump current direction      = ");
    switch(pump_state1.pump_current_dir.bit.draw_pump)
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


    /*AC_pump*/
    APP_TRACE("AC_pump order                    =  ");
    switch(control_order_r.order_indicate.bit.AC_pump)
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
    APP_TRACE("AC_pump set speed                =  %3d.%-2d rpm\r\n",
                                                    pump_set1.AC_pump_speed_limit/100,
                                                    pump_set1.AC_pump_speed_limit%100);
    APP_TRACE("AC_pump current speed            =  %3d.%-2d rpm\r\n",
                                                    pump_state1.AC_pump_current_speed/100,
                                                    pump_state1.AC_pump_current_speed%100);
    APP_TRACE("AC_pump set distance             = %4d.%1d  r\r\n",
                                                    pump_set2.AC_pump_need_distance/10,
                                                    pump_set2.AC_pump_need_distance%10);
    APP_TRACE("AC_pump current distance         = %4d.%1d  r\r\n",
                                                    pump_state2.AC_pump_moved_distance/10,
                                                    pump_state2.AC_pump_moved_distance%10);
    APP_TRACE("AC_pump set  direction           = ");
    switch(pump_set1.pump_direction.bit.AC_pump)
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
    APP_TRACE("AC_pump current direction        = ");
    switch(pump_state1.pump_current_dir.bit.AC_pump)
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


    /*feed_back_pump*/
    APP_TRACE("feed_back_pump order             =  ");
    switch(control_order_r.order_indicate.bit.feed_back_pump)
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
    APP_TRACE("feed_back_pump set speed         =  %3d.%-2d rpm\r\n",
                                                    pump_set1.feed_back_pump_speed_limit/100,
                                                    pump_set1.feed_back_pump_speed_limit%100);
    APP_TRACE("feed_back_pump current speed     =  %3d.%-2d rpm\r\n",
                                                    pump_state1.feed_back_pump_current_speed/100,
                                                    pump_state1.feed_back_pump_current_speed%100);
    APP_TRACE("feed_back_pump set distance      = %4d.%1d  r\r\n",
                                                    pump_set2.feed_back_pump_need_distance/10,
                                                    pump_set2.feed_back_pump_need_distance%10);
    APP_TRACE("feed_back_pump current distance  = %4d.%1d  r\r\n",
                                                    pump_state2.feed_back_pump_moved_distance/10,
                                                    pump_state2.feed_back_pump_moved_distance%10);
    APP_TRACE("feed_back_pump set direction     = ");
    switch(pump_set1.pump_direction.bit.feed_back_pump)
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
    APP_TRACE("feed_back_pump current direction = ");
    switch(pump_state1.pump_current_dir.bit.feed_back_pump)
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

/*
 * Function   : slave1_show_sensor_table
 * Description: show sensor table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave1_show_sensor_table()
{
    u8  sign;
    u16 pressure;

    APP_TRACE("sensor_state_handle.blood_collect_preasure = 0x%x,%d\r\n", sensor_state_handle.blood_collect_preasure,sensor_state_handle.blood_collect_preasure);
    
    /*for blood_collect_preasure */
    sign     = (sensor_state.blood_collect_preasure>>13)&0x01;
    pressure = sensor_state.blood_collect_preasure&0x1fff; //12:0

    if(sign == 0x00)//+
    {
        APP_TRACE("blood_collect_preasure  = +%4d mmHG\r\n",pressure);
    }
    else
    {
        APP_TRACE("blood_collect_preasure  = -%4d mmHG\r\n",pressure);
    }
    /*for blood_feedback_preasure */
    sign     = (sensor_state.blood_feedback_preasure>>13)&0x01;
    pressure = (sensor_state.blood_feedback_preasure&0x1fff); //12:0
    if(sign == 0x00)//+
    {
        APP_TRACE("blood_feedback_preasure = +%4d mmHG\r\n",pressure);
    }
    else
    {
        APP_TRACE("blood_feedback_preasure = -%4d mmHG\r\n",pressure);
    }

    APP_TRACE("\r\n");

    /*for AC_bubble sensor */
    if(sensor_state.liquid_level.bit.AC_bubble)
    {
        APP_TRACE("NOTE : AC_bubble detected bubble!\r\n");
    }
    else
    {
        APP_TRACE("AC_bubble no bubble!\r\n");
    }
    /*for high_level sensor */
    if(sensor_state.liquid_level.bit.high_level)
    {
        APP_TRACE("high_level is triggered!\r\n");
    }
    else
    {
        APP_TRACE("high_level isn't triggered!\r\n");
    }
    /*for low_level sensor */
    if(sensor_state.liquid_level.bit.low_level)
    {
        APP_TRACE("low_level  is triggered!\r\n");
    }
    else
    {
        APP_TRACE("low_level  isn't triggered!\r\n");
    }

    APP_TRACE("\r\n");

    /*for hoare sensor */
    if(sensor_state.pump_init.bit.draw_pump_init)
    {
        APP_TRACE("draw_pump_init       is finished!\r\n");
    }
    else
    {
        APP_TRACE("draw_pump_init       is unfinished!\r\n");
    }
    if(sensor_state.pump_init.bit.AC_pump_init)
    {
        APP_TRACE("AC_pump_init         is finished!\r\n");
    }
    else
    {
        APP_TRACE("AC_pump_init         is unfinished!\r\n");
    }
    if(sensor_state.pump_init.bit.feed_back_pump_init)
    {
        APP_TRACE("feed_back_pump_init  is finished!\r\n");
    }
    else
    {
        APP_TRACE("feed_back_pump_init  is unfinished!\r\n");
    }



}


/*
 * Function   : slave1_show_error_table
 * Description: show error table for shell cmd
 * Param      : void
 *            :
 * Return     : void
*/

void slave1_show_error_table()
{
    /*err_code of draw_pump*/
    switch(pump_errcode.pump_errcode1.bit.draw_pump)
    {
        case 0x00:
            APP_TRACE("draw_pump     is normal\r\n");
            break;
        case 0x01:
            APP_TRACE("draw_pump     is failspeed\r\n");
            break;
        case 0x02:
            APP_TRACE("draw_pump     is overspeed\r\n");
            break;
        case 0x03:
            APP_TRACE("draw_pump     is err direction\r\n");
            break;
        case 0x04:
            APP_TRACE("draw_pump     is timeout\r\n");
            break;
        case 0x05:
            APP_TRACE("draw_pump     is shake\r\n");
            break;
        case 0x07:
            APP_TRACE("draw_pump     is encoder err\r\n");
            break;
        default:
            APP_TRACE("the errcode of draw pump is unknow!\r\n");
            break;
    }
    /*err_code of AC_pump*/
    switch(pump_errcode.pump_errcode1.bit.AC_pump)
    {
        case 0x00:
            APP_TRACE("AC_pump       is normal\r\n");
            break;
        case 0x01:
            APP_TRACE("AC_pump       is failspeed\r\n");
            break;
        case 0x02:
            APP_TRACE("AC_pump       is overspeed\r\n");
            break;
        case 0x03:
            APP_TRACE("AC_pump       is err direction\r\n");
            break;
        case 0x04:
            APP_TRACE("AC_pump       is timeout\r\n");
            break;
        case 0x05:
            APP_TRACE("AC_pump       is shake\r\n");
            break;
        case 0x07:
            APP_TRACE("AC_pump       is encoder err\r\n");
            break;
        default:
            APP_TRACE("the errcode of AC pump is unknow!\r\n");
            break;
    }
    /*err_code of feedback_pump*/
    switch(pump_errcode.pump_errcode2.bit.feedback_pump)
    {
        case 0x00:
            APP_TRACE("feedback_pump is normal\r\n");
            break;
        case 0x01:
            APP_TRACE("feedback_pump is failspeed\r\n");
            break;
        case 0x02:
            APP_TRACE("feedback_pump is overspeed\r\n");
            break;
        case 0x03:
            APP_TRACE("feedback_pump is err direction\r\n");
            break;
        case 0x04:
            APP_TRACE("feedback_pump is timeout\r\n");
            break;
        case 0x05:
            APP_TRACE("feedback_pump is shake\r\n");
            break;
        case 0x07:
            APP_TRACE("feedback_pump is encoder err\r\n");
            break;
        default:
            APP_TRACE("the errcode of feedback pump is unknow!\r\n");
            break;
    }



}


