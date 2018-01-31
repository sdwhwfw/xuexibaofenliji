/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_centrifuge_control.c
 * Author             : su
 * Date First Issued  : 2013/12/09
 * Description        : This file contains the software implementation for the
 *                      centrifuge control task of board3
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/31 | v1.0  | su         | initial released
 * 2015/06/17 | v1.1  | su         | modify
 *******************************************************************************/
#ifndef _APP_CENTRIFUGE_CONTROL_C_
#define _APP_CENTRIFUGE_CONTROL_C_

#include "app_centrifuge_control.h"
#include "trace.h"
#include "usart.h"
#include "string.h"
//#include "basic_timer.h"
#include "app_usart_dma_route.h"
#include "app_slave_board3.h"
//define
#define GET_COMMAND  0X67    	// 'g'
#define SET_PARAMETER  0X73  	// 's'
#define RESET_AMPLIFIER  0X72 	// 'r'
#define VALUE 0X76  			// 'v'
#define SPACE 0X20
#define USART_RECV_DATA_BUF_SIZE  32
#define USART_RECV_DATA_BUF_LEN   64
#define DEFAULT_SPEED_ADD_GRAD	  200
// parameter
CmdTypeDef status;
//上电之后先要对驱动器进行参数的初始化，parameter_flag主要标示usart2_data_buf是否需要对数据进行处理
u8 parameter_flag  = 0;

static void*    get_usart_rx_QueueTbl[30];
static uint8_t  usart2_data_buf[ USART_RECV_DATA_BUF_SIZE][USART_RECV_DATA_BUF_LEN];

static OS_STK centrifuge_control_task_stk[CENTRIFUGE_CONTROL_TASK_STK_SIZE];
static OS_STK centrifuge_monitor_task_stk[CENTRIFUGE_MONITOR_TASK_STK_SIZE];

static void centrifuge_control_task(void *p_arg);
static void centrifuge_monitor_task(void *p_arg);
static u8 usart2_recv_packet(u8* centrifuge_data, u16 data_len);
void init_amplifier_parameter(void);

void init_centrifuge_control_task(void)
{
    INT8U os_err;
    /* centrifuge control task ***************************************/
    os_err = OSTaskCreateExt((void (*)(void *)) centrifuge_control_task,
                         (void          * ) 0,
                         (OS_STK        * )& centrifuge_control_task_stk[CENTRIFUGE_CONTROL_TASK_STK_SIZE - 1],
                         (INT8U           ) CENTRIFUGE_CONTROL_TASK_PRIO,
                         (INT16U          ) CENTRIFUGE_CONTROL_TASK_PRIO,
                         (OS_STK        * )& centrifuge_control_task_stk[0],
                         (INT32U          ) CENTRIFUGE_CONTROL_TASK_STK_SIZE,
                         (void          * ) 0,
                         (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(CENTRIFUGE_CONTROL_TASK_PRIO, (INT8U *)"CENTRIFUGE CTR", &os_err);
}


static void centrifuge_control_task(void *p_arg)
{
    u8 *data;
	u8 m_err = 0;
    DataBlockTypeDef*  recv_data_block;
    init_usart_with_recv_callback(COM2, usart2_recv_packet,9600, PARITY_NONE);

	// init USART2 recv queue
	get_centrifuge_cmd_event = (OS_EVENT*)OSQCreate(get_usart_rx_QueueTbl, USART_RECV_DATA_BUF_SIZE);
	assert_param(get_centrifuge_cmd_event);

	// init USART2 recv mem
	get_usart2_mem= OSMemCreate(usart2_data_buf, USART_RECV_DATA_BUF_SIZE, USART_RECV_DATA_BUF_LEN, &m_err);
	assert_param(OS_ERR_NONE == m_err);

	//last command execute finish sem
	cmd_finish_sem = OSSemCreate(1);

	//初始化驱动器参数
	init_amplifier_parameter();

	APP_TRACE("ENTER centrifuge control task!\r\n");
    for(;;)
    {
        u16 l_data_len = 0;

        recv_data_block = OSQPend(get_centrifuge_cmd_event, 0, &m_err);
        assert_param(OS_ERR_NONE == m_err);
        OSSemPend(cmd_finish_sem, 0, &m_err);
        assert_param(OS_ERR_NONE == m_err);
        l_data_len = recv_data_block->rev_data_len;
        data = recv_data_block->data_recv;
		parameter_flag = 1;
        switch(data[5])
        {
            /*command 0x18 is get moter current speed*/
            case '1':
                if(data[6] == '8')
                    status = GET_CURRENT_SPEED;
            break;

            case '2':
                /*command 0x20 is get amplifier current temperature*/
                if(data[6] == '0')
                    status = GET_AMPLIFIER_TEMP;
                /*command s r0x24 13 set amplifier mode to enable*/
                else if(data[0] == 's'&&data[6] == '4'&&data[8] == '1')
				{
                    status = SET_AMPLIFIER_ENABLE;

				}
				 /*command s r0x24 0 set amplifier mode to disable*/
				else if(data[0] == 's'&&data[6] == '4'&&data[8] == '0')
				{
                    status = SET_AMPLIFIER_DISABLE;

				}
                /*command g r0x2c get command speed*/
                else if(data[6] == 'c')
				{
                    status = GET_COMMAND_SPEED;

                }
				/*command g r0x21 get peak current*/
                else if(data[6] == '1')
                {
                    status = GET_COMMEND_PEAK_CURRENT;

                }
             break;
            case '3':
                 /*command g r0x38 get actual current*/
                if(data[6] == '8')
                    status = GET_ACTUAL_CURRENT;
            break;
            case 'a':
                 /*command g r0xa1 get amplifier status*/
                 if(data[0] == 'g'&&data[6] == '1')
                {
                    status = GET_AMPLIFIER_STATUS;
                }
                 /*command s r0xa9 xxxxx set moter speed to xxxx*/
                else if(data[0] == 's'&&data[6] == '9')
                {
	                status = SET_MOERT_SPEED;
	//              APP_TRACE("SET_MOERT_SPEED status is %d\r\n",status);
                }
            break;
            case '9':
                /*command 0x90 is get/set baud rate*/
 				if(data[0] == 's'&&data[6] == '0')
                {
                    status = SET_BAUD_RATE;

                }
            break;
            default:
            break;
        }
//        for(i=0;i<l_data_len;i++)
//        APP_TRACE("data is %c\r\n",data[i]);
        send_data_packet(COM2,data,l_data_len);
        OSMemPut(get_usart2_mem, (void *)recv_data_block);
    }
}

void send_speed_value_to_amplifier(u16 speed_value)
{
	DataBlockTypeDef*  speed_data_block;
	u8 speed_acsii[5]={0};
    int i=0;
	uint8_t  send_packet[15];
	INT8U err = 0;

	for(i=4;i>=0;i--)
	{
	   speed_acsii[i] = speed_value % 10+'0';
	   speed_value= speed_value/10;
	}
	send_packet[0] = SET_PARAMETER;
	send_packet[1] = SPACE;
	send_packet[2] = 'r';
	send_packet[3] = '0';
	send_packet[4] = 'x';
	send_packet[5] = 'a';
	send_packet[6] = '9';
	send_packet[7] = SPACE;
	send_packet[8] = speed_acsii[0];
	send_packet[9] = speed_acsii[1];
	send_packet[10] = speed_acsii[2];
	send_packet[11] = speed_acsii[3];
	send_packet[12] = speed_acsii[4];
	send_packet[13] = 0x0d;
	speed_data_block = OSMemGet(get_usart2_mem, &err);

	speed_data_block->rev_data_len = 14;
	memcpy(speed_data_block->data_recv,send_packet,14);

	err = OSQPost(get_centrifuge_cmd_event, (void*)speed_data_block);
	assert_param(OS_ERR_NONE == err);

}


//set amplifier to enable or disable
// @enable_or_dis  1 is enable,0 is disable
void set_amplifier_status(StatusTypeDef enable_or_dis)
{

	uint8_t  cmd_packet[15] = {0};
	DataBlockTypeDef*  status_data_block;
	INT8U  error = 0;
    /*set  amplifier mode to enable */
    cmd_packet[0] = SET_PARAMETER;
    cmd_packet[1] = SPACE;
    cmd_packet[2] = 'r';
    cmd_packet[3] = '0';
    cmd_packet[4] = 'x';
    cmd_packet[5] = '2';
    cmd_packet[6] = '4';
    cmd_packet[7] = SPACE;
	if(enable_or_dis == AMPLIFIER_ENABLE)
	{
		cmd_packet[8] = '1';
		cmd_packet[9] = '3';
	}
	else if(enable_or_dis == AMPLIFIER_DISABLE)
	{
		cmd_packet[8] = '0';
		cmd_packet[9] = '0';
	}
    cmd_packet[10] = 0x0d;
    status_data_block = OSMemGet(get_usart2_mem, &error);
    status_data_block->rev_data_len = 11;
    memcpy(status_data_block->data_recv,cmd_packet,11);
    error = OSQPost(get_centrifuge_cmd_event, (void*)status_data_block);
    assert_param(OS_ERR_NONE == error);
}


void init_amplifier_parameter(void)
{
	INT8U mor_err = 0;
	/*set amplifier to disable */
	uint8_t set_amplifier_disable[10] = {SET_PARAMETER,SPACE,'r','0','x','2','4',SPACE,'0',0x0d};
	/*set init speed to 0 */
	uint8_t  set_speed_0[10] = {SET_PARAMETER,SPACE,'r','0','x','a','9',SPACE,'0',0x0d};
	parameter_flag = 0;
	send_data_packet(COM2,set_speed_0,10);
	OSTimeDlyHMSM(0,0,0,10);
	send_data_packet(COM2,set_amplifier_disable,10);
	mor_err = OSSemPost(start_monitor_sem);
    assert_param(OS_ERR_NONE == mor_err);
}

void get_actual_speed(void)
{
	uint8_t  cmd_packet[10] = {0};
	DataBlockTypeDef*  speed_data_block;
	INT8U  error = 0;
	/*get actual speed */
	cmd_packet[0] = GET_COMMAND;
	cmd_packet[1] = SPACE;
	cmd_packet[2] = 'r';
	cmd_packet[3] = '0';
	cmd_packet[4] = 'x';
	cmd_packet[5] = '1';
	cmd_packet[6] = '8';
	cmd_packet[7] = 0x0d;

	speed_data_block = OSMemGet(get_usart2_mem, &error);
	speed_data_block->rev_data_len = 8;
	memcpy(speed_data_block->data_recv,cmd_packet,8);
	error = OSQPost(get_centrifuge_cmd_event, (void*)speed_data_block);
	assert_param(OS_ERR_NONE == error);

}



void get_command_speed(void)
{
    u8 q_err = 0;
	uint8_t  cmd_packet[10] = {0};
	DataBlockTypeDef*  speed_data_block;
	INT8U  error = 0;
	/*get command speed */
	cmd_packet[0] = GET_COMMAND;
	cmd_packet[1] = SPACE;
	cmd_packet[2] = 'r';
	cmd_packet[3] = '0';
	cmd_packet[4] = 'x';
	cmd_packet[5] = '2';
	cmd_packet[6] = 'c';
	cmd_packet[7] = 0x0d;

	speed_data_block = OSMemGet(get_usart2_mem, &error);
	speed_data_block->rev_data_len = 8;
	memcpy(speed_data_block->data_recv,cmd_packet,8);
	q_err = OSQPost(get_centrifuge_cmd_event, (void*)speed_data_block);
	assert_param(OS_ERR_NONE == q_err);

}
void get_command_peak_current(void)
{
    u8 q_err = 0;
	uint8_t  cmd_packet[10] = {0};
	DataBlockTypeDef*  current_data_block;
	INT8U  error = 0;
	/*get command peak current */
	cmd_packet[0] = GET_COMMAND;
	cmd_packet[1] = SPACE;
	cmd_packet[2] = 'r';
	cmd_packet[3] = '0';
	cmd_packet[4] = 'x';
	cmd_packet[5] = '2';
	cmd_packet[6] = '1';
	cmd_packet[7] = 0x0d;
	current_data_block = OSMemGet(get_usart2_mem, &error);
	current_data_block->rev_data_len = 8;
	memcpy(current_data_block->data_recv,cmd_packet,8);
	q_err = OSQPost(get_centrifuge_cmd_event, (void*)current_data_block);
	assert_param(OS_ERR_NONE == q_err);

}

void get_actual_peak_current(void)
{
    u8 q_err = 0;
	uint8_t  cmd_packet[10] = {0};
	DataBlockTypeDef*  current_data_block;
	INT8U  error = 0;
	/*get actual current */
	cmd_packet[0] = GET_COMMAND;
	cmd_packet[1] = SPACE;
	cmd_packet[2] = 'r';
	cmd_packet[3] = '0';
	cmd_packet[4] = 'x';
	cmd_packet[5] = '3';
	cmd_packet[6] = '8';
	cmd_packet[7] = 0x0d;
	current_data_block = OSMemGet(get_usart2_mem, &error);
	current_data_block->rev_data_len = 8;
	memcpy(current_data_block->data_recv,cmd_packet,8);
	q_err = OSQPost(get_centrifuge_cmd_event, (void*)current_data_block);
	assert_param(OS_ERR_NONE == q_err);
}

void init_centrifuge_motor(void)
{
	send_speed_value_to_amplifier(0);
	OSTimeDlyHMSM(0, 0, 0, 5);
	set_amplifier_status(AMPLIFIER_ENABLE);
}

u8 start_centrifuge_motor(u16 speed)
{
	int j=0;

	u8 change_count = 0;
	u16 speed_grag = 0;
	u16 residue_value = 0;
	//enable amplifier
	set_amplifier_status(AMPLIFIER_ENABLE);
    if(speed>1500)
    {
			APP_TRACE("speed value error!");
			return 1;

    }
    else
     /*按位转换成ASCII码*/
    {
    	if(speed == 0)
    	{
    		send_speed_value_to_amplifier(0);
    	}
		else
		{
	        /*注意发给驱动器的速度单位是0.1counts/s，不是rpm，speed必须乘4 */
	        speed = speed*4;
			change_count = speed/(DEFAULT_SPEED_ADD_GRAD*4);
			residue_value = speed%(DEFAULT_SPEED_ADD_GRAD*4);
			//将速度值按梯度DEFAULT_SPEED_ADD_GRAD增长，余数最后设置
			for(j=0;j<change_count;j++)
			{
			    speed_grag = DEFAULT_SPEED_ADD_GRAD*4*change_count;
				send_speed_value_to_amplifier(speed_grag);
				OSTimeDlyHMSM(0,0,1,0);
			}
	        if(residue_value >0)
	        {
	        	send_speed_value_to_amplifier(speed);
	        }
		}
        /*启动时电机速度上升需要一段时间，启动定时器，这段时间monitor不比较速度电流*/
       // basic_timer_start();
       // stop_compare_flag = 1;
        APP_TRACE("start cent!\r\n");
    }
	return 0;
}




void stop_centrifuge_motor(void)
{
    //DataBlockTypeDef*  stop_data_block;
	//反转，立即disable
	if(centrifuge_direction == 0)
	{
		set_amplifier_status(AMPLIFIER_DISABLE);
	}
	else
	{
		if(centrifuge_motor_current_speed >800)
		{
			send_speed_value_to_amplifier(500);
			OSTimeDlyHMSM(0,0,4,0);
			send_speed_value_to_amplifier(200);
			OSTimeDlyHMSM(0,0,4,0);
			set_amplifier_status(AMPLIFIER_DISABLE);
		}
		else if(centrifuge_motor_current_speed >200)
		{
			send_speed_value_to_amplifier(200);
			OSTimeDlyHMSM(0,0,4,0);
			send_speed_value_to_amplifier(100);
			OSTimeDlyHMSM(0,0,2,0);
			set_amplifier_status(AMPLIFIER_DISABLE);
		}
		else
		{
			set_amplifier_status(AMPLIFIER_DISABLE);
		}
	}
}




/*
 * @brief: This function will be called by usart_dma_route task
 *         So, if you do not want to do things in this function
 *         you should send queue message to your own task.
 * @param: p_data, point to data received
 * @param: len, length of received data packet
 * @ret  : 0
 */


static u8 usart2_recv_packet(u8* centrifuge_data, u16 data_len)
{
    u8 sem_err;
    u8 j=0,i=0,k=0;
	s32 cmd_index=0;
	s32 cur_index=0;
	s32 current_speed=0;
	s32 command_speed=0;
	s32 peak_index = 0,peak_current = 0;
	s32 actual_index =0,actual_current = 0 ;

    char callback_ok[3] = {'o','k',0x0d};
    //char callback_baud_rate[7] = {'v',SPACE,'9','6','0','0',0x0d};
	if(parameter_flag == 0)
		return 0;
    switch(status)
    {
        case SET_MOERT_SPEED:

            if(strncmp(callback_ok,(const char*)centrifuge_data, data_len) == 0)
            {
                sem_err = OSSemPost(cmd_finish_sem);
                assert_param(OS_ERR_NONE == sem_err);

                APP_TRACE("set moter speed ok!\r\n");
            }
            else
        	{
               for(j=0;j<data_len;j++)
                  APP_TRACE("set moter speed callback is %d\r\n",centrifuge_data[j]);
            }
        break;
        case SET_AMPLIFIER_DISABLE:
            if(strncmp(callback_ok,(const char*)centrifuge_data, data_len) == 0)
            {
                sem_err = OSSemPost(cmd_finish_sem);
                assert_param(OS_ERR_NONE == sem_err);
//                APP_TRACE("stop moter ok!\r\n");
            }
            else
			{
               for(j=0;j<data_len;j++)
                  APP_TRACE("stop moter callback is  %c\r\n",centrifuge_data[j]);
		    }
        break;
		case SET_AMPLIFIER_ENABLE:
            if(strncmp(callback_ok,(const char*)centrifuge_data, data_len) == 0)
            {
                sem_err = OSSemPost(cmd_finish_sem);
                assert_param(OS_ERR_NONE == sem_err);
//                APP_TRACE("set amplifier enable ok!\r\n");
            }
            else
			{
               for(j=0;j<data_len;j++)
                  APP_TRACE("set amplifier enable callback is %d\r\n",centrifuge_data[j]);
		    }
        break;

        case GET_COMMAND_SPEED:

            if(centrifuge_data[0] == 'v')
            {
                /*ascii change to decimal*/
                for(i=2;i<data_len-1;i++)
                {
                    cmd_index = centrifuge_data[i]-'0';
                    for(k=0;k<data_len-2-i;k++)
                    {
                        cmd_index *= 10;
                    }
                    command_speed = command_speed+cmd_index;

                }
				APP_TRACE("g command speed is %d!\r\n",command_speed/4);
                sem_err = OSSemPost(cmd_finish_sem);
                assert_param(OS_ERR_NONE == sem_err);
                //cmd_index = 0;
				//command_speed = 0;
            }
            else
			{
               for(j=0;j<data_len;j++)
                  APP_TRACE("get command speed callback is %d\r\n",centrifuge_data[j]);
		    }
         break;
        case GET_CURRENT_SPEED:

            if(centrifuge_data[2] == '-')
            {
                for(i=3;i<data_len-1;i++)
                {
                    cur_index = centrifuge_data[i]-'0';
                    for(k=0;k<data_len-2-i;k++)
                        cur_index *= 10;
                    current_speed = current_speed+cur_index;
                }

                if(current_speed > 30)
                {
                    APP_TRACE("speed value is negative,centrifuge reverse rotation!\r\n");
                    centrifuge_direction = 0x0;
                    centrifuge_motor_current_speed = current_speed;
					//如果反向旋转，立即停止电机
					set_amplifier_status(AMPLIFIER_DISABLE);
                }
				else
				{
					centrifuge_motor_current_speed = 0;
				}
				sem_err = OSSemPost(cmd_finish_sem);
				assert_param(OS_ERR_NONE == sem_err);
				APP_TRACE("current speed is -%d!\r\n",centrifuge_motor_current_speed/4);
            }
            else
            {
                centrifuge_direction = 0x01;
                /*ascii change to decimal*/
                for(i=2;i<data_len-1;i++)
                {
                    cur_index = centrifuge_data[i]-'0';
                    for(k=0;k<data_len-2-i;k++)
                        cur_index *= 10;
                    current_speed = current_speed+cur_index;
                }
                centrifuge_motor_current_speed = current_speed;
				if(centrifuge_motor_current_speed >1600)
				{
					set_amplifier_status(AMPLIFIER_DISABLE);
				}
				if((centrifuge_motor_current_speed >(command_speed+200))||(centrifuge_motor_current_speed > command_speed*1.5))
				{
					set_amplifier_status(AMPLIFIER_DISABLE);
				}
                sem_err = OSSemPost(cmd_finish_sem);
                assert_param(OS_ERR_NONE == sem_err);
				APP_TRACE("current speed is %d!\r\n",centrifuge_motor_current_speed/4);

                //clear variables
                cur_index = 0 ;
                current_speed = 0;
				command_speed = 0;
            }

        break;
		case GET_COMMEND_PEAK_CURRENT:
			if(centrifuge_data[0] == 'v')
			{
				for(i=2;i<data_len-1;i++)
				{
					peak_index = centrifuge_data[i]-'0';
					for(k=0;k<data_len-2-i;k++)
						peak_index *= 10;
					peak_current = peak_current + peak_index;
				}
				sem_err = OSSemPost(cmd_finish_sem);
				assert_param(OS_ERR_NONE == sem_err);
//				APP_TRACE("get command peak current ok!\r\n");
				peak_index = 0;
			}

		break;
		case GET_ACTUAL_CURRENT:
			if(centrifuge_data[0] == 'v')
			{
				for(i=2;i<data_len-1;i++)
				{
					actual_index = centrifuge_data[i]-'0';
					for(k=0;k<data_len-2-i;k++)
						actual_index *= 10;
					actual_current = actual_current + actual_index;
				}
				/*单位0.01A,电机正转，返回参数时0-32767，反转时63335-32768，电机不转时，会有+-0.07A的电流反馈，过滤掉*/
				if(actual_current<10||actual_current>65525)
				{
					actual_current = 0;
				}
				else
				{
					if((actual_current<32768)&&(actual_current - peak_current > 0))
					{
						APP_TRACE( "centrifuge current is too high!\r\n");
						//disable amplifier
						set_amplifier_status(AMPLIFIER_DISABLE);
						sem_err = OSSemPost(cmd_finish_sem);
						assert_param(OS_ERR_NONE == sem_err);
					}
					else if((actual_current>32768)&&((65535 - actual_current) - peak_current > 0))
					{
						//disable amplifier
						set_amplifier_status(AMPLIFIER_DISABLE);
						sem_err = OSSemPost(cmd_finish_sem);
						assert_param(OS_ERR_NONE == sem_err);
//						APP_TRACE("get actual current ok!\r\n");
					}
				}
				//clear variables
				peak_current = 0;
				actual_current = 0;
				actual_index = 0;
			}
		 break;
        default:
             sem_err = OSSemPost(cmd_finish_sem);
             assert_param(OS_ERR_NONE == sem_err);
        break;
    }

    return 0;
}




void init_centrifuge_monitor_task(void)
{
    INT8U cen_err;
    /* centrifuge monitor task ***************************************/
    cen_err = OSTaskCreateExt((void (*)(void *)) centrifuge_monitor_task,
                         (void          * ) 0,
                         (OS_STK        * )& centrifuge_monitor_task_stk[CENTRIFUGE_MONITOR_TASK_STK_SIZE - 1],
                         (INT8U           ) CENTRIFUGE_MONITOR_TASK_PRIO,
                         (INT16U          ) CENTRIFUGE_MONITOR_TASK_PRIO,
                         (OS_STK        * )& centrifuge_monitor_task_stk[0],
                         (INT32U          ) CENTRIFUGE_MONITOR_TASK_STK_SIZE,
                         (void          * ) 0,
                         (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == cen_err);
    OSTaskNameSet(CENTRIFUGE_MONITOR_TASK_PRIO, (INT8U *)"CENTRIFUGE MOR", &cen_err);
}

static void centrifuge_monitor_task(void *p_arg)
{
    u8 sem_err = 0;
    OSSemPend(start_monitor_sem, 0, &sem_err);
    assert_param(OS_ERR_NONE == sem_err);
    for(;;)
    {
        /*get command speed */
		get_command_speed();
		OSTimeDlyHMSM(0, 0, 0, 5);
        /*get current actual speed  */
		get_actual_speed();
		OSTimeDlyHMSM(0, 0, 0, 5);
		/*get command peak current */
		get_command_peak_current();
		OSTimeDlyHMSM(0, 0, 0, 5);
		/*get actual current */
		get_actual_peak_current();

        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}


#endif/*_APP_CENTRIFUGE_CONTROL_C_*/
