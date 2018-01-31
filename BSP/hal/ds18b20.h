/**
  ******************************************************************************
  * @file    ds18b20.h
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-20
  * @brief   This file contains DS18B20 HAL functions.
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "stm32f2xx.h"
#include "app.h"        // for Delay_us();


#define  SkipROM         0xCC     //����ROM
#define  SearchROM       0xF0     //����ROM
#define  ReadROM         0x33     //��ROM
#define  MatchROM        0x55     //ƥ��ROM
#define  AlarmROM        0xEC     //�澯ROM
#define  StartConvert    0x44     //��ʼ�¶�ת�������¶�ת���ڼ����������0��ת�����������1
#define  ReadScratchpad  0xBE     //���ݴ�����9���ֽ�
#define  WriteScratchpad 0x4E     //д�ݴ������¶ȸ澯TH��TL
#define  CopyScratchpad  0x48     //���ݴ������¶ȸ澯���Ƶ�EEPROM���ڸ����ڼ����������0������������1
#define  RecallEEPROM    0xB8     //��EEPROM���¶ȸ澯���Ƶ��ݴ����У������ڼ����0��������ɺ����1
#define  ReadPower       0xB4     //����Դ�Ĺ��緽ʽ��0Ϊ������Դ���磻1Ϊ�ⲿ��Դ����

/**
 * 9BIT:  0x1F
 * 10BIT: 0x3F
 * 11BIT: 0x5F
 * 12BIT: 0x7F
 */
#define DS_PRECISION       0x7f
#define DS_ALARMTH         0x64
#define DS_ALARMTL         0x8a

#define SET_DQ() 		   GPIO_SetBits(GPIOE, GPIO_Pin_3)
#define CLR_DQ() 		   GPIO_ResetBits(GPIOE, GPIO_Pin_3)
#define GET_DQ() 		   GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)


void ds18b20_start(void);
uint16_t ds18b20_read(void);




#endif /* __DS18B20_H__ */

