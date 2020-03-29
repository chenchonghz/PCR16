#include "app_temp.h"
#include "ad7124.h"
#include "PID.h"

//堆栈
__align(4) OS_STK  TASK_TEMP_STK[STK_SIZE_TEMP]; //任务堆栈声?

_app_temp_t app_temp;
pid_ctrl_t TempPid[PIDCTRL_NUM];
#define	HOLE_TECPWM_PLUSE		400
#define	COVER_TECPWM_PLUSE		800
#define	HOLE_TECPWM_MAX		62//TEC pwm占空比最大值
#define	COVER_TECPWM_MAX		100//TEC pwm占空比最大值
static  message_pkt_t    msg_pkt_temp;
static void AppTempTask (void *parg);

void AppTempInit (void)
{
	OSTaskCreate(AppTempTask,  (void * )0, (OS_STK *)&TASK_TEMP_STK[STK_SIZE_TEMP-1], TASK_PRIO_TEMP);
}

static void TempDatInit(void)
{
	TempPid[HOLE_TEMP].PIDid = PID_ID1;
	TempPid[HOLE_TEMP].pTECPWM = &htim8;
	TempPid[HOLE_TEMP].TimCH = TIM_CHANNEL_1;
	TempPid[HOLE_TEMP].TimPluse = HOLE_TECPWM_PLUSE;
	TempPid[HOLE_TEMP].DutyMax = HOLE_TECPWM_MAX;
	TempPid[HOLE_TEMP].target_t = 3700;//0.01
	TempPid[HOLE_TEMP].PIDParam = 0.0;
	
	TempPid[COVER_TEMP].PIDid = PID_ID2;
	TempPid[COVER_TEMP].pTECPWM = &htim2;
	TempPid[COVER_TEMP].TimCH = TIM_CHANNEL_4;
	TempPid[COVER_TEMP].TimPluse = COVER_TECPWM_PLUSE;
	TempPid[COVER_TEMP].DutyMax = COVER_TECPWM_MAX;
	TempPid[COVER_TEMP].target_t = 0;	
	TempPid[COVER_TEMP].PIDParam = 0.0;
}
//TEC pwm控制
void StartTECPWM(pid_ctrl_t *pTempPid, u8 duty)
{
	static u8 dutybk;
	u16 temp;
	
	if(dutybk==duty)
		return;
	temp = (pTempPid->TimPluse/100)*duty;
	if(duty==100)	{
		temp++;
	}
	StartPWM(pTempPid->pTECPWM, pTempPid->TimCH, temp);
	dutybk = duty;
}

//停止温度控制
static u32 StopTempCtrl(pid_ctrl_t *pTempPid)
{
	StopPWM(pTempPid->pTECPWM, pTempPid->TimCH);
	pTempPid->PIDParam = 0.0;
}
u16 setval;
//pid调节半导体片温度 采样增量法计算 pwm占空比不能超过50%
static void TempCtrl(pid_ctrl_t *pTempPid, u16 cur_t)
{
	s16 dat;
	float temp;
//	u16 setval;
	
	temp = pTempPid->PIDParam;
	temp += PID_control(pTempPid->PIDid, dat, cur_t);
	pTempPid->PIDParam = temp;
	dat = (s16)floatToInt(temp);
	if(dat<0)	{//当前温度高于目标温度 将TEC切换到制冷模式 快速降温
		if(dat < -pTempPid->DutyMax)
			setval = pTempPid->DutyMax;
		else
			setval = -dat;
		TEC_DIR_COLD();
	}
	else {//当前温度低于目标温度 将TEC切换到制热模式 快速升温
		if(dat > pTempPid->DutyMax)
			setval = pTempPid->DutyMax;
		else
			setval = dat;
		TEC_DIR_HOT();
	}
	StartTECPWM(pTempPid, setval);
//	SYS_PRINTF("D:%d,T:%d ",dat,cur_t);
}
//
void StartTempCtrl(u16 target)
{
	TempPid[HOLE_TEMP].target_t = target;
	ClearPIDDiff(TempPid[HOLE_TEMP].PIDid);
	Sys.devstate = DevState_Running;
}

#define	TEMPCTRL_ACCURACY		10//温控精度0.1
#define	TEMPCOLLECT_ACCURACY		5//温度采集精度 0.05
static void AppTempTask (void *parg)
{
	s32 cur_temp;
	
	TempDatInit();
	PIDParamInit();
	StopTempCtrl(&TempPid[HOLE_TEMP]);
	OSTimeDly(1000);
//	COOLFAN_ON();
//	SetPIDVal(PID_ID1, 1, 0, 0);
	for(;;)
    {
		if(Sys.devstate == DevState_Running)	
		{
			if(CalcTemperature(GetADCVol(TEMP_ID1), &cur_temp)==0)	{
				app_temp.current_t[TEMP_ID1] = cur_temp;//0.01
				{
					SetPIDTarget(TempPid[HOLE_TEMP].PIDid, TempPid[HOLE_TEMP].target_t);//设置控制目标
//					diff = abs(TempPid[HOLE_TEMP].target_t - cur_temp);//获取PID当前误差
//					if(diff > 200)	{//根据温度差 设置pid参数
//						SetPIDVal(PID_ID1, 1, 0, 0);
//					}
//					else 
//						SetPIDVal(PID_ID1, 1, 0, 0);
//					if(diff > TEMPCOLLECT_ACCURACY)	
					{//大于0.1度 
//						TempCtrl(&TempPid[HOLE_TEMP], cur_temp);//pid调节 增量法计算
					}
//					else	{//小<=0.1度 不调节 TEC停止工作
//						ClearPIDDiff(TempPid[HOLE_TEMP].PIDid);
//						StopTempCtrl(&TempPid[HOLE_TEMP]);			
//					}
				}
			}else	{//温度传感器脱落
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID2), (s32 *)&cur_temp)==0)	{
				app_temp.current_t[TEMP_ID2] = cur_temp;				
			}else	{
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID3), (s32 *)&cur_temp)==0)	{
				app_temp.current_t[TEMP_ID3] = cur_temp;
				SetPIDTarget(TempPid[COVER_TEMP].PIDid, TempPid[COVER_TEMP].target_t);//设置控制目标
				TempCtrl(&TempPid[COVER_TEMP], cur_temp);//热盖pid调节 增量法计算
			}else	{
			
			}
//			if(CalcTemperature(GetADCVol(TEMP_ID4), (s32 *)&cur_temp)==0)	{//散热器 预留
//				app_temp.current_t[TEMP_ID4] = cur_temp;				
//			}else	{
//			
//			}
		}
		else	{
			ClearPIDDiff(TempPid[HOLE_TEMP].PIDid);
			StopTempCtrl(&TempPid[HOLE_TEMP]);
			ClearPIDDiff(TempPid[COVER_TEMP].PIDid);
			StopTempCtrl(&TempPid[COVER_TEMP]);
		}
		OSTimeDly(80);
	}
}
