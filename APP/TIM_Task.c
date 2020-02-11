#include "TIM_Task.h"
//#include "tim.h"

//-------------------------------------------------------------------------------------------------
SYS_TIM  SysTim;
//-------------------------------------------------------------------------------------------------
volatile uint16_t	_5MS_Event = 0;
volatile uint16_t	_10MS_Event = 0;
volatile uint16_t	_100MS_Event = 0;
volatile uint16_t	_1S_Event = 0;
volatile uint16_t	_2S_Event = 0;
//====================================================================================================

void TIMDataInit(void)
{
	SysTim.SumMs = 0;
	SysTim.SumSec = 0;
	SysTim.SumMinute = 0;
	SysTim.SumNumber = 0;
}

void TimeEventCallback(void)
{
//    if(htim == &htim2)
    {
		SysTim.SumMs++;
		if(SysTim.SumMs>=999)	{//1s
			SysTim.SumMs = 0;
			SysTim.SumSec++;
			_1S_Event = 1;//1s事件
		}
		if((SysTim.SumMs%5) == 0)	{//5ms事件
			_5MS_Event = 1;
		}
		if((SysTim.SumMs%10) == 0)	{//10ms事件
			_10MS_Event = 1;
		}
		if((SysTim.SumMs%100) == 0)	{//10ms事件
			_100MS_Event = 1;
		}
    }
}
//===========================================END===================================================

