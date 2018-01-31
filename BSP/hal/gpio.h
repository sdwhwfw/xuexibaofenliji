/**
  ******************************************************************************
  * @file    gpio.h
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-15
  * @brief   This file contains GPIO HAL functions.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  * 2013/11/2   ZhangQ              Add ARM0 GPIO setting
  ******************************************************************************
  */

#ifndef __GPIO_H__
#define __GPIO_H__


#include "stm32f2xx.h"

typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;


/* LED **********************************************************/
void init_led_gpio(Led_TypeDef Led);
void led_trun_on(Led_TypeDef Led);
void led_trun_off(Led_TypeDef Led);
void led_toggle(Led_TypeDef Led);






/* KEY **********************************************************/
void user_key_init(void);


#if defined(CONTROL_MAIN_BOARD)
// Init ARM0 pause/resume key, stop key, alarm led and alarm buzzer
void init_arm0_exter_gpio(void);
// turn on alarm led
void alarm_led_on(void);
// turn off alarm led
void alarm_led_off(void);
// turn on alarm buzzer
void alarm_buzzer_on(void);
// turn off alarm buzzer
void alarm_buzzer_off(void);
// get alarm led status, 1-ON, 0-OFF
u8 get_alarm_status(void);

#endif

/* power gate control only for arm3*/
#ifdef SLAVE_BOARD_3

typedef enum
{
  GATE1 = 0,
  GATE2 = 1
} GATE_TypeDef;

#define IS_GATE_TYPEDEF(GATE) ((GATE) == GATE1 || (GATE) == GATE2)

void power_gate_init(void);
void enable_power_gate(GATE_TypeDef gate);
void disable_power_gate(GATE_TypeDef gate);



#endif



#endif /* __GPIO_H__ */

