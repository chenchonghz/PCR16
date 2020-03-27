#ifndef _TIMER_H
#define _TIMER_H

#include "includes.h"

void timer_update(TIM_HandleTypeDef *tmr,u32 val);
void timer_start(TIM_HandleTypeDef *tmr)  ;
void timer_stop(TIM_HandleTypeDef *tmr);
void UpdatePWM(TIM_HandleTypeDef *pPWM, u8 ch, INT16U val);
void StopPWM(TIM_HandleTypeDef *pPWM, u8 ch);
void StartPWM(TIM_HandleTypeDef *pPWM, u8 ch, u16 val);
#endif
