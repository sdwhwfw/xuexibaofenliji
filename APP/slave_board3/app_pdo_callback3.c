/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_pdo_callback3.c
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
#include "app_pdo_callback3.h"
#include "app_slave_board3.h"
#include "trace.h"

/*module_indicate*/
#define MODULE_INDICATE_NULLITY_ORDER       0x00
#define MODULE_INDICATE_MOTOR_CONTROL       0x01
#define MODULE_INDICATE_MAGNET_CONTROL      0x02
#define MODULE_INDICATE_INIT                0xA5
#define MODULE_INDICATE_PARA_UPDATA_SYNC    0xff

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

    if(  (pump_set1.centrifuge_motor_speed_limit != 0x00)\
        |(pump_set1.motor_direction.all != 0x00)\
        )/*屏蔽第一次全0指令*/
    {
        OSFlagPost(start_pump_sync_flag,0x01,OS_FLAG_SET,&err);
        APP_TRACE("post RPDO1 start_pump_sync_flag \r\n");
    }

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
	APP_TRACE("RPDO2_callback\r\n");

	// do someting...
	memcpy(&pump_set2,p_data,8);
	// for debug
	//DumpData(p_data, len);

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

	PumpValveOrder pump_order;
    static u8 is_initialized = 0;

	memset(&pump_order,0xff,sizeof(PumpValveOrder));//2bit '11'  is ignore

	APP_TRACE("RPDO3_callback\r\n");

	// for debug
	DumpData(p_data, len);

	big2little_endion((u8*)&control_order_r,p_data,8);
	DumpData((u8*)&control_order_r, len);
    
    control_order_sync_t.fn = control_order_r.fn;//updata FN
    control_order_sync_t.answer = ACK;//init
    control_order_sync_t.continue_retransmit.bit.retransmit = control_order_r.continue_retransmit.bit.retransmit;//retransmit bit
    

    
	switch(control_order_r.module_indicate)
	{
		case MODULE_INDICATE_NULLITY_ORDER://nullity order
			APP_TRACE("nullity order!\r\n");
			break;
		case MODULE_INDICATE_MOTOR_CONTROL://motor control
		
            APP_TRACE("control_order_r.order_indicate.bit.cassette_motor = %d\r\n",control_order_r.order_indicate.bit.cassette_motor);
		    if(is_initialized == 1)
		    {
		    /*
    			pump_order.pump_or_valve		     =	0x01;
    			pump_order.pump.bit.centrifuge_motor =	control_order_r.order_indicate.bit.centrifugeo_or_electromagnet;
    			pump_order.pump.bit.cassette_motor	 =	control_order_r.order_indicate.bit.cassette_motor;

                pump_mem_ptr = (u8*)OSMemGet(pump_mem_order,&err);
                assert_param(pump_mem_ptr);

    			memcpy(pump_mem_ptr,&pump_order,sizeof(PumpValveOrder));
    			OSQPost(pump_q_send, pump_mem_ptr);
                */
                
                OSFlagPost(start_pump_sync_flag,0x02,OS_FLAG_SET,&err);
                APP_TRACE("post RPDO3 start_pump_sync_flag _motor \r\n");

                if(control_order_r.para1.bit.centrifuge_motor)
                 {
                    pump_state2.centrifuge_motor_moved_distance = 0;/*行程清零*/
                 }
                 else
                 {
                     /*继续记录行程*/
                 }
                if(control_order_r.para1.bit.cassette_motor)
                 {
                    pump_state2.cassette_motor_moved_distance = 0;/*行程清零*/
                 }
                 else
                 {
                     /*继续记录行程*/
                 }
		    }

			break;
		case MODULE_INDICATE_MAGNET_CONTROL://magnet
		    if(is_initialized == 1)
		    {
    			/*pump_order.pump_or_valve	  =	0x02;
    			pump_order.valve.bit.magnet	  =	control_order_r.order_indicate.bit.centrifugeo_or_electromagnet;

    			memcpy(pump_mem_ptr,&pump_order,sizeof(PumpValveOrder));
    			OSQPost(pump_q_send, pump_mem_ptr);
    			*/
               OSFlagPost(start_pump_sync_flag,0x04,OS_FLAG_SET,&err);
               APP_TRACE("post RPDO3 start_pump_sync_flag cassette_motor \r\n");
		    }
			break;
        case MODULE_INDICATE_INIT://init
            if((control_order_r.order_indicate.all != 0x00)
                ||(control_order_r.para1.all != 0x00)
                ||(control_order_r.para2 != 0x00)
                ||(control_order_r.para3 != 0x00)
                ||(control_order_r.para4 != 0x00)
                //||(control_order_r.retransmit != 0x00)
              )
            {
                control_order_sync_t.answer = NAK;
            }
            else
            {
                is_initialized = 1;//must first init

                //sync sensor_handle_task and monitor_task
                OSFlagPost(init_order_flag,0x03,OS_FLAG_SET,&err);
                APP_TRACE("post A5 init order\r\n");
            }
            break;
		case MODULE_INDICATE_PARA_UPDATA_SYNC://para updata sync
			//do something
			break;
		default: //undefined order
            control_order_sync_t.answer = NAK;
			APP_TRACE("undefined module_indicate!\r\n");
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
    if(control_order_sync_r.fn == task_errcode.fn)
    {
       if(control_order_sync_r.answer == ACK)//ACK
           OSFlagPost(task_errcode_acknak_flag,0x01,OS_FLAG_SET,&err);
       else if(control_order_sync_r.answer == NAK)//NAK
           OSFlagPost(task_errcode_acknak_flag,0x02,OS_FLAG_SET,&err);
    }
    else
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
	//APP_TRACE("TPDO_callback\r\n");

	// for debug
	//DumpData(p_data, len);

	// do someting...
	// do someting...
	big2little_endion(p_data,  (u8*)(&pump_state1),8);		//TPDO1
	big2little_endion(p_data+8,(u8*)(&pump_state2),8);		//TPDO2
	big2little_endion(p_data+16,(u8*)(&sensor_state),8);		//TPDO3
	big2little_endion(p_data+24,(u8*)(&control_order_sync_t),8);	//TPDO4
	big2little_endion(p_data+32,(u8*)(&task_errcode),8);		//TPDO5
 //   big2little_endion(p_data+40,(u8*)(&motor_errcode),8);      //TPDO6

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


