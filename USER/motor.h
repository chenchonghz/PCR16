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
#define Motor_MicroSteps              4          // 电机细分数
#define Motor_StepsPerRound           200         // 电机每圈整步数
#define Motor_StepsPerRound_With_MicroSteps     (Motor_MicroSteps*Motor_StepsPerRound)//800
#define Motor_NumPerRound             12		  // 电机每圈 mm
#define Motor_NumPerStep              ((FP32)Motor_NumPerRound/(FP32)Motor_StepsPerRound_With_MicroSteps)//1步多少mm，12/800=0.015mm
#define Motor_StepsPerum              ((FP32)Motor_StepsPerRound_With_MicroSteps/(FP32)Motor_NumPerRound)//1um多少步，800/12=66.6
//#define Motor_InitSpeed                0x57e3

#define Motor_Move_MAX_LEN      80//97//mm
#define Motor_Move_MAX_STEP     Motor_Move_MAX_LEN*Motor_StepsPerum  //    
//#define Motor_Timer_PSC         1
//#define Motor_Timer_CLK         (rcc_clocks.PCLK2_Frequency/Motor_Timer_PSC)
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

#define MOTOR_TO_MAX         DEF_False        // To Max
#define MOTOR_TO_MIN         DEF_True       // To Min

#define MOTOR_ID1_EN_PORT	TMC260_EN_GPIO_Port
#define MOTOR_ID1_EN_PIN	TMC260_EN_Pin
#define MOTOR_ID1_DIR_PORT	TMC260_DIR_GPIO_Port
#define MOTOR_ID1_DIR_PIN	TMC260_DIR_Pin
#define MOTOR_ID1_STEP_PORT	TMC260_STEP_GPIO_Port
#define MOTOR_ID1_STEP_PIN	TMC260_STEP_Pin

#define	Motor_MaxLimit()				HAL_GPIO_ReadPin(LimitSwitchLeft_GPIO_Port, LimitSwitchLeft_Pin)
#define	Motor_MinLimit()				HAL_GPIO_ReadPin(LimitSwitchRgiht_GPIO_Port, LimitSwitchRgiht_Pin)

enum eMotorState {
    MotorState_Stop         = 0,    // Motor State:stop
    MotorState_Run          = 1,     // Motor State:run
    MotorState_Stuck        = 2,    // motor stuck
    MotorState_Stop_Reset_Failed  = 4,//找不到零点，复位失败
    MotorState_Stop_Unreachable  = 5,     // unreachable
    MotorState_Stop_MoveOutDownLimitFailed  = 6,     // z复位时，无法移出下限区域
    MotorState_Unkown = 0xff
};

enum eActionState {
    ActionState_Unknown     = 0,    // Action State:Unknown
    ActionState_Doing       = 1,    // Action State:Doing
    ActionState_OK          = 2,    // Action State:OK
    ActionState_Fail        = 3     // Action State:Fail
};

enum eMotorAbort {
	MotorAbort_Normal    = 0,       // Motor Abort:Normal
	MotorAbort_OverSteps = 1,       // Motor Abort:Over steps
	MotorAbort_Stuck   = 2,       // Motor Abort:door opened
	MotorAbort_LimitOpt  = 3,        // Motor Abort:touch limit opt
	MotorAbort_Min_LimitOpt  = 4,
	MotorAbort_Max_LimitOpt  = 5,
	MotorAbort_Reset_err,
	//MotorAbort_YReset_err,
	MotorAbort_Zero_err,
	//MotorAbort_YZero_err,
	MotorAbort_LostSteps,
};

enum eMotorAction {
    MotorAction_Unknown             = 0,    // Motor Action:Unknown
    MotorAction_Resetting           = 1,          // Motor Action:Reset doing
    MotorAction_ResetOK             = 2,          // Motor Action:Reset OK
    MotorAction_ResetFail           = 3,          // Motor Action:Reset Fail
	MotorAction_Moving              = 4, // Motor Action:Move doing
    MotorAction_MoveOK              = 5, // Motor Action:Move OK
    MotorAction_MoveFail            = 6, // Motor Action:Move Fail
};

typedef struct {
volatile    INT8U   is_run;
volatile    INT8U   action;
volatile    INT8U   abort_type;
} motor_state_t;

typedef struct _velprofile_t {
	const u16 *pVelBuf;
	u8 MaxIdx;
	u16 SaStep;//加速路程
	u16 Vo;
	u16 Vmax;
	u16 AccStep;
}velprofile_t;

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
//    INT8U               lock_current;
    INT32S		CurSteps;//当前编码器角度值

    INT32U              StepCnt;                    
    INT32U              MoveTotalSteps;      
	INT32S              SysAbsoluteOffsetStep;//系统绝对偏移量，以0点为基准, 0.1um为单位    
//    INT32S              SysAbsoluteOffset;
    INT32S              AccSteps;                    
    INT32U              DecSteps;
    INT32U              ConSteps;//匀速
//    INT32U              ConSteps2;//匀速
    tmc260_dev_t        *tmc260dev;
	velprofile_t *pCurve;
    void (*StepsCallback)(struct Motor_t *);    
    void (*OptCallback)(struct Motor_t *);      
} TMotor;
extern TMotor tMotor[MOTOR_ID_NUMS];

void Motor_Init(void);
u8 StartMotor(TMotor *pMotor, INT8U dir, INT32U steps,INT8U if_acc);
void StopMotor(TMotor *pMotor);
void MotorAccDec(TMotor *pMotor);
#endif

