#include "app_temp.h"
#include "ad7124.h"
#include "PID.h"

//堆栈
__align(4) OS_STK  TASK_TEMP_STK[STK_SIZE_TEMP]; //任务堆栈声?

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
	app_temp.target_t[HOLE_TEMP] = 900;//0.1
	app_temp.target_t[COVER_TEMP] = 0;
	app_temp.duty[HOLE_TEMP] = 0;
	app_temp.duty[COVER_TEMP] = 0;
	app_temp.pTECPWM = &htim8;
}

#define	TECPWM_CH		TIM_CHANNEL_1
#define	TECPWM_PLUSE		400
static void StopTECPWM(void)
{
	HAL_TIM_PWM_Stop(app_temp.pTECPWM, TECPWM_CH);
}
//修改TEC pwm占空比
static void UpdateTECPWM(INT16U duty)
{
	u16 temp;
	
	temp = (TECPWM_PLUSE/100)*duty;
    __HAL_TIM_SET_AUTORELOAD(app_temp.pTECPWM, TECPWM_PLUSE);
    __HAL_TIM_SET_COMPARE(app_temp.pTECPWM, TECPWM_CH, temp);
}

static void StartTECPWM(u16 duty)
{
	static u16 dutybk;
	
	if(dutybk==duty)
		return;
	if(duty==100)	{
		duty++;
	}
	__HAL_TIM_CLEAR_FLAG(app_temp.pTECPWM, TIM_FLAG_UPDATE);
	UpdateTECPWM(duty);
	HAL_TIM_PWM_Start(app_temp.pTECPWM, TECPWM_CH);	
	dutybk = duty;
}
//pid调节半导体片温度 采样增量法计算 pwm占空比不能超过50%
static u16 TempCtrl(u8 id, s16 *pwmduty, u16 cur_t)
{
	s16 dat;
	u16 setval;
	
	dat = *pwmduty;
	dat += (s16)PID_control(id, dat, cur_t);
	if(dat<0)	{//当前温度高于目标温度 将TEC切换到制冷模式 快速降温
		TEC_DIR_COLD();
		setval = -dat;
	}
	else {//当前温度低于目标温度 将TEC切换到制热模式 快速升温
		TEC_DIR_HOT();
		setval = dat;
	}
	if(setval>TECPWMDUTY_MAX)	{
		setval = TECPWMDUTY_MAX;
	}
	StartTECPWM(setval);
	*pwmduty = dat;
}

static void AppTempTask (void *parg)
{
	s32 cur_temp;
	u16 diff;
	
	TempDatInit();
	PIDParamInit();
	TEC_OFF();
	
	for(;;)
    {
//		if(Sys.devstate == DevState_Running)	
		{
			if(CalcTemperature(GetADCVol(TEMP_ID1), &cur_temp)==0)	{
				app_temp.current_t[TEMP_ID1] = cur_temp;//0.1
				SetPIDTarget(PID_ID1, app_temp.target_t[HOLE_TEMP]);//设置控制目标
				diff = abs(app_temp.current_t[TEMP_ID1] - app_temp.target_t[HOLE_TEMP]);
//				if(diff > 200)	{//根据温度差 设置pid参数
//					SetPIDVal(PID_ID1, 1, 0, 0);
//				}
//				else 
//					SetPIDVal(PID_ID1, 1, 0, 0);
				if(diff > 3)	{//大于0.3度 
					TempCtrl(PID_ID1, &app_temp.duty[HOLE_TEMP], cur_temp);//pid调节 增量法计算
				}else	{//小于0.3度 不调节 TEC停止工作
					StopTECPWM();
				}
			}else	{//温度传感器脱落
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID2), (s32 *)&cur_temp)==0)	{
				app_temp.current_t[TEMP_ID2] = cur_temp;
			}else	{
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID3), (s32 *)&cur_temp)==0)	{
				app_temp.current_t[TEMP_ID3] = cur_temp;
			}else	{
			
			}
			if(CalcTemperature(GetADCVol(TEMP_ID4), (s32 *)&cur_temp)==0)	{
				app_temp.current_t[TEMP_ID4] = cur_temp;
			}else	{
			
			}
		}
		OSTimeDly(80);
	}
}
