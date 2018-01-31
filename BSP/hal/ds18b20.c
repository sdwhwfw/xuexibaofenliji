/**
  ******************************************************************************
  * @file    ds18b20.c
  * @author  Bruce.zhu
  * @version V0.0.1
  * @date    2013-04-21
  * @brief   DS18B20 HAL functions.
  *          Open207V OneWire pin---> PE3
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 VINY</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "ds18b20.h"

static unsigned char TempX_TAB[16] =
{
	0x00,0x01,0x01,0x02,
	0x03,0x03,0x04,0x04,
	0x05,0x06,0x06,0x07,
	0x08,0x08,0x09,0x09
};

void ds18b20_pin_out_pull(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void ds18b20_pin_in_pull(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**
 * RESET DS18B20
 * ========= Timing =========
 * (1) SET PIN HIGH
 * (2) delay
 * (3) CLR PIN
 * (4) delay 490us [480us, 960us]
 * (5) SET PIN HIGH
 * (6) wait pin low
 * (7) delay 480us
 * (8) SET PIN HIGH
 */
void reset_ds18b20(void)
{
	ds18b20_pin_out_pull();
	SET_DQ();
	delay_us(50);
	CLR_DQ();
	delay_us(500);
	SET_DQ();
	delay_us(40);
	ds18b20_pin_in_pull();
	while(GET_DQ());
	delay_us(500);
	ds18b20_pin_out_pull();
	SET_DQ();
}


/**
 * read one byte from ds18b20
 * 
 */
uint8_t read_ds18b20(void)
{
	uint8_t data = 0, i = 0;

	ds18b20_pin_out_pull();
	SET_DQ();
	delay_us(5);
	
	for(i = 0; i < 8; i++)
	{
		CLR_DQ();
		delay_us(5);
		SET_DQ();
		delay_us(5);

		data = data >> 1;
		ds18b20_pin_in_pull();
		if(GET_DQ())
			data |= 0x80;
		else
			data &= 0x7f;
		ds18b20_pin_out_pull();
		SET_DQ();
		delay_us(65);
	}

    return(data);
}

/**
 * write one byte to ds18b20
 * 
 */
void write_ds18b20(uint8_t data)
{
	uint8_t i = 0;

	ds18b20_pin_out_pull();

	for(i = 0; i < 8; i++)
	{
		CLR_DQ();
		delay_us(2);
		if(data & 0x01)
		{
			SET_DQ();
		}
		else
		{
			CLR_DQ();
		}

		delay_us(65);
		SET_DQ();
		delay_us(2);
		data = data >> 1;
    }

	SET_DQ();
}


void read_rom(unsigned char *Addr)
{
	unsigned char i;

	write_ds18b20(ReadROM);

	for(i = 0; i < 8; i--)
	{
		*Addr = read_ds18b20();
		Addr++;
	}
}


void ds18b20_init(unsigned char Precision,unsigned char AlarmTH,unsigned char AlarmTL)
{
#if OS_CRITICAL_METHOD == 3u
	OS_CPU_SR cpu_sr;
#endif

	OS_ENTER_CRITICAL();
	reset_ds18b20();
	write_ds18b20(SkipROM);
	write_ds18b20(WriteScratchpad);
	write_ds18b20(AlarmTL);
	write_ds18b20(AlarmTH);
	write_ds18b20(Precision);

	reset_ds18b20();
	write_ds18b20(SkipROM);
	write_ds18b20(CopyScratchpad);
	OS_EXIT_CRITICAL();
	ds18b20_pin_in_pull();
	while(!GET_DQ());
}


void ds18b20_convert(void)
{
#if OS_CRITICAL_METHOD == 3u
	OS_CPU_SR cpu_sr;
#endif

	OS_ENTER_CRITICAL();
	reset_ds18b20();
	write_ds18b20(SkipROM);
	write_ds18b20(StartConvert);
	OS_EXIT_CRITICAL();
}

/*
 * PE3
 */
void ds18b20_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*============ init ds18b20 ============*/
void ds18b20_start(void)
{
	ds18b20_config();
	ds18b20_init(DS_PRECISION, DS_ALARMTH, DS_ALARMTL);
	ds18b20_convert();
}

/*============ read ds18b20 ============*/
uint16_t ds18b20_read(void)
{
	uint8_t TemperatureL, TemperatureH;
	uint32_t  Temperature;
#if OS_CRITICAL_METHOD == 3u
	OS_CPU_SR cpu_sr;
#endif

	OS_ENTER_CRITICAL();
	reset_ds18b20();
	write_ds18b20(SkipROM);
	write_ds18b20(ReadScratchpad);
	TemperatureL = read_ds18b20();
	TemperatureH = read_ds18b20();
	reset_ds18b20();
	OS_EXIT_CRITICAL();

	if(TemperatureH & 0x80)
	{
		TemperatureH = (~TemperatureH) | 0x08;
		TemperatureL = ~TemperatureL + 1;

		if(TemperatureL == 0)
			TemperatureH += 1;
	}

	TemperatureH = (TemperatureH << 4) + ((TemperatureL & 0xf0) >> 4);
	TemperatureL = TempX_TAB[TemperatureL & 0x0f];

	//bit0-bit7为小数位，bit8-bit14为整数位，bit15为正负位
	Temperature = TemperatureH;
	Temperature = (Temperature << 8) | TemperatureL;
	ds18b20_convert();

	return  Temperature;
}




