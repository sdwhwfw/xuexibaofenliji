/*
 * Board Support Package
 *
 */

#include "bsp.h"
#include <stdio.h>


/*
 * init hardware
 * Note: This function must be called after OSStart()!!!
 *
 */
void bsp_init(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    /* systick */
    RCC_GetClocksFreq(&RCC_Clocks);
    /* HCLK_Frequency = 120000000 */
    SysTick_Config(RCC_Clocks.HCLK_Frequency / OS_TICKS_PER_SEC);

    /* Set NVIC Group Priority */
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);

    /* for delay */
    //delay_init(DELAY_FOR_US);

#if defined(USE_ISSI_SRAM_FTR)
    /**
      * init FSMC interface
      * NOTE: we must init FSMC before USART!!!
      * 2013.10.21 move SRAM init code to system_stm32f2xx.c
      */
    //sram_init();
#endif /* USE_ISSI_SRAM_FTR */

#if defined(_COMX100_MODULE_FTR_)
    /**
      * init comX100 moudle with FSMC interface
      * NOTE: we must init FSMC before USART!!!
      */
    COMX100_init();
#endif /* _COMX100_MODULE_FTR_ */

}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();
	OSIntExit();
}



