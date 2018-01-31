/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_sensor_board1.c
 * Author             : su
 * Date First Issued  : 2013/11/24
 * Description        : This file contains the software implementation for the
 *                      sensor unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/24 | v1.0  |            | initial released, move code from app.c
 *******************************************************************************/

#include "app_sensor_board1.h"

/* Private variables ---------------------------------------------------------*/
#define ADC1_DR_Address    ((u32)0x4001204C)
#define AD_AVERAGE_VALUE_CNT  10

u8 arm3_recv_fun(u8 *p_data, u16 len);
uint8_t draw_pump_current_direction=0;
uint8_t AC_pump_current_direction=0;
uint8_t feedback_pump_current_direction=0;
u16 draw_pump_current_speed=0;
u16 AC_pump_current_speed=0;
u16 feedback_pump_current_speed=0;


static OS_STK sensor_task_stk[SENSOR_TASK_STK_SIZE];
static uint8_t read_draw_pump_current_direction(void);
static uint8_t read_AC_pump_current_direction(void);
static uint8_t read_feedback_pump_current_direction(void);
static u16 read_draw_pump_current_speed(void);
static u16 read_AC_pump_current_speed(void);
static u16 read_feedback_pump_current_speed(void);
static void sensor_task(void *p_arg);
static void read_blood_collect_preasure_value(void);
float average_AD_value(void);
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

/*force sensor initialization*/

static void sensor_task(void *p_arg)
{
    /* Configure ADC1 Channel 14 (PC4) as analog input ****************************/
    ADC1_GPIO_CONFIG(ADC1_CHANNAL15);
    /* Configure ADC1 mode and DMA channal , ADC1,DMA2,channal0,Stream0************/
    ADC1_DMA_Config();
    /*Enables the ADC1 software start conversion. *********************************/
    ADC_SoftwareStartConv(ADC1);
    /*sensor external interrupt */
    user_timer_init(1000);
    arm1_exti_gpio_init();
    arm1sensor_tmr_init();
    /*scan sensors status */
    arm1_switchscan_gpio_init();
    APP_TRACE("enter sensor task!\r\n");
    for (;;)
    {
        /*read blood collect preasure sensor*/
        read_blood_collect_preasure_value();
        /*draw pump current direction of rotation*/
        pump_state1_handle.pump_current_dir.bit.draw_pump = read_draw_pump_current_direction();
        /*AC pump current direction of rotation*/
        pump_state1_handle.pump_current_dir.bit.AC_pump = read_AC_pump_current_direction();

        /*feedback pump current direction of rotation*/
        pump_state1_handle.pump_current_dir.bit.feed_back_pump = read_feedback_pump_current_direction();
        /*draw pump current speed*/
        pump_state1_handle.draw_pump_current_speed = read_draw_pump_current_speed();
        /*AC pump current speed*/
        pump_state1_handle.AC_pump_current_speed = read_AC_pump_current_speed();
        /*feedback pump current speed*/
        pump_state1_handle.feed_back_pump_current_speed = read_feedback_pump_current_speed();

        /*draw pump current distance*/
        switch(read_pump_distance_flag(DRAW_PUMP))
       {
           case 0x01://total
                //send total_distance and its flag
                pump_state2_handle.draw_pump_moved_distance = draw_pump_total_distance;
                pump_state2_handle.single_or_total.bit.draw_pump = 0x01;

           break;
           case 0x00://single
               pump_state2_handle.draw_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM3));
               pump_state2_handle.single_or_total.bit.draw_pump = 0x00;

           break;
           case 0x03://clear total
               pump_state2_handle.draw_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM3));
              // pump_state2_handle.single_or_total.bit.draw_pump = 0x00;
              control_order_r.single_or_total.bit.draw_pump = 0x00;
               draw_pump_total_distance = 0;
           break;
           default:
               APP_TRACE("undnfined single_or_total order!\r\n");
           break;
       }

        /*AC pump current distance*/
         switch(read_pump_distance_flag(AC_PUMP))
        {
            case 0x01://total
                 //send total_distance and its flag
                 pump_state2_handle.AC_pump_moved_distance = AC_pump_total_distance;
                 pump_state2_handle.single_or_total.bit.AC_pump = 0x01;

            break;
            case 0x00://single
                pump_state2_handle.AC_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM2));
                pump_state2_handle.single_or_total.bit.AC_pump = 0x00;

            break;
            case 0x3://clear total
                pump_state2_handle.AC_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM2));
                //pump_state2_handle.single_or_total.bit.AC_pump = 0x00;
                control_order_r.single_or_total.bit.AC_pump = 0x00;
                AC_pump_total_distance = 0;
                APP_TRACE("clear AC_pump_total_distance = %d\r\n",AC_pump_total_distance);
            break;
            default:
                APP_TRACE("undnfined single_or_total order!\r\n");
            break;
        }



        /*feedback pump current distance*/
         switch(read_pump_distance_flag(FEEDBACK_PUMP))
        {
            case 0x01://total
                 //send total_distance and its flag
                 pump_state2_handle.feed_back_pump_moved_distance = feedback_pump_total_distance;
                 pump_state2_handle.single_or_total.bit.feedback_pump = 0x01;

            break;
            case 0x00://single
                pump_state2_handle.feed_back_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM1));
                pump_state2_handle.single_or_total.bit.feedback_pump = 0x00;
                //APP_TRACE("fb_s = %d\r\n", pump_state2_handle.feed_back_pump_moved_distance);
            break;
            case 0x03://clear total
                APP_TRACE("feedback clear total \r\n");
                pump_state2_handle.feed_back_pump_moved_distance = (u16)(get_dc_motor_current_distance(MOTOR_NUM1));
                //pump_state2_handle.single_or_total.bit.feedback_pump = 0x00;
                control_order_r.single_or_total.bit.feedback_pump = 0x00;
                feedback_pump_total_distance = 0;
            break;
            default:
                APP_TRACE("undnfined single_or_total order!\r\n");
            break;
        }


        /*AC bubble status*/
        sensor_state_handle.liquid_level.bit.AC_bubble = arm1_sensor_status.liquid_level.bit.AC_bubble;
        // TODO: for debug
        if(sensor_state_handle.liquid_level.bit.AC_bubble == 0x01)
        {
            APP_TRACE("ac bubble pass!!!\r\n");
            arm1_sensor_status.liquid_level.bit.AC_bubble =0;
            sensor_state_handle.liquid_level.bit.AC_bubble  = 0;
        }
        // TODO: for debug
        /*high liquid level sensor status*/
        sensor_state_handle.liquid_level.bit.high_level = arm1_sensor_status.liquid_level.bit.high_level;

        /*low liquid level sensor status*/
        sensor_state_handle.liquid_level.bit.low_level = arm1_sensor_status.liquid_level.bit.low_level;
//        APP_TRACE("liquid_level.bit.low = %d!\r\n",arm1_sensor_status.liquid_level.bit.low_level);
        /*read hall and liquid level sensor status*/
//        read_hall_liquid_level_status();
        /*draw blood pump initiate*/
        sensor_state_handle.pump_init.bit.draw_pump_init = arm1_sensor_status.pump_init.bit.draw_pump_init;
         // TODO: for debug
        if(sensor_state_handle.pump_init.bit.draw_pump_init == 0x01)
        {
            APP_TRACE("draw_pump_init!!!\r\n");
            arm1_sensor_status.pump_init.bit.draw_pump_init =0;
            sensor_state_handle.pump_init.bit.draw_pump_init  = 0;
        }
         // TODO: for debug
        /*AC pump initiate*/
        sensor_state_handle.pump_init.bit.AC_pump_init = arm1_sensor_status.pump_init.bit.AC_pump_init ;
        // TODO: for debug
       if(sensor_state_handle.pump_init.bit.AC_pump_init == 0x01)
       {
           APP_TRACE("AC_pump_init!!!\r\n");
           arm1_sensor_status.pump_init.bit.AC_pump_init=0;
           sensor_state_handle.pump_init.bit.AC_pump_init  = 0;
       }
          // TODO: for debug
        /*feedback pump initiate*/
        sensor_state_handle.pump_init.bit.feed_back_pump_init = arm1_sensor_status.pump_init.bit.feed_back_pump_init;
           // TODO: for debug
        if( sensor_state_handle.pump_init.bit.feed_back_pump_init == 0x01)
        {
            APP_TRACE("feed_back_pump_init!!!\r\n");
            arm1_sensor_status.pump_init.bit.feed_back_pump_init =0;
            sensor_state_handle.pump_init.bit.feed_back_pump_init  = 0;
        }
           // TODO: for debug
        /*scan sensors of arm1 status*/
        read_arm1_SensorState();
        sensor_state_handle.feedback_pump_state = scan_state_feedback.feedback_pump_state;
        if(sensor_state_handle.feedback_pump_state)
        {
            APP_TRACE("sensor_state_handle.feedback_pump_state = %d\r\n",sensor_state_handle.feedback_pump_state);
        }

      //  OSTimeDlyHMSM(0, 0, 0, 10);

        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}
/*
 * @brief: read the value of blood collect preasure sensor,then convert the unit to mmHg
 * Bit13: positive number  0 ,negative number  1
 * Bit12~0：-1500mmHg~+1500mmHg
 */


 /* 1kg correspond 1.8V (1.8/3.3*4096=2234)
  * 1kg correspond 750mmHg
  * 1mmHg correspond ADC value 1514/750 = 2.98
  */
    static void read_blood_collect_preasure_value(void)
    {
        u16 blood_collect_preasure;
        float ADC_value = 0.0;
        float ADC_average_value = 0.0;
				u8 i = 0;
        ADC_average_value = average_AD_value();
        //APP_TRACE("ad_preasure: %d\r\n",ADC1_convert_value);
        //APP_TRACE("ad_preasure: %d\r\n",ADC_average_value);
        
      	//for(i = 0;i < AD_AVERAGE_VALUE_CNT;i++)
	    //{
	    //    APP_TRACE("ad_value[%d]: %d\r\n",i,ad_value[i]);
        //}
        if((ADC_average_value<4096) && (ADC_average_value>0))
        {
            //y = 0.0001x2 - 0.0496x,y = -1E-07x3 + 0.0004x2 - 0.2307x
           /*hardware circuit voltage rise 1.35V,so the value should subtract 1.35/3.3*4096 = 1675*/
           ADC_value = (float)(ADC_average_value - 790);
            
            if(ADC_value > 0)
            {
                blood_collect_preasure = -0.0000001*ADC_average_value*ADC_average_value*ADC_average_value
                                        +0.0004*ADC_average_value*ADC_average_value -0.2307*ADC_average_value;
                
                /*blood collect preasure value >0, Bit13: 0 */
                //blood_collect_preasure &= 0xdfff;//mask by wq
                blood_collect_preasure &= 0x1fff;
                
               // APP_TRACE("arm1 blood preasure: 0x%x +%d mmHg ADC1_value = %d\r\n",blood_collect_preasure,blood_collect_preasure,(int)ADC_average_value);
            }
            else if(ADC_value < 0)
            {
                blood_collect_preasure = abs((s16)(-0.0000001*ADC_average_value*ADC_average_value*ADC_average_value
                                        +0.0004*ADC_average_value*ADC_average_value -0.2307*ADC_average_value));
                /*blood collect preasure value <0, Bit13: 1 */
                blood_collect_preasure |= 0x2000;
                
				
                //APP_TRACE("arm1 blood preasure: 0x%x -%d mmHg ADC1_value = %d\r\n",blood_collect_preasure,blood_collect_preasure,(int)ADC_average_value);
            }
            else
            {
                blood_collect_preasure = 0x00;
            }
        #if 0                                     //对电压进行处理  
            
            if(ADC_value > 0)
            {
                blood_collect_preasure = (u16)(ADC_value/2.98);
                /*blood collect preasure value >0, Bit13: 0 */
                //blood_collect_preasure &= 0xdfff;//mask by wq
                blood_collect_preasure &= 0x1fff;
                
                APP_TRACE("arm1 blood preasure: 0x%x +%d mmHg ADC1_value = %d\r\n",blood_collect_preasure,blood_collect_preasure,ADC1_convert_value);
            }
            else if(ADC_value < 0)
            {
                blood_collect_preasure = abs((s16)(ADC_value/2.02));
                /*blood collect preasure value <0, Bit13: 1 */
                blood_collect_preasure |= 0x2000;
                
                APP_TRACE("arm1 blood preasure: 0x%x -%d mmHg ADC1_value = %d\r\n",blood_collect_preasure,abs((s16)(ADC_value/2.02)),ADC1_convert_value);
            }
            else
            {
                blood_collect_preasure = 0x00;
            }
        #endif
            sensor_state_handle.blood_collect_preasure = blood_collect_preasure;
    
        }
    }



float average_AD_value(void)
{
    
	float sum = 0.0;
	float max = 0.0;
	float min = 0.0;
    float ad_value[AD_AVERAGE_VALUE_CNT] = {0.0};    
	u8 i;
	for(i = 0;i < AD_AVERAGE_VALUE_CNT;i++)
	{
		ad_value[i] = (float)ADC1_convert_value;
		if(ad_value[i] > max)
		{
			max = ad_value[i];
		}
		if(ad_value[i] < min)
		{
			min = ad_value[i];
		}
		sum += ad_value[i];
		OSTimeDlyHMSM(0, 0, 0, 1);
	}
	return (sum-max-min)/(AD_AVERAGE_VALUE_CNT - 2);
    
}

static uint8_t read_draw_pump_current_direction(void)
{
    if(get_dc_motor_sp(MOTOR_NUM3)==0)
        draw_pump_current_direction=0;
    else if(get_dc_motor_sp(MOTOR_NUM3)<0)
            draw_pump_current_direction=2;//Bit76：10
        else
            draw_pump_current_direction=1;//Bit76：01
    return  draw_pump_current_direction;
}

static uint8_t read_AC_pump_current_direction(void)
{

    if(get_dc_motor_sp(MOTOR_NUM2)==0)
        AC_pump_current_direction=0;
    else if(get_dc_motor_sp(MOTOR_NUM2)<0)
            AC_pump_current_direction=2;//Bit54：10
        else
            AC_pump_current_direction=1;//Bit54：01

    return  AC_pump_current_direction;
}

static uint8_t read_feedback_pump_current_direction(void)
{
    if(get_dc_motor_sp(MOTOR_NUM1)==0)
        feedback_pump_current_direction=0;
    else if(get_dc_motor_sp(MOTOR_NUM1)<0)
            feedback_pump_current_direction=2;//Bit32：10
        else
            feedback_pump_current_direction=1;//Bit32：01
    return  feedback_pump_current_direction;
}

static u16 read_draw_pump_current_speed(void)
{
    if(get_dc_motor_sp(MOTOR_NUM3)<0)
        draw_pump_current_speed = (u16)(-get_dc_motor_sp(MOTOR_NUM3));
    else
        draw_pump_current_speed = (u16)get_dc_motor_sp(MOTOR_NUM3);
    return draw_pump_current_speed;
}

static u16 read_AC_pump_current_speed(void)
{
    if(get_dc_motor_sp(MOTOR_NUM2)<0)
        AC_pump_current_speed = (u16)(-get_dc_motor_sp(MOTOR_NUM2));
    else
        AC_pump_current_speed = (u16)get_dc_motor_sp(MOTOR_NUM2);
    return AC_pump_current_speed;
}

static u16 read_feedback_pump_current_speed(void)
{
    if(get_dc_motor_sp(MOTOR_NUM1)<0)
        feedback_pump_current_speed = (u16)(-get_dc_motor_sp(MOTOR_NUM1));
    else
        feedback_pump_current_speed = (u16)get_dc_motor_sp(MOTOR_NUM1);
    return feedback_pump_current_speed;
}

