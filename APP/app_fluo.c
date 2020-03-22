#include "app_fluo.h"
//#include "ad7124.h"

//¶ÑÕ»
__align(4) OS_STK  TASK_FLUO_STK[STK_SIZE_FLUO]; //ÈÎÎñ¶ÑÕ»Éù?
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
u8 fluoflag=0xff;
static void AppFLUOTask (void *parg)
{
	INT8U err;
    message_pkt_t *msg;
	
	FLUODatInit();
//	Led_FluoGreen_On();
//	Led_Fluo_On();
	
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
