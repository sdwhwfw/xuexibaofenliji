/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pdo_callback1.c
 * Author             : WQ
 * Date First Issued  : 2013/10/12
 * Description        : This file contains the software implementation for the
 *                      callback function of PDO
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/12 | v1.0  | WQ         | initial released
 *******************************************************************************/

#include "stm32f2xx.h"
#include "app_pdo_callback1.h"
#include "app_slave_board1.h"
#include "trace.h"
#include "gpio.h"
#include "app_dc_motor.h"


/*module_indicate*/
#define MODULE_INDICATE_NULLITY_ORDER      0x00
#define MODULE_INDICATE_PUMP_CONTROL       0x01
#define MODULE_INDICATE_INIT               0xA5
#define MODULE_INDICATE_PARA_UPDATA_SYNC   0xff

#define ACK 0x06
#define NAK 0x15


static void big2little_endion(u8* dest ,const u8* src, u16 len);

/*
 * Function   : RPDO1_callback
 * Description: call back function for RPDO1(202 speed and dir)
 * Param      : p_data => receive data
 *            : len => receive length
 * Return     : u8
*/

u8 RPDO1_callback(u8* p_data, u16 len)
{
     u8 err;
     APP_TRACE("RPDO1_callback\r\n");

 // do someting...
     big2little_endion((u8*)&pump_set1,p_data,8);

    if((pump_set1.AC_pump_speed_limit != 0x00)\
        |(pump_set1.feed_back_pump_speed_limit != 0x00)\
        |(pump_set1.draw_pump_speed_limit != 0x00)\
        |(pump_set1.pump_direction.all != 0x00)
        )/*屏蔽第一次全0指令*/
    {
        OSFlagPost(start_pump_sync_flag,0x01,OS_FLAG_SET,&err);
      //  APP_TRACE("post RPDO1 start_pump_sync_flag \r\n");
    }

     //APP_TRACE("pump_set1.DRAW_pump_speed_limit = 0x%x\r\n",pump_set1.draw_pump_speed_limit);
    // for debug
    DumpData(p_data, len);

    return 0;
}
/*
 * Function   : RPDO2_callback
 * Description: call back function for RPDO2(302 distance)
 * Param      : p_data => receive data
 *            : len => receive length
 * Return     : u8
*/

u8 RPDO2_callback(u8* p_data, u16 len)
{
    u8 err;
     APP_TRACE("RPDO2_callback\r\n");
	 // do someting...
	 big2little_endion((u8*)&pump_set2,p_data,8);

     if(  (pump_set2.AC_pump_need_distance != 0x00)\
         |(pump_set2.draw_pump_need_distance != 0x00)\
         |(pump_set2.feed_back_pump_need_distance != 0x00)\
         |(pump_set2.reserved2 != 0x00)\
         )/*屏蔽第一次全0指令*///////???????????
     {
         OSFlagPost(start_pump_sync_flag,0x02,OS_FLAG_SET,&err);
        // APP_TRACE("post RPDO2 start_pump_sync_flag \r\n");
     }
	 // for debug
	 DumpData(p_data, len);

	 return 0;
}
/*
 * Function   : RPDO3_callback
 * Description: call back function for RPDO3(402 cmd)
 * Param      : p_data => receive data
 *            : len => receive length
 * Return     : u8
*/

u8 RPDO3_callback(u8* p_data, u16 len)
{
	 u8 err = 0;
     static u8 is_initialized = 0;

	// PumpValveOrder pump_order;

	// memset(&pump_order,0xff,sizeof(PumpValveOrder));//0x03 is ignore

	APP_TRACE("RPDO3_callback\r\n");
	// for debug
	DumpData(p_data, len);

	big2little_endion((u8*)&control_order_r,p_data,8);

    control_order_sync_t.fn = control_order_r.fn;//updata FN
    control_order_sync_t.answer = ACK;//init
    control_order_sync_t.continue_retransmit.bit.retransmit = control_order_r.continue_retransmit.bit.retransmit;//retransmit bit

	 switch(control_order_r.module_indicate)
	 {
		 case MODULE_INDICATE_NULLITY_ORDER://nullity order
			 APP_TRACE("nullity order!\r\n");
			 break;

		 case MODULE_INDICATE_PUMP_CONTROL://pump control
           if(is_initialized == 1)//is_initialized
            {
                 /*  pump_order.fn = control_order_r.fn;
                 //pump_order.go_on = control_order_r.continue_retransmit.bit.go_on;
                 pump_order.pump.bit.draw_pump	   =  control_order_r.order_indicate.bit.draw_pump;
    			 pump_order.pump.bit.AC_pump	   =  control_order_r.order_indicate.bit.AC_pump;
    			 pump_order.pump.bit.feedback_pump =  control_order_r.order_indicate.bit.feed_back_pump;

                 pump_mem_ptr = (u8*)OSMemGet(pump_mem_order,&err);
                 assert_param(pump_mem_ptr);
                 memcpy(pump_mem_ptr,&pump_order,sizeof(PumpValveOrder));
    			 OSQPost(pump_q_send, pump_mem_ptr);
                */
                // APP_TRACE("POST pump_mem_ptr\r\n");

                OSFlagPost(start_pump_sync_flag,0x04,OS_FLAG_SET,&err);
                APP_TRACE("post RPDO3 start_pump_sync_flag \r\n");

                 if(control_order_r.para1.bit.draw_pump)/*行程清零*/
                 {
                    //add to total_distance
                    //draw_pump_total_distance +=  pump_state2.draw_pump_moved_distance;
                    draw_pump_total_distance += get_dc_motor_current_distance(MOTOR_NUM3);
                   // pump_state2.draw_pump_moved_distance = 0;
                   clear_dc_motor_current_distance(MOTOR_NUM3);
                 }
                 else
                 {
                     /*继续记录行程*/
                    //pump_state2.draw_pump_moved_distance = get_current_distance(draw_pump);
                    //sensor_task write the moved_distance
                 }

                 if(control_order_r.para1.bit.AC_pump)/*行程清零*/
                 {

                    AC_pump_total_distance +=  get_dc_motor_current_distance(MOTOR_NUM2);
                    //pump_state2_handle.AC_pump_moved_distance = 0;
                   // pump_state2.AC_pump_moved_distance = 0;//
                   clear_dc_motor_current_distance(MOTOR_NUM2);
                   APP_TRACE("AC_pump_ total_distance = %d\r\n",AC_pump_total_distance);
                   APP_TRACE("AC_pump_single_distance = %d\r\n",pump_state2.AC_pump_moved_distance);
                 }
                 else
                 {
                     /*继续记录行程*/
                 }

                 if(control_order_r.para1.bit.feed_back_pump)/*行程清零*/
                 {
                  //  feedback_pump_total_distance +=  pump_state2.feed_back_pump_moved_distance;

                    feedback_pump_total_distance +=  get_dc_motor_current_distance(MOTOR_NUM1);
                    //pump_state2.feed_back_pump_moved_distance = 0;
                    clear_dc_motor_current_distance(MOTOR_NUM1);
                    APP_TRACE("feedback_pump_total_distance = %d\r\n",feedback_pump_total_distance);
                 }
                 else
                 {
                     /*继续记录行程*/
                 }
            }
			 break;

        case MODULE_INDICATE_INIT://init 0xA5
            if(control_order_r.init_or_diatance == 0x00) //is init not distance
            {
                is_initialized = 1;//must first init
                //sync sensor_handle_task and monitor_task
                OSFlagPost(init_order_flag,0x07,OS_FLAG_SET,&err);
                APP_TRACE("post A5\r\n");
            }
            else if(control_order_r.init_or_diatance == 0x01) // is distance
            {
              /*  switch(control_order_r.single_or_total.bit.draw_pump)
                {
                    case 0x01://total
                         //send total_distance and its flag
                         //write_pump_distance_flag(DRAW_PUMP,TOTAL);
                        // write_pump_moved_distance(); write total by jiulong

                    break;
                    case 0x00://single
                        //write_pump_distance_flag(DRAW_PUMP,SINGLE);

                    break;
                    case 0x11://clear total
                        //write_pump_distance_flag(clear_total); no need
                        write_pump_distance_flag(DRAW_PUMP,SINGLE);
                        draw_pump_total_distance = 0;
                    break;
                    default:
                        APP_TRACE("undnfined single_or_total order!\r\n");
                    break;
                }

                switch(control_order_r.single_or_total.bit.AC_pump)
                {
                    case 0x01://total
                         //send total_distance and its flag
                         write_pump_distance_flag(AC_PUMP,TOTAL);

                    break;
                    case 0x00://single
                        write_pump_distance_flag(AC_PUMP,SINGLE);
                    break;
                    case 0x11://clear total

                        write_pump_distance_flag(AC_PUMP,SINGLE);
                        AC_pump_total_distance = 0;
                    break;
                    default:
                        APP_TRACE("undnfined single_or_total order!\r\n");
                    break;
                }

                switch(control_order_r.single_or_total.bit.feedback_pump)
                {
                    case 0x01://total
                         //send total_distance and its flag
                         write_pump_distance_flag(FEEDBACK_PUMP,TOTAL);

                    break;
                    case 0x00://single
                        write_pump_distance_flag(FEEDBACK_PUMP,SINGLE);

                    break;
                    case 0x11://clear total
                        write_pump_distance_flag(FEEDBACK_PUMP,SINGLE);
                        feedback_pump_total_distance = 0;
                    break;
                    default:
                        APP_TRACE("undnfined single_or_total order!\r\n");
                    break;
                }
                */
            }
            else //undefine  order
            {
                control_order_sync_t.answer = NAK;
		    	 APP_TRACE("undefined init_or_diatance order!\r\n");
            }

            break;
		 case MODULE_INDICATE_PARA_UPDATA_SYNC://para updata sync
			 //do something
			 break;

		 default: //undefined order
             control_order_sync_t.answer = NAK;
			 APP_TRACE("undefined order!\r\n");
			 break;

	 }

	return err;
}

/*
 * Function   : RPDO4_callback
 * Description: call back function for RPDO4(502 ACK)
 * Param      : p_data => receive data
 *            : len => receive length
 * Return     : u8
*/

u8 RPDO4_callback(u8* p_data, u16 len)
{
    uint8_t err;
     APP_TRACE("RPDO4_callback\r\n");

	 // for debug
	 DumpData(p_data, len);

	 // do someting...
	 big2little_endion((u8*)&control_order_sync_r,p_data,8);

     //post flag to TPDO task
    //APP_TRACE("control_order_sync_r.fn = %d task_errcode.fn = %d\r\n",control_order_sync_r.fn,task_errcode.fn);
     if(control_order_sync_r.fn == task_errcode.fn)
     {
        if(control_order_sync_r.answer == ACK)//ACK
            OSFlagPost(task_errcode_acknak_flag,0x01,OS_FLAG_SET,&err);
        else if(control_order_sync_r.answer == NAK)//NAK
            OSFlagPost(task_errcode_acknak_flag,0x02,OS_FLAG_SET,&err);
      TRACE("post flag ACK/NAK'\r\n");
     }
  //   else
     {
        //do nothing wait timeout
     }

	 return 0;
}


u8 RPDO5_callback(u8* p_data, u16 len)
{
	 APP_TRACE("RPDO5_callback\r\n");

	 // for debug
	 DumpData(p_data, len);

	 // do someting...

	 return 0;
}

/*
 * Function   : TPDO_callback
 * Description: call back function for TPDO1:6()
 * Param      : p_data => receive data
 *            : len => receive length
 * Return     : u8
*/


u8 TPDO_callback(u8* p_data, u16 len)
{
	// APP_TRACE("TPDO_callback\r\n");

	 // for debug
	 //DumpData(p_data, len);
/*
	 pump_state1.draw_pump_current_speed++;
	 pump_state1.draw_pump_current_speed &= 0xff;
     pump_state2.draw_pump_moved_distance++;
     pump_state2.draw_pump_moved_distance &= 0xff;
     sensor_state.blood_collect_preasure++;
     sensor_state.blood_collect_preasure &= 0xff;
     control_order_sync_t.fn++;//for test
     control_order_sync_t.fn &= 0xff;//for test
     task_errcode.fn++;
     task_errcode.fn &= 0xff;

    // pump_errcode.pump_errcode1.all++;
    // pump_errcode.pump_errcode1.all &= 0xff;
     pump_errcode.errcode_task++;
     pump_errcode.errcode_task &= 0xff;
     pump_errcode.reserved3 = 0x05;
     pump_errcode.reserved4 = 0x06;
     pump_errcode.reserved5 = 0x07;
     pump_errcode.reserved6 = 0x08;
*/
	 // do someting...
	/* memcpy(p_data,  &pump_state1,8);		 //TPDO1
	 memcpy(p_data+8,&pump_state2,8);		 //TPDO2
	 memcpy(p_data+16,&sensor_state,8); 	 //TPDO3
	 memcpy(p_data+24,&control_order_sync_t,8);  //TPDO4
	 memcpy(p_data+32,&task_errcode,8);		 //TPDO5
	 memcpy(p_data+40,&pump_errcode,8);		 //TPDO6
*/
     /*if( (read_pump_distance_flag(DRAW_PUMP) == 0x01)\
        |(read_pump_distance_flag(AC_PUMP) == 0x01)\
        |(read_pump_distance_flag(FEEDBACK_PUMP) == 0x01)\
        |(control_order_r.order_indicate.bit.draw_pump == 0x01)\
        |(control_order_r.order_indicate.bit.AC_pump == 0x01)\
        |(control_order_r.order_indicate.bit.feed_back_pump == 0x01)\
        )*/
        /*泵启动的时候 反馈总行程 的时候发送*/

     // if(1)
    /* {
         big2little_endion(p_data,  (u8*)(&pump_state1),8);      //TPDO1
         big2little_endion(p_data+8,(u8*)(&pump_state2),8);        //TPDO2

     }
*/
    big2little_endion(p_data,  (u8*)(&pump_state1),8);      //TPDO1
    big2little_endion(p_data+8,(u8*)(&pump_state2),8);        //TPDO2
    big2little_endion(p_data+16,(u8*)(&sensor_state),8);      //TPDO3



    big2little_endion(p_data+24,(u8*)(&control_order_sync_t),8);  //TPDO4 test arm0 timeout

    big2little_endion(p_data+32,(u8*)(&task_errcode),8);       //TPDO5


/* for test
    pump_errcode.pump_errcode1.bit.AC_pump ++;
    if(pump_errcode.pump_errcode1.bit.AC_pump == 0x05)
    {
    pump_errcode.pump_errcode1.bit.AC_pump = 0;
    }
*/
    big2little_endion(p_data+40,(u8*)(&pump_errcode),8);      //TPDO6

	 return 0;
}





void big2little_endion(u8* dest ,const u8* src, u16 len)
{
    u16 i = 0;

    assert_param(dest);
    assert_param(src);

    for(i=0;i<len;i++)
    {
        dest[i+1] = src[i];
        dest[i] = src[i+1];
        i++;
    }
}





