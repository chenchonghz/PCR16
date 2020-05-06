#include "app_motor.h"

//堆栈
__align(4) OS_STK  TASK_MOTOR_STK[STK_SIZE_MOTOR]; //任务堆栈声?
static  message_pkt_t    msg_pkt_motor;
static void AppMotorTask (void *parg);

void AppMotorInit (void)
{
	OSTaskCreate(AppMotorTask,  (void * )0, (OS_STK *)&TASK_MOTOR_STK[STK_SIZE_MOTOR-1], TASK_PRIO_MOTOR);
}

static void MotorDatInit (void)
{
	tMotor[MOTOR_ID1].id                  = MOTOR_ID1;
    //tMotor[X_MOTOR_ID].PWMBASE             = PWM0_BASE;
    tMotor[MOTOR_ID1].Sem                 = OSSemCreate(0);
    tMotor[MOTOR_ID1].Mbox                = OSMboxCreate((void *)0);
    tMotor[MOTOR_ID1].status.is_run       = MotorState_Stop;
    tMotor[MOTOR_ID1].status.action       = MotorAction_Resetting;
    tMotor[MOTOR_ID1].status.abort_type   = MotorAbort_Normal;
    tMotor[MOTOR_ID1].Dir                 = MOTOR_TO_MIN;
	tMotor[MOTOR_ID1].tmr = &htim3;
	tMotor[MOTOR_ID1].tmc260dev = TMC260_get_dev((TMC260_ID)MOTOR_ID1);
//	tMotor[MOTOR_ID1].StepsCallback = &MotorArrivedCheck;
	tMotor[MOTOR_ID1].ConSteps = 10;//匀速步数
}

static void MotorReset(void)
{
    tMotor[MOTOR_ID1].status.action     = MotorAction_Resetting;
	if(!Motor_MinLimit())	{//已经在下限位 前进
		StartMotor(&tMotor[MOTOR_ID1], MOTOR_TO_MAX, 10*Motor_StepsPerum, DEF_False);//前进10mm
		OSTimeDly(1000);
	} 
	tMotor[MOTOR_ID1].status.abort_type = MotorAbort_Normal;
//	CalcAnyPosAtResetSteps(&tMotor[MOTOR_ID1], Motor_Move_MAX_STEP);
	StartMotor(&tMotor[MOTOR_ID1], MOTOR_TO_MIN, Motor_Move_MAX_STEP, DEF_False);
	if(tMotor[MOTOR_ID1].status.abort_type == MotorAbort_Min_LimitOpt)	{//复位过程碰到零点 成功
		tMotor[MOTOR_ID1].status.action     = MotorAction_ResetOK;
	}
	else 	{
		tMotor[MOTOR_ID1].status.action     = MotorAction_ResetFail;
	}
	OSTimeDly(1000);
}

u8 tmc260_status=0;
static void AppMotorTask (void *parg)
{
    INT8U err;
    message_pkt_t *msg;
	
	MotorDatInit();
	Motor_Init();
	__HAL_TIM_ENABLE_IT(tMotor[MOTOR_ID1].tmr, TIM_IT_UPDATE);
	TMC260_install(tMotor[MOTOR_ID1].tmc260dev);
	tmc260_status = TMC260_read_status(tMotor[MOTOR_ID1].tmc260dev);
	MotorReset();
	tMotor[MOTOR_ID1].CurSteps = 0;
	
	for(;;)
    {
		StartMotor(&tMotor[MOTOR_ID1], MOTOR_TO_MAX, Motor_Move_MAX_STEP, DEF_True);
		OSTimeDly(1000);
		StartMotor(&tMotor[MOTOR_ID1], MOTOR_TO_MIN, Motor_Move_MAX_STEP, DEF_True);
		OSTimeDly(1000);
//        msg = (message_pkt_t *)OSMboxPend(tMotor[MOTOR_ID1].Mbox, 0, &err);
//		if(err==OS_ERR_NONE)    {
//			
//		}
	}

}
