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


#define  SkipROM         0xCC     //跳过ROM
#define  SearchROM       0xF0     //搜索ROM
#define  ReadROM         0x33     //读ROM
#define  MatchROM        0x55     //匹配ROM
#define  AlarmROM        0xEC     //告警ROM
#define  StartConvert    0x44     //开始温度转换，在温度转换期间总线上输出0，转换结束后输出1
#define  ReadScratchpad  0xBE     //读暂存器的9个字节
#define  WriteScratchpad 0x4E     //写暂存器的温度告警TH和TL
#define  CopyScratchpad  0x48     //将暂存器的温度告警复制到EEPROM，在复制期间总线上输出0，复制完后输出1
#define  RecallEEPROM    0xB8     //将EEPROM的温度告警复制到暂存器中，复制期间输出0，复制完成后输出1
#define  ReadPower       0xB4     //读电源的供电方式：0为寄生电源供电；1为外部电源供电

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

