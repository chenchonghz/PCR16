#include "motor.h"

struct _motor_port{
	struct _io_map en;
	struct _io_map dir;
	struct _io_map step;
};

typedef struct _motor_priv{
	struct _motor_port *pport;
}motor_priv_t;

/*
*********************************************************************************************************
*                                       PRIVATE FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void     UpdateMotorTimer        (MOTOR_ID id, INT16U val);
static void     CalcSpedingProcedure    (MOTOR_ID id,INT8U if_acc);
static void UpdateMotorTimer(MOTOR_ID id, INT16U val);
static void StartMotorPWM(MOTOR_ID id);
/*
*********************************************************************************************************
*                                  PRIVATE GLOBAL CONSTANTS & VARIABLES
*********************************************************************************************************
*/

static struct _motor_port g_motor_port[MOTOR_ID_NUMS]={
	[MOTOR_ID1] = {
		.en ={MOTOR_ID1_EN_PORT ,MOTOR_ID1_EN_PIN},
		.dir={MOTOR_ID1_DIR_PORT,MOTOR_ID1_DIR_PIN},
		.step={MOTOR_ID1_STEP_PORT,MOTOR_ID1_STEP_PIN}
	},
/*	[MOTOR_ID2] = {
		.en ={MOTOR_ID2_EN_PORT ,MOTOR_ID2_EN_PIN},
		.dir={MOTOR_ID2_DIR_PORT,MOTOR_ID2_DIR_PIN},
		.step={MOTOR_ID2_STEP_PORT,MOTOR_ID2_STEP_PIN}
	},*/
};

static motor_priv_t g_motor_priv[MOTOR_ID_NUMS]={
	[MOTOR_ID1] = {
		.pport = &g_motor_port[MOTOR_ID1]
	},
/*	[MOTOR_ID2] = {
		.pport = &g_motor_port[MOTOR_ID2]
	},*/
};
TMotor tMotor[MOTOR_ID_NUMS];

void Motor_init(void)
{
	TMC260_install(tMotor[MOTOR_ID1].tmc260dev);
	tMotor[MOTOR_ID1].tmr = &htim3;
}


void enable_motor(TMotor *pdev)
{
  struct _io_map const *m_en = &g_motor_port[pdev->id].en;
  
  SET_L(m_en);
}

void disable_motor(TMotor *pdev)
{
  struct _io_map const *m_en = &g_motor_port[pdev->id].en;
  
  SET_H(m_en);
}

void StopMotor(MOTOR_ID id)
{
   	__HAL_TIM_DISABLE_IT(tMotor[id].tmr, TIM_IT_CC3);
	__HAL_TIM_DISABLE_IT(tMotor[id].tmr,TIM_IT_UPDATE);
    tMotor[id].status.is_run        = MotorState_Stop;
}

u8 StartMotor(TMotor *pMotor, INT8U dir, INT32U steps,INT8U if_acc)
{
    INT8U err;
    INT32U len;

   if(pMotor->status.is_run == MotorState_Run)
	   return 0;
    struct _io_map const *m_dir = &g_motor_port[pMotor->id].dir;
//    pMotor->StepCnt       = 0;
	if (steps)
    {
        pMotor->Dir = dir;
        if(pMotor->Dir) {//前进
//            if(QueryZ1Opt())//在上限开关位置
//                goto _end;
            SET_H(m_dir);
        }
        else    {//后退
//            if(QueryZ2Opt())//在下限开关位置
//                goto _end;
            SET_L(m_dir);
        }
        enable_motor(pMotor);
        pMotor->MoveTotalSteps    = steps;
		pMotor->status.is_run = MotorState_Run;
//        CalcSpedingProcedure(pMotor->id,if_acc);
        pMotor->TableIndex    = 0;
//        pMotor->StepsCallback = &MotorSpeedUpDn;
        StartMotorPWM(pMotor->id);
	}
}

static void UpdateMotorTimer(MOTOR_ID id, INT16U val)
{
	__HAL_TIM_SET_COUNTER(tMotor[id].tmr, 0);
    __HAL_TIM_SET_AUTORELOAD(tMotor[id].tmr, val);
    __HAL_TIM_SET_COMPARE(tMotor[id].tmr, TIM_CHANNEL_3, val/2);
}

static void StartMotorPWM(MOTOR_ID id)
{
//    UpdateMotorTimer(id,(Motor_Timer_CLK/tMotor[id].pCurve->freq_min)-1);    
	__HAL_TIM_ENABLE_IT(tMotor[id].tmr, TIM_IT_CC3);
	__HAL_TIM_CLEAR_FLAG(tMotor[id].tmr, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(tMotor[id].tmr,TIM_IT_UPDATE);
}
