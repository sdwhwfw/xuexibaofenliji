/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_slave_board1.h
 * Author             : WQ
 * Date First Issued  : 2013/09/24
 * Description        : This file contains the define of code struct(TPDO/RPDO)
 *
 *******************************************************************************
 * History:
 * DATE 	  | VER   | AUTOR	   | Description
 * 2013/09/24 | v1.0  | WQ		   | initial released
 * 2013/10/15 | v1.1  | WQ		   | add err_code struct
 *******************************************************************************/
#ifndef _STRUCT_SLAVE_BOARD1_H_
#define _STRUCT_SLAVE_BOARD1_H_

#include "stm32f2xx.h"

/********************************************/
/*********����ָ�����               ********/
/********************************************/
__packed struct OrderBits        /*ָ���ʾ*/
{
	uint8_t reserved:2;         /* 0:1   ����  */
	uint8_t feed_back_pump:2;   /* 2:3   �����ָ���ʾ 0x0,0x1,0x2,0x3 */
	uint8_t AC_pump:2;          /* 4:5   AC��ָ���ʾ 0x0,0x1,0x2,0x3   */
	uint8_t draw_pump:2;        /* 6:7   ��Ѫ��ָ���ʾ 0x0,0x1,0x2,0x3 */
};

__packed union  OrderReg
{
   uint8_t            all;
   struct OrderBits   bit;
};

__packed struct Para1Bits       /*����1*/
{
	uint8_t  :2;           /* 0:1  ����*/
	uint8_t feed_back_pump:1;/* 2    ����� 0x0:������¼�г�,0x1:�����г���������¼�¼*/
	uint8_t  :2;           /* 3:4  ����*/
	uint8_t AC_pump:1;      /* 5      AC�� 0x0:������¼�г�,0x1:�����г���������¼�¼*/
	uint8_t  :1;           /* 6    ����*/
	uint8_t draw_pump:1;    /* 7    ��Ѫ�� 0x0:������¼�г�,0x1:�����г���������¼�¼*/

};

__packed union  Para1Reg
{
   uint8_t          all;
   struct Para1Bits bit;
};

__packed struct SingleTotalBit       /**/
{
	uint8_t  :2;              /* 0:1        ����*/
	uint8_t feedback_pump:2;  /* 3:2    0x1:��¼���г�,0x0:��¼�����г�,0x03:���г�����*/
	uint8_t AC_pump:2;        /* 5:4    0x1:��¼���г�,0x0:��¼�����г�,0x03:���г�����*/
	uint8_t draw_pump:2;      /* 7:6    0x1:��¼���г�,0x0:��¼�����г�,0x03:���г�����*/

};

__packed union  SingleTotalReg
{
   uint8_t          all;
   struct SingleTotalBit bit;
};

__packed struct RetransmitBit       /**/
{
	uint8_t retransmit :2;      /* 0:1 �ش���ʾ 0x01 0x02 0x03*/
	uint8_t            :5;      /* 2:6  ����*/
	uint8_t go_on   :1;      /* 7   0x1:����,0x0:����*/
};

__packed union  RetransmitReg
{
   uint8_t          all;
   struct RetransmitBit bit;
};


typedef __packed struct
{
	volatile uint8_t fn;
	volatile uint8_t module_indicate;          /*ģ���ʾ 0x00 0x01 0xA5 0xff*/
	volatile union OrderReg order_indicate;	 /*ָ���ʾ*/
	volatile union Para1Reg para1;			 /*����1*/
	volatile uint8_t init_or_diatance;        /*0x00:init / 0x01:distance */
	volatile union SingleTotalReg single_or_total;   /*0:single / 1:total 3:total clear*/
	volatile uint8_t para4;
	volatile union RetransmitReg continue_retransmit;   /*�ش���ʾ or ����*/
}ControlOrder;

/********************************************/
/*********����ָ���(TASK ERR)     ********/
/********************************************/

__packed struct NumBits
{
	uint8_t num_board:4;    /* 0:3 board number */
	uint8_t num_task :4;    /* 4:7 task number */

};

__packed union NumReg
{
   uint8_t        all;
   struct NumBits bit;
};



typedef __packed struct
{
	volatile uint8_t fn;
	volatile uint8_t module_indicate;          /* 0x00 0x01 */
	volatile uint8_t order_indicate;	          /* 0x01=>task war 0x02=>task err*/
	volatile union NumReg number;			  /* board and task number*/
	volatile uint8_t errcode;
	volatile uint8_t para3;
	volatile uint8_t para4;
	volatile union RetransmitReg continue_retransmit;   /*�ش���ʾ or ����*/
}ControlOrder_t;

/********************************************/
/*********����ָ���ͬ��           ********/
/*********����ָ�����ͬ��           ********/
/********************************************/


typedef __packed struct
{
	volatile uint8_t fn;
	volatile uint8_t answer;			/* ACK/NCK */
	volatile uint8_t status; 		/* ״̬�� */
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t reserved4;
	volatile union RetransmitReg continue_retransmit;   /*�ش���ʾ or ����*/
}ControlOrderSync;

/********************************************/
/*********     ������1               ********/
/*********        					 ********/
/********************************************/

__packed struct PumpDirectionBits
{
	uint8_t  :2;            /* 0:1  ����*/
	uint8_t feed_back_pump:1; /* 2    ����÷��� 0x0,0x1*/
	uint8_t  :1;            /* 3    ����*/
	uint8_t AC_pump:1;       /* 4      AC�÷��� 0x0,0x1*/
	uint8_t  :1;            /* 5    ����*/
	uint8_t draw_pump:1;     /* 6    ��Ѫ�÷��� 0x0,0x1*/
	uint8_t  :1;            /* 7    ����*/
};

__packed union PumpDirectionReg
{
   uint8_t         		    all;
   struct PumpDirectionBits bit;
};



__packed struct PumpCountBits
{
	uint8_t :2;    			/* 0:1  ����*/
	uint8_t feed_back_pump:2; /* 2:3 ����ü���on/off 0x0,0x1*/
	uint8_t AC_pump:2;       /* 4:5   AC�ü���on/off 0x0,0x1*/
	uint8_t draw_pump:2;     /* 6:7 ��Ѫ�ü���on/off 0x0,0x1*/

};

__packed union PumpCountReg
{
   uint8_t          	all;
   struct PumpCountBits bit;
};

typedef __packed struct
{
	volatile uint16_t draw_pump_speed_limit;			/* ��Ѫ���ٶ����� */
	volatile uint16_t AC_pump_speed_limit;				/* AC���ٶ����� */
	volatile uint16_t feed_back_pump_speed_limit; 		/* ������ٶ����� */
	volatile union PumpDirectionReg pump_direction;	/* �÷���*/
	volatile union PumpCountReg pump_count;			/* �ü���*/

}PumpSet1;

/********************************************/
/*********     ��״̬1               ********/
/*********        					 ********/
/********************************************/

__packed struct PumpCurrentDirBits
{
	uint8_t :2;    			/* 0:1  ����*/
	uint8_t feed_back_pump:2; /* 2:3 ����õ�ǰ���� 0x0,0x1,0x2*/
	uint8_t AC_pump:2;       /* 4:5   AC�õ�ǰ���� 0x0,0x1,0x2*/
	uint8_t draw_pump:2;     /* 6:7 ��Ѫ�õ�ǰ���� 0x0,0x1,0x2*/
};

__packed union PumpCurrentDirReg
{
   uint8_t          		 all;
   struct PumpCurrentDirBits bit;
};


typedef __packed struct
{
	volatile uint16_t draw_pump_current_speed;			/* ��Ѫ�õ�ǰ�ٶ� */
	volatile uint16_t AC_pump_current_speed;			/*   AC�õ�ǰ�ٶ� */
	volatile uint16_t feed_back_pump_current_speed; 		/* ����õ�ǰ�ٶ� */
	volatile union PumpCurrentDirReg pump_current_dir;	/* �õ�ǰ����*/
	volatile uint8_t fn;
}PumpState1;


/********************************************/
/*********     ������2               ********/
/*********        					 ********/
/********************************************/

typedef __packed struct
{
	volatile uint16_t draw_pump_need_distance;		/* ��Ѫ���������г� */
	volatile uint16_t AC_pump_need_distance;		/*   AC���������г�*/
	volatile uint16_t feed_back_pump_need_distance; 	/* ������������г� */
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
}PumpSet2;

/********************************************/
/*********     ��״̬2               ********/
/*********        					 ********/
/********************************************/

typedef __packed struct
{
	volatile uint16_t draw_pump_moved_distance;			/* ��Ѫ���������г� */
	volatile uint16_t AC_pump_moved_distance;			/*   AC���������г�*/
	volatile uint16_t feed_back_pump_moved_distance; 	/* ������������г� */
	volatile union SingleTotalReg single_or_total;      /*0:single / 1:total */
	volatile uint8_t fn;
}PumpState2;

/********************************************/
/*********     ��������ȡ            ********/
/*********        					 ********/
/********************************************/
__packed struct LiquidLevelBits
{
	uint8_t :5;    			/* 0:4  ����*/
	uint8_t low_level:1;     /* 5 ��Һλ̽���� 0x0,0x1*/
	uint8_t	high_level:1;    /* 6 ��Һλ̽���� 0x0,0x1*/
	uint8_t AC_bubble:1;     /* 7 AC����̽���� 0x0,0x1*/
};

__packed union LiquidLevelReg
{
   uint8_t                all;
   struct LiquidLevelBits bit;
};


__packed struct PumpInitBits
{
	uint8_t :5;    					/* 0:4  ����*/
	uint8_t feed_back_pump_init:1;     /* 5 ����ó�ʼ��״̬ 0x0,0x1*/
	uint8_t	AC_pump_init:1;            /* 6   AC�ó�ʼ��״̬ 0x0,0x1*/
	uint8_t draw_pump_init:1;          /* 7 ��Ѫ�ó�ʼ��״̬ 0x0,0x1*/
};

__packed union PumpInitReg
{
   uint8_t          all;
   struct PumpInitBits bit;
};



typedef __packed struct
{
	volatile uint16_t blood_collect_preasure;			/* ��Ѫѹ�� */
	volatile uint16_t blood_feedback_preasure;			/* ����ѹ��*/
	volatile union LiquidLevelReg liquid_level;       /* AC���ݼ�Һλ̽����*/
	volatile union PumpInitReg pump_init;					/* �ó�ʼ�� */
	volatile uint8_t feedback_pump_state;             /*�����״̬ Ӳ��IO���� 0x01 0x00*/
	volatile uint8_t reserved2;
}SensorState;


/********************************************/
/*********  �á����ſ�����Ϣ����     ********/
/*********        					 ********/
/********************************************/

struct PumpOrderBits
{
	uint16_t draw_pump :2;             /*lowbyte 0:1  ��Ѫ��   0x0,0x1,0x2,0x3*/
	uint16_t AC_pump :2;               /*lowbyte 2:3  AC��     0x0,0x1,0x2,0x3*/
	uint16_t feedback_pump :2;         /*lowbyte 4:5  �����   0x0,0x1,0x2,0x3*/
	uint16_t PLT_pump:2;               /*lowbyte 6:7  ѪС��� 0x0,0x1,0x2,0x3*/

	uint16_t plasma_pump:2;            /*highbyte 0:1   Ѫ����   0x0,0x1,0x2,0x3*/
	uint16_t centrifuge_motor:2;       /*highbyte 2:3   ���Ļ�   0x0,0x1,0x2,0x3*/
	uint16_t cassette_motor:2;         /*highbyte 4:5   ��ϻ��� 0x0,0x1,0x2,0x3*/
	uint16_t  :2;          			   /*highbyte 6:7   ���� */
};

union  PumpOrderReg
{
   uint16_t          all;
   struct PumpOrderBits bit;
};


struct ValveOrderBits
{
	uint8_t magnet:2;  				 /* 0:1  ����� 0x0,0x1,0x2,0x3*/
	uint8_t RBC_valve:2;				 /* 2:3 ��ϸ����ָ���ʾ 0x0,0x1,0x2,0x3*/
	uint8_t plasma_valve:2;     		/* 4:5   Ѫ����ָ���ʾ 0x0,0x1,0x2,0x3*/
	uint8_t PLT_valve:2;  			 /* 6:7 ѪС�巧ָ���ʾ 0x0,0x1,0x2,0x3*/
};


union  ValveOrderReg
{
   uint8_t          all;
   struct ValveOrderBits bit;
};


typedef struct
{
    uint8_t fn;
    uint8_t go_on;
    volatile uint8_t pump_or_valve;        //module_indicate 0x00,0x01,0x02,0xff
	volatile union PumpOrderReg pump;    /*��/���Ļ�/��ϻ����*/
	volatile union ValveOrderReg valve;  /*���š����������*/
} PumpValveOrder;           /*Pump valve�������ƽṹ��*/


/********************************************/
/*********  �ô�����                 ********/
/*********        					 ********/
/********************************************/

__packed struct PumpErrcode1Bits
{
	uint8_t draw_pump:4;    	/* 0:3 draw_pump errcode  0x0,0x1,0x2��0x3��0x4*/
	uint8_t AC_pump  :4;          /* 4:7   AC_pump errcode  0x0,0x1,0x2��0x3��0x4*/

};

__packed union PumpErrcode1Reg
{
   uint8_t          	  all;
   struct PumpErrcode1Bits bit;
};

__packed struct PumpErrcode2Bits
{
	uint8_t feedback_pump:4;    /* 0:3 feedback_pump errcode  0x0,0x1,0x2��0x3��0x4*/
	uint8_t reserved :4;        /* 4:7  reserved*/

};

__packed union PumpErrcode2Reg
{
   uint8_t          	  all;
   struct PumpErrcode2Bits bit;
};

__packed struct SensorErrcodeBits
{
    uint8_t blood_preesure      :1;    /*bit 0  0x00 0x01 */
    uint8_t AC_bubble           :1;    /*bit 1  0x00 0x01 */
    uint8_t high_level          :1;    /*bit 2  0x00 0x01 */
    uint8_t low_level           :1;    /*bit 3  0x00 0x01 */
    uint8_t                     :4;    /*reserved  */

};

__packed union SensorErrcodeReg
{
   uint8_t          	     all;
   struct SensorErrcodeBits  bit;
};


typedef __packed struct
{
	volatile union PumpErrcode1Reg  pump_errcode1;	/*draw_pump and AC_pump errcode */
	volatile union PumpErrcode2Reg  pump_errcode2;    /*feedback_pump errcode */
	volatile union SensorErrcodeReg sensor_errcode;    /*sensor errcode */
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t reserved4;
	volatile uint8_t reserved5;
	volatile uint8_t reserved6;
}PumpErrCode;



/********************************************/
/********* para table interface      ********/
/*********        					 ********/
/********************************************/
typedef enum
{
	DRAW_PUMP ,    //0x00
	AC_PUMP,    //0x01
	FEEDBACK_PUMP
} pump_type;
#define IS_PUMP_TYPE(TYPE) ((TYPE) == DRAW_PUMP || (TYPE) == AC_PUMP || (TYPE) == FEEDBACK_PUMP)


typedef enum
{
	STOP ,    //0x00
	CLOCKWISE,    //0x01
	UNCLOCKWISE  //0x02
} pump_direction;
#define IS_PUMP_DIRECTION(DIRECTION) ((DIRECTION) == STOP || (DIRECTION) == CLOCKWISE || (DIRECTION) == UNCLOCKWISE)

typedef enum
{
	SINGLE ,    //0x00
	TOTAL,   //0x01
	CLEAR = 0x03
} distance_totalsingle;

#define IS_DISTANCE_TOTALSINGLE(FLAG) ((FLAG) == SINGLE || (FLAG) == TOTAL )


typedef enum
{
	NORMAL ,    //0x00
	ABNORMAL    //0x01
} sensor_status;

#define IS_SENSOR_STATUS(STATUS) ((STATUS) == NORMAL || (STATUS) == ABNORMAL)

/*
*AC_bubble exist or none
*/
typedef enum
{
	BUBBLE_NONE ,  //0x00
	BUBBLE_EXIST    //0x01
} AC_bubble_status;

#define IS_AC_BUBBLE_STATUS(STATUS) ((STATUS) == BUBBLE_NONE || (STATUS) == BUBBLE_EXIST)

typedef enum
{
	LEVEL_NONE ,  //0x00
	LEVEL_EXIST    //0x01
} level_status;

#define IS_LEVEL_STATUS(STATUS) ((STATUS) == LEVEL_NONE || (STATUS) == LEVEL_EXIST)


typedef enum
{
	INIT_UNFINISHED ,  //0x00
	INIT_FINISHED      //0x01
} pump_init_status;

#define IS_PUMP_INIT_STATUS(STATUS) ((STATUS) == INIT_UNFINISHED || (STATUS) == INIT_FINISHED)



void write_pump_direction(pump_type pump,pump_direction dir);
void write_pump_speed(pump_type pump,uint16_t speed);
void write_pump_moved_distance(pump_type pump,uint16_t distance);
void write_pump_distance_flag(pump_type pump,distance_totalsingle flag);
distance_totalsingle read_pump_distance_flag(pump_type pump);




void write_blood_collect_preesure(u16 preesure);
void write_AC_bubble(AC_bubble_status bubble);
void write_hige_level(level_status level);
void write_low_level(level_status level);
void write_feedback_pump_state(sensor_status status);

void write_draw_pump_init(pump_init_status status);
void write_AC_pump_init(pump_init_status status);
void write_feedback_pump_init(pump_init_status status);





#endif



