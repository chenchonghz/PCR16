#include "timer.h"
#include "motor.h"

void timer_update(TIM_HandleTypeDef *tmr,u32 val)
{
  __HAL_TIM_SET_COUNTER(tmr,0); //设置计数值 
  __HAL_TIM_SET_AUTORELOAD(tmr,val);
}

void timer_start(TIM_HandleTypeDef *tmr)  
{
  __HAL_TIM_SET_COUNTER(tmr,0); //设置计数值 
  HAL_TIM_Base_Start(tmr);
//  TIM_ClearITPendingBit(tmr, TIM_IT_Update);
	__HAL_TIM_CLEAR_IT(tmr, TIM_IT_UPDATE);
}

void timer_stop(TIM_HandleTypeDef *tmr)
{
  HAL_TIM_Base_Stop(tmr);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == tMotor[MOTOR_ID1].tmr)	{
	    __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);  //清除TIMx的中断待处理位:TIM 中断源 
		if(tMotor[MOTOR_ID1].status.is_run == MotorState_Run)      {
		  if(tMotor[MOTOR_ID1].Dir == MOTOR_TO_MIN)
			tMotor[MOTOR_ID1].CurSteps--;
		  else
			tMotor[MOTOR_ID1].CurSteps++;
//		  tMotor[MOTOR_ID1].StepCnt++;          

//		  if ((void *)tMotor[MOTOR_ID1].StepsCallback != (void *)0) {
//			(*tMotor[MOTOR_ID1].StepsCallback)(&tMotor[MOTOR_ID1]);
//		  }
		}
	}
}
