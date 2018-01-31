/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : datafilter_variance.h
 * Author             : MAX.liu
 * Date First Issued  : 2013/10/16
 * Description        : This file contains the software implementation for
 						filtering algorithm
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/16 | v1.0  | MAX.liu    | initial released
 *******************************************************************************/


#ifndef _DATAFILTER_VARIANCE_H_
#define _DATAFILTER_VARIANCE_H_


// please def your own data type
typedef signed   short  int16;
typedef unsigned short  Uint16;
typedef signed   int    int32;
typedef unsigned int    Uint32;
typedef float           float32;
typedef double          float64;


typedef struct
{
    // custom focus
    volatile Uint16 DataInput;			// in:source data input   输入
    volatile Uint16 DataOutput; 		// processed data output

	volatile Uint16 VarianceFlag;		// 1st level filter variance flag 0:normal
	volatile Uint16 FilterLevel2;		// 2st level filter: (p->DataOutput - p->ExceptionValve)*(p->DataOutput - p->ExceptionValve) > p->variance_stand)
    volatile Uint16 FilterLevel3;       // 3st level filter: VarianceFlag

    volatile Uint16 ExceptionValve;		//阈值
	volatile Uint16 VarianceThread;		// vavle level  误差
	volatile Uint16 Output;		    	// 输出0x00: normal
	                                    //       0x10: high
	                                    //       0x20: low
	                                    //       0x30: shake

	volatile Uint16 high;		    	// 1: high 0:normal  输出
	volatile Uint16 low;		    	// 1: low 0:normal  输出

	volatile Uint32 DataInputValTotal;  // out:total value of variance cnt


    // private items
    volatile Uint16 i;
	volatile Uint16 cnt;
    volatile Uint16 cnt_shake;          // 保存了一轮循环，variance中有多少次抖动
	volatile Uint16 level_num;
	volatile int16  level[100];//10ms

	volatile Uint16 level_cnt;
	volatile int32  level_total;
	volatile Uint16 is_inited;
	volatile Uint16 level_in_cnt;

	// filter ref
	volatile int32 variance[10];
	volatile Uint16 variance_ver;// variance　下标,表示variance_ver组数据
	volatile int32 avg_last;
	volatile int32 avg_realtime_cali;
	volatile Uint16 variance_num; /*表示１０个一组为计算单位，共存储１０组*/
	                              /*只有１０的整数倍可以这么定义，否则需要定义两个变量以作区分*/
	volatile Uint16 variance_cnt;
	volatile int32 variance_stand;//hight level = VarianceThread; low level = -VarianceThread;

	void (*init)();       		    	// Pointer to the init function
	unsigned short (*update)();       	// Pointer to the update function
}
DATAFILTER_VARIANCE ;

/*-----------------------------------------------------------------------------
Define a DATAFILTER_VARIANCE handle
-----------------------------------------------------------------------------*/
typedef DATAFILTER_VARIANCE *DATAFILTER_VARIANCE_handle;

/*------------------------------------------------------------------------------
Default Initializers for the DATAFILTER_VARIANCE Object
the fourth para: 100 means 100*10ms = 1s
------------------------------------------------------------------------------*/
#define DATAFILTER_VARIANCE_DEFAULT 	{0, 0,40,100, 3, 0, 0, 0, 0,0,0, 0, 1000, 0, 100,\
                                         {0},\
                                         0, 0, 0, 0,\
                                         {0},\
                                         0, 0, 0, 10, 0, 0,\
                             			DataFilter_Variance_Init,\
                             			DataFilter_Variance_Update\
                             			}

void DataFilter_Variance_Init(DATAFILTER_VARIANCE *p);
Uint16 DataFilter_Variance_Update(DATAFILTER_VARIANCE *p);


#endif

