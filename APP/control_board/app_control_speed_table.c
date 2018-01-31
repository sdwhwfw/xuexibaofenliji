/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : app_control_speed_table.c
 * Author             : ZhangQ
 * Date First Issued  : 2013/11/20
 * Description        : This file contains the global status of ARM0
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/11/20 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#include "app_control_speed_table.h"
#include "app_control_board.h"

static uint8_t g_flow_phase;
FLOW_PUMP_SPEED g_flow_speedTable[FLOW_SPEED_TABLE_SIZE];


void init_flow_speed_table(FLOW_PUMP_SPEED maxSpeed)
{
    g_flow_phase = 0xFF;
    memset(g_flow_speedTable, 0x00, sizeof(g_flow_speedTable));

    if (maxSpeed.draw_speed < 6250)
    {
        g_flow_speedTable[0].draw_speed = maxSpeed.draw_speed;
        g_flow_speedTable[0].ac_speed = (u16)((u32)maxSpeed.draw_speed*100/g_cur_program_param.rate_plt);
        g_flow_speedTable[0].feedBack_speed = 0;
        g_flow_speedTable[0].pla_speed = 570;
        g_flow_speedTable[0].plt_speed = 1200;

        g_flow_speedTable[1].draw_speed = maxSpeed.draw_speed/7*4;
        g_flow_speedTable[1].ac_speed = 0;
        g_flow_speedTable[1].feedBack_speed = 4724;
        g_flow_speedTable[1].pla_speed = 570;
        g_flow_speedTable[1].plt_speed = 1200;

        memcpy(&g_flow_speedTable[2], &g_flow_speedTable[0], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[3], &g_flow_speedTable[1], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[4], &g_flow_speedTable[0], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[5], &g_flow_speedTable[1], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[6], &g_flow_speedTable[0], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[7], &g_flow_speedTable[0], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[8], &g_flow_speedTable[1], sizeof(FLOW_PUMP_SPEED));
    }
    else if (maxSpeed.draw_speed < 8000)
    {
        g_flow_speedTable[0].draw_speed = 6250;
        g_flow_speedTable[0].ac_speed = 560;
        g_flow_speedTable[0].feedBack_speed = 0;
        g_flow_speedTable[0].pla_speed = 570;
        g_flow_speedTable[0].plt_speed = 1200;
        
        g_flow_speedTable[1].draw_speed = 4316;
        g_flow_speedTable[1].ac_speed = 0;
        g_flow_speedTable[1].feedBack_speed = 4724;
        g_flow_speedTable[1].pla_speed = 570;
        g_flow_speedTable[1].plt_speed = 1200;

        g_flow_speedTable[2].draw_speed = maxSpeed.draw_speed;
        g_flow_speedTable[2].ac_speed = (u16)((u32)maxSpeed.draw_speed*100/g_cur_program_param.rate_plt);
        g_flow_speedTable[2].feedBack_speed = 0;
        g_flow_speedTable[2].pla_speed = 570;
        g_flow_speedTable[2].plt_speed = 1200;

        g_flow_speedTable[3].draw_speed = maxSpeed.draw_speed/7*4;
        g_flow_speedTable[3].ac_speed = 0;
        g_flow_speedTable[3].feedBack_speed = 6000;
        g_flow_speedTable[3].pla_speed = 570;
        g_flow_speedTable[3].plt_speed = 1200;

        memcpy(&g_flow_speedTable[4], &g_flow_speedTable[2], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[5], &g_flow_speedTable[3], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[6], &g_flow_speedTable[2], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[7], &g_flow_speedTable[2], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[8], &g_flow_speedTable[3], sizeof(FLOW_PUMP_SPEED));
    }
    else if (maxSpeed.draw_speed < 9419)
    {
        g_flow_speedTable[0].draw_speed = 6250;
        g_flow_speedTable[0].ac_speed = 560;
        g_flow_speedTable[0].feedBack_speed = 0;
        g_flow_speedTable[0].pla_speed = 570;
        g_flow_speedTable[0].plt_speed = 1200;
        
        g_flow_speedTable[1].draw_speed = 4316;
        g_flow_speedTable[1].ac_speed = 0;
        g_flow_speedTable[1].feedBack_speed = 4724;
        g_flow_speedTable[1].pla_speed = 570;
        g_flow_speedTable[1].plt_speed = 1200;

        g_flow_speedTable[2].draw_speed = 8000;
        g_flow_speedTable[2].ac_speed = 689;
        g_flow_speedTable[2].feedBack_speed = 0;
        g_flow_speedTable[2].pla_speed = 570;
        g_flow_speedTable[2].plt_speed = 1200;

        g_flow_speedTable[3].draw_speed = 5059;
        g_flow_speedTable[3].ac_speed = 0;
        g_flow_speedTable[3].feedBack_speed = 6000;
        g_flow_speedTable[3].pla_speed = 570;
        g_flow_speedTable[3].plt_speed = 1200;

        g_flow_speedTable[4].draw_speed = maxSpeed.draw_speed;
        g_flow_speedTable[4].ac_speed = (u16)((u32)maxSpeed.draw_speed*100/g_cur_program_param.rate_plt);
        g_flow_speedTable[4].feedBack_speed = 0;
        g_flow_speedTable[4].pla_speed = 570;
        g_flow_speedTable[4].plt_speed = 1200;

        g_flow_speedTable[5].draw_speed = maxSpeed.draw_speed/7 * 4;
        g_flow_speedTable[5].ac_speed = 0;
        g_flow_speedTable[5].feedBack_speed = 6000;
        g_flow_speedTable[5].pla_speed = 570;
        g_flow_speedTable[5].plt_speed = 1200;
        
        memcpy(&g_flow_speedTable[6], &g_flow_speedTable[4], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[7], &g_flow_speedTable[4], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[8], &g_flow_speedTable[5], sizeof(FLOW_PUMP_SPEED));
    }
    else if (maxSpeed.draw_speed < 11236)
    {
        g_flow_speedTable[0].draw_speed = 6250;
        g_flow_speedTable[0].ac_speed = 560;
        g_flow_speedTable[0].feedBack_speed = 0;
        g_flow_speedTable[0].pla_speed = 570;
        g_flow_speedTable[0].plt_speed = 1200;
        
        g_flow_speedTable[1].draw_speed = 4316;
        g_flow_speedTable[1].ac_speed = 0;
        g_flow_speedTable[1].feedBack_speed = 4724;
        g_flow_speedTable[1].pla_speed = 570;
        g_flow_speedTable[1].plt_speed = 1200;

        g_flow_speedTable[2].draw_speed = 8000;
        g_flow_speedTable[2].ac_speed = 689;
        g_flow_speedTable[2].feedBack_speed = 0;
        g_flow_speedTable[2].pla_speed = 570;
        g_flow_speedTable[2].plt_speed = 1200;

        g_flow_speedTable[3].draw_speed = 5059;
        g_flow_speedTable[3].ac_speed = 0;
        g_flow_speedTable[3].feedBack_speed = 6000;
        g_flow_speedTable[3].pla_speed = 570;
        g_flow_speedTable[3].plt_speed = 1200;

        g_flow_speedTable[4].draw_speed = 9416;
        g_flow_speedTable[4].ac_speed = 815;
        g_flow_speedTable[4].feedBack_speed = 0;
        g_flow_speedTable[4].pla_speed = 570;
        g_flow_speedTable[4].plt_speed = 1200;

        g_flow_speedTable[5].draw_speed = 5928;
        g_flow_speedTable[5].ac_speed = 0;
        g_flow_speedTable[5].feedBack_speed = 6977;
        g_flow_speedTable[5].pla_speed = 570;
        g_flow_speedTable[5].plt_speed = 1200;

        g_flow_speedTable[6].draw_speed = maxSpeed.draw_speed;
        g_flow_speedTable[6].ac_speed = (u16)((u32)maxSpeed.draw_speed*100/g_cur_program_param.rate_plt);
        g_flow_speedTable[6].feedBack_speed = 0;
        g_flow_speedTable[6].pla_speed = 570;
        g_flow_speedTable[6].plt_speed = 1200;

        g_flow_speedTable[8].draw_speed = maxSpeed.draw_speed/7*4;
        g_flow_speedTable[8].ac_speed = 0;
        g_flow_speedTable[8].feedBack_speed = maxSpeed.feedBack_speed;
        g_flow_speedTable[8].pla_speed = 570;
        g_flow_speedTable[8].plt_speed = 1200;

        memcpy(&g_flow_speedTable[7], &g_flow_speedTable[6], sizeof(FLOW_PUMP_SPEED));
    }
    else if (maxSpeed.draw_speed < 13000)
    { 
        g_flow_speedTable[0].draw_speed = 6250;
        g_flow_speedTable[0].ac_speed = 560;
        g_flow_speedTable[0].feedBack_speed = 0;
        g_flow_speedTable[0].pla_speed = 570;
        g_flow_speedTable[0].plt_speed = 1200;
        
        g_flow_speedTable[1].draw_speed = 4316;
        g_flow_speedTable[1].ac_speed = 0;
        g_flow_speedTable[1].feedBack_speed = 4724;
        g_flow_speedTable[1].pla_speed = 570;
        g_flow_speedTable[1].plt_speed = 1200;

        g_flow_speedTable[2].draw_speed = 8000;
        g_flow_speedTable[2].ac_speed = 689;
        g_flow_speedTable[2].feedBack_speed = 0;
        g_flow_speedTable[2].pla_speed = 570;
        g_flow_speedTable[2].plt_speed = 1200;

        g_flow_speedTable[3].draw_speed = 5059;
        g_flow_speedTable[3].ac_speed = 0;
        g_flow_speedTable[3].feedBack_speed = 6000;
        g_flow_speedTable[3].pla_speed = 570;
        g_flow_speedTable[3].plt_speed = 1200;

        g_flow_speedTable[4].draw_speed = 9416;
        g_flow_speedTable[4].ac_speed = 815;
        g_flow_speedTable[4].feedBack_speed = 0;
        g_flow_speedTable[4].pla_speed = 570;
        g_flow_speedTable[4].plt_speed = 1200;

        g_flow_speedTable[5].draw_speed = 5928;
        g_flow_speedTable[5].ac_speed = 0;
        g_flow_speedTable[5].feedBack_speed = 6977;
        g_flow_speedTable[5].pla_speed = 570;
        g_flow_speedTable[5].plt_speed = 1200;

        g_flow_speedTable[6].draw_speed = 11236;
        g_flow_speedTable[6].ac_speed = 1000;
        g_flow_speedTable[6].feedBack_speed = 0;
        g_flow_speedTable[6].pla_speed = 570;
        g_flow_speedTable[6].plt_speed = 1200;

        g_flow_speedTable[7].draw_speed = maxSpeed.draw_speed;
        g_flow_speedTable[7].ac_speed = (u16)((u32)maxSpeed.draw_speed*100/g_cur_program_param.rate_plt);
        g_flow_speedTable[7].feedBack_speed = 0;
        g_flow_speedTable[7].pla_speed = 570;
        g_flow_speedTable[7].plt_speed = 1200;

        g_flow_speedTable[8].draw_speed = maxSpeed.draw_speed/7*4;
        g_flow_speedTable[8].ac_speed = 0;
        g_flow_speedTable[8].feedBack_speed = maxSpeed.feedBack_speed;
        g_flow_speedTable[8].pla_speed = 570;
        g_flow_speedTable[8].plt_speed = 1200;
    }
    else
    {
        g_flow_speedTable[0].draw_speed = 6250;
        g_flow_speedTable[0].ac_speed = 560;
        g_flow_speedTable[0].feedBack_speed = 0;
        g_flow_speedTable[0].pla_speed = 570;
        g_flow_speedTable[0].plt_speed = 1200;
        
        g_flow_speedTable[1].draw_speed = 4316;
        g_flow_speedTable[1].ac_speed = 0;
        g_flow_speedTable[1].feedBack_speed = 4724;
        g_flow_speedTable[1].pla_speed = 570;
        g_flow_speedTable[1].plt_speed = 1200;

        g_flow_speedTable[2].draw_speed = 8000;
        g_flow_speedTable[2].ac_speed = 689;
        g_flow_speedTable[2].feedBack_speed = 0;
        g_flow_speedTable[2].pla_speed = 570;
        g_flow_speedTable[2].plt_speed = 1200;

        g_flow_speedTable[3].draw_speed = 5059;
        g_flow_speedTable[3].ac_speed = 0;
        g_flow_speedTable[3].feedBack_speed = 6000;
        g_flow_speedTable[3].pla_speed = 570;
        g_flow_speedTable[3].plt_speed = 1200;

        g_flow_speedTable[4].draw_speed = 9416;
        g_flow_speedTable[4].ac_speed = 815;
        g_flow_speedTable[4].feedBack_speed = 0;
        g_flow_speedTable[4].pla_speed = 570;
        g_flow_speedTable[4].plt_speed = 1200;

        g_flow_speedTable[5].draw_speed = 5928;
        g_flow_speedTable[5].ac_speed = 0;
        g_flow_speedTable[5].feedBack_speed = 6977;
        g_flow_speedTable[5].pla_speed = 570;
        g_flow_speedTable[5].plt_speed = 1200;

        g_flow_speedTable[6].draw_speed = 11236;
        g_flow_speedTable[6].ac_speed = 1000;
        g_flow_speedTable[6].feedBack_speed = 0;
        g_flow_speedTable[6].pla_speed = 570;
        g_flow_speedTable[6].plt_speed = 1200;

        g_flow_speedTable[7].draw_speed = 13000;
        g_flow_speedTable[7].ac_speed = 1200;
        g_flow_speedTable[7].feedBack_speed = 0;
        g_flow_speedTable[7].pla_speed = 700;//maxSpeed.pla_speed;
        g_flow_speedTable[7].plt_speed = 1400;// maxSpeed.plt_speed;

        g_flow_speedTable[8].draw_speed = 7500;
        g_flow_speedTable[8].ac_speed = 0;
        g_flow_speedTable[8].feedBack_speed = maxSpeed.feedBack_speed;
        g_flow_speedTable[8].pla_speed = 700;//maxSpeed.pla_speed;
        g_flow_speedTable[8].plt_speed = 1400;//maxSpeed.plt_speed;
    }
}

void set_flow_speed_table(FLOW_PUMP_SPEED flowSpeed)
{
        
    if (0==g_flow_phase || 2==g_flow_phase || 
     4==g_flow_phase)///g_flow_phase==???
    {
        g_flow_speedTable[g_flow_phase].draw_speed = flowSpeed.draw_speed;
        g_flow_speedTable[g_flow_phase].ac_speed = flowSpeed.ac_speed;
        g_flow_speedTable[g_flow_phase].feedBack_speed = 0;
        g_flow_speedTable[g_flow_phase].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[g_flow_phase].plt_speed = flowSpeed.plt_speed;

        g_flow_speedTable[g_flow_phase+1].draw_speed = flowSpeed.draw_speed/7*4;
        g_flow_speedTable[g_flow_phase+1].ac_speed = 0;
        g_flow_speedTable[g_flow_phase+1].feedBack_speed = flowSpeed.feedBack_speed;
        g_flow_speedTable[g_flow_phase+1].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[g_flow_phase+1].plt_speed = flowSpeed.plt_speed;
        
        if (0 == g_flow_phase)
        {
            memcpy(&g_flow_speedTable[2], &g_flow_speedTable, sizeof(FLOW_PUMP_SPEED)); 
            memcpy(&g_flow_speedTable[3], &g_flow_speedTable[g_flow_phase+1], sizeof(FLOW_PUMP_SPEED));
        }
        if (g_flow_phase < 4)
        {
            memcpy(&g_flow_speedTable[4], &g_flow_speedTable, sizeof(FLOW_PUMP_SPEED));            
            memcpy(&g_flow_speedTable[5], &g_flow_speedTable[g_flow_phase+1], sizeof(FLOW_PUMP_SPEED));
        }
        if (g_flow_phase < 6)
        {
            memcpy(&g_flow_speedTable[6], &g_flow_speedTable, sizeof(FLOW_PUMP_SPEED));  
            memcpy(&g_flow_speedTable[7], &g_flow_speedTable, sizeof(FLOW_PUMP_SPEED));     
        }
        memcpy(&g_flow_speedTable[8], &g_flow_speedTable[g_flow_phase+1], sizeof(FLOW_PUMP_SPEED));
                
    }
    else if (6==g_flow_phase || 7==g_flow_phase)
    {
        if (6==g_flow_phase)
        {
            g_flow_speedTable[g_flow_phase].draw_speed = flowSpeed.draw_speed;
            g_flow_speedTable[g_flow_phase].ac_speed = flowSpeed.ac_speed;
            g_flow_speedTable[g_flow_phase].feedBack_speed = 0;
            g_flow_speedTable[g_flow_phase].pla_speed = flowSpeed.pla_speed;
            g_flow_speedTable[g_flow_phase].plt_speed = flowSpeed.plt_speed;
        }
        g_flow_speedTable[7].draw_speed = flowSpeed.draw_speed;
        g_flow_speedTable[7].ac_speed = flowSpeed.ac_speed;
        g_flow_speedTable[7].feedBack_speed = 0;
        g_flow_speedTable[7].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[7].plt_speed = flowSpeed.plt_speed;

        g_flow_speedTable[8].draw_speed = flowSpeed.draw_speed/7*4;
        g_flow_speedTable[8].ac_speed = 0;
        g_flow_speedTable[8].feedBack_speed = flowSpeed.feedBack_speed;
        g_flow_speedTable[8].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[8].plt_speed = flowSpeed.plt_speed;
    }
    else if (1==g_flow_phase || 3==g_flow_phase || 5==g_flow_phase)
    { 
        g_flow_speedTable[g_flow_phase].draw_speed = flowSpeed.draw_speed;
        g_flow_speedTable[g_flow_phase].ac_speed = 0;
        g_flow_speedTable[g_flow_phase].feedBack_speed = flowSpeed.feedBack_speed;
        g_flow_speedTable[g_flow_phase].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[g_flow_phase].plt_speed = flowSpeed.plt_speed;

        g_flow_speedTable[g_flow_phase+1].draw_speed = flowSpeed.draw_speed/7*4;
        g_flow_speedTable[g_flow_phase+1].ac_speed = flowSpeed.ac_speed;
        g_flow_speedTable[g_flow_phase+1].feedBack_speed = 0;
        g_flow_speedTable[g_flow_phase+1].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[g_flow_phase+1].plt_speed = flowSpeed.plt_speed;

        if (1==g_flow_phase)
        {
            memcpy(&g_flow_speedTable[3], &g_flow_speedTable[g_flow_phase], sizeof(FLOW_PUMP_SPEED));
            memcpy(&g_flow_speedTable[4], &g_flow_speedTable[g_flow_phase+1], sizeof(FLOW_PUMP_SPEED));
        }
        if (g_flow_phase < 5)
        { 
            memcpy(&g_flow_speedTable[5], &g_flow_speedTable[g_flow_phase], sizeof(FLOW_PUMP_SPEED));
            memcpy(&g_flow_speedTable[6], &g_flow_speedTable[g_flow_phase+1], sizeof(FLOW_PUMP_SPEED));
        }
        memcpy(&g_flow_speedTable[7], &g_flow_speedTable[g_flow_phase+1], sizeof(FLOW_PUMP_SPEED));
        memcpy(&g_flow_speedTable[8], &g_flow_speedTable[g_flow_phase], sizeof(FLOW_PUMP_SPEED));
    }
    else if (8==g_flow_phase)
    {
        g_flow_speedTable[7].draw_speed = flowSpeed.draw_speed;
        g_flow_speedTable[7].ac_speed = flowSpeed.ac_speed;
        g_flow_speedTable[7].feedBack_speed = 0;
        g_flow_speedTable[7].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[7].plt_speed = flowSpeed.plt_speed;
        
        g_flow_speedTable[8].draw_speed = flowSpeed.draw_speed/7*4;
        g_flow_speedTable[8].ac_speed = 0;
        g_flow_speedTable[8].feedBack_speed = flowSpeed.feedBack_speed;
        g_flow_speedTable[8].pla_speed = flowSpeed.pla_speed;
        g_flow_speedTable[8].plt_speed = flowSpeed.plt_speed;
    }
    
}

FLOW_PUMP_SPEED get_flow_speed_table(uint8_t offset)
{
    assert_param(offset < FLOW_SPEED_TABLE_SIZE);
    
    g_flow_phase = offset;

    return g_flow_speedTable[offset];
}

uint8_t get_phase_status(void)
{
    return g_flow_phase;
}

FLOW_PUMP_SPEED get_cur_phase_speed(void)
{
    FLOW_PUMP_SPEED l_flow_speed;
    
    if (0==g_flow_phase || 2==g_flow_phase || 4==g_flow_phase || 6==g_flow_phase || 7==g_flow_phase)
    {
        l_flow_speed.draw_speed = g_flow_speedTable[g_flow_phase].draw_speed;
        l_flow_speed.ac_speed = g_flow_speedTable[g_flow_phase].ac_speed;
        l_flow_speed.feedBack_speed = g_flow_speedTable[g_flow_phase+1].feedBack_speed;
        l_flow_speed.pla_speed = g_flow_speedTable[g_flow_phase+1].pla_speed;
        l_flow_speed.plt_speed = g_flow_speedTable[g_flow_phase+1].plt_speed;        
    }
    else if (1==g_flow_phase || 3==g_flow_phase || 5==g_flow_phase)
    {
        l_flow_speed.draw_speed = g_flow_speedTable[g_flow_phase+1].draw_speed;
        l_flow_speed.ac_speed = g_flow_speedTable[g_flow_phase+1].ac_speed;
        l_flow_speed.feedBack_speed = g_flow_speedTable[g_flow_phase].feedBack_speed;
        l_flow_speed.pla_speed = g_flow_speedTable[g_flow_phase].pla_speed;
        l_flow_speed.plt_speed = g_flow_speedTable[g_flow_phase].plt_speed;        
    }
    else if (8==g_flow_phase)
    {
        l_flow_speed.draw_speed = g_flow_speedTable[g_flow_phase-1].draw_speed;
        l_flow_speed.ac_speed = g_flow_speedTable[g_flow_phase-1].ac_speed;
        l_flow_speed.feedBack_speed = g_flow_speedTable[g_flow_phase].feedBack_speed;
        l_flow_speed.pla_speed = g_flow_speedTable[g_flow_phase].pla_speed;
        l_flow_speed.plt_speed = g_flow_speedTable[g_flow_phase].plt_speed;
    }
    
    return l_flow_speed;
}

