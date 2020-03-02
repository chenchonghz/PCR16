#include "motor.h"

struct _motor_port{
	struct _io_map en;
	struct _io_map dir;
	struct _io_map step;
};

typedef struct _motor_priv{
	struct _motor_port *pport;
}motor_priv_t;

#define	Motor_LeftLimit()				HAL_GPIO_ReadPin(LimitSwitchLeft_GPIO_Port, LimitSwitchLeft_Pin)
#define	Motor_RigtLimit()				HAL_GPIO_ReadPin(LimitSwitchRgiht_GPIO_Port, LimitSwitchRgiht_Pin)
/*
*********************************************************************************************************
*                                       PRIVATE FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void     UpdateMotorTimer        (MOTOR_ID id, INT16U val);
static void     CalcSpedingProcedure    (MOTOR_ID id,INT8U if_acc);
static void UpdateMotorTimer(MOTOR_ID id, INT16U val);
static void StartMotorPWM(MOTOR_ID id);
static void StopMotorPWM(MOTOR_ID id);
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

void StopMotor(TMotor *pMotor)
{
	StopMotorPWM(pMotor->id);
	if(pMotor->status.is_run==MotorState_Run)	{
		OSSemPost(pMotor->Sem);
		pMotor->status.is_run        = MotorState_Stop;
	}
}
//位置到达判断
static void MotorArrivedCheck(TMotor *pMotor)
{
	if(pMotor->CurSteps >= pMotor->MoveTotalSteps)	{
		StopMotor(pMotor);
	}
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
            if(Motor_LeftLimit())//在上限开关位置
                goto _end;
            SET_H(m_dir);
        }
        else    {//后退
            if(Motor_LeftLimit())//在下限开关位置
                goto _end;
            SET_L(m_dir);
        }
        enable_motor(pMotor);
        pMotor->MoveTotalSteps    = steps;
		pMotor->status.is_run = MotorState_Run;
//        CalcSpedingProcedure(pMotor->id,if_acc);
        pMotor->TableIndex    = 0;
        pMotor->StepsCallback = &MotorArrivedCheck;
        StartMotorPWM(pMotor->id);
		OSSemPend(pMotor->Sem, 0, &err);//等待事件触发
	}
_end:
	
	return 1;
}

static void StopMotorPWM(MOTOR_ID id)
{
	HAL_TIM_PWM_Stop(tMotor[id].tmr, TIM_CHANNEL_3);
    tMotor[id].status.is_run        = MotorState_Stop;
}

static void UpdateMotorPWM(MOTOR_ID id, INT16U val)
{
	__HAL_TIM_SET_COUNTER(tMotor[id].tmr, 0);
    __HAL_TIM_SET_AUTORELOAD(tMotor[id].tmr, val);
    __HAL_TIM_SET_COMPARE(tMotor[id].tmr, TIM_CHANNEL_3, val/2);
}

static void StartMotorPWM(MOTOR_ID id)
{
	__HAL_TIM_CLEAR_FLAG(tMotor[id].tmr, TIM_FLAG_UPDATE);
	HAL_TIM_PWM_Start(tMotor[id].tmr, TIM_CHANNEL_3);	
}
