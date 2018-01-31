/*-------------------------------------------------------------------------
 * File   : bsp.h
 * Created: 2013.03.22
 *-------------------------------------------------------------------------*/

#ifndef __BSP_H__
#define __BSP_H__

#include "stm32f2xx.h"
#include "ucos_ii.h"
//#include "GLCD.h"
#ifndef USE_ARM0_REV_0_1
#include "adc_dac.h"
#endif
#include "watchdog.h"
#include "can.h"
#include "gpio.h"
#include "basic_timer.h"
#include "usart.h"
#include "ds18b20.h"
#include "sram.h"
#include "delay.h"
#include "comx100.h"
#include "rtc.h"
#include "pwm.h"

/* must be called at first for init the hardware */
void bsp_init(void);

#endif /* __BSP_H__ */

