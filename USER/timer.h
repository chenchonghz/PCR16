#ifndef _TIMER_H
#define _TIMER_H

#include "includes.h"

void timer_update(TIM_HandleTypeDef *tmr,u32 val);
void timer_start(TIM_HandleTypeDef *tmr)  ;
void timer_stop(TIM_HandleTypeDef *tmr);
void UpdatePWMTimer(TIM_HandleTypeDef *tmr, INT16U val);
void StopPWM(TIM_HandleTypeDef *tmr);
void StartPWM(TIM_HandleTypeDef *tmr);
#endif
