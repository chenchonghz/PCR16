#include "app_ad.h"

//¶ÑÕ»
__align(4) OS_STK  TASK_AD_STK[STK_SIZE_AD]; //ÈÎÎñ¶ÑÕ»Éù?

static  message_pkt_t    msg_pkt_motor;
static void AppADTask (void *parg);
_app_ad_t app_ad;

void AppADInit (void)
{
	OSTaskCreate(AppADTask,  (void * )0, (OS_STK *)&TASK_AD_STK[STK_SIZE_AD-1], TASK_PRIO_AD);
}

static void ADDatInit(void)
{
	app_ad.sem                 = OSSemCreate(0);
	app_ad.rflag = DEF_True;
	app_ad.wait_t = 2;
}

void StartAppADTask(void)
{
	app_ad.wait_t = 2;
}

void StopAppADTask(void)
{
	app_ad.wait_t = 2;
}
//u16 timecnt=0;
//u32 advol;
//extern float ad_vol;
static void AppADTask (void *parg)
{
	u8 err;
	ADDatInit();
	AD7124Init();
	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE(&htim6);
	__HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	app_ad.rflag = DEF_False;
	
	for(;;)
    {
		OSSemPend(app_ad.sem, 0, &err);
		if(err==OS_ERR_NONE)	{
			app_ad.rflag = DEF_True;			
			StartADDataCollect();
			app_ad.rflag = DEF_False;			
		}
//		StartADDataCollect();
//		OSTimeDly(app_ad.wait_t);
//		timecnt++;
//		if(timecnt>300)	{
//			timecnt = 0;
//			advol = (u32)(ad_vol*100);
//			SYS_PRINTF("vol:%d.%d",advol/100,advol%100);
//		}
	}
}
