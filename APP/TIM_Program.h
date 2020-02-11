#ifndef __TIM_PROGRAM_H__
#define __TIM_PROGRAM_H__

#include "stm32l4xx_hal.h"

typedef struct _SYS_TIM
{
    volatile uint32_t  SumNumber;       //累加计数
    volatile uint32_t  SumMs;          //累加ms数
    volatile uint32_t  SumSec;         //累加秒数
    volatile uint32_t  SumMinute;      //累加分钟数
}SYS_TIM;
extern SYS_TIM  SysTim;
//extern struct SYS_TIM  deviceFlowTim;
//extern struct SYS_TIM  deviceSendDataTim;
extern volatile uint16_t	_5MS_Event;
extern volatile uint16_t	_10MS_Event;
extern volatile uint16_t	_100MS_Event;
extern volatile uint16_t	_1S_Event;
extern volatile uint16_t	_2S_Event;

void TIMDataInit(void);
void TIM_Event(void);

#endif
