/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_slave_board2.h
 * Author             : wenquan
 * Date First Issued  : 2013/09/24
 * Description        : This file contains the define of code struct(TPDO/RPDO)
 *
 *******************************************************************************
 * History:
 * DATE 	  | VER   | AUTOR	   | Description
 * 2013/09/24 | v1.0  | WQ		   | initial released
 * 2013/10/15 | v1.1  | WQ		   | add err_code struct
 *******************************************************************************/
#ifndef _STRUCT_SLAVE_BOARD2_H_
#define _STRUCT_SLAVE_BOARD2_H_

#include "stm32f2xx.h"


/********************************************/
/*********控制指令接收               ********/
/********************************************/
__packed struct OrderBits        /*指令标示*/
{
	uint8_t reserved:2;  			 /* 0:1  保留*/
	uint8_t RBC_valve:2;				 /* 2:3 红细胞阀指令标示 0x0,0x1,0x2,0x3*/
	uint8_t plasma_valve_or_pump:2;     /* 4:5   血浆阀或血浆泵指令标示 0x0,0x1,0x2,0x3*/
	uint8_t PLT_valve_or_pump:2;  		 /* 6:7 血小板阀或血小板泵指令标示 0x0,0x1,0x2,0x3*/
};

__packed union  OrderReg
{
   uint8_t            all;
   struct OrderBits   bit;
};

__packed struct Para1Bits       /*参数1*/
{
	uint8_t  :5;           /* 0:4  保留*/
	uint8_t plasma_pump:1;  /* 5      AC泵 0x0:继续行程记录 0x1:行程清零后继续记录*/
	uint8_t  :1;           /* 6    保留*/
	uint8_t PLT_pump:1;     /* 7    PLT泵 0x0:继续行程记录 0x1:行程清零后继续记录 */

};

__packed union  Para1Reg
{
   uint8_t          all;
   struct Para1Bits bit;
};

__packed struct SingleTotalBit       /**/
{
	uint8_t  :4;              /* 0:3  保留*/
	uint8_t plasma_pump:2;    /* 5:4    0x1:记录总行程,0x0:记录单次行程,0x03 总行程清零*/
	uint8_t PLT_pump:2;       /* 7:6    0x1:记录总行程,0x0:记录单次行程，0x03 总行程清零*/

};

__packed union  SingleTotalReg
{
   uint8_t          all;
   struct SingleTotalBit bit;
};

__packed struct RetransmitBit       /**/
{
	uint8_t retransmit :2;      /* 0:1 重传标示 0x01 0x02 0x03*/
	uint8_t            :5;      /* 2:6  保留*/
	uint8_t go_on   :1;      /* 7   0x1:继续,0x0:运行*/

};

__packed union  RetransmitReg
{
   uint8_t          all;
   struct RetransmitBit bit;
};

typedef __packed struct
{
	volatile uint8_t fn;
	volatile uint8_t module_indicate;          /*模块标示 0x00 0x01 0xff*/
	volatile union OrderReg order_indicate;	  /*指令标示*/
	volatile union Para1Reg para1;			  /*参数1*/
	volatile uint8_t init_or_diatance;         /*0x00:init / 0x01:distance */
	volatile union SingleTotalReg single_or_total;   /*0:single / 1:total 3:total clear */
	volatile uint8_t para4;
	volatile union RetransmitReg continue_retransmit;   /*重传标示 or 继续*/
}ControlOrder;
/********************************************/
/*********控制指令发送(TASK ERR)     ********/
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
	volatile union RetransmitReg continue_retransmit;   /*重传标示 or 继续*/
}ControlOrder_t;


/********************************************/
/*********控制指令发送同步           ********/
/*********控制指令接收同步           ********/
/********************************************/

typedef __packed struct
{
	volatile uint8_t fn;
	volatile uint8_t answer;			/* ACK/NCK */
	volatile uint8_t status; 		/* 状态字 */
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t reserved4;
	volatile union RetransmitReg continue_retransmit;   /*重传标示 */
}ControlOrderSync;

/********************************************/
/*********     泵设置1               ********/
/*********        					 ********/
/********************************************/

__packed struct PumpDirectionBits
{
	uint8_t  :4;            	/* 0:3  保留*/
	uint8_t plasma_pump:1;       /* 4      血浆泵方向 0x0,0x1*/
	uint8_t  :1;           		/* 5    保留*/
	uint8_t PLT_pump:1;    		/* 6   PLT泵方向 0x0,0x1*/
	uint8_t  :1;           		/* 7    保留*/
};

__packed union PumpDirectionReg
{
   uint8_t         		    all;
   struct PumpDirectionBits bit;
};



__packed struct PumpCountBits
{
	uint8_t :4;    				/* 0:3  保留*/
	uint8_t plasma_pump:2;       /* 4:5  血浆泵计数on/off 0x0,0x1*/
	uint8_t PLT_pump:2;    		/* 6:7  PLT泵计数on/off 0x0,0x1*/

};

__packed union PumpCountReg
{
   uint8_t          	all;
   struct PumpCountBits bit;
};

typedef __packed struct
{
	volatile uint16_t PLT_pump_speed_limit;				/* PLT泵速度限制 */
	volatile uint16_t plasma_pump_speed_limit;				/* 血浆泵速度限制 */
	volatile uint16_t reserved;
	volatile union PumpDirectionReg pump_direction;	/* 泵方向*/
	volatile union PumpCountReg Pump_count;			/* 泵计数*/

}PumpSet1;

/********************************************/
/*********     泵状态1               ********/
/*********        					 ********/
/********************************************/

__packed struct PumpCurrentDirBits
{
	uint8_t :4;    				/* 0:3  保留*/
	uint8_t plasma_pump:2;       /* 4:5   血浆泵当前方向 0x0,0x1,0x2*/
	uint8_t PLT_pump:2;    		/* 6:7 血小板泵当前方向 0x0,0x1,0x2*/
};

__packed union PumpCurrentDirReg
{
   uint8_t          		 all;
   struct PumpCurrentDirBits bit;
};


typedef __packed struct
{
	volatile uint16_t PLT_pump_current_speed;			/* PLT泵当前速度 */
	volatile uint16_t plasma_pump_current_speed;			/* Plasma泵当前速度 */
	volatile uint16_t reserved1;
	volatile union PumpCurrentDirReg pump_current_dir;	/* 泵当前方向*/
	volatile uint8_t fn;                                 /*收到的RPDO3的fn，用于标示哪次行程的反馈*/
}PumpState1;


/********************************************/
/*********     泵设置2               ********/
/*********        					 ********/
/********************************************/

typedef __packed struct
{
	volatile uint16_t PLT_pump_need_distance;		/* PLT泵需运行行程 */
	volatile uint16_t plasma_pump_need_distance;		/*   血浆泵需运行行程*/
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t reserved4;
}PumpSet2;

/********************************************/
/*********     泵状态2               ********/
/*********        					 ********/
/********************************************/

typedef __packed struct
{
	volatile uint16_t PLT_pump_moved_distance;			/* PLT泵已运行行程 */
	volatile uint16_t plasma_pump_moved_distance;			/*   Plasma泵已运行行程*/
	volatile union SingleTotalReg single_or_total;   /*0:single / 1:total */
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t fn;                       /*收到的RPDO3的fn，用于标示哪次行程的反馈*/
}PumpState2;

/********************************************/
/*********     传感器读取            ********/
/*********        					 ********/
/********************************************/

__packed struct vlaveBits
{
	uint8_t RBC_valve_left:1;    					 /* 0  红细胞阀左 0x0,0x1*/
	uint8_t RBC_valve_mid:1;    					 /* 1  红细胞阀中 0x0,0x1*/
	uint8_t RBC_valve_right:1;    				     /* 2  红细胞阀右 0x0,0x1*/
	uint8_t plsama_valve_left:1;    				 /* 3    血浆阀左 0x0,0x1*/
	uint8_t plsama_valve_mid:1;    				     /* 4    血浆阀中 0x0,0x1*/
	uint8_t plsama_valve_right:1;    				 /* 5    血浆阀右 0x0,0x1*/
	uint8_t PLT_valve_left:1;    					 /* 6     PLT阀左 0x0,0x1*/
	uint8_t PLT_valve_mid:1;    					 /* 7     PLT阀中 0x0,0x1*/
	uint8_t PLT_valve_right:1;    				     /* 8     PLT阀右 0x0,0x1*/
	uint8_t :7;    					                 /* 9~17  保留*/
};

__packed union ValveReg
{
   uint16_t         all;
   struct vlaveBits bit;
};

__packed struct PumpInitBits
{
	uint8_t :6;    					 /* 0:5  保留*/
	uint8_t	PLT_pump_init:1;        /* 6   PLT泵初始化 0x0,0x1*/
	uint8_t plasma_pump_init:1;     /* 7   血浆泵初始化 0x0,0x1*/
};

__packed union PumpInitReg
{
   uint8_t          all;
   struct PumpInitBits bit;
};

typedef __packed struct
{
	volatile uint16_t RBC_detector;			/* 红细胞探测器 */
	volatile union ValveReg valve;           /* 9个阀光电开关信号*/
	volatile union PumpInitReg pump_init;			 /* 泵初始化 */
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
}SensorState;


/********************************************/
/*********  泵、阀门控制消息队列     ********/
/*********        					 ********/
/********************************************/

struct PumpOrderBits
{
	uint16_t draw_pump :2;            /*lowbyte 0:1  采血泵  0x0,0x1,0x2,0x3*/
	uint16_t AC_pump :2;               /*lowbyte 2:3  AC泵   0x0,0x1,0x2,0x3*/
	uint16_t feedback_pump :2;        /*lowbyte 4:5  回输泵 0x0,0x1,0x2,0x3*/
	uint16_t PLT_pump:2;              /*lowbyte 6:7  血小板泵 0x0,0x1,0x2,0x3*/

	uint16_t plasma_pump:2;           /*highbyte 0:1   血浆泵 0x0,0x1,0x2,0x3*/
	uint16_t centrifuge_motor:2;      /*highbyte 2:3   离心机 0x0,0x1,0x2,0x3*/
	uint16_t cassette_motor:2;        /*highbyte 4:5   卡匣电机 0x0,0x1,0x2,0x3*/
	uint16_t  :2;          			 /*highbyte 6:7   保留 */
};

union  PumpOrderReg
{
   uint16_t          all;
   struct PumpOrderBits bit;
};


struct ValveOrderBits
{
	uint8_t magnet:2;  				 /* 0:1  电磁体 0x0,0x1,0x2,0x3*/
	uint8_t RBC_valve:2;				 /* 2:3 红细胞阀指令标示 0x0,0x1,0x2,0x3*/
	uint8_t plasma_valve:2;     		/* 4:5   血浆阀指令标示 0x0,0x1,0x2,0x3*/
	uint8_t PLT_valve:2;  			 /* 6:7 血小板阀指令标示 0x0,0x1,0x2,0x3*/
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
	volatile union PumpOrderReg pump;    /*泵/离心机/卡匣动作*/
	volatile union ValveOrderReg valve;  /*阀门、电磁铁动作*/
} PumpValveOrder;           /*Pump valve动作控制结构体*/


/********************************************/
/*********  泵 阀门 错误码           ********/
/*********        					 ********/
/********************************************/

__packed struct PumpErrcode1Bits
{
	uint8_t plasma_pump:4;    	/* 0:3 plasma_pump errcode  0x0,0x1,0x2，0x3，0x4*/
	uint8_t PLT_pump   :4;      /* 4:7    PLT_pump errcode  0x0,0x1,0x2，0x3，0x4*/

};

__packed union PumpErrcode1Reg
{
   uint8_t          	  all;
   struct PumpErrcode1Bits bit;
};

__packed struct ValveErrcode1Bits
{
	uint8_t RBC_valve     :2;    /* 0:1       RBC_valve errcode  0x0,0x03*/
	uint8_t plasma_valve  :2;    /* 2:3    plasma_valve errcode  0x0,0x03*/
    uint8_t PLT_valve     :2;    /* 4:5       PLT_valve errcode  0x0,0x03*/
	uint8_t   :2;                /* 6:7   */

};

__packed union ValveErrcode1Reg
{
   uint8_t          	    all;
   struct ValveErrcode1Bits bit;
};

__packed struct SensorErrcodeBits
{
    uint8_t RBC_detector        :1;    /*bit 0  0x00 0x01 */
    uint8_t                     :7;    /*reserved  */

};

__packed union SensorErrcodeReg
{
   uint8_t          	     all;
   struct SensorErrcodeBits  bit;
};

typedef __packed struct
{
	volatile union PumpErrcode1Reg  pump_errcode1;	 /*plasma_pump and PLT_pump errcode */
	volatile union ValveErrcode1Reg valve_errcode1;    /*RBC_valve plasma_valve PLT_valve errcode */
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
	PLT_PUMP ,    //0x00
	PLASMA_PUMP    //0x01
} pump_type;
#define IS_PUMP_TYPE(TYPE) ((TYPE) == PLT_PUMP || (TYPE) == PLASMA_PUMP )


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
	UNOVERFLOW , //0x00
	OVERFLOW    //0x01
} RBC_detector;

#define IS_RBC_DETECTOR_STATUS(STATUS) ((STATUS) == UNOVERFLOW || (STATUS) == OVERFLOW)

typedef enum
{
	INIT_UNFINISHED ,  //0x00
	INIT_FINISHED      //0x01
} pump_init_status;

#define IS_PUMP_INIT_STATUS(STATUS) ((STATUS) == INIT_UNFINISHED || (STATUS) == INIT_FINISHED)

typedef enum
{
	RBC_VALVE ,      //0x00
    PLASMA_VALVE ,   //0x01
    PLT_VALVE
} valve_type;

#define IS_VALVE_TYPE(TYPE) (   (TYPE) == RBC_VALVE || \
                                (TYPE) == PLASMA_VALVE || \
                                (TYPE) == PLT_VALVE \
                             )

typedef enum
{
	LEFT_VALVE = 0x00,      //0x00
    MID_VALVE = 0x01,   //0x01
    RIGHT_VALVE = 0x02,
    UNKNOW_VALVE = 0x03
} valve_location_type;

#define IS_VALVE_LOCATION_TYPE(TYPE) ((TYPE) == LEFT_VALVE ||(TYPE) == MID_VALVE || (TYPE) == RIGHT_VALVE || (TYPE) == UNKNOW_VALVE)




void write_pump_direction(pump_type pump,pump_direction dir);
void write_pump_speed(pump_type pump,uint16_t speed);
void write_pump_moved_distance(pump_type pump,uint16_t distance);
void write_pump_distance_flag(pump_type pump,distance_totalsingle flag);
distance_totalsingle read_pump_distance_flag(pump_type pump);




void write_RBC_detector(RBC_detector statue);
void write_valve_location(valve_type type,valve_location_type location);
void write_PLT_pump_init(pump_init_status status);
void write_plasma_pump_init(pump_init_status status);


#endif







