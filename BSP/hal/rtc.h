/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : rtc.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/10
 * Description        : This file contains the software implementation for the
 *                      RTC unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/10 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/
#ifndef _RTC_H_
#define _RTC_H_
#include "stm32f2xx.h"





u32 rtc_init(void);

void clear_bkpsram(void);
void write_byte_to_bkpsram(u32 add_offset, void* p_data, u32 len);
void read_byte_from_bkpsram(u32 add_offset, void* p_data, u32 len);
void write_u32_to_bkpsram(u32 add_offset, void* p_data, u32 len);
void read_u32_from_bkpsram(u32 add_offset, void* p_data, u32 len);
void write_backup_reg(u8 index, u32 data);
u32  read_backup_reg(u8 index);
void set_rtc_time(u8 second, u8 minute, u8 hour);
void get_rtc_time(u8* second, u8* minute, u8* hour);
void set_rtc_date(u32 weekday, u32 date, u32 month, u32 year);
void get_rtc_date(u32* weekday, u32* date, u32* month, u32* year);

// use ADC to measure VBAT
void init_VBAT_measure(void);
void get_vbat_value(u32* VBAT_V, u32* VBAT_mV);



#endif /* _RTC_H_ */

