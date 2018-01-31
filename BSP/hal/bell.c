/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : bell.c
 * Author             : WQ
 * Date First Issued  : 2013/10/29
 * Description        : This file contains the software implementation for the
 *                      buzzer control unit
 *******************************************************************************
 * History:
 *------------------------------------------------------------------------------
 *    DATE    |  VER  |   AUTOR    | Description
 *------------------------------------------------------------------------------
 * 2013/10/29 | v1.0  | WQ         | initial released
 * 2013/11/09 | v1.1  | Bruce.zhu  | add support for ARM0 ARM1 ARM2 ARM3 board
 *******************************************************************************/

#include "bell.h"


#if defined(USE_ARM0_REV_0_1)
/* PB15 */
#define BELL_PIN                    GPIO_Pin_15
#define BELL_GPIO_PORT              GPIOB
#define BELL_GPIO_CLK               RCC_AHB1Periph_GPIOB
#else
/* PB5 */
#define BELL_PIN                    GPIO_Pin_5
#define BELL_GPIO_PORT              GPIOB
#define BELL_GPIO_CLK               RCC_AHB1Periph_GPIOB
#endif /* USE_ARM0_REV_0_1 */



void bell_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the BELL Clock */
    RCC_AHB1PeriphClockCmd(BELL_GPIO_CLK, ENABLE);

    /* Configure the BELL pin */
    GPIO_InitStructure.GPIO_Pin = BELL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BELL_GPIO_PORT, &GPIO_InitStructure);
}


void bell_on()
{
    BELL_GPIO_PORT->BSRRL = BELL_PIN;
}


void bell_off()
{
    BELL_GPIO_PORT->BSRRH = BELL_PIN;
}



