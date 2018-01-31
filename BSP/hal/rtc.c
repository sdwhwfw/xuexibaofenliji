/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : rtc.c
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/10/10
 * Description        : This file contains the software implementation for the
 *                      RTC unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/11 | v1.0  | Bruce.zhu  | initial released
 *******************************************************************************/

#include "rtc.h"

#define RTC_BKP_DR_NUMBER   20
#define RTC_FIRST_DATA      0x32F2

/* Private macro -------------------------------------------------------------*/
#define VBAT_ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)

/* Private variables ---------------------------------------------------------*/

// measure VBAT
static __IO uint16_t ADC1_converted_value = 0;


static u32 BKP_data_reg[RTC_BKP_DR_NUMBER] =
{
	RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2,
	RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
	RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8,
	RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11,
	RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14,
	RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17,
	RTC_BKP_DR18,  RTC_BKP_DR19
};



static void rtc_config(void);


/**
 * @brief  init RTC and BKPSRAM
 * @retval 1: first init
 *         0: RTC and BKPSRAM has been init
 */
u32 rtc_init(void)
{
	u32 ret = 0;

	/* Enable the PWR APB1 Clock Interface */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	if (read_backup_reg(0) != RTC_FIRST_DATA)
	{
		/* RTC Configuration */
		rtc_config();

		/* clear memory */
		clear_bkpsram();

		/* first init RTC */
		ret = 1;
	}
	else
	{
		/* Wait for RTC APB registers synchronisation */
		RTC_WaitForSynchro();

		/*	Backup SRAM ***************************************************************/
		/* Enable BKPSRAM Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

		ret = 0;
	}

	return ret;
}



/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void rtc_config(void)
{
	RTC_InitTypeDef   RTC_InitStructure;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Write to the first RTC Backup Data Register */
	RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_FIRST_DATA);

	/* Calendar Configuration */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/*  Backup SRAM ***************************************************************/
	/* Enable BKPRAM Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

	/* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
	PWR_BackupRegulatorCmd(ENABLE);

	/* Wait until the Backup SRAM low power Regulator is ready */
	while (PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
	{
	}

	/* RTC Backup Data Registers **************************************************/
	/* Write to first RTC Backup Data Registers */
	write_backup_reg(0, RTC_FIRST_DATA);

}


void clear_bkpsram(void)
{
	u32 i;

	for (i = 0x0; i < 0x1000; i++)
	{
		*(__IO uint8_t *) (BKPSRAM_BASE + i) = 0;
	}
}

void write_byte_to_bkpsram(u32 add_offset, void* p_data, u32 len)
{
	u32 i;
	u8* p_temp = (u8*)p_data;

	for (i = 0x0; i < len; i++)
	{
		*(__IO uint8_t *) (BKPSRAM_BASE + add_offset + i) = p_temp[i];
	}
}


void read_byte_from_bkpsram(u32 add_offset, void* p_data, u32 len)
{
	u32 i;
	u8* p_temp = (u8*)p_data;

	for (i = 0x0; i < len; i++)
	{
		p_temp[i] = *(__IO uint8_t *) (BKPSRAM_BASE + add_offset + i);
	}
}

void write_u32_to_bkpsram(u32 add_offset, void* p_data, u32 len)
{
	u32 i;
	u32* p_temp = (u32*)p_data;

	assert_param(add_offset % 4 == 0);

	for (i = 0x0; i < len; i++)
	{
		*(__IO uint32_t *) (BKPSRAM_BASE + add_offset + 4*i) = p_temp[i];
	}
}

void read_u32_from_bkpsram(u32 add_offset, void* p_data, u32 len)
{
	u32 i;
	u32* p_temp = (u32*)p_data;

	assert_param(add_offset % 4 == 0);

	for (i = 0x0; i < len; i++)
	{
		p_temp[i] = *(__IO uint32_t *) (BKPSRAM_BASE + add_offset + 4*i);
	}
}



/**
  * @brief  Writes data to Backup data registers.
  * @param  index: which backup register do you want to write?
  * @param  data: data to write to backup data register.
  * @retval None
  */
void write_backup_reg(u8 index, u32 data)
{
	assert_param(index < RTC_BKP_DR_NUMBER);

	RTC_WriteBackupRegister(BKP_data_reg[index], data);
}


/**
  * @brief  read backup register
  * @param  index: which backup register do you want to read?
  * @retval - the data in backup register
  */
u32 read_backup_reg(u8 index)
{
	assert_param(index < RTC_BKP_DR_NUMBER);

	return RTC_ReadBackupRegister(BKP_data_reg[index]);
}


void set_rtc_time(u8 second, u8 minute, u8 hour)
{
	RTC_TimeTypeDef   RTC_TimeStructure;

	RTC_TimeStructInit(&RTC_TimeStructure);
	RTC_TimeStructure.RTC_Hours   = hour;
	RTC_TimeStructure.RTC_Minutes = minute;
	RTC_TimeStructure.RTC_Seconds = second;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
}

void get_rtc_time(u8* second, u8* minute, u8* hour)
{
	RTC_TimeTypeDef   RTC_TimeStructure;

	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	*second = RTC_TimeStructure.RTC_Seconds;
	*minute = RTC_TimeStructure.RTC_Minutes;
	*hour   = RTC_TimeStructure.RTC_Hours;
}


void set_rtc_date(u32 weekday, u32 date, u32 month, u32 year)
{
	RTC_DateTypeDef RTC_DateStructure;

	assert_param(year >= 2000);

	RTC_DateStructInit(&RTC_DateStructure);
	RTC_DateStructure.RTC_WeekDay = (u8)weekday;
	RTC_DateStructure.RTC_Date    = (u8)date;
	RTC_DateStructure.RTC_Month   = (u8)month;
	RTC_DateStructure.RTC_Year    = (u8)(year - 2000);

	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

void get_rtc_date(u32* weekday, u32* date, u32* month, u32* year)
{
	RTC_DateTypeDef RTC_DateStructure;

	/* Get the current Date */
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	*weekday = RTC_DateStructure.RTC_WeekDay;
	*date    = RTC_DateStructure.RTC_Date;
	*month   = RTC_DateStructure.RTC_Month;
	*year    = 2000 + RTC_DateStructure.RTC_Year;
}


/**
  * @brief  ADC1 Channel Vbat configuration (DMA, ADC, CLK)
  * @param  None
  * @retval None
  */
static void VBAT_measure_config(void)
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
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)VBAT_ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1_converted_value;
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
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat, 1, ADC_SampleTime_15Cycles);

	/* Enable VBAT channel */
	ADC_VBATCmd(ENABLE); 

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 **************************************************************/
	ADC_Cmd(ADC1, ENABLE);
}


void init_VBAT_measure(void)
{
	VBAT_measure_config();
	ADC_SoftwareStartConv(ADC1);
}

void get_vbat_value(u32* VBAT_V, u32* VBAT_mV)
{
	vu32 VBAT_voltage = 0;

	/*The VBAT pin is internally connected to a bridge divider by 2 */
	VBAT_voltage = (ADC1_converted_value * 2) * 3300/0xFFF;
	*VBAT_V  = VBAT_voltage / 1000;
	*VBAT_mV = (VBAT_voltage % 1000) / 100;
}


