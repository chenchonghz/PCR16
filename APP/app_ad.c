#include "app_ad.h"
#include "ad7124.h"

//¶ÑÕ»
__align(4) OS_STK  TASK_AD_STK[STK_SIZE_AD]; //ÈÎÎñ¶ÑÕ»Éù?

static  message_pkt_t    msg_pkt_motor;
static void AppADTask (void *parg);

void AppADInit (void)
{
	OSTaskCreate(AppADTask,  (void * )0, (OS_STK *)&TASK_AD_STK[STK_SIZE_AD-1], TASK_PRIO_AD);
}

static void ADDatInit(void)
{
	
}
u16 timecnt=0;
u32 advol;
extern float ad_vol;
static void AppADTask (void *parg)
{
	ADDatInit();
	AD7124Init();
	
	for(;;)
    {
		StartADDataCollect();
		OSTimeDly(3);
		timecnt++;
		if(timecnt>300)	{
			timecnt = 0;
			advol = (u32)(ad_vol*100);
			SYS_PRINTF("vol:%d.%d",advol/100,advol%100);
		}
	}
}
