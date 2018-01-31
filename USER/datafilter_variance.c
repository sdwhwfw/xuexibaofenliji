/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : datafilter_variance.c
 * Author             : MAX.liu
 * Date First Issued  : 2013/10/16
 * Description        : This file contains the software implementation for
 						filtering algorithm
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/16 | v1.0  | MAX.liu    | initial released
 *******************************************************************************/


#include "datafilter_variance.h"


void DataFilter_Variance_Init(DATAFILTER_VARIANCE *p)
{
    p->is_inited = 1;

    for(p->i=0;p->i < p->level_num;p->i++)
    {
        p->level[p->i] = 0;
    }
    for(p->i=0;p->i < p->variance_num;p->i++)
    {
        p->variance[p->i] = 0;
    }
    p->variance_stand = (p->VarianceThread * p->VarianceThread);
}

Uint16 DataFilter_Variance_Update(DATAFILTER_VARIANCE *p)
{
    if(0 == p->level_in_cnt++ % 1)
    {
    	// over flow protect
    	if(p->level_in_cnt > 10000)
    	{
    		p->level_in_cnt = 0;
    	}

    	//valve
    	if(p->level_cnt >= p->level_num)
    	{
    		p->level_cnt = 0;
    	}
    	else
    	{
    		p->level_total += p->DataInput;
    		p->level_total -= p->level[p->level_cnt];
    		p->level[p->level_cnt] = p->DataInput;

    		p->level_cnt++;
    	}

    	// variance
    	if(p->variance_cnt >= p->variance_num)
    	{
    		p->variance[p->variance_ver] /= p->variance_num;//avg cali
    		if(p->variance[p->variance_ver] > p->variance_stand)
    		{
    			p->VarianceFlag |= (0x01 << p->variance_ver);

                if((p->cnt_shake > 0x00)&&(p->cnt_shake < p->FilterLevel3))
                {
                    p->cnt_shake++;
                }
    		}
    		else
    		{
    			p->VarianceFlag &= ~(0x01 << p->variance_ver);
    		}

    		// update variance_ver
    		p->variance_ver++;
    		if(p->variance_ver >= p->variance_num)
    		{
                // reset variance_ver&cnt_shake
    			p->variance_ver = 0;
                p->cnt_shake = 0;
    		}

    		// update variance_cnt
    		p->variance_cnt = 0;

    		// update avg_last & avg_realtime_cali
    		p->avg_last = p->avg_realtime_cali/p->variance_num;
    		p->avg_realtime_cali = 0;
    	}
    	else
    	{
    		// 以variance_num为单位，更新均值
    		p->avg_realtime_cali += p->DataInput;

    		// 实时更新偏差
    		if(0 == p->variance_cnt)// if the first arrange data come, init data
    		{
    			p->variance[p->variance_ver] = (p->DataInput-p->avg_last)*(p->DataInput-p->avg_last);
    		}
    		else
    		{
    			p->variance[p->variance_ver] += (p->DataInput-p->avg_last)*(p->DataInput-p->avg_last);
    		}
    		p->variance_cnt++;
    	}
    }


    // update DataOutput
    p->DataOutput = (Uint16)(p->level_total/p->level_num);

    // level out
    // judge variance first
    #if 0
    if((!p->VarianceFlag)||((p->DataOutput - p->ExceptionValve)*(p->DataOutput - p->ExceptionValve) > p->variance_stand))
    {
        // un-normal branch
    	if(p->cnt > (1000+p->Filter))
    	{
            if(!p->VarianceFlag)
            {
                //
                p->Output = 0x30;
            }
            else
            {
                // data output un-normal
                if(p->DataOutput > p->ExceptionValve)
                {
                    // high
                    p->Output = 0x10;
                }
                else
                {
                    // low
                    p->Output = 0x20;
                }
            }
    	}
    	else
    	{
            p->cnt++;
    	}
    }
    else
    {
        // normal branch
        if(p->cnt > 1000)
        {
    		p->cnt--;
        }
    	else
    	{
    		p->Output = 0x00;
    	}
    }
    #endif

    if((p->VarianceFlag && (p->cnt_shake >= p->FilterLevel3))||((p->DataOutput - p->ExceptionValve)*(p->DataOutput - p->ExceptionValve) > p->variance_stand))
    {
        // un-normal branch
    	if(p->cnt > (1000+p->FilterLevel2))
    	{
            if(p->VarianceFlag && (p->cnt_shake >= p->FilterLevel3))
            {
                // shake
                p->Output = 0x30;
            }
            else
            {
                // data output un-normal
                if(p->DataOutput > p->ExceptionValve)
                {
                    // high
                    p->Output = 0x10;
                }
                else
                {
                    // low
                    p->Output = 0x20;
                }
            }
    	}
    	else
    	{
            p->cnt++;
    	}
    }
    else
    {
        // normal branch
        if(p->cnt > 1000)
        {
    		p->cnt--;
        }
    	else
    	{
    		p->Output = 0x00;
    	}
    }

    return (p->DataOutput);
}



