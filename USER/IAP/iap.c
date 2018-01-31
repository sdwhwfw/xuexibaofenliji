/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : iap.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/06/21
 * Description        : This file contains the software implementation for the
 *                      iap unit
 *******************************************************************************
 * History:
 * 2013/06/21 v1.0
 *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "iap.h"
#include "ymodem.h"
#include "flash_if.h"
#include "common.h"
#include "queue.h"
#include <stdio.h>
#include "usart.h"
#include "delay.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static p_app_fun Jump_To_Application;
static uint32_t JumpAddress;

static uint8_t ymodem_tab_1024[1024] = {0};
static uint8_t FileName[FILE_NAME_LENGTH];


/* Private function prototypes -----------------------------------------------*/
static void display_info_to_user(void);
static void check_internal_flash(void);
static uint8_t wait_key_press(void);
static void download_firmware(void);
static void upload_firmware(void);
static void run_app(void);


/* Private functions ---------------------------------------------------------*/

cir_queue recv_queue;

int main()
{
    init_queue(&recv_queue);
	IAP_process();
}


/**
  * @brief  IAP main program.
  * @param  None
  * @retval None
  */
void IAP_process(void)
{
	uint8_t key;

	/* for delay init */
	delay_init(DELAY_FOR_MS);

	/* init internal flash */
	flash_if_init();

	/* init UART1 */
	init_usart(COM1, 115200, 8, PARITY_NONE, USART_MODE_INT);

	/* display information */
	display_info_to_user();

	/* Receive a key */
	key = wait_key_press();
	switch (key)
	{
		case 0x31:
			check_internal_flash();
			/* INFO: READY */
			usart_send_string(COM1, "\r\n$READY$\r\n");
			download_firmware();
			break;
		case 0x32:
			upload_firmware();
			break;
		case 0x33:
			run_app();
			break;
		default:
			usart_send_string(COM1, "\r\n error: input key invalid\r\n");
			// stay here
			while(1);
	}

	while (1)
	{}
}


static void display_info_to_user()
{
	usart_send_string(COM1, "\r\n======================================================================");
	usart_send_string(COM1, "\r\n=              (C) COPYRIGHT 2013 VINY                               =");
	usart_send_string(COM1, "\r\n=                                                                    =");
	usart_send_string(COM1, "\r\n=  STM32F2xx In-Application Programming Application  (Version 1.0.0) =");
	usart_send_string(COM1, "\r\n=                                                                    =");
	usart_send_string(COM1, "\r\n=                                           Bruce.zhu(20130624)      =");
	usart_send_string(COM1, "\r\n======================================================================");
	usart_send_string(COM1, "\r\n");
	usart_send_string(COM1, "\r\n======================================================================\r\n");
	usart_send_string(COM1, "  Download Image To the STM32F2xx Internal Flash ------- 1\r\n\n");
	usart_send_string(COM1, "  Upload Image From the STM32F2xx Internal Flash ------- 2\r\n\n");
	usart_send_string(COM1, "  Execute The New Program ------------------------------ 3\r\n\n");
}


static void check_internal_flash()
{
	uint16_t ret;

	ret = FLASH_If_GetWriteProtectionStatus();
	/* Some sectors inside the user flash area are write protected */
	if(ret == 0)
	{
		/* Some sectors inside the user flash area are write protected */
		usart_send_string(COM1, "FLASH write protected, disable the write protection...\r\n\n");
		/* Disable the write protection */
		switch (FLASH_If_DisableWriteProtection())
		{
			case 1:
			{
				usart_send_string(COM1, "Write Protection disabled...\r\n");
				break;
			}
			case 2:
			{
				usart_send_string(COM1, "Error: Flash write unprotection failed...\r\n");
				// stay here...
				while(1);
			}
			default:
			{
			}
		}
	}
}


static uint8_t wait_key_press(void)
{
	data_type data;

	while(1)
	{
		if(queue_empty(&recv_queue) == 0)
		{
			de_queue(&recv_queue, &data);
			return (uint8_t)(data.x);
		}
	}
}

static void download_firmware(void)
{
	uint8_t vl_buf[128];
	int32_t Size = 0;

	usart_send_string(COM1, "Waiting for the file to be sent ... (press 'a' to abort)\r\n");
	memset(FileName, 0x00, sizeof(FileName));
	Size = Ymodem_Receive(&ymodem_tab_1024[0], FileName);
	delay(1000);
	if (Size > 0)
	{
		usart_send_string(COM1, "\n\r\n Programming Completed Successfully!\r\n");
		usart_send_string(COM1, "---------------------------------------\r\n");
		snprintf((char*)vl_buf, sizeof(vl_buf), "- File: %s\r\n", FileName);
		usart_send_string(COM1, vl_buf);
		snprintf((char*)vl_buf, sizeof(vl_buf), "- Size: %d Bytes\r\n", Size);
		usart_send_string(COM1, vl_buf);
		usart_send_string(COM1, "---------------------------------------\r\n");
	}
	else if (Size == -1)
	{
		usart_send_string(COM1, "\n\n\rThe image size is higher than the allowed space memory!\n\r");
	}
	else if (Size == -2)
	{
		usart_send_string(COM1, "\n\n\rVerification failed!\n\r");
	}
	else if (Size == -3)
	{
		usart_send_string(COM1, "\r\n\nAborted by user.\n\r");
	}
	else
	{
		usart_send_string(COM1, "\n\rFailed to receive the file!\n\r");
	}

}



static void upload_firmware(void)
{
}


static void run_app(void)
{
	/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (p_app_fun) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	}
}

void USART1_IRQHandler(void)
{
	uint16_t ch;
    data_type data;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART1);
        data.x = ch;
        en_queue(&recv_queue, &data);
	}
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

