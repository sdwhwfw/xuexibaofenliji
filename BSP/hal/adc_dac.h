/**
  ******************************************************************************
  * @file    adc_dac.h
  * @author  SU
  * @version V0.0.1
  * @date    2013-11-14
  * @brief   This file contains ADC and DAC HAL functions.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

#ifndef __ADC_DAC_H__
#define __ADC_DAC_H__


#include "stm32f2xx.h"
#include "ucos_ii.h"

#ifndef _ADC_DAC_C_
#define GLOBAL extern
#else
#define GLOBAL
#endif


#define ADC1_DR_Address    ((u32)0x4001204C)
#define ADC2_DR_Address    ((u32)0x4001214C)
#define ADC3_DR_Address    ((u32)0x4001224C)


#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410

GLOBAL  __IO uint16_t  ADC_ConvertedValue[8];
GLOBAL  __IO uint16_t  ADC1_convert_value;

GLOBAL  __IO uint16_t  ADC1_convert_values[100];

//GLOBAL OS_EVENT 	*ADC_sampling_result;

#if defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2)
    typedef enum
    {
        ADC1_CHANNAL15 = 0

    } ADC1_TypeDef;

#elif defined(USE_ARM2_REV_0_1)|| defined(USE_OPEN_207Z_MINI_BOARD)|| defined(USE_OPEN_207Z_BOARD)
    typedef enum
    {
        ADC1_CHANNAL15 = 0

    } ADC1_TypeDef;


#elif defined(USE_ARM3_REV_0_1)|| defined(USE_PORT_207Z_BOARD) || defined(USE_OPEN_207Z_BOARD) || defined(NO_RTOS_BOARD)

    typedef enum
    {
        ADC1_CHANNAL08 = 0,
        ADC1_CHANNAL09 = 1,
        ADC1_CHANNAL10 = 2,
        ADC1_CHANNAL11 = 3,
        ADC1_CHANNAL12 = 4,
        ADC1_CHANNAL13 = 5,
        ADC1_CHANNAL14 = 6,
        ADC1_CHANNAL15 = 7
    } ADC1_TypeDef;

#endif

typedef enum DAC_CHANNAL
{
    DAC_Output1= DAC_Channel_1,
    DAC_Output2= DAC_Channel_2
} DAC_OUTPUT_TypeDef;


typedef enum
{
    DAC1_OUT= 0,
    DAC2_OUT= 1
} DAC_GPIO_TypeDef;

/* ADC **********************************************************/
void ADC1_ChVbat_DMA_Config(void);
void ADC1_GPIO_CONFIG(ADC1_TypeDef ADC_CHANNALx);
void ADC1_GPIO_ALLCONFIG(void);
void ADC1_DMA_Config(void);


/* DAC **********************************************************/
void DAC_GPIO_Config(DAC_GPIO_TypeDef DACx_OUT);
void DAC_GPIO_ALLCONFIG(void);
void DAC_SINGLE_Config(DAC_OUTPUT_TypeDef DAC_Outputx, uint16_t DAC_Data);
void DAC_DUAL_Config(uint16_t DAC_Data1, uint16_t DAC_Data2);

void ADC_start(void);
void ADC_stop(void);

/* Private function prototypes -----------------------------------------------*/
static void TIM6_Config(void);

#undef GLOBAL
#endif /* __ADC_DAC_H__ */

