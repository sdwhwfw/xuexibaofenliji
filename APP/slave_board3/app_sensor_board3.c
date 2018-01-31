/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_board3.c
 * Author             : su
 * Date First Issued  : 2013/11/23
 * Description        : This file contains the software implementation for the
 *                      sensor unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/23 | v1.0  |            | initial released, move code from app.c
 *******************************************************************************/

#include "app_sensor_board3.h"
#include "app_centrifuge_control.h"//for debug
/* Private variables ---------------------------------------------------------*/
#define ADC1_DR_Address    ((u32)0x4001204C)
u8 arm3_recv_fun(u8 *p_data, u16 len);
static OS_STK sensor_task_stk[SENSOR_TASK_STK_SIZE];
static void read_volage_state(void);
static void sensor_task(void *p_arg);
static void read_centrifugemotor_pressure_value(void);
/* sensor task initialization*/

void init_sensor_task(void)
{
    INT8U os_err;
	/* sensor task ************************************************************/

    os_err = OSTaskCreateExt((void (*)(void *)) sensor_task,
                        (void          * ) 0,
                        (OS_STK        * )&sensor_task_stk[SENSOR_TASK_STK_SIZE - 1],
                        (INT8U           ) SENSOR_TASK_PRIO,
                        (INT16U          ) SENSOR_TASK_PRIO,
                        (OS_STK        * )&sensor_task_stk[0],
                        (INT32U          ) SENSOR_TASK_STK_SIZE,
                        (void          * )0,
                        (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    assert_param(OS_ERR_NONE == os_err);
    OSTaskNameSet(SENSOR_TASK_PRIO, (INT8U *)"sensor", &os_err);
}

/*Voltage sampling and judge*/
static void read_volage_state(void)
{
    Read_Volage_TypeDef Read_Volage ;

    Read_Volage.volage_5V = ADC_ConvertedValue[0];
    Read_Volage.volage_70V = ADC_ConvertedValue[2];
    Read_Volage.volage_24V = ADC_ConvertedValue[3];
    Read_Volage.volage_S16 = ADC_ConvertedValue[4];
    Read_Volage.volage_S19 = ADC_ConvertedValue[5];
    Read_Volage.volage_negative_12V= ADC_ConvertedValue[6];
    Read_Volage.volage_positive_12V= ADC_ConvertedValue[7];
#if 0
    APP_TRACE("Read_Volage.volage_5V = %d\r\n",Read_Volage.volage_5V);
    APP_TRACE("Read_Volage.volage_70V = %d\r\n",Read_Volage.volage_70V);
    APP_TRACE("Read_Volage.volage_24V = %d\r\n",Read_Volage.volage_24V);
    APP_TRACE("Read_Volage.volage_S16 = %d\r\n",Read_Volage.volage_S16);
    APP_TRACE("Read_Volage.volage_S19 = %d\r\n",Read_Volage.volage_S19);
    APP_TRACE("Read_Volage.volage_negative_12V = %d\r\n",Read_Volage.volage_negative_12V);
    APP_TRACE("Read_Volage.volage_positive_12V = %d\r\n",Read_Volage.volage_positive_12V);
#endif

    if((Read_Volage.volage_5V<2668)&&(Read_Volage.volage_5V>2917))
        motor_errcode_handle.errcode_voltage.bit.volage_5V = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_5V = ABNORMAL;

    if((Read_Volage.volage_70V<3875)&&(Read_Volage.volage_70V>3820))
        motor_errcode_handle.errcode_voltage.bit.volage_70V = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_70V= ABNORMAL;

    if((Read_Volage.volage_24V<3836)&&(Read_Volage.volage_24V>3436))
        motor_errcode_handle.errcode_voltage.bit.volage_24V = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_24V = ABNORMAL;

    if((Read_Volage.volage_S16<3836)&&(Read_Volage.volage_S16>3436))
        motor_errcode_handle.errcode_voltage.bit.volage_S16 = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_S16 = ABNORMAL;

    if((Read_Volage.volage_S19<3836)&&(Read_Volage.volage_S19>3436))
        motor_errcode_handle.errcode_voltage.bit.volage_S19 = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_S19 = ABNORMAL;

    if((Read_Volage.volage_negative_12V<2830)&&(Read_Volage.volage_negative_12V>2607))
        motor_errcode_handle.errcode_voltage.bit.volage_negative_12V = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_negative_12V = ABNORMAL;

    if((Read_Volage.volage_positive_12V<2184)&&(Read_Volage.volage_positive_12V>2011))
        motor_errcode_handle.errcode_voltage.bit.volage_positive_12V = NORMAL;
    else
        motor_errcode_handle.errcode_voltage.bit.volage_positive_12V = ABNORMAL;
}

/*read the centrifuge motor pressure sensor value*/

/* 1kg correspond 0.893V (0.893/3.3*4096=1108)
 * 1kg correspond 750mmHg
 * 1mmHg correspond ADC value 1108/750 = 1.48
 */

static void read_centrifugemotor_pressure_value(void)
{
    uint16_t PressureSensor_value=0;
    s32 ADC_value=0;
    //APP_TRACE("ADC_ConvertedValue is%d\r\n",ADC_ConvertedValue[1]);
    if(ADC_ConvertedValue[1]<4096&&ADC_ConvertedValue[1]>0)
    {
        /*hardware circuit voltage rise 1.35V,zero point is 0,so the value should subtract 1.35/3.3*4096*/
        ADC_value = (s32)ADC_ConvertedValue[1]-1675;
        if(ADC_value>0)
        {
            PressureSensor_value=ADC_value/1.48;
            /*blood collect preasure value >0, Bit13: 0 */
            PressureSensor_value &= 0xdfff;
        }
        else if(ADC_value<0)
        {
            PressureSensor_value=-(ADC_value/1.48);
            /*blood collect preasure value <0, Bit13: 1 */
            PressureSensor_value |= 0x2000;
        }
        else
        {
            PressureSensor_value = 0;
        }
        sensor_state_handle.centrifuge_motor_preasure = PressureSensor_value;
       //APP_TRACE("centrifuge_motor_preasure is  = %d\r\n",sensor_state_handle.centrifuge_motor_preasure );   
    }
}

static void sensor_task(void *p_arg)
{

    /* Configure all ADC1 Channels (channal 08-15) as analog input ****************************/
    ADC1_GPIO_ALLCONFIG();
    /* Configure ADC1 mode and DMA channal , ADC1,DMA2,channal0,Stream0************/
    ADC1_DMA_Config();
    /*Enables the ADC1 software start conversion. *********************************/
    ADC_SoftwareStartConv(ADC1);
    /* Configure Channel DAC1 as analog output************************/
    DAC_GPIO_Config(DAC1_OUT);
    /* Configure DAC1(PA4) output DAC_Data1/4096*Vref ************/
    DAC_SINGLE_Config(DAC_Output1, 4095);
    /*sensor external interrupt */
    user_timer_init(1000);

    arm3_exti_gpio_init();
    init_arm3sensor_tmr();
    /*arm3 switch sensors GPIO init*/
    arm3_switchscan_gpio_init();
    APP_TRACE("enter sensor task!\r\n");

    for (;;)
    {
        /*read voltage status */
        read_volage_state();
        /*read the centrifuge motor pressure sensor value*/
        read_centrifugemotor_pressure_value();
        /*scan sensors of arm3 status*/
        read_arm3_SensorState();
        /*centrifuge motor current direction of rotation*/
        pump_state1_handle.motor_current_dir.bit.centrifuge_motor = centrifuge_direction;
        /*centrifuge motor current speed*/
        pump_state1_handle.centrifuge_motor_current_speed = (u16)centrifuge_motor_current_speed;

        /*weeping detector*/
        sensor_state_handle.weeping_detector = scan_state_feedback.weeping_detector;
//        APP_TRACE("1111weeping_detector is %d!!!!\r\n",scan_state_feedback.weeping_detector);
        /*read status of electromagnet optical1*/
        sensor_state_handle.sensor.bit.door_switch1 = scan_state_feedback.door_switch1;
        // TODO: for debug 2014.08.02
        if(sensor_state_handle.sensor.bit.door_switch1 == 0x01)
        {
           // APP_TRACE("door_switch1 is %d!!!!!!!!!!!\r\n",sensor_state_handle.sensor.bit.door_switch1);
            scan_state_feedback.door_switch1 = 0;
            sensor_state_handle.sensor.bit.door_switch1 = 0;
        }
         // TODO: for debug 2014.08.02
        /*read status of electromagnet optical2*/
        sensor_state_handle.sensor.bit.door_switch2 = scan_state_feedback.door_switch2;
         // TODO: for debug 2014.08.02
         if(sensor_state_handle.sensor.bit.door_switch2 == 0x01)
         {
           //  APP_TRACE("door_switch2 is %d!!!!!!!!!!!\r\n",sensor_state_handle.sensor.bit.door_switch2);
             sensor_state_handle.sensor.bit.door_switch2 = 0;
             scan_state_feedback.door_switch2 = 0;
         }
          // TODO: for debug 2014.08.02


        /*read status of door hell*/
        sensor_state_handle.sensor.bit.door_hoare = scan_state_feedback.door_hoare;
 //       APP_TRACE("door_hoare is %d!!!!!!!!!!!\r\n",sensor_state_handle.sensor.bit.door_hoare);
        /*read status of centrifuge motor*/
        sensor_state_handle.centrifuge_motor_state = scan_state_feedback.centrifuge_motor_state;
//        APP_TRACE("centrifuge_motor_stateis %d!!!!!!!!!!!\r\n",sensor_state_handle.centrifuge_motor_state);
        /*read current cassette location*/
        read_optical_status_arm3();
        sensor_state_handle.sensor.bit.cassette_location=arm3_sensor_status.sensor.bit.cassette_location;
//        APP_TRACE("cassette_location is %d\r\n",arm3_sensor_status.sensor.bit.cassette_location);
        OSTimeDlyHMSM(0, 0, 1, 10);

       // APP_TRACE("kaxia  xiaweizhi : %d\r\n",GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1));

    }

}



