/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_user_tmr.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/08
 * Description        : This file complete the stm32's timer
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/05 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _APP_USER_TMR_H_
#define _APP_USER_TMR_H_

#include "stm32f2xx.h"

// The Max size of timer
#define TIMER_MAX_COUNT                     15u

// tmr run mode
#define USER_TMR_OPT_ONE_SHOT               1u
#define USER_TMR_OPT_PERIODIC               2u

// callback mode
#define USER_TMR_OPT_NONE                   0u
#define USER_TMR_OPT_CALLBACK               1u
#define USER_TMR_OPT_CALLBACK_ARG           2u

// tmr state
#define USER_TMR_STATE_UNUSED               0u
#define USER_TMR_STATE_USED                 1u
#define USER_TMR_STATE_RUNNING              2u
#define USER_TMR_STATE_STOPPED              3u

// tmr error type
#define USER_ERR_NONE                       0u
#define USER_ERR_TMR_NON_AVAIL              1u
#define USER_ERR_TMR_INVALID_OPT            2u
#define USER_ERR_TMR_INVALID_PERIOD         3u
#define USER_ERR_TMR_INVALID_DLY            4u
#define USER_ERR_TMR_NOTSTART               5u
#define USER_ERR_TMR_INACTIVE               6u

// tmr callback
typedef void (*USER_TMR_CALLBACK)(void *ptmr, void *parg);

// tmr struct
typedef struct USER_TMR_STRU
{
    INT32U TmrDly;
    INT32U TmrPeriod;
    INT32U TmrMatch;
    INT8U  TmrOpt;
    void  *TmrCallbackArg;
    USER_TMR_CALLBACK TmrCallback;
    INT8U  TmrPos;
    INT8U  TmrState;
}USER_TMR;

/*
 * Function: user_timer_init
 * Description: Init user timer when os start
 * Param: nUS, timer's basic period uint is us
 * Return: NULL
*/
void user_timer_init(u16 nUS);


/*
 * Function: user_timer_del
 * Description: delete a user timer
 * Param: NULL
 * Return: NULL
*/
void user_timer_del(USER_TMR  *ptmr,  INT8U   *perr);

/*
* Function: user_timer_create
* Description: This function is called by your application code to create a timer.
* Param:      dly           Initial delay.
*                            If the timer is configured for ONE-SHOT mode, this is the timeout used.
*                            If the timer is configured for PERIODIC mode, this is the first timeout to 
*                               wait for before the timer starts entering periodic mode.
*
*              period        The 'period' being repeated for the timer.
*                               If you specified 'USER_TMR_OPT_PERIODIC' as an option, when the timer 
*                               expires, it will automatically restart with the same period.
*
*              opt           Specifies either:
*                               USER_TMR_OPT_ONE_SHOT       The timer counts down only once
*                               USER_TMR_OPT_PERIODIC       The timer counts down and then reloads itself
*
*              callback      Is a pointer to a callback function that will be called when the timer expires. 
*                               The callback function must be declared as follows:
*
*                               void MyCallback (USER_TMR *ptmr, void *p_arg);
*
*              callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_TMR_INVALID_DLY     you specified an invalid delay
*                               OS_ERR_TMR_INVALID_PERIOD  you specified an invalid period
*                               OS_ERR_TMR_INVALID_OPT     you specified an invalid option
*                               OS_ERR_TMR_NON_AVAIL       if there are no free timers from the timer pool
 * Return: A pointer to an USER_TMR data structure.
*/
USER_TMR* user_timer_create(INT32U dly, INT32U period, INT8U opt, 
                        void *callback_arg, USER_TMR_CALLBACK callback, INT8U *perr);


/*
* Function: user_timer_start
* Description: This function is called by your application code to start a timer.
* Param:    ptmr          Is a pointer to an USER_TMR
*           perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               USER_ERR_NONE
*                               USER_ERR_TMR_INVALID
 * Return: NULL
*/
void user_timer_start(USER_TMR *p_tmr, INT8U *perr);


/*
* Function: user_timer_stop
* Description: This function is called by your application code to stop a timer.
* Param:       ptmr          Is a pointer to the timer to stop.
*
*              opt           Allows you to specify an option to this functions which can be:
*
*                               USER_TMR_OPT_NONE          Do nothing special but stop the timer
*                               USER_TMR_OPT_CALLBACK      Execute the callback function, pass it the 
*                                                        callback argument specified when the timer 
*                                                        was created.
*                               USER_TMR_OPT_CALLBACK_ARG  Execute the callback function, pass it the 
*                                                        callback argument specified in THIS function call.
*
*              callback_arg  Is a pointer to a 'new' callback argument that can be passed to the callback 
*                            function instead of the timer's callback argument.  In other words, use 
*                            'callback_arg' passed in THIS function INSTEAD of ptmr->OSTmrCallbackArg.
*
*              perr          Is a pointer to an error code.  '*perr' will contain one of the following:
*                               USER_ERR_NONE
*                               USER_ERR_TMR_NOTSTART         user tmr no started
 * Return: NULL
*/
void user_timer_stop(USER_TMR *p_tmr, INT8U opt, void *callback_arg, INT8U *perr);

#endif

