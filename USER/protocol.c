/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : protocol.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/05/21
 * Description        : This file contains the software implementation for the
 *                      DC motor control protocol unit
 *******************************************************************************
 * History:
 * 2013/05/21 v1.0
 * 2013/06/03 V1.1 Add support for 3 motors
 *******************************************************************************/

#include "protocol.h"
#include "queue.h"
#include "trace.h"


static uint8_t get_motor_id(uint8_t ch)
{
	uint8_t index;

	switch (ch)
	{
		case '1':
			index = 0;
			break;
		case '2':
			index = 1;
			break;
		case '3':
			index = 2;
			break;
		default:
			index = 0xff;
			break;
	}

	return index;
}



/*
 *---------------------------------------------------------------
 * S      设置PID参数
 * 10     KP参数，实际为10/1000 = 0.01
 * 1150   KPP参数，实际运算为1.15*KP
 * 110    KPI参数，实际运算为0.11*KP
 * 0      KPD参数，实际运算为0*KP
 * 2000   电机转速，单位为RPM（每分钟转速）
 *
 * example:
 * S1 10 1150 110 0 2000;
 *---------------------------------------------------------------
 * B1    启动电机
 * example:
 * B1;
 *---------------------------------------------------------------
 * T1     停止电机
 * example:
 * T1;
 *---------------------------------------------------------------
 * P1     改变电机转速
 * example:
 * P1 2000;
 * --------------------------------------------------------------
 * G1     获取电机参数
 * example:
 * G1;
 */
void do_cmd_data(uint8_t ch)
{
	static uint8_t vl_cmd[64];
	static int pos = 0;
	uint8_t i = 0;
	extern cir_queue queue;
	// 1: cmd OK, 0: cmd error
	cmd_type cmd_flag = SET_CMD_UNKNOWM;
	uint8_t index;
	s32 cmd_data[5];
	uint8_t *str;
	uint8_t delims[] = " ";
	uint8_t *result = NULL;

	// we recv a full cmd data and now we need to deal with it.
	if (ch == ';')
	{
		index  = get_motor_id(vl_cmd[1]);
		if (index == 0xff)
		{
			// cmd error
			pos = 0;
			return;
		}

		// B1;
		// Start motor
		if( vl_cmd[0] == 'B' && pos == 2 )
		{
			cmd_flag = START_MOTOR;
		}

		// T1;
		// Stop motor
		if( vl_cmd[0] == 'T' && pos == 2 )
		{
			cmd_flag = STOP_MOTOR;
		}

        // G1;
        if ( vl_cmd[0] == 'G' && pos == 2 )
        {
            cmd_flag = GET_DC_MOTOR_PARAM;
        }

		// P1 2000;
		// Set motor speed
		if( vl_cmd[0] == 'P' )
		{
			cmd_flag = SET_CMD_UNKNOWM;
			vl_cmd[pos] = 0;
			str = vl_cmd;
			result = (uint8_t*)strtok( (char*)str, (char*)delims );
			while (result != NULL)
			{
				result = (uint8_t*)strtok( NULL, (char*)delims );
				if( result != NULL && i < 1 )
				{
					cmd_data[i] = atoi((const char*)result);
					i++;
				}
				else if( result == NULL && i == 1 )
				{
					// last loop
					cmd_flag = SET_MOTOR_SPEED;
				}
				else
				{
					// cmd error
					cmd_flag = SET_CMD_UNKNOWM;
				}
			}
		}

		// S 1000 20000 10 11 1 1 0;
		// S1 10 1150 110 0 2000;
		// Set PID param
		if( vl_cmd[0] == 'S' )
		{
			cmd_flag = SET_CMD_UNKNOWM;
			vl_cmd[pos] = 0;
			str = vl_cmd;
			result = (uint8_t*)strtok( (char*)str, (char*)delims );
			while( result != NULL )
			{
				result = (uint8_t*)strtok( NULL, (char*)delims );
				if( result != NULL && i < 5 )
				{
					cmd_data[i] = atoi((const char*)result);
					i++;
				}
				else if (result == NULL && i == 5)
				{
					// the last loop
					cmd_flag = SET_PID_TYPE;
				}
				else
				{
					// cmd error
					cmd_flag = SET_CMD_UNKNOWM;
					break;
				}
			}
		}

		// SET MOTOR CMD DATA
		switch( cmd_flag )
		{
			case SET_PID_TYPE:
				dc_motor_pid_param_config((MotorType)index, cmd_data[0], cmd_data[1], cmd_data[2], cmd_data[3]);
                //set_motor_speed((MotorType)index, cmd_data[4], 0);
				break;
			case START_MOTOR:
				start_dc_motor((MotorType)index);
				break;
			case STOP_MOTOR:
				stop_dc_motor((MotorType)index);
				break;
			case SET_MOTOR_SPEED:
				set_motor_speed((MotorType)index, cmd_data[0], 0);
				break;
            case GET_DC_MOTOR_PARAM:
                //get_dc_motor_param((MotorType)index);
                break;
			default:
				break;
		}

		// reset position
		pos = 0;
	}
	else if (pos < 64)
	{
		// only save data
		vl_cmd[pos] = ch;
		pos++;
	}
	else
	{
		//cmd error
		pos = 0;
	}
}



