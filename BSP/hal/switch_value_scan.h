#ifndef _SWITCH_VALUE_SCAN_H_
#define _SWITCH_VALUE_SCAN_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "gpio.h"
#include "trace.h"
#include "stdint.h"

#ifndef _SWITCH_VALUE_SCAN_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif

typedef enum 
{
    RETRANSFUSIONPUMP_FEEDBACK = 0
} Scan_Board1_TypeDef;


typedef enum 
{
    LIQUIDLEAKAGE_SENSOR = 0,
    ELECTROMAGNET_OPTICAL1 =1,
    ELECTROMAGNET_OPTICAL2 =2,
    DOOR_HALL =3,
    CENTRIFUGE_FAULT =4
} Scan_Board3_TypeDef;

typedef struct
{
    uint8_t door_switch2;
    uint8_t door_switch1;
    uint16_t weeping_detector;
    uint8_t feedback_pump_state;
    uint8_t door_hoare;
    uint8_t centrifuge_motor_state;
}State_Feedback_TypeDef;

GLOBAL State_Feedback_TypeDef scan_state_feedback;

void arm1_switchscan_gpio_init(void);
void arm3_switchscan_gpio_init(void);
void read_arm1_SensorState(void);
void read_arm3_SensorState(void);

#endif

