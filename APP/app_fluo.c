#include "app_fluo.h"
#include "bsp_max5401.h"

//堆栈
__align(4) OS_STK  TASK_FLUO_STK[STK_SIZE_FLUO]; //任务堆栈声?
Fluo_t fluo;
static  message_pkt_t    msg_pkt_fluo;
static void AppFLUOTask (void *parg);

void AppFluoInit (void)
{
	OSTaskCreate(AppFLUOTask,  (void * )0, (OS_STK *)&TASK_FLUO_STK[STK_SIZE_FLUO-1], TASK_PRIO_FLUO);
}

static void FLUODatInit(void)
{
	fluo.Mbox                = OSMboxCreate((void *)0);
}
//开启荧光采集
void StartCollFluo(void)
{
	u8 m,n;
	
	m = temp_data.CurStage;
	n = temp_data.stage[m].CurStep;
	if(temp_data.stage[m].step[n].CollEnable)	{//如果荧光采集使能 启动电机 采集荧光
		
	}
}

u8 fluoflag=0xff;
static void AppFLUOTask (void *parg)
{
	INT8U err;
    message_pkt_t *msg;
	
	FLUODatInit();
//	Led_FluoGreen_On();
//	Led_Fluo_On();
//	MAX5401WriteResistor();
	
	for(;;)
    {
		msg = (message_pkt_t *)OSMboxPend(fluo.Mbox, 1000, &err);
		if(err==OS_ERR_TIMEOUT)	{
			if(fluoflag==0)	{
				Led_Fluo_Off();
				Led_FluoGreen_Off();
				Led_FluoBlue_Off();
			}
			else if(fluoflag==1)	{
				Led_Fluo_Off();
				OSTimeDly(1000);
				Led_FluoGreen_Off();
				Led_FluoBlue_On();
				Led_Fluo_On();
			}else if(fluoflag==2)	{
				Led_Fluo_Off();
				OSTimeDly(1000);
				Led_FluoBlue_Off();
				Led_FluoGreen_On();
				Led_Fluo_On();
			}
		}
	}
}
