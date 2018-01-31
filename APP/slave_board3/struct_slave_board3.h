/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_slave_board3.h
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
#ifndef _STRUCT_SLAVE_BOARD3_H_
#define _STRUCT_SLAVE_BOARD3_H_

#include "stm32f2xx.h"


/********************************************/
/*********����ָ�����               ********/
/********************************************/
__packed struct OrderBits        /*ָ���ʾ*/
{
	uint8_t reserved:4;  					 /* 0:3  ����*/
	uint8_t cassette_motor:2;    			 /* 4:5    ��ϻ���ָ���ʾ 0x1,0x2,0x3*/
	uint8_t centrifugeo_or_electromagnet:2;  	 /* 6:7 ���Ļ�����ŵ����ָ���ʾ 0x0,0x1,0x2,0x3*/
};

__packed union  OrderReg
{
   uint8_t            all;
   struct OrderBits   bit;
};

__packed struct Para1Bits       /*����1*/
{
	uint8_t  :5;         		 	 /* 0:4  ����*/
	uint8_t cassette_motor:1; 		 /* 5    ��ϻ��� 0x0:�����г̼�¼ 0x1:�г�����������¼*/
	uint8_t  :1;          		 	 /* 6    ����*/
	uint8_t centrifuge_motor:1;       /* 7    ���Ļ� 0x0:�����г̼�¼ 0x1:�г�����������¼ */

};

__packed union  Para1Reg
{
   uint8_t          all;
   struct Para1Bits bit;
};

__packed struct RetransmitBit       /**/
{
	uint8_t retransmit :2;      /* 0:1 �ش���ʾ 0x01 0x02 0x03*/
	uint8_t            :5;      /* 2:6  ����*/
	uint8_t            :1;      /* 7 ����*/

};

__packed union  RetransmitReg
{
   uint8_t          all;
   struct RetransmitBit bit;
};

typedef __packed struct
{
	volatile uint8_t fn;
	volatile uint8_t module_indicate;          /*ģ���ʾ 0x00 0x01 0xff*/
	volatile union OrderReg order_indicate;	 /*ָ���ʾ*/
	volatile union Para1Reg para1;			 /*����1*/
	volatile uint8_t para2;
	volatile uint8_t para3;
	volatile uint8_t para4;
	volatile union RetransmitReg continue_retransmit;   /*�ش���ʾ */
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
	volatile union RetransmitReg continue_retransmit;   /*�ش���ʾ */
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
	volatile union RetransmitReg continue_retransmit;   /*�ش���ʾ */
}ControlOrderSync;

/********************************************/
/*********     ������1               ********/
/*********        					 ********/
/********************************************/

__packed struct MotorDirectionBits
{
	uint8_t  :4;            	/* 0:3  ����*/
	uint8_t cassette_motor:1;    /* 4    ��ϻ������� 0x0,0x1*/
	uint8_t  :1;           		/* 5    ����*/
	uint8_t centrifuge_motor:1;  /* 6    ���Ļ����� 0x0,0x1*/
	uint8_t  :1;           		/* 7    ����*/
};

__packed union MotorDirectionReg
{
   uint8_t         		    all;
   struct MotorDirectionBits bit;
};



__packed struct MotorCountBits
{
	uint8_t :4;    					/* 0:3  ����*/
	uint8_t cassette_motor:2;        /* 4:5  ��ϻ�������on/off 0x0,0x1*/
	uint8_t centrifuge_motor:2;    	/* 6:7  ���Ļ�����on/off 0x0,0x1*/

};

__packed union MotorCountReg
{
   uint8_t          	all;
   struct MotorCountBits bit;
};

typedef __packed struct
{
	volatile uint16_t centrifuge_motor_speed_limit;				/* ���Ļ��ٶ����� */
	volatile uint16_t cassette_motor_speed_limit;				/* ��ϻ����ٶ����� */
	volatile uint16_t reserved;
	volatile union MotorDirectionReg motor_direction;	/* ���Ļ�/��ϻ�������*/
	volatile union MotorCountReg motor_count;			/* ���Ļ�/��ϻ�������*/

}PumpSet1;

/********************************************/
/*********     ��״̬1               ********/
/*********        					 ********/
/********************************************/

__packed struct MotorCurrentDirBits
{
	uint8_t :4;    				/* 0:3  ����*/
	uint8_t cassette_motor:2;       /* 4:5   ��ϻ�����ǰ���� 0x0,0x1,0x2*/
	uint8_t centrifuge_motor:2;    		/* 6:7 ���Ļ���ǰ���� 0x0,0x1,0x2*/
};

__packed union MotorCurrentDirReg
{
   uint8_t          		 all;
   struct MotorCurrentDirBits bit;
};


typedef __packed struct
{
	volatile uint16_t centrifuge_motor_current_speed;		/* ���ĵ����ǰ�ٶ� */
	volatile uint16_t cassette_motor_current_speed;			/* ��ϻ�����ǰ�ٶ� */
	volatile uint16_t reserved1;
	volatile union MotorCurrentDirReg motor_current_dir;	/* �õ�ǰ����*/
	volatile uint8_t reserved2;
}PumpState1;


/********************************************/
/*********     ������2               ********/
/*********        					 ********/
/********************************************/

typedef __packed struct
{
	volatile uint16_t centrifuge_motor_need_distance;		/* ���ĵ���������г� */
	volatile uint16_t cassette_motor_need_distance;			/*   ��ϻ����������г�*/
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t reserved4;
}PumpSet2;

/********************************************/
/*********     ��״̬2               ********/
/*********        					 ********/
/********************************************/

typedef __packed struct
{
	volatile uint16_t centrifuge_motor_moved_distance;			/* ���Ļ��������г� */
	volatile uint16_t cassette_motor_moved_distance;			/*   ��ϻ����������г�*/
	volatile uint8_t reserved1;
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
	volatile uint8_t reserved4;
}PumpState2;

/********************************************/
/*********     ��������ȡ            ********/
/*********        					 ********/
/********************************************/

__packed struct SensorBits
{
	uint8_t :3;    					/* 0:2  ����*/
	uint8_t	cassette_location:2;	/*3:4  ��ϻλ�ô����� 0x01 0x02 0x00 0x03*/
	uint8_t	door_hoare:1;			/* 5  ���Ż���������*/
	uint8_t	door_switch2:1;    		/* 6  ���Ź�翪��2 0x0,0x1*/
	uint8_t door_switch1:1;      	/* 7  ���Ź�翪��1 0x0,0x1*/
};

__packed union SensorReg
{
   uint8_t          all;
   struct SensorBits bit;
};



typedef __packed struct
{
	volatile uint16_t weeping_detector;			   /*©Һ̽���� */
	volatile uint16_t centrifuge_motor_preasure;   /*���Ļ�ѹ��������*/
	volatile union SensorReg sensor;		       /*���Ŵ���������ϻλ�ô����� */
	volatile uint8_t centrifuge_motor_state;       /*���Ļ�״̬ Ӳ������ 0x00 0x01*/
	volatile uint8_t reserved2;
	volatile uint8_t reserved3;
}SensorState;


/********************************************/
/*********  �á����ſ�����Ϣ����     ********/
/*********        					 ********/
/********************************************/

struct PumpOrderBits
{
	uint16_t draw_pump :2;            /*lowbyte 0:1  ��Ѫ��  0x0,0x1,0x2,0x3*/
	uint16_t AC_pump :2;               /*lowbyte 2:3  AC��   0x0,0x1,0x2,0x3*/
	uint16_t feedback_pump :2;        /*lowbyte 4:5  ����� 0x0,0x1,0x2,0x3*/
	uint16_t PLT_pump:2;              /*lowbyte 6:7  ѪС��� 0x0,0x1,0x2,0x3*/

	uint16_t plasma_pump:2;           /*highbyte 0:1   Ѫ���� 0x0,0x1,0x2,0x3*/
	uint16_t centrifuge_motor:2;      /*highbyte 2:3   ���Ļ� 0x0,0x1,0x2,0x3*/
	uint16_t cassette_motor:2;        /*highbyte 4:5   ��ϻ��� 0x0,0x1,0x2,0x3*/
	uint16_t  :2;          			 /*highbyte 6:7   ���� */
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
	volatile uint8_t pump_or_valve;        //module_indicate  0x01,0x02,0xff is ignore
	volatile union PumpOrderReg pump;    /*��/���Ļ�/��ϻ����*/
	volatile union ValveOrderReg valve;  /*���š����������*/
} PumpValveOrder;           /*Pump valve�������ƽṹ��*/



/********************************************/
/*********  ���Ļ� ��ϻ���������    ********/
/*********        					 ********/
/********************************************/

__packed struct MotorErrcode1Bits
{
	uint8_t centrifuge_motor   :4;    /* 0:3 centrifuge_motor errcode  0x0,0x1,0x2��0x3��0x4*/
	uint8_t cassette_location  :4;    /* 4:7   cassette_location     0x0,0x1 */

};

__packed union MotorErrcode1Reg
{
   uint8_t          	  all;
   struct MotorErrcode1Bits bit;
};


__packed struct VoltageErrcodeBits
{
    uint8_t volage_70V              :1;    /*bit 0  0x00 0x01 */
    uint8_t volage_24V              :1;    /*bit 1  0x00 0x01 */
    uint8_t volage_S16              :1;    /*bit 2  0x00 0x01 */
    uint8_t volage_S19              :1;    /*bit 3  0x00 0x01 */
    uint8_t volage_positive_12V     :1;    /*bit 4  0x00 0x01 */
    uint8_t volage_negative_12V     :1;    /*bit 5  0x00 0x01 */
    uint8_t volage_5V               :1;    /*bit 6  0x00 0x01 */
    uint8_t                         :1;    /*bit 7  reserved  */

};

__packed union VoltageErrcodeReg
{
   uint8_t          	     all;
   struct VoltageErrcodeBits bit;
};

__packed struct SensorErrcodeBits
{
    uint8_t centrifuge_motor_preesure      :1;    /*bit 0  0x00 0x01 */
    uint8_t weeping_detector               :1;    /*bit 1  0x00 0x01 */
    uint8_t                                :6;    /*reserved  */

};

__packed union SensorErrcodeReg
{
   uint8_t          	     all;
   struct SensorErrcodeBits  bit;
};

__packed struct FanErrcodeBits
{
    uint8_t fan1               :1;    /*fan1 bit 0  0x00 0x01 */
    uint8_t fan2               :1;    /*fan2 bit 1  0x00 0x01 */
    uint8_t fan3               :1;    /*fan3 bit 2  0x00 0x01 */
    uint8_t fan4               :1;    /*fan4 bit 3  0x00 0x01 */
    uint8_t                    :4;    /*reserved  */
};

__packed union FanErrcodeReg
{
   uint8_t          	  all;
   struct FanErrcodeBits  bit;
};




typedef __packed struct
{
	volatile union MotorErrcode1Reg  motor_errcode1;	/*centrifuge_motor and cassette_motor errcode */
	volatile union VoltageErrcodeReg errcode_voltage;
	volatile union SensorErrcodeReg  errcode_sensor;
	volatile union FanErrcodeReg  errcode_fan;
	volatile uint8_t reserved5;
	volatile uint8_t reserved6;
	volatile uint8_t reserved7;
	volatile uint8_t reserved8;
}MotorErrCode;


/********************************************/
/********* para table interface      ********/
/*********        					 ********/
/********************************************/

typedef enum
{
	STOP ,    //0x00
	CLOCKWISE,    //0x01

	UNCLOCKWISE  //0x02
} motor_direction;
#define IS_MOTOR_DIRECTION(DIRECTION) ((DIRECTION) == STOP || (DIRECTION) == CLOCKWISE || (DIRECTION) == UNCLOCKWISE)




typedef enum
{
	NORMAL ,    //0x00
	ABNORMAL    //0x01
} sensor_status;

#define IS_SENSOR_STATUS(STATUS) ((STATUS) == NORMAL || (STATUS) == ABNORMAL)

typedef enum
{
	NOSIGNAL ,  //0x00
	SIGNALED    //0x01
} door_hall_status;

#define IS_DOOR_HALL_STATUS(STATUS) ((STATUS) == NOSIGNAL || (STATUS) == SIGNALED)

typedef enum
{
	LOCK ,    //0x00
	UNLOCK    //0x01
} door_electromagne_status;

#define IS_DOOR_ELECTROMAGNE_STATUS(STATUS) ((STATUS) == LOCK || (STATUS) == UNLOCK)


typedef enum
{
	UNKNOW ,  //0x00
	UP,       //0x01
	DOWN      //0x02
} cassette_location_status;

#define IS_CASSETTE_LACATION_STATUS(STATUS) ((STATUS) == UNKNOW || (STATUS) == UP || (STATUS) == DOWN )

typedef enum
{
	STATUS_70V ,   //0x00
    STATUS_24V ,   //0x01
    STATUS_S16 ,   //0x02
    STATUS_S19 ,   //0x03
    STATUS_P_12 ,  //0x04
    STATUS_N_12 ,  //0x05
    STATUS_5V      //0x06
} voltage_type;

#define IS_VOLTAGE_TYPE(TYPE) (     (TYPE) == STATUS_70V || \
                                    (TYPE) == STATUS_24V || \
                                    (TYPE) == STATUS_S16 || \
                                    (TYPE) == STATUS_S19 || \
                                    (TYPE) == STATUS_P_12 || \
                                    (TYPE) == STATUS_N_12 || \
                                    (TYPE) == STATUS_5V  \
                                    )

typedef enum
{
	FAN1 ,   //0x00
    FAN2 ,   //0x01
    FAN3 ,   //0x02
    FAN4
} fan_type;

#define IS_FAN_TYPE(TYPE) ( (TYPE) == FAN1 || \
                            (TYPE) == FAN2 || \
                            (TYPE) == FAN3 || \
                            (TYPE) == FAN4 \
                           )



void write_motor_direction(motor_direction dir);
void write_motor_speed(uint16_t speed);

void write_centrifuge_motor_preesure(u16 preesure);
void write_centrifuge_motor_state(sensor_status status);
void write_weeping_detector(sensor_status status);
void write_door_hall(door_hall_status status);
void write_cassette_location(cassette_location_status status);
void write_voltage_status(voltage_type voltage,sensor_status status);
void write_fan_state(fan_type fan,sensor_status status);


#endif

