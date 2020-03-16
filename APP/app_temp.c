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
	s16 temp;
	TempDatInit();
	
	for(;;)
    {
//		if(Sys.devstate == DevState_Running)	
		{
			if(CalcTemperature(GetADCVol(TEMP_ID1), (s32 *)&temp)==0)	{
				app_temp.current_t[TEMP_ID1] = temp;
			}else	{//ÎÂ¶È´«¸ÐÆ÷ÍÑÂä
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID2), (s32 *)&temp)==0)	{
				app_temp.current_t[TEMP_ID2] = temp;
			}else	{
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID3), (s32 *)&temp)==0)	{
				app_temp.current_t[TEMP_ID3] = temp;
			}else	{
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID4), (s32 *)&temp)==0)	{
				app_temp.current_t[TEMP_ID4] = temp;
			}else	{
			
			}
		}
		OSTimeDly(100);
	}
}