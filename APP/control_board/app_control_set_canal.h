/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_set_canal.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/14
 * Description        : This file contains the load suit & remove air 
 *                      & canal test & AC priming
 *                      program.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/14 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_CONTROL_SET_SUIT_H_
#define _APP_CONTROL_SET_SUIT_H_

// TODO: The distance of feedback pump turn to remove air from product packet
#define DISTANCE_REMOVE_AIR             46

/*
 * Function: set_canal
 * Description: excute the load suit & remove air & canal test & ac priming
 * Param: NONE
 * Return: uint8_t,0-OK;1-ERROR
*/
uint8_t set_canal(void);


#endif

