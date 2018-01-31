/******************** (C) COPYRIGHT 2013 VINY **********************************
 * File Name          : struct_control_board.h
 * Author             : ZhangQ
 * Date First Issued  : 2013/10/16
 * Description        : This file contains the define of code struct(TPDO/RPDO)
 *                      in ARM0.
 *******************************************************************************
 * History:
 * DATE       | VER   | AUTOR      | Description
 * 2013/10/16 | v1.0  | ZhangQ     | initial released
 *******************************************************************************/
#ifndef _STRUCT_CONTROL_BOARD_H_
#define _STRUCT_CONTROL_BOARD_H_

/********************************************/
/***************ARM0 TPDO Table**************/
/********************************************/
/******** Common control cmd ****/
typedef __packed struct ARM_ANSWER_CMD_STRU
{
    uint8_t fn;             /* frame number [1~7,0] */
    uint8_t answer;         /* ACK/NAK */
    uint8_t status;         /* status [not use] */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
} ARM_ANSWER_CMD;
/*******************************************/

/************ ARM0 -> ARM1 *****************/
// ARM1 pump control cmd
__packed struct ARM1_PUMP_CTRLCMD_STRU
{
    uint8_t reserved:2;       /* 0:1  reserved */
    uint8_t feedBack_pump:2;  /* 2:3  feedBack_pump cmd: 0x0,0x1,0x2,0x3*/
    uint8_t AC_pump:2;        /* 4:5  AC_pump cmd: 0x0,0x1,0x2,0x3*/
    uint8_t draw_pump:2;      /* 6:7  draw_pump cmd: 0x0,0x1,0x2,0x3*/
};

__packed union ARM1_PUMP_CTRLCMD_UNION
{
    uint8_t                       all;
    struct ARM1_PUMP_CTRLCMD_STRU bit;
};

// ARM1 distance record control cmd
__packed struct ARM1_DISTANCE_CTRLCMD_STRU
{
    uint8_t :2;                 /* 0:1  reserved */
    uint8_t feedBack_pump:1;    /* 2    feedBack_pump 0x0:continue record distance,0x1:restart record distance */
    uint8_t :2;                 /* 3:4  reserved */
    uint8_t AC_pump:1;          /* 5    AC_pump 0x0:continue record distance,0x1:restart record distance */
    uint8_t :1;                 /* 6    reserved */
    uint8_t draw_pump:1;        /* 7    draw_pump 0x0:continue record distance,0x1:restart record distance */
};

__packed union ARM1_DISTANCE_CTRLCMD_UNION
{
    uint8_t                           all;
    struct ARM1_DISTANCE_CTRLCMD_STRU bit;
};

__packed struct ARM1_SINGLE_ALL_DISTANCE_STRU
{
    uint8_t :2;                 /* 0:1: reserved */
    /* 2:3: feedback pump, 0x01:feedback all distance,0x00:feedback single distance,0x11,clear all distance */
    uint8_t feedback_pump:2;
    /* 4:5: AC pump, 0x01:feedback all distance,0x00:feedback single distance,0x11,clear all distance */
    uint8_t AC_pump:2;
    /* 6:7: draw_pump, 0x01:feedback all distance,0x00:feedback single distance,0x11,clear all distance */
    uint8_t draw_pump:2;
};

__packed union ARM1_SINGLE_ALL_DISTANCE_UNION
{
    uint8_t                              all;
    struct ARM1_SINGLE_ALL_DISTANCE_STRU bit;
};

// ARM1 TPOD3 ( ARM1 order PDO)
typedef __packed struct ARM1_TPDO3_STRU
{
    uint8_t fn;
    uint8_t module_indicate;                        /* module indicate: 0x00 0x01 0xff */
    union ARM1_PUMP_CTRLCMD_UNION order_indicate;   /* pump cmd */
    union ARM1_DISTANCE_CTRLCMD_UNION param1;		/* param1 */
    uint8_t param2;                                 /* order_indicate:0x00-init, 0x01-distacne */
    union ARM1_SINGLE_ALL_DISTANCE_UNION param3;    /* indicate the pumps */
    uint8_t param4;
    uint8_t param5;
} ARM1_TPDO3;

// Set ARM1 pump direction
__packed struct ARM1_PUMP_DIR_STRU
{
    uint8_t :2;             /* 0:1 reserved */
    uint8_t feedBack_pump:1;/* 2   feedBack_pump dir:0x0,0x1*/
    uint8_t :1;             /* 3   reserved */
    uint8_t AC_pump:1;      /* 4   AC_pump dir:0x0,0x1*/
    uint8_t :1;             /* 5   reserved */
    uint8_t draw_pump:1;    /* 6   draw_pump dir:0x0,0x1*/
    uint8_t :1;             /* 7   reserved */
};

__packed union ARM1_PUMP_DIR_UNION
{
   uint8_t         		     all;
   struct ARM1_PUMP_DIR_STRU bit;
};

// Set ARM1 pump revolve count switch
__packed struct ARM1_COUNTSWTICH_STRU
{
    uint8_t reserved:2;     /* 0:1 reserved */
    uint8_t feedBack_pump:2;/* 2:3 feedBack_pump Count on/off 0x0,0x1*/
    uint8_t AC_pump:2;      /* 4:5 AC_pump on/off 0x0,0x1*/
    uint8_t draw_pump:2;    /* 6:7 draw_pump on/off 0x0,0x1*/
};

__packed union ARM1_COUNTSWTICH_UNION
{
    uint8_t          	         all;
    struct ARM1_COUNTSWTICH_STRU bit;
};

// ARM1 TPDO1 (Set ARM1 pump speed limit & count switch & dir)
typedef __packed struct ARM1_TPDO1_STRU
{
    uint16_t draw_pump_speed_limit;
    uint16_t AC_pump_speed_limit;   
    uint16_t feed_back_pump_speed_limit;    
    union ARM1_PUMP_DIR_UNION pump_direction;   /* pump dir */
    union ARM1_COUNTSWTICH_UNION pump_count;    /* pump count switch */
} ARM1_TPDO1;

// ARM1 TPDO2 (Set ARM1 pump distance)
typedef __packed struct ARM1_TPDO2_STRU
{
    uint16_t draw_pump_need_distance;
    uint16_t AC_pump_need_distance;
    uint16_t feed_back_pump_need_distance;
    uint8_t reserved1;
    uint8_t reserved2;
} ARM1_TPDO2;
/*******************************************/

/************ ARM0 -> ARM2 *****************/
// ARM2 pump/valve control cmd
__packed struct ARM2_PUMP_VALVE_CTRLCMD_STRU
{
    uint8_t reserved:2;             /* 0:1  reserved */
    uint8_t RBC_valve:2;            /* 2:3  RBC valve cmd: 0x0,0x1,0x2,0x3*/
    uint8_t plasma_valve_or_pump:2; /* 4:5  plasma pump or valve cmd: 0x0,0x1,0x2,0x3*/
    uint8_t PLT_valve_or_pump:2;    /* 6:7  PLT pump or valve cmd: 0x0,0x1,0x2,0x3*/
};

__packed union ARM2_PUMP_VALVE_CTRLCMD_UNION
{
    uint8_t                             all;
    struct ARM2_PUMP_VALVE_CTRLCMD_STRU bit;
};

// ARM2 distance record control cmd
__packed struct ARM2_DISTANCE_CTRLCMD_STRU
{
    uint8_t :5;             /* 0:4 reserved */
    uint8_t plasma_pump:1;  /* 5   plasma_pump 0x0:continue record distance,0x1:restart record distance */
    uint8_t :1;             /* 6   reserved */
    uint8_t PLT_pump:1;     /* 7   PLT_pump 0x0:continue record distance,0x1:restart record distance */
};

__packed union ARM2_DISTANCE_CTRLCMD_UNION
{
    uint8_t                           all;
    struct ARM2_DISTANCE_CTRLCMD_STRU bit;
};

__packed struct ARM2_SINGLE_ALL_DISTANCE_STRU
{
    uint8_t :4;                 /* 0:3 reserved */
    /* 4:5: pla pump, 0x00:feedback all distance, 0x01:feedback single distance,0x11:clear all distance */
    uint8_t pla_pump:2;
    /* 6:7: plt pump, 0x00:feedback all distance,0x01:feedback single distance,0x11:clear all distance */
    uint8_t plt_pump:2;
};

__packed union ARM2_SINGLE_ALL_DISTANCE_UNION
{
    uint8_t                              all;
    struct ARM2_SINGLE_ALL_DISTANCE_STRU bit;
};

// ARM2 TPOD3 ( ARM2 order PDO)
typedef __packed struct ARM2_TPDO3_STRU
{
    uint8_t fn;
    uint8_t module_indicate;                            /* module indicate: 0x00 0x01 0xff */
    union ARM2_PUMP_VALVE_CTRLCMD_UNION order_indicate; /* pump or valve cmd */
    union ARM2_DISTANCE_CTRLCMD_UNION param1;           /* param1 */
    uint8_t param2;                                     /* order_indicate:0x00-init, 0x01-distacne */
    union ARM2_SINGLE_ALL_DISTANCE_UNION param3;        /* indicate the pumps */
    uint8_t param4;
    uint8_t param5;
} ARM2_TPDO3;

// Set ARM2 pump direction
__packed struct ARM2_PUMP_DIR_STRU
{
    uint8_t  :4;            /* 0:3 reserved */
    uint8_t plasma_pump:1;  /* 4   plasma_pump dir:0x0,0x1*/
    uint8_t  :1;            /* 5   reserved */
    uint8_t PLT_pump:1;     /* 6   PLT_pump dir:0x0,0x1*/
    uint8_t  :1;            /* 7   reserved */
};

__packed union ARM2_PUMP_DIR_UNION
{
   uint8_t         		     all;
   struct ARM2_PUMP_DIR_STRU bit;
};

// Set ARM2 pump revolve count switch
__packed struct ARM2_COUNTSWTICH_STRU
{
    uint8_t reserved:4;     /* 0:3 reserved */
    uint8_t plasma_pump:2;  /* 4:5 plasma_pump count switch on/off 0x0,0x1 */
    uint8_t PLT_pump:2;     /* 6:7 PLT_pump count switch on/off 0x0,0x1 */
};

__packed union ARM2_COUNTSWTICH_UNION
{
   uint8_t          	        all;
   struct ARM2_COUNTSWTICH_STRU bit;
};

// ARM2 TPDO1 (Set ARM2 pump speed limit & count switch & dir)
typedef __packed struct ARM2_TPDO1_STRU
{
    uint16_t PLT_pump_speed_limit;
    uint16_t plasma_pump_speed_limit;
    uint16_t reserved;
    union ARM2_PUMP_DIR_UNION pump_direction;   /* pump dir */
    union ARM2_COUNTSWTICH_UNION Pump_count;    /* revolve count */
} ARM2_TPDO1;

// ARM2 TPDO2 (Set ARM2 pump distance)
typedef __packed struct ARM2_TPDO2_STRU
{
    uint16_t PLT_pump_need_distance;
    uint16_t plasma_pump_need_distance;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
} ARM2_TPDO2;
/*******************************************/

/************ ARM0 -> ARM3 *****************/
// ARM3 pump control cmd
__packed struct ARM3_MOTOR_CTRLCMD_STRU
{
    uint8_t reserved:4;                     /* 0:3  reserved */
    uint8_t cassette_motor:2;               /* 4:5  cassette motor cmd: 0x0,0x1,0x2,0x3*/
    uint8_t centrifuge_or_electromagnet:2;  /* 6:7  centrifuge or electromagnet cmd: 0x0,0x1,0x2,0x3*/
};

__packed union ARM3_MOTOR_CTRLCMD_UNION
{
    uint8_t                        all;
    struct ARM3_MOTOR_CTRLCMD_STRU bit;
};

// ARM3 distance record control cmd
__packed struct ARM3_DISTANCE_CTRLCMD_STRU
{
    uint8_t :5;                 /* 0:4 reserved */
    uint8_t cassette_motor:1;   /* 5   cassette_motor 0x0:continue record distance,0x1:restart record distance */
    uint8_t :1;                 /* 6   reserved */
    uint8_t centrifuge_motor:1; /* 7   centrifuge_motor 0x0:continue record distance,0x1:restart record distance */
};

__packed union ARM3_DISTANCE_CTRLCMD_UNION
{
    uint8_t                           all;
    struct ARM3_DISTANCE_CTRLCMD_STRU bit;
};

// ARM3 TPOD3 ( ARM3 order PDO)
typedef __packed struct ARM3_TPDO3_STRU
{
    uint8_t fn;
    uint8_t module_indicate;                       /* module indicate: 0x00 0x01 0xff */
    union ARM3_MOTOR_CTRLCMD_UNION order_indicate; /* motor or door cmd */
    union ARM3_DISTANCE_CTRLCMD_UNION param1;      /* param1 */
    uint8_t param2;
    uint8_t param3;
    uint8_t param4;
    uint8_t param5;
} ARM3_TPDO3;

// Set ARM3 motor direction
__packed struct ARM3_MOTOR_DIR_STRU
{
    uint8_t  :4;                /* 0:3 reserved */
    uint8_t cassette_motor:1;   /* 4   cassette dir:0x0,0x1 */
    uint8_t  :1;                /* 5   reserved */
    uint8_t centrifuge_motor:1; /* 6   centrifuge dir:0x0,0x1 */
    uint8_t  :1;                /* 7   reserved */
};

__packed union ARM3_MOTOR_DIR_UNION
{
   uint8_t                    all;
   struct ARM3_MOTOR_DIR_STRU bit;
};

// Set ARM3 Motor revolve count switch
__packed struct ARM3_COUNTSWITCH_STRU
{
    uint8_t :4;                 /* 0:3  reserved */
    uint8_t cassette_motor:2;   /* 4:5  cassette count switch on/off:0x0,0x1 */
    uint8_t centrifuge_motor:2; /* 6:7  centrifuge count swtich on/off:0x0,0x1 */
};

__packed union ARM3_COUNTSWITCH_UNION
{
    uint8_t                      all;
    struct ARM3_COUNTSWITCH_STRU bit;
};

// ARM3 TPDO1 (Set ARM3 motor speed limit & count switch & dir)
typedef __packed struct ARM3_TPDO1_STRU
{
    uint16_t centrifuge_motor_speed_limit;
    uint16_t cassette_motor_speed_limit;
    uint16_t reserved;
    union ARM3_MOTOR_DIR_UNION motor_direction;
    union ARM3_COUNTSWITCH_UNION motor_count;
} ARM3_TPDO1;

// ARM3 TPDO2 (Set ARM3 motor distance)
typedef __packed struct ARM3_TPDO2_STRU
{
    uint16_t centrifuge_motor_need_distance;
    uint16_t cassette_motor_need_distance;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
} ARM3_TPDO2;

/*******************************************/
/*******************************************/

/********************************************/
/***************ARM0 RPDO Table**************/
/********************************************/

/*****************Common struct**************/
__packed struct PDO_ERROR_SOURCE_STRU
{
    uint8_t boardNo:4;        /* 0:3 0001-ARM1,0010-ARM2,0011:ARM3 */
    uint8_t taskNo:4;         /* 4:7 error task prio */
};

__packed union PDO_ERROR_SOURCE_UNION
{
    uint8_t                      all;
    struct PDO_ERROR_SOURCE_STRU bit;
};

typedef __packed struct ARM_RPDO_CMD_STRU
{
    uint8_t fn;
    uint8_t module;
    uint8_t order;
    uint8_t param1;
    uint8_t param2;
    uint8_t param3;
    uint8_t param4;
    uint8_t param5;
} ARM_RPDO_CMD;
/********************************************/

/*****************ARM1 -> ARM0***************/
// ARM1 pump current direction
__packed struct ARM1_PUMP_CURDIR_STRU
{
    uint8_t :2;                 /* 0:1 reserved */
    uint8_t feedBack_pump:2;    /* 2:3 feedBack_pump current direction:0x0,0x1,0x2 */
    uint8_t AC_pump:2;          /* 4:5 AC_pump current direction:0x0,0x1,0x2 */
    uint8_t draw_pump:2;        /* 6:7 draw_pump current direction:0x0,0x1,0x2 */
};

__packed union ARM1_PUMP_CURDIR_UNION
{
    uint8_t                      all;
    struct ARM1_PUMP_CURDIR_STRU bit;
};

// ARM1 RPDO1 (Get ARM1 Pump speed)
typedef __packed struct ARM1_RPDO1_STRU
{
    uint16_t draw_pump_current_speed;
    uint16_t AC_pump_current_speed;
    uint16_t feed_back_pump_current_speed;
    union ARM1_PUMP_CURDIR_UNION pump_current_dir;
    uint8_t fn;
} ARM1_RPDO1;


// ARM1 RPDO2 (Get ARM1 Pump moved distance)
typedef __packed struct ARM1_RPDO2_STRU
{
    uint16_t draw_pump_moved_distance;
    uint16_t AC_pump_moved_distance;
    uint16_t feed_back_pump_moved_distance;
    union ARM1_SINGLE_ALL_DISTANCE_UNION distance_type;
    uint8_t fn;
} ARM1_RPDO2;

// ARM1 liquid_level result
__packed struct ARM1_LIQUID_LEVEL_STRU
{
    uint8_t :5;             /* 0:4  reserved */
    uint8_t low_level:1;    /* 5 low_level result 0x0,0x1*/
    uint8_t	high_level:1;   /* 6 high_level result 0x0,0x1*/
    uint8_t AC_bubble:1;    /* 7 AC_bubble result 0x0,0x1*/
};

__packed union ARM1_LIQUID_LEVEL_UNION
{
    uint8_t                       all;
    struct ARM1_LIQUID_LEVEL_STRU bit;
};

// ARM1 pump init position result
__packed struct ARM1_PUMP_INIT_STRU
{
    uint8_t :5;                     /* 0:4 reserved */
    uint8_t feedBack_pump_init:1;   /* 5   feedBack_pump_hoare ret:0x0,0x1 */
    uint8_t	AC_pump_init:1;         /* 6   AC_pump hoare ret:0x0,0x1 */
    uint8_t draw_pump_init:1;       /* 7   draw_pump hoare ret:0x0,0x1 */
};

__packed union ARM1_PUMP_INIT_UNION
{
    uint8_t                    all;
    struct ARM1_PUMP_INIT_STRU bit;
};

// ARM1 pump init position result
__packed struct ARM1_BLOOD_PRESSURE_STRU
{
    uint16_t pressure:13;
    uint16_t sign:1;
    uint16_t reserved:2;
};

__packed union ARM1_BLOOD_PRESSURE_UNION
{
    uint16_t                        all;
    struct ARM1_BLOOD_PRESSURE_STRU bit;
};


// ARM1 RPDO3 (Get ARM1 collect pressure & feedback pressure & liquid level & hoare status)
typedef __packed struct ARM1_RPDO3_STRU
{
	union ARM1_BLOOD_PRESSURE_UNION blood_pressure;
	uint16_t reserved;
	union ARM1_LIQUID_LEVEL_UNION liquid_level;
	union ARM1_PUMP_INIT_UNION pump_init;
	uint8_t feedback_pump;          /* feedback pump status,0x00:Normal,0x01:Abnormal */
	uint8_t reserved2;
} ARM1_RPDO3;

// ARM1 pump(draw_pump & AC_pump) error code
__packed struct ARM1_PUMP_ERRCODE1_STRU
{
    uint8_t draw_pump:4;    	/* 0:3 draw_pump errcode  0x0,0x1,0x2£¬0x3£¬0x4 */
    uint8_t AC_pump  :4;        /* 4:7 AC_pump errcode  0x0,0x1,0x2£¬0x3£¬0x4 */
};

__packed union ARM1_PUMP_ERRCODE1_UNION
{
    uint8_t          	  all;
    struct ARM1_PUMP_ERRCODE1_STRU bit;
};

// ARM1 pump(feedBack_pump) error code
__packed struct ARM1_PUMP_ERRCODE2_STRU
{
    uint8_t feedBack_pump:4;    /* 0:3 feedBack_pump errcode  0x0,0x1,0x2£¬0x3£¬0x4*/
    uint8_t reserved :4;        /* 4:7  reserved*/
};

__packed union ARM1_PUMP_ERRCODE2_UNION
{
    uint8_t                        all;
    struct ARM1_PUMP_ERRCODE2_STRU bit;
};

__packed struct ARM1_SENSOR_ERROR_STRU
{
    uint8_t press_sensor:1;     /* press sensor is ok? 0-OK,1-Error */
    uint8_t bubble_sensor:1;    /* bubble sensor is ok? 0-OK,1-Error */
    uint8_t high_level:1;       /* high level sensor is ok? 0-OK,1-Error */
    uint8_t low_level:1;        /* low level sensor is ok? 0-OK,1-Error */
    uint8_t reserved:4;         /* reserved */
};

__packed union ARM1_SENSOR_ERROR_UNION
{
    uint8_t                       all;
    struct ARM1_SENSOR_ERROR_STRU bit;
};

// ARM1 RPDO6 (Get ARM1 pump error code)
typedef __packed struct ARM1_RPDO6_STRU
{
    union ARM1_PUMP_ERRCODE1_UNION pump_errcode1;	 /* draw_pump and AC_pump errcode */
    union ARM1_PUMP_ERRCODE2_UNION pump_errcode2;    /* feedback_pump errcode */
    union ARM1_SENSOR_ERROR_UNION sensor_error;      /* pdo error_source */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
} ARM1_RPDO6;

/********************************************/

/*****************ARM2 -> ARM0***************/
// ARM2 pump current direction
__packed struct ARM2_PUMP_CURDIR_STRU
{
    uint8_t :4;             /* 0:3 reserved */
    uint8_t plasma_pump:2;  /* 4:5 plasma_pump cur dir:0x0,0x1,0x2*/
    uint8_t PLT_pump:2;     /* 6:7 PLT_pump cur dir:0x0,0x1,0x2*/
};

__packed union ARM2_PUMP_CURDIR_UNION
{
    uint8_t                      all;
    struct ARM2_PUMP_CURDIR_STRU bit;
};

// ARM2 RPDO1 (Get ARM2 pump current speed & current direction)
typedef __packed struct ARM2_RPDO1_STRU
{
    uint16_t PLT_pump_current_speed;
    uint16_t plasma_pump_current_speed;
    uint16_t reserved1;
    union ARM2_PUMP_CURDIR_UNION pump_current_dir;
    uint8_t fn;
} ARM2_RPDO1;

// ARM2 RPDO2 (Get ARM2 pump moved distance)
typedef __packed struct ARM2_RPDO2_STRU
{
    uint16_t PLT_pump_moved_distance;
    uint16_t plasma_pump_moved_distance;
    union ARM2_SINGLE_ALL_DISTANCE_UNION distance_type;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t fn;
} ARM2_RPDO2;

// ARM2 valve current direction
__packed struct ARM2_VALVE_STATUS_STRU
{
    uint8_t RBC_valve_left:1;
    uint8_t RBC_valve_mid:1;
    uint8_t RBC_valve_right:1;
    uint8_t plsama_valve_left:1;
    uint8_t plsama_valve_mid:1;
    uint8_t plsama_valve_right:1;
    uint8_t PLT_valve_left:1;
    uint8_t PLT_valve_mid:1;
    uint8_t PLT_valve_right:1;
    uint8_t :7;               /* 9~17  reserved */
};

__packed union ARM2_VALVE_STATUS_UNION
{
    uint16_t                      all;
    struct ARM2_VALVE_STATUS_STRU bit;
};

// ARM2 pump hoare sensor status
__packed struct ARM2_PUMP_INIT_STRU
{
	uint8_t :6;    					/* 0:5 reserved */
	uint8_t	PLT_pump_init:1;
	uint8_t plasma_pump_init:1;
};

__packed union ARM2_PUMP_INIT_UNION
{
    uint8_t                    all;
    struct ARM2_PUMP_INIT_STRU bit;
};

// ARM2 RPDO3 (Get ARM2 sensor status)
typedef __packed struct ARM2_RPDO3_STRU
{
    uint16_t RBC_detector;
    union ARM2_VALVE_STATUS_UNION valve;
    union ARM2_PUMP_INIT_UNION pump_init;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
} ARM2_RPDO3;

// ARM2 pump(plasma pump & PLT pump) error code
__packed struct ARM2_PUMP_ERRCODE_STRU
{
    uint8_t plasma_pump:4;    	/* 0:3 plasma_pump errcode  0x0,0x1,0x2£¬0x3£¬0x4*/
    uint8_t PLT_pump   :4;      /* 4:7    PLT_pump errcode  0x0,0x1,0x2£¬0x3£¬0x4*/
};

__packed union ARM2_PUMP_ERRCODE_UNION
{
    uint8_t          	          all;
    struct ARM2_PUMP_ERRCODE_STRU bit;
};

// ARM2 valve(RBC valve & plasma valve) error code
__packed struct ARM2_VALVE_ERRCODE_STRU
{
    uint8_t RBC_valve     :2;   /* 0:1      RBC_valve errcode */
    uint8_t plasma_valve  :2;   /* 2:3      plasma_valve errcode */
    uint8_t PLT_valve     :2;   /* 4:5      PLT_valve errcode */
    uint8_t reserved      :2;   /* 6:7      reserved */
};

__packed union ARM2_VALVE_ERRCODE_UNION
{
    uint8_t          	    all;
    struct ARM2_VALVE_ERRCODE_STRU bit;
};

__packed struct ARM2_SENSOR_ERROR_STRU
{
    uint8_t rbg_sensor:1;     /* rbg sensor is ok? 0-OK,1-Error */
    uint8_t reserved:7;       /* reserved */
};

__packed union ARM2_SENSOR_ERROR_UNION
{
    uint8_t                       all;
    struct ARM2_SENSOR_ERROR_STRU bit;
};

// ARM2 RPDO6 (Get ARM2 pump & valve error code)
typedef __packed struct ARM2_RPDO6_STRU
{
    union ARM2_PUMP_ERRCODE_UNION  pump_errcode;   /*plasma_pump and PLT_pump errcode */
    union ARM2_VALVE_ERRCODE_UNION valve_errcode;  /*RBC_valve plasma_valve and plt_valve errcode */
    union ARM2_SENSOR_ERROR_UNION sensor_error;    /* sensor error */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
} ARM2_RPDO6;
/********************************************/


/*****************ARM3 -> ARM0***************/
// ARM3 motor current direction
__packed struct ARM3_MOTOR_CURDIR_STRU
{
    uint8_t :4;                 /* 0:3 reserved */
    uint8_t cassette_motor:2;   /* 4:5 cassette current dir:0x0,0x1,0x2 */
    uint8_t centrifuge_motor:2; /* 6:7 cassette current dir:0x0,0x1,0x2 */
};

__packed union ARM3_MOTOR_CURDIR_UNION
{
    uint8_t                       all;
    struct ARM3_MOTOR_CURDIR_STRU bit;
};

// ARM3 RPDO1 (Get ARM3 motor current speed & current direction
typedef __packed struct ARM3_RPDO1_STRU
{
    uint16_t centrifuge_motor_current_speed;
    uint16_t cassette_motor_current_speed;
    uint16_t reserved1;
    union ARM3_MOTOR_CURDIR_UNION motor_current_dir;
    uint8_t reserved2;
} ARM3_RPDO1;

// ARM3 RPDO2 (Get ARM3 motor moved distance)
typedef __packed struct ARM3_RPDO2_STRU
{
    uint16_t centrifuge_motor_moved_distance;
    uint16_t cassette_motor_moved_distance;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
} ARM3_RPDO2;

// ARM3 sensor status
__packed struct ARM3_SENSOR_STRU
{
    uint8_t reserved:3;                     /* 0:2 reserved */
    uint8_t	cassette_up_location:1;         /* 3 0x1,0x0 */
    uint8_t cassette_down_location:1;       /* 4 0x1,0x0 */
    uint8_t	door_hoare:1;
    uint8_t	door_switch2:1;
    uint8_t door_switch1:1;
};

__packed union ARM3_SENSOR_UNION
{
    uint8_t                 all;
    struct ARM3_SENSOR_STRU bit;
};

__packed struct ARM3_CENTRI_PRESSURE_STRU
{
    uint16_t pressure:13;
    uint16_t sign:1;
    uint16_t reserved:2;
};

__packed union ARM3_CENTRI_PRESSURE_UNION
{
    uint16_t                         all;
    struct ARM3_CENTRI_PRESSURE_STRU bit;
};


// ARM3 RPDO3 (Get ARM3 sensor status)
typedef __packed struct ARM3_RPDO3_STRU
{
	uint16_t weeping_detector;
	union ARM3_CENTRI_PRESSURE_UNION centrifuge_motor_pressure;
	union ARM3_SENSOR_UNION sensor;
	uint8_t centrifuge;             /* centrifuge status,0x0:Normal,0x1:Abnormal */
	uint8_t reserved2;
	uint8_t reserved3;
} ARM3_RPDO3;

// ARM3 motor error code
__packed struct ARM3_MOTOR_ERRCODE_STRU
{
    uint8_t centrifuge_motor :4;    /* 0:3 centrifuge_motor errcode  0x0,0x1,0x2£¬0x3£¬0x4*/
    uint8_t cassette_motor   :4;    /* 4:7   cassette_motor errcode  0x0,0x1,0x2£¬0x3£¬0x4 */
};

__packed union ARM3_MOTOR_ERRCODE_UNION
{
    uint8_t                        all;
    struct ARM3_MOTOR_ERRCODE_STRU bit;
};

__packed struct ARM3_SENSOR_ERROR_STRU
{
    uint8_t press_sensor:1;     /* centrifuge press sensor sensor is ok? 0-OK,1-Error */
    uint8_t weeping_sensor:1;   /* weeping sensor is ok? 0-OK,1-Error */
    uint8_t reserved:6;         /* reserved */
};

__packed union ARM3_SENSOR_ERROR_UNION
{
    uint8_t                       all;
    struct ARM3_SENSOR_ERROR_STRU bit;
};

__packed struct ARM3_VOLTAGE_ERROR_STRU
{
    uint8_t voltage_70V:1;
    uint8_t voltage_24v:1;
    uint8_t voltage_s16:1;
    uint8_t voltage_s19:1;
    uint8_t voltage_positive_12v:1;
    uint8_t volage_negative_12V:1;
    uint8_t volage_5V:1;
    uint8_t reserved:1;
};

__packed union ARM3_VOLTAGE_ERROR_UNION
{
    uint8_t                        all;
    struct ARM3_VOLTAGE_ERROR_STRU bit;
};


// ARM3 RPDO6 (Get ARM3 motor Error code)
typedef __packed struct ARM3_RPDO6_STRU
{
    union ARM3_MOTOR_ERRCODE_UNION motor_errcode1;	/*centrifuge_motor and cassette_motor errcode */
    union ARM3_VOLTAGE_ERROR_UNION voltage_error;   /* voltage error */
    union ARM3_SENSOR_ERROR_UNION sensor_error;     /* sensor error */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
} ARM3_RPDO6;
/********************************************/


#endif

