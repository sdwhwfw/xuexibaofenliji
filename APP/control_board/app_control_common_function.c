/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_common_function.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/23
 * Description        : This file contains the common function of control board
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/23 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_common_function.h"
#include "ucos_ii.h"

uint8_t uart_crc_check_func(uint8_t *pData, uint8_t len)
{
    uint8_t i;
    uint8_t l_crcData = 0;
    
    if (len<=13)
        return 1;
    for (i=1; i<10; i++)
    {
        l_crcData ^= pData[i];
    }
    if (l_crcData == pData[10])
    {
        return 0; // crc check OK!
    }
    else
    {
        return 1; // crc check fail!
    }
}

uint8_t uart_crc_generate_func(uint8_t *pData, uint8_t len)
{
    uint8_t i;
    uint8_t l_crcData = 0;
    
    if (len<13)
        return 1;
    for (i=1; i<10; i++)
    {
        l_crcData ^= pData[i];
    }
    return l_crcData;
}

uint8_t get_current_fn(FN_SOURCE source, uint8_t fn_opt)
{
    static uint8_t s_uart_fn = 0, s_arm1_fn = 0, s_arm2_fn = 0, s_arm3_fn = 0;
    uint8_t ret;

    if (FN_CHECK == fn_opt)
    {
        switch (source)
        {
            case IPC_DEST:
                ret = s_uart_fn;
                break;
            case ARM1_DEST:
                ret = s_arm1_fn;
                break;
            case ARM2_DEST:
                ret = s_arm2_fn;
                break;
            case ARM3_DEST:
                ret = s_arm3_fn;
                break;
            default:
                break;
        }
        return ret;
    }
    
    switch (source)
    {
        case IPC_DEST:
        {
            s_uart_fn = ++s_uart_fn % 8;
            ret = s_uart_fn;
            break;
        }
        case ARM1_DEST:
        {
            s_arm1_fn = ++s_arm1_fn % 8;
            ret = s_arm1_fn;
            break;
        }
        case ARM2_DEST:
        {
            s_arm2_fn = ++s_arm2_fn % 8;
            ret = s_arm2_fn;
            break;
        }
        case ARM3_DEST:
        {
            s_arm3_fn = ++s_arm3_fn % 8;
            ret = s_arm3_fn;
            break;
        }
        default:
            break;
    }
		return ret;
}

uint8_t arm0_get_cur_prio(void)
{
    uint8_t l_prio;
    OS_TCB l_tcb;

    OSTaskQuery(OS_PRIO_SELF, &l_tcb);
    l_prio = l_tcb.OSTCBPrio;
    return l_prio;
}

uint8_t judge_array_null(uint8_t *p_data, uint8_t len)
{
    uint8_t i;
    
    for (i=0; i<len; i++)
    {
        if (0 != p_data[i])
        {
            return 0;
        }
    }
    return 1;
}
/*
uint16_t get_array_max_value(uint16_t *p_data, uint8_t len)
{
    uint8_t i;
    uint16_t l_tempMax = 0;

    for (i=0; i<len; i++)
    {
        l_tempMax = p_data[i] > l_tempMax ? p_data[i] : l_tempMax;
    }
    return l_tempMax;
}

*/
uint32_t get_array_max_value(uint32_t *p_data, uint8_t len)
{
    uint8_t i;
    uint32_t l_tempMax = 0;

    for (i=0; i<len; i++)
    {
        l_tempMax = p_data[i] > l_tempMax ? p_data[i] : l_tempMax;
    }
    return l_tempMax;
}

void arm_resend_cmd(uint8_t *p_data, uint8_t number)
{
    if (1==number)
    {// TPDO3
        p_data[9]++;
    }
    else if(3==number)
    {// TPDO1,2,3
        p_data[25]++;
    }
}

float get_average_value(uint16_t *p_data, uint8_t number)
{
    uint8_t i;
    uint16_t l_total = 0;
    float l_ret;
    
    for (i=0; i<number; i++)
    {
        l_total += p_data[i];
    }

    l_ret = (float)l_total / number;
    return l_ret;
}

