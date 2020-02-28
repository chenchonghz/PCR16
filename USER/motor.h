#ifndef _MOTOR_H
#define _MOTOR_H

#include "includes.h"
#include "bsp_tmc260.h"

/*
*********************************************************************************************************
*                                            GLOBAL DATA TYPES
*********************************************************************************************************
*/
#define Motor_DriverRatio             1           // 电机传动比
#define Motor_MicroSteps              32          // 电机细分数
#define Motor_StepsPerRound           200         // 电机每圈整步数
#define Motor_StepsPerRound_With_MicroSteps     (Motor_MicroSteps*Motor_StepsPerRound)//6400
#define Motor_NumPerRound             1000//250		  // 电机每圈 0.1um*7000
#define Motor_NumPerStep              ((FP32)Motor_NumPerRound/(FP32)Motor_StepsPerRound_With_MicroSteps)//1步多少um，1000/6400=0.15625
#define Motor_StepsPerum              ((FP32)Motor_StepsPerRound_With_MicroSteps/(FP32)Motor_NumPerRound)//1um多少步，6400/1000=6.4
//#define Motor_InitSpeed                0x57e3

#define Motor_Move_MAX_LEN      19000//13mm
#define Motor_Move_MAX_STEP     Motor_Move_MAX_LEN*Motor_StepsPerum  //    
#define Motor_Timer_PSC         1
#define Motor_Timer_CLK         (rcc_clocks.PCLK2_Frequency/Motor_Timer_PSC)
//#define DEF_FREQ_MIN            2000

#define Motor_Constant1_Steps          0//启动阶段匀速路程
#define Motor_Constant2_Steps          600//最后阶段匀速路程；1600//小于1600步，即小于半圈，匀速运行（半圈1600步）

typedef enum {
    MOTOR_ID1      = 0,
//    MOTOR_ID2      = 1
} MOTOR_ID;
#define MOTOR_ID_MIN 			MOTOR_ID1
#define MOTOR_ID_MAX 			MOTOR_ID1
#define MOTOR_ID_NUMS       	(MOTOR_ID_MAX-MOTOR_ID_MIN+1)
#define MOTOR_FLAGS_INIT		1

#define MOTOR_TO_MAX         DEF_True        // To Max
#define MOTOR_TO_MIN         DEF_False       // To Min

#define MOTOR_ID1_EN_PORT	TMC260_EN_GPIO_Port
#define MOTOR_ID1_EN_PIN	TMC260_EN_Pin
#define MOTOR_ID1_DIR_PORT	TMC260_DIR_GPIO_Port
#define MOTOR_ID1_DIR_PIN	TMC260_DIR_Pin
#define MOTOR_ID1_STEP_PORT	TMC260_STEP_GPIO_Port
#define MOTOR_ID1_STEP_PIN	TMC260_STEP_Pin

enum eMotorState {
    MotorState_Stop         = 0,    // Motor State:stop
    MotorState_Run          = 1,     // Motor State:run
    MotorState_Stuck        = 2,    // motor stuck
    MotorState_Stop_Reset_Failed  = 4,//找不到零点，复位失败
    MotorState_Stop_Unreachable  = 5,     // unreachable
    MotorState_Stop_MoveOutDownLimitFailed  = 6,     // z复位时，无法移出下限区域
    MotorState_Unkown = 0xff
};

typedef struct {
    INT8U   is_run;
    INT8U   action;
    INT8U   abort_type;
} motor_state_t;

typedef struct Motor_t {
     MOTOR_ID     	id;
	    OS_EVENT            *Sem;
    OS_EVENT            *Mbox;
    TIM_HandleTypeDef   *tmr;                        
    motor_state_t       status;       
    INT8U               Dir;                        
    INT16S              TableIndex;                 
    INT8U               max_idx;
    INT8U		err_num;					
 
    INT8U		CurrentScale;	//电流
    INT16U		micro_step;		//细分	
    INT16U		speed;			//速度
    INT16U		cur_speed;
    INT16U		acceleration;	//加速度
    INT8U               lock_current;
    INT32S		CurSteps;//当前编码器角度值

    INT32U              StepCnt;                    
    INT32U              MoveTotalSteps;          
    INT32S              SysAbsoluteOffset;
    INT32U              UpSteps;                    
    INT32U              DnSteps;
    INT32U              ConSteps1;//匀速
    INT32U              ConSteps2;//匀速
    tmc260_dev_t        *tmc260dev;
	
    void (*StepsCallback)(struct Motor_t *);    
    void (*OptCallback)(struct Motor_t *);      
} TMotor;
extern TMotor tMotor[MOTOR_ID_NUMS];

void Motor_init(void);
u8 StartMotor(TMotor *pMotor, INT8U dir, INT32U steps,INT8U if_acc);

#endif

