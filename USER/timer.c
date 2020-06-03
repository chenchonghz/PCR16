#include "timer.h"

_softtimer_t SoftTimer1,SoftTimer2;

void timer_update(TIM_HandleTypeDef *tmr,u32 val)
{
  __HAL_TIM_SET_COUNTER(tmr,0); //设置计数值 
  __HAL_TIM_SET_AUTORELOAD(tmr,val);
}

void timer_start(TIM_HandleTypeDef *tmr)  
{
  __HAL_TIM_SET_COUNTER(tmr,0); //设置计数值 
  HAL_TIM_Base_Start(tmr);
	__HAL_TIM_ENABLE_IT(tmr, TIM_IT_UPDATE);
//  TIM_ClearITPendingBit(tmr, TIM_IT_Update);
	__HAL_TIM_CLEAR_IT(tmr, TIM_IT_UPDATE);
}

void timer_stop(TIM_HandleTypeDef *tmr)
{
  HAL_TIM_Base_Stop(tmr);
}
//软件定时器 以100ms为周期的硬定时器7为基础
void SoftTimerInit(void)
{
	SoftTimer1.cnt = 0;
	SoftTimer1.period = 0;
	SoftTimer1.state = DEF_Stop;
	SoftTimer1.TIM = &htim7;
}

u8 GetSoftTimerState(_softtimer_t *psofttimer)
{
	return psofttimer->state;
}
//restart_flag 是否重新初始化硬定时器
u8 SoftTimerStart(_softtimer_t *psofttimer, u32 value, u8 restart_flag)
{
	if(GetSoftTimerState(psofttimer) == DEF_Stop)	{
		psofttimer->cnt = 0;
		psofttimer->period = value;
		psofttimer->state = DEF_Run;
		if(restart_flag)
			timer_start(psofttimer->TIM);
		return 1;
	}
	return 0;
}
//restart_flag 是否停止硬定时器
void SoftTimerStop(_softtimer_t *psofttimer, u8 stop_flag)
{
	psofttimer->cnt = 0;
	psofttimer->state = DEF_Stop;
	if(stop_flag)
		timer_stop(psofttimer->TIM);
}

void SoftTimerCallback(void)
{
	if(SoftTimer1.state == DEF_Run)	{//恒温保持时间控制
		SoftTimer1.cnt ++;
		if(SoftTimer1.cnt>=SoftTimer1.period)	{
			SoftTimer1.cnt = 0;
			if(SoftTimer1.pCallBack != NULL)
				(*SoftTimer1.pCallBack)();
		}
	}
	if(SoftTimer2.state == DEF_Run)	{//恒温保持时间控制
		SoftTimer2.cnt ++;
		if(SoftTimer2.cnt>=SoftTimer2.period)	{
			SoftTimer2.cnt = 0;
			if(SoftTimer2.pCallBack != NULL)
				(*SoftTimer2.pCallBack)();
		}
	}
}







///////////////////////////////////////////PWM控制/////////////////////////////////////////////////
void StopPWM(TIM_HandleTypeDef *pPWM, u8 ch)
{
	HAL_TIM_PWM_Stop(pPWM, ch);
}
//修改TEC pwm占空比
void UpdatePWM(TIM_HandleTypeDef *pPWM, u8 ch, INT16U val)
{
//	u16 temp;
	
//	temp = (HOLE_TECPWM_PLUSE/100)*duty;
//    __HAL_TIM_SET_AUTORELOAD(pPWM, HOLE_TECPWM_PLUSE);
    __HAL_TIM_SET_COMPARE(pPWM, ch, val);
}

void StartPWM(TIM_HandleTypeDef *pPWM, u8 ch, u16 val)
{
	__HAL_TIM_CLEAR_FLAG(pPWM, TIM_FLAG_UPDATE);
	UpdatePWM(pPWM, ch, val);
	HAL_TIM_PWM_Start(pPWM, ch);	
}
