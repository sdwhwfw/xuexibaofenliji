
/**
  ******************************************************************************
  * @file    adc_dac.c
  * @author  su
  * @version V0.0.1
  * @date    2013-11-14
  * @brief   ADC and DAC HAL functions.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#ifndef _ADC_DAC_C_
#define _ADC_DAC_C_

#include "adc_dac.h"
#include "trace.h"
/** @addtogroup DAC_SignalsGeneration
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((u32)0x4001204C)
//#define ADC2_DR_Address    ((u32)0x4001214C)
//#define ADC3_DR_Address    ((u32)0x4001224C)
/*
 *==========================================================================
 * ARM1 has a channal ADC and dual channals DAC :
 * ADC:   ADC1_channal 14 (PC4)
 * DAC: DAC1_OUT(PA4)
 *      DAC2_OUT(PA5)

 * ARM2 has a channal ADC and dual channals DAC :
 * ADC:   ADC1_channal 15 (PC5)
 * DAC: DAC1_OUT(PA4)
 *      DAC2_OUT(PA5)

 * ARM3 has 8 channals ADC and a channal DAC :
 * ADC:   ADC1_channal 10 (PC0)  ADC1_channal 11 (PC1) ADC1_channal 12 (PC2) ADC1_channal 13 (PC3)
          ADC1_channal 14 (PC5)  ADC1_channal 08 (PB0) ADC1_channal 09 (PB1)
 * DAC: DAC1_OUT(PA4)
 *

 *==========================================================================
 */

#if defined(USE_ARM1_REV_0_1)|| defined(USE_ARM1_REV_0_2)
#define ADC1_CHANNALS_NUM           1
#define ADC1_CHANNAL15_PIN          GPIO_Pin_5
#define ADC1_CHANNAL15_PORT         GPIOC
#define ADC1_CHANNAL15_CLK          RCC_AHB1Periph_GPIOC

#define DAC_CHANNALS_NUM            2
#define DAC1_OUT_PIN                GPIO_Pin_4
#define DAC1_OUT_PORT               GPIOA
#define DAC1_OUT_CLK                RCC_AHB1Periph_GPIOA

#define DAC2_OUT_PIN                GPIO_Pin_5
#define DAC2_OUT_PORT               GPIOA
#define DAC2_OUT_CLK                RCC_AHB1Periph_GPIOA

#elif defined(USE_ARM2_REV_0_1)|| defined(USE_OPEN_207Z_MINI_BOARD)|| defined(USE_OPEN_207Z_BOARD)
#define ADC1_CHANNALS_NUM           1
#define ADC1_CHANNAL15_PIN          GPIO_Pin_5
#define ADC1_CHANNAL15_PORT         GPIOC
#define ADC1_CHANNAL15_CLK          RCC_AHB1Periph_GPIOC

#define DAC_CHANNALS_NUM            2
#define DAC1_OUT_PIN                GPIO_Pin_4
#define DAC1_OUT_PORT               GPIOA
#define DAC1_OUT_CLK                RCC_AHB1Periph_GPIOA


#define DAC2_OUT_PIN                GPIO_Pin_5
#define DAC2_OUT_PORT               GPIOA
#define DAC2_OUT_CLK                RCC_AHB1Periph_GPIOA

#elif defined(USE_ARM3_REV_0_1)|| defined(USE_PORT_207Z_BOARD) || defined(USE_OPEN_207Z_BOARD) || defined(NO_RTOS_BOARD)

/* 5V voltage check*/
#define ADC1_CHANNALS_NUM           8
#define ADC1_CHANNAL08_PIN          GPIO_Pin_0
#define ADC1_CHANNAL08_PORT         GPIOB
#define ADC1_CHANNAL08_CLK          RCC_AHB1Periph_GPIOB
/* Centrifugal pressure sensor */
#define ADC1_CHANNAL09_PIN          GPIO_Pin_1
#define ADC1_CHANNAL09_PORT         GPIOB
#define ADC1_CHANNAL09_CLK          RCC_AHB1Periph_GPIOB
/* 70V voltage check*/
#define ADC1_CHANNAL10_PIN          GPIO_Pin_0
#define ADC1_CHANNAL10_PORT         GPIOC
#define ADC1_CHANNAL10_CLK          RCC_AHB1Periph_GPIOC
/* 24V voltage check*/
#define ADC1_CHANNAL11_PIN          GPIO_Pin_1
#define ADC1_CHANNAL11_PORT         GPIOC
#define ADC1_CHANNAL11_CLK          RCC_AHB1Periph_GPIOC
/* S16 voltage check*/
#define ADC1_CHANNAL12_PIN          GPIO_Pin_2
#define ADC1_CHANNAL12_PORT         GPIOC
#define ADC1_CHANNAL12_CLK          RCC_AHB1Periph_GPIOC
/* S19 voltage check*/
#define ADC1_CHANNAL13_PIN          GPIO_Pin_3
#define ADC1_CHANNAL13_PORT         GPIOC
#define ADC1_CHANNAL13_CLK          RCC_AHB1Periph_GPIOC
/* -12V voltage check*/
#define ADC1_CHANNAL14_PIN          GPIO_Pin_4
#define ADC1_CHANNAL14_PORT         GPIOC
#define ADC1_CHANNAL14_CLK          RCC_AHB1Periph_GPIOC
/* 12V voltage check*/
#define ADC1_CHANNAL15_PIN          GPIO_Pin_5
#define ADC1_CHANNAL15_PORT         GPIOC
#define ADC1_CHANNAL15_CLK          RCC_AHB1Periph_GPIOC

#define DAC_CHANNALS_NUM            2
/* liquid leakage sensor*/
#define DAC1_OUT_PIN                GPIO_Pin_4
#define DAC1_OUT_PORT               GPIOA
#define DAC1_OUT_CLK                RCC_AHB1Periph_GPIOA

#define DAC2_OUT_PIN                GPIO_Pin_5
#define DAC2_OUT_PORT               GPIOA
#define DAC2_OUT_CLK                RCC_AHB1Periph_GPIOA
#endif

#if defined(USE_ARM1_REV_0_1)|| defined(USE_ARM1_REV_0_2)
static GPIO_TypeDef* ADC1_PORT[ADC1_CHANNALS_NUM] = { ADC1_CHANNAL15_PORT};
static const uint16_t ADC1_PIN[ADC1_CHANNALS_NUM] = { ADC1_CHANNAL15_PIN };
static const uint32_t ADC1_CLK[ADC1_CHANNALS_NUM] = { ADC1_CHANNAL15_CLK };
static const uint16_t  DAC_PIN[DAC_CHANNALS_NUM] =  {DAC1_OUT_PIN, DAC2_OUT_PIN};
#elif defined(USE_ARM2_REV_0_1)|| defined(USE_OPEN_207Z_MINI_BOARD)|| defined(USE_OPEN_207Z_BOARD)
static GPIO_TypeDef* ADC1_PORT[ADC1_CHANNALS_NUM] = { ADC1_CHANNAL15_PORT};
static const uint16_t ADC1_PIN[ADC1_CHANNALS_NUM] = { ADC1_CHANNAL15_PIN };
static const uint32_t ADC1_CLK[ADC1_CHANNALS_NUM] = { ADC1_CHANNAL15_CLK };
static const uint16_t  DAC_PIN[DAC_CHANNALS_NUM] =  {DAC1_OUT_PIN, DAC2_OUT_PIN};

#elif defined(USE_ARM3_REV_0_1)|| defined(USE_PORT_207Z_BOARD) || defined(USE_OPEN_207Z_BOARD) || defined(NO_RTOS_BOARD)

static GPIO_TypeDef* ADC1_PORT[ADC1_CHANNALS_NUM] = {ADC1_CHANNAL08_PORT,ADC1_CHANNAL09_PORT,
                                                     ADC1_CHANNAL10_PORT,ADC1_CHANNAL11_PORT,
                                                     ADC1_CHANNAL12_PORT,ADC1_CHANNAL13_PORT,
                                                     ADC1_CHANNAL14_PORT,ADC1_CHANNAL15_PORT};
static const uint16_t ADC1_PIN[ADC1_CHANNALS_NUM] = {ADC1_CHANNAL08_PIN, ADC1_CHANNAL09_PIN,
                                                     ADC1_CHANNAL10_PIN, ADC1_CHANNAL11_PIN,
                                                     ADC1_CHANNAL12_PIN, ADC1_CHANNAL13_PIN,
                                                     ADC1_CHANNAL14_PIN, ADC1_CHANNAL15_PIN};
static const uint32_t ADC1_CLK[ADC1_CHANNALS_NUM] = {ADC1_CHANNAL08_CLK, ADC1_CHANNAL09_CLK,
                                                     ADC1_CHANNAL10_CLK, ADC1_CHANNAL11_CLK,
                                                     ADC1_CHANNAL12_CLK, ADC1_CHANNAL13_CLK,
                                                     ADC1_CHANNAL14_CLK, ADC1_CHANNAL15_CLK};
static const uint16_t  DAC_PIN[DAC_CHANNALS_NUM] =  {DAC1_OUT_PIN, DAC2_OUT_PIN};

#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* If you want to use the Vbat,you should enable this variable*/
//__IO uint32_t VBATVoltage = 0;


/**
  * @brief  This function is ADC GPIO configuration.
  *         ADC1_GPIO_CONFIG(ADC1_CHANNALx)
  * @param  ADC_CHANNALx   you can set x as 08,09,10,11,12,13,14,15
  * @retval None
*/

void ADC1_GPIO_CONFIG(ADC1_TypeDef ADC_CHANNALx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable ADC clock */
    RCC_AHB1PeriphClockCmd(ADC1_CLK[ADC_CHANNALx], ENABLE);
    /* Configure which pin  as analog input */
    GPIO_InitStructure.GPIO_Pin =  ADC1_PIN[ADC_CHANNALx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(ADC1_PORT[ADC_CHANNALx], &GPIO_InitStructure);
}

/*
 * @brief  If you want to use all channals ,you can use this function.
 *         but now all channals means 08,09,10,11,12,13,14,15.
 *         ADC1_GPIO_ALLCONFIG()
 * @param  None
 * @retval None
 */
#if defined(USE_ARM3_REV_0_1)|| defined(USE_PORT_207Z_BOARD)  || defined(NO_RTOS_BOARD)  //|| defined(USE_OPEN_207Z_MINI_BOARD)|| defined(USE_OPEN_207Z_BOARD)
void ADC1_GPIO_ALLCONFIG(void)
{
   ADC1_GPIO_CONFIG(ADC1_CHANNAL08);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL09);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL10);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL11);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL12);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL13);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL14);
   ADC1_GPIO_CONFIG(ADC1_CHANNAL15);
}
#endif

void ADC_DMA_NVIC_config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel					 = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_STREAM0_PRE_PRI;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = DMA2_STREAM0_SUB_PRI;
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	APP_TRACE("enter ADC_DMA_NVIC_config !\r\n");
}

/*
 * @brief  This function is ADC1 and use DMA2 configuration.
 *
 *         ADC1_DMA_Config();
 * @param  None
 * @retval None
 */

void ADC1_DMA_Config(void)
{
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* DMA2 Stream0 channe0 configuration **************************************/
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;

    #if defined(USE_ARM3_REV_0_1)||defined(_DEBUG)
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 8;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    #endif/*defined(USE_ARM3_REV_0_1)||defined(_DEBUG)*/
	#if defined(USE_ARM1_REV_0_1)||defined(USE_ARM1_REV_0_2)
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1_convert_value;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	//#elif defined(USE_ARM2_REV_0_1)||defined(USE_OPEN_207Z_MINI_BOARD)
    #elif defined(USE_OPEN_207Z_MINI_BOARD)||defined(USE_ARM2_REV_0_1)|| defined(USE_OPEN_207Z_BOARD)
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1_convert_values;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 100;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    #endif/*defined(USE_ARM1_REV_0_1)||defined(USE_ARM2_REV_0_1)*/

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_6Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv=0;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

    #if defined(USE_ARM3_REV_0_1)||defined(_DEBUG)
    ADC_InitStructure.ADC_NbrOfConversion = 8;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    #endif/*defined(SLAVE_BOARD_3)||defined(_DEBUG)*/

    #if defined(USE_OPEN_207Z_MINI_BOARD)||defined(USE_ARM1_REV_0_1)|| \
        defined(USE_ARM1_REV_0_2)||defined(USE_ARM2_REV_0_1)|| defined(USE_OPEN_207Z_BOARD)
    ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    #endif/*defined(SLAVE_BOARD_1)||defined(SLAVE_BOARD_2)*/

    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channe0-5,10,11 configuration *************************************/

    #if defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_3Cycles);
    #endif/*SLAVE_BOARD_1*/

    #if defined(USE_OPEN_207Z_MINI_BOARD)||defined(USE_ARM2_REV_0_1)|| defined(USE_OPEN_207Z_BOARD)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_144Cycles);
    #endif/*SLAVE_BOARD_2*/

    #if defined(USE_ARM3_REV_0_1)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 7, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 8, ADC_SampleTime_3Cycles);
    #endif/*SLAVE_BOARD_3*/

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	/*ADC_DMA_NVIC_config*/
	ADC_DMA_NVIC_config();
	#if defined(USE_ARM3_REV_0_1)||defined(USE_ARM1_REV_0_1) || defined(USE_ARM1_REV_0_2)
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */

    ADC_Cmd(ADC1, ENABLE);
	#endif
}

void ADC_start(void)
{
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConv(ADC1);
}

void ADC_stop(void)
{
	ADC_DMACmd(ADC1, DISABLE);
    ADC_Cmd(ADC1, DISABLE);
}


/*
 * @brief  This function is DAC GPIO configuration.
 *         DAC_GPIO_Config(DAC1_OUTx)
 * @param  DACx_OUT   you can set x as 01,02.DAC1_OUT is PA4 ,DAC2_OUT is PA5

 * Once the DAC channel is enabled, the corresponding GPIO pin is automatically
 * connected to the DAC converter. In order to avoid parasitic consumption,
 * the GPIO pin should be configured in analog
 */


void DAC_GPIO_Config(DAC_GPIO_TypeDef DACx_OUT)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable peripheral clocks ------------------------------------------------*/
    /* GPIOA Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    /* DAC Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* TIM2 Periph clock enable ,if you want to output Triangle,you should enable the TIM6*/
    //  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    //  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* DAC1_OUT &DAC2_OUT (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration */
    GPIO_InitStructure.GPIO_Pin = DAC_PIN[DACx_OUT];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
 * @brief  If you want to use all DAC channals ,you can use this function.
 *         DAC_GPIO_ALLCONFIG();
 * @param  None
*/
void  DAC_GPIO_ALLCONFIG(void)
 {
    DAC_GPIO_Config(DAC1_OUT);
    DAC_GPIO_Config(DAC2_OUT);
 }

/**
  * @brief  Set  the DAC channal you want to use and the value to putout.
  * @param  DAC_Outputx: You can select Output1 or Output2, Output1 is PA4,Output2 is PA5.
  * @param  DAC_Data: This parameter must be between 1 to 4095.
  *
  * @note   If the Vref is 3.3V ,it will output voltage as DAC_Data/4095*3.3V
  * @retval None
  */

void DAC_SINGLE_Config(DAC_OUTPUT_TypeDef DAC_Outputx, uint16_t DAC_Data)
{
    DAC_InitTypeDef  DAC_InitStructure;

    /* DAC channel1 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Outputx, &DAC_InitStructure);

    /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
       automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Outputx, ENABLE);
    /* Set DAC dual channel DHR12RD register */
    if(DAC_Outputx==DAC_Output1)
    {
        DAC_SetChannel1Data(DAC_Align_12b_R, DAC_Data);
    }
    if(DAC_Outputx==DAC_Output2)
    {
        DAC_SetChannel2Data(DAC_Align_12b_R, DAC_Data);
    }
}

/**
  * @brief  If you want to use two channals DAC output ,you can use this function.
  * @param  DAC_Data1: This parameter must be between 1 to 4095.
  * @param  DAC_Data2: This parameter must be between 1 to 4095.
  * @note   If the Vref is 3.3V ,it will output voltage as DAC_Data/4095*3.3V.
  * @retval None
  */

void DAC_DUAL_Config(uint16_t DAC_Data1, uint16_t DAC_Data2)
{
    DAC_SINGLE_Config(DAC_Output1,DAC_Data1);
    DAC_SINGLE_Config(DAC_Output2,DAC_Data2);
}
/**
  * @brief  If you want to output Triangle,you can use this function.
  * @param
  * @param
  * @note
  * @retval None
  */

void DAC_Triangle_Config(void)
{
    DAC_InitTypeDef  DAC_InitStructure;

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
     connected to the DAC converter. In order to avoid parasitic consumption,
     the GPIO pin should be configured in analog */

//    DAC_GPIO_Config();
    TIM6_Config();
    /* DAC channel1 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_2047;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* DAC channel2 Configuration */
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
       automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_1, ENABLE);

    /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is
       automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_2, ENABLE);

    /* Set DAC dual channel DHR12RD register */
    DAC_SetDualChannelData(DAC_Align_12b_R, 0x100, 0x100);

}


static void TIM6_Config(void)
{
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* --------------------------------------------------------
	TIM3 input clock (TIM6CLK) is set to 2 * APB1 clock (PCLK1),
	since APB1 prescaler is different from 1.
	TIM6CLK = 2 * PCLK1
	TIM6CLK = HCLK / 2 = SystemCoreClock /2

	TIM6 Update event occurs each TIM6CLK/256

	Note:
	SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
	Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	function to update SystemCoreClock variable value. Otherwise, any configuration
	based on this variable will be incorrect.

	----------------------------------------------------------- */
	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 0x1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0xA;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	/* TIM6 TRGO selection */
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);
}

/**
  * @brief  ADC1 Channel Vbat configuration (DMA, ADC, CLK)
  * @param  None
  * @retval None
  */
void ADC1_ChVbat_DMA_Config(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;

	/* Enable peripheral clocks *************************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA2_Stream0 channel0 configuration **************************************/
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1_convert_value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	/* DMA2_Stream0 enable */
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* ADC1 regular channel18 (VBAT) configuration ******************************/
    //ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat, 1, ADC_SampleTime_15Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_15Cycles);

    /* Enable VBAT channel */
    //ADC_VBATCmd(ENABLE);
    ADC_TempSensorVrefintCmd(ENABLE);

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    /* Enable ADC1 **************************************************************/
    ADC_Cmd(ADC1, ENABLE);
}


void DMA2_Stream0_IRQHandler(void)
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
		ADC_stop();
    }
    OSIntExit();
}

#endif


