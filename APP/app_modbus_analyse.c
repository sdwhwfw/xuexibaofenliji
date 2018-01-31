/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_modbus_analyse.c
 * Author             : WQ
 * Date First Issued  : 2013/08/12
 * Description        : This file contains the software implementation for the
 *                      modbus analyse task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/08/12 | v1.0  | WQ         | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | merge source code to NEW framework
 *******************************************************************************/

#include "app_modbus_analyse.h"
#include "app_usart_dma_route.h"
#include "crc.h"
#include "usart.h"
#include "trace.h"
#include "string.h"

static OS_STK modbus_analyse_task_stk[MODBUS_ANALYSE_TASK_STK_SIZE];

static p_RPDO_function p_RPDO[SLAVE_RPDO_NUM] = {0, 0, 0, 0, 0};
static p_TPDO_function p_TPDO = 0;


/************************************************/
/*      MODBUS FUNCTION CODES                   */
/************************************************/
#define MB_READ_COILS                0x01
#define MB_READ_DISCRETE_INPUTS      0x02
#define MB_READ_HOIDIN_GREGISTERS    0x03  //03
#define MB_READ_INPUT_REGISTERS      0x04
#define MB_WRITE_COIL                0x05
#define MB_WRITE_REGISTER            0x06
#define MB_WRITE_MULT_REG            0x10  //16
#define MB_READ_WRITE_REGISTER       0x17  //23

#define RPDO1                        0x00
#define RPDO2                        0x08
#define RPDO3                        0x10
#define RPDO4                        0x18
#define RPDO5                        0x20


static void modbus_analyse_task(void *p_arg);

void init_modbus_analyse_task()
{
    INT8U os_err;

	/* modbus analyse task *************************************/
	os_err = OSTaskCreateExt((void (*)(void *)) modbus_analyse_task,
							(void		   * ) 0,
							(OS_STK 	   * )&modbus_analyse_task_stk[MODBUS_ANALYSE_TASK_STK_SIZE - 1],
							(INT8U			 ) MODBUS_ANALYSE_TASK_PRIO,
							(INT16U 		 ) MODBUS_ANALYSE_TASK_PRIO,
							(OS_STK 	   * )&modbus_analyse_task_stk[0],
							(INT32U 		 ) MODBUS_ANALYSE_TASK_STK_SIZE,
							(void		   * ) 0,
							(INT16U 		 )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	assert_param(OS_ERR_NONE == os_err);
	OSTaskNameSet(MODBUS_ANALYSE_TASK_PRIO, (INT8U *)"MODBUS ANALYSE", &os_err);

}




void init_RPDO_call_back(p_RPDO_function* p_fun, u8 RPDO_num)
{
	u8 i;

	// check param
	assert_param(RPDO_num == SLAVE_RPDO_NUM);

	for (i = 0; i < SLAVE_RPDO_NUM; i++)
	{
		p_RPDO[i] = p_fun[i];

		//make sure callback function not NULL
		assert_param(p_RPDO[i] != 0);
	}
}


void init_TPDO_call_back(p_RPDO_function p_fun)
{
	// check callback pointer
	assert_param(p_fun != 0);

	p_TPDO = p_fun;
}


/*
 * We send TPDO in this function
 */
static void mb_read_register(uint8_t* packet, uint8_t length)
{
	//uint16_t temp_addr;
	uint16_t temp_num;
	uint16_t temp_crc;
	uint8_t  send_packet[255];        // 5+125*2        max 125 register can read one time

	//temp_addr = (packet[2] << 8) | packet[3];
	temp_num  = (packet[4] << 8) | packet[5];

	memset(send_packet,0,sizeof(send_packet));
	send_packet[0] = MB_SLAVE_ID;
	send_packet[1] = MB_READ_INPUT_REGISTERS;
	send_packet[2] = temp_num * 2;

	if (p_TPDO)
	{
		/* TODO: the param need be checked again! */
		p_TPDO(&(send_packet[3]), temp_num);
	}
	else
	{
		APP_TRACE("Warning: TPDO callback function not init!\r\n");
	}

	// add CRC code to packet
	temp_crc = CRC16(send_packet, temp_num * 2 + 3);
	send_packet[temp_num * 2 + 3] = temp_crc >> 8;
	send_packet[temp_num * 2 + 4] = temp_crc & 0x00FF;
/*
#if defined(USE_ARM3_REV_0_1)
    #ifdef USE_USART1_DMA_FTR
	    send_data_packet(COM1, send_packet, temp_num * 2 + 5);
    #else
        #error "ARM3 use USART1 to communication with CANOpen moudle!"
    #endif // USE_USART1_DMA_FTR 
#else
    send_data_packet(COM3, send_packet, temp_num * 2 + 5);
#endif
*/

#if defined(USE_ARM3_REV_0_1)||defined(USE_ARM1_REV_0_2)||defined(USE_ARM2_REV_0_1)
    #ifdef USE_USART1_DMA_FTR
            send_data_packet(COM1, send_packet, temp_num * 2 + 5);
    #else
        #error "ARM3 use USART1 to communication with CANOpen moudle!"
    #endif // USE_USART1_DMA_FTR 
#else
        send_data_packet(COM3, send_packet, temp_num * 2 + 5);
#endif







    //DumpData(send_packet,temp_num * 2 + 5);//for debug

}


/*
 * We receive RPDO in this function
 *
 */
static void mb_write_register(uint8_t* packet, uint8_t length)
{
	//uint16_t temp_num;
	uint16_t temp_addr, temp_crc;
	uint8_t  send_packet[8], i;

	temp_addr = (packet[2] << 8) | packet[3];
	//temp_num  = (packet[4] << 8) | packet[5];

	switch(temp_addr)
	{
		case RPDO1:
			if (p_RPDO[0])
			{
				p_RPDO[0](&packet[7], 8);
			}
			else
			{
				APP_TRACE("Warning: RPDO0 callback function not init!\r\n");
			}
			break;
		case RPDO2:
			if (p_RPDO[1])
			{
				p_RPDO[1](&packet[7], 8);
			}
			else
			{
				APP_TRACE("Warning: RPDO1 callback function not init!\r\n");
			}
			break;
		case RPDO3:
			if (p_RPDO[2])
			{
				p_RPDO[2](&packet[7], 8);
			}
			else
			{
				APP_TRACE("Warning: RPDO2 callback function not init!\r\n");
			}
			break;
		case RPDO4:
			if (p_RPDO[3])
			{
				p_RPDO[3](&packet[7], 8);
			}
			else
			{
				APP_TRACE("Warning: RPDO3 callback function not init!\r\n");
			}
			break;
		case RPDO5:
			if (p_RPDO[4])
			{
				p_RPDO[4](&packet[7], 8);
			}
			else
			{
				APP_TRACE("Warning: RPDO4 callback function not init!\r\n");
			}
			break;
		default:
			APP_TRACE("Warning: RPDO[%x] callback function not init!\r\n", temp_addr);
			break;
	}

	for (i = 0; i < 6; i++)
	{
		send_packet[i] = packet[i];
	}

	// add CRC code to packet
	temp_crc       =  CRC16(send_packet, 6);
	send_packet[6] = (temp_crc >> 8);
	send_packet[7] = temp_crc;


#if defined(USE_ARM3_REV_0_1)||defined(USE_ARM1_REV_0_2)
    #ifdef USE_USART1_DMA_FTR
        send_data_packet(COM1, send_packet, 8);
    #else
        #error "ARM3 use USART1 to communication with CANOpen moudle!"
    #endif // USE_USART1_DMA_FTR 
#elif  defined(USE_ARM2_REV_0_1)
    send_data_packet(COM1, send_packet, 8);
	#else
    send_data_packet(COM3, send_packet, 8);
#endif



}


/*******************************************************************
 * mb_parse:
 *		   resolve the message receive from the modbus master and response
 *
 * INPUTS
 * rece_buf 		: message receive from the modbus master
 * rece_length		: length of message
 *
 * RETURNS          :   0  success
 *                      1  faild
 *******************************************************************/
static uint8_t mb_parse(uint8_t* packet, uint8_t length)
{
	uint8_t  ret = 0;
	uint16_t temp_crc;

	if(packet[0] == MB_SLAVE_ID)
	{
		// compute CRC code
		temp_crc = CRC16(packet, length - 2);

		// check CRC code
		if((packet[length - 2] == (temp_crc >> 8))
			&&(packet[length - 1] == (temp_crc & 0x00ff)))
		{
			switch(packet[1])
			{
				case MB_READ_INPUT_REGISTERS: //04 read, we need send TPDO here
					mb_read_register(packet, length);//////??????????????
				break;

				case MB_WRITE_MULT_REG:       //16 write, we need read RPDO here
					mb_write_register(packet, length);
				break;

				default:
                    ret = 1;
					APP_TRACE("Unknow Function Code!\r\n");
				break;
			}
		}
		else
		{
			APP_TRACE("Mb CRC Err!\r\n");
			ret = 1;
		}
	}
	else
	{
		APP_TRACE("Slave_ID Err!\r\n");
		ret = 1;
	}

	return ret;
}


/*
 * @brief: This function will be called by usart_dma_route task
 *         So, if you do not want to do things in this function
 *         you should send queue message to your own task.
 * @param: p_data, point to data received
 * @param: len, length of received data packet
 * @ret  : 0
 */
static u8 usart_recv_packet(u8* p_data, u16 len)
{
	// for debug
	//APP_TRACE("\r\nRece from NetIC : ");
	//DumpData(p_data, len);

	mb_parse(p_data, len);

	return 0;
}


static void modbus_analyse_task(void *p_arg)
{
    // init usart dma route task
    init_usart_dma_route_task();
	// init callback funtion

    // -------- ARM3 --------
    // use USART1 ---> CANOpen
    // use USART3 ---> DEBUG
    // -------- ARM1 VER0.2 --------
    // use USART1 ---> CANOpen
    // use USART3 ---> Debug
    // ------- others -------
    // use USART3 ---> CANOpen
    // use USART1 ---> DEBUG

#if defined(USE_ARM3_REV_0_1)
    #ifdef USE_USART1_DMA_FTR
    init_usart_with_recv_callback(COM1, usart_recv_packet, 115200, PARITY_EVEN);
    #else
    #error "ARM3 use USART1 to communication with CANOpen moudle!"
    #endif /* USE_USART1_DMA_FTR */
#elif defined(USE_ARM1_REV_0_2)
    init_usart_with_recv_callback(COM1, usart_recv_packet, 115200, PARITY_EVEN);
#elif defined(USE_ARM2_REV_0_1)
    init_usart_with_recv_callback(COM1, usart_recv_packet, 115200, PARITY_EVEN);
#else
    init_usart_with_recv_callback(COM3, usart_recv_packet, 115200, PARITY_EVEN);
#endif /* USE_ARM3_REV_0_1 */

	// delay some time to wait netIC power on
	//OSTimeDlyHMSM(0, 0, 3, 0);

	APP_TRACE("netIC module ready...\r\n");

	while (1)
	{
		OSTimeDlyHMSM(0, 0, 0, 500);
		// do something here...

	}
}




