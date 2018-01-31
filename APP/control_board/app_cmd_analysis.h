/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_cmd_analysis.h
 * Author             : Bruce.zhu
 * Date First Issued  : 2013/09/16
 * Description        : This file contains the software implementation for the
 *                      cmd analysis task unit
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/09/16 | v1.0  |            | initial released
 * 2013/09/26 | v1.1  | Bruce.zhu  | This task is only to analysis cmd packet, 
 *                                   not using to receive usart dma data
 * 2013/10/24 | v1.2  | ZhangQ     | complete common function of this task
 *******************************************************************************/
#ifndef APP_CMD_ANALYSIS_H
#define APP_CMD_ANALYSIS_H

/*
 * Function: control_uart_cmd_analysis_init
 * Description: Init the uart recv task
 * Param: None
 * Return: None
*/
void control_uart_cmd_analysis_init(void);

#endif /* APP_CMD_ANALYSIS_H */

