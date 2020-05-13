#include "app_fluo.h"
#include "bsp_max5401.h"
#include "PD_DataProcess.h"
//堆栈
__align(4) OS_STK  TASK_FLUO_STK[STK_SIZE_FLUO]; //任务堆栈声?
__align(4) OS_STK  TASK_CalcFLUO_STK[STK_SIZE_CalcFLUO];
Fluo_t fluo;
static  message_pkt_t    msg_pkt_fluo;
static void AppFLUOTask (void *parg);
static void AppCalcFLUOTask (void *parg);

void AppFluoInit (void)
{
//	OSTaskCreate(AppFLUOTask,  (void * )0, (OS_STK *)&TASK_FLUO_STK[STK_SIZE_FLUO-1], TASK_PRIO_FLUO);
	OSTaskCreate(AppCalcFLUOTask,  (void * )0, (OS_STK *)&TASK_CalcFLUO_STK[STK_SIZE_CalcFLUO-1], TASK_PRIO_CalcFLUO);
}

static void FLUODatInit(void)
{
	fluo.Mbox                = OSMboxCreate((void *)0);
}
//准备荧光采集
void StartCollFluo(void)
{
//	u8 m,n;
//	
//	m = temp_data.CurStage;
//	n = temp_data.stage[m].CurStep;
//	if(temp_data.stage[m].step[n].CollEnable)	{//如果荧光采集使能 启动电机 采集荧光
//		
//	}
}

void StopCollFluo(void)
{
	
}

static void AppFLUOTask (void *parg)
{
	INT8U err;
    message_pkt_t *msg;
	
	FLUODatInit();
	PD_DataInit();
	
	for(;;)
    {
		msg = (message_pkt_t *)OSMboxPend(fluo.Mbox, 1000, &err);
		if(err==OS_ERR_TIMEOUT)	{
		}
	}
}

static void AppCalcFLUOTask (void *parg)
{
	INT8U err;
    message_pkt_t *msg;
	
	fluo.CalcMbox                = OSMboxCreate((void *)0);
	
	for(;;)
    {
		msg = (message_pkt_t *)OSMboxPend(fluo.CalcMbox, 0, &err);
		if(msg->Src == MSG_CalcPD_EVENT)	{
			CalcPDData(msg->Cmd);
		}
	}
}
