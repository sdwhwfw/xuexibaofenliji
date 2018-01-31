/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/stm32f2xx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "includes.h"


#if defined(USE_USART1_TO_DEBUG)
#define ISR_USART_NUM    COM1
#elif defined(USE_USART3_TO_DEBUG)
#define ISR_USART_NUM    COM3
#else
#define ISR_USART_NUM    COM1
#endif


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "NMI_Handler\r\n");
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "HardFault_Handler\r\n");
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

void hard_fault_handler_c(unsigned int * hardfault_args)
{
    vu32 i = 0;
	uint8_t      vl_buf[32];
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0  = ((unsigned long) hardfault_args[0]);
	stacked_r1  = ((unsigned long) hardfault_args[1]);
	stacked_r2  = ((unsigned long) hardfault_args[2]);
	stacked_r3  = ((unsigned long) hardfault_args[3]);
	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr  = ((unsigned long) hardfault_args[5]);
	stacked_pc  = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	usart_send_string(ISR_USART_NUM, "[======= Hard fault handler =======]\r\n");
	snprintf((char*)vl_buf, sizeof(vl_buf), "R0 = %x\r\n",  stacked_r0);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "R1 = %x\r\n",  stacked_r1);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "R2 = %x\r\n",  stacked_r2);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "R3 = %x\r\n",  stacked_r3);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "R12 = %x\r\n", stacked_r12);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "LR = %x\r\n",  stacked_lr);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "PC = %x\r\n",  stacked_pc);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "PSR = %x\r\n", stacked_psr);
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "BFAR = %x\r\n", (*((volatile unsigned long *)(0xE000ED38))));
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "CFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED28))));
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "HFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED2C))));
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "DFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED30))));
	usart_send_string(ISR_USART_NUM, vl_buf);
	snprintf((char*)vl_buf, sizeof(vl_buf), "AFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED3C))));
	usart_send_string(ISR_USART_NUM, vl_buf);

    while(1);
}


/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "MemManage_Handler\r\n");
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "BusFault_Handler\r\n");
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "UsageFault_Handler\r\n");
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "SVC_Handler\r\n");
	while (1)
	{
	}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
	usart_send_string(ISR_USART_NUM, "DebugMon_Handler\r\n");
 	while (1)
	{
	}
}


/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/




/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
