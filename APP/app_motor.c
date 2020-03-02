#include "app_motor.h"

//¶ÑÕ»
__align(4) OS_STK  TASK_MOTOR_STK[STK_SIZE_MOTOR]; //ÈÎÎñ¶ÑÕ»Éù?

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
//	tMotor[MOTOR_ID1].CurSteps = 0;
}

static void MotorReset(void)
{
//    u8 reset_cnt=0;
//    s32 movelen;

    tMotor[MOTOR_ID1].status.abort_type = MotorAbort_Normal;
    tMotor[MOTOR_ID1].status.action     = MotorAction_Resetting;
//	tMotor[MOTOR_ID1].Con1Len = Motor_Constant1_LEN;
	StartMotor(&tMotor[MOTOR_ID1], MOTOR_TO_MIN, MOTOR_LEN_MAX, DEF_False);
}

static void AppMotorTask (void *parg)
{
    INT8U err;
    message_pkt_t *msg;
	
	MotorDatInit();
	__HAL_TIM_ENABLE_IT(tMotor[MOTOR_ID1].tmr, TIM_IT_UPDATE);
	TMC260_install(tMotor[MOTOR_ID1].tmc260dev);
	MotorReset();
	tMotor[MOTOR_ID1].CurSteps = 0;
	
	for(;;)
    {
        msg = (message_pkt_t *)OSMboxPend(tMotor[MOTOR_ID1].Mbox, 0, &err);
		if(err==OS_ERR_NONE)    {
			
		}
	}

}
