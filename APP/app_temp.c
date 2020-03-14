#include "app_temp.h"
#include "ad7124.h"
//¶ÑÕ»
__align(4) OS_STK  TASK_TEMP_STK[STK_SIZE_TEMP]; //ÈÎÎñ¶ÑÕ»Éù?

_app_temp_t app_temp;

static  message_pkt_t    msg_pkt_temp;
static void AppTempTask (void *parg);

void AppTempInit (void)
{
	OSTaskCreate(AppTempTask,  (void * )0, (OS_STK *)&TASK_TEMP_STK[STK_SIZE_TEMP-1], TASK_PRIO_TEMP);
}

static void TempDatInit(void)
{
//	app_temp.pTemp = &tempctrl;
//	app_temp.target_t = 0;
}

static void AppTempTask (void *parg)
{
	TempDatInit();
	
	for(;;)
    {
//		if(Sys.devstate == DevState_Running)	
		{
			CalcTemperature(GetADCVol(TEMP_ID1), (s32 *)&app_temp.current_t[TEMP_ID1]);
		}
		OSTimeDly(100);
	}
}