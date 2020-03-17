#ifndef __APP_TEMP_H__
#define __APP_TEMP_H__

#include "includes.h"
#include "TempCalc.h"

#define	HOLE_TEMP		0
#define	COVER_TEMP		1

#define	PIDCTRL_NUM		2//需要pid控制数量
#define	TECPWMDUTY_MAX		50//TEC pwm占空比最大值

typedef struct _app_temp	{
	OS_EVENT           *MSG_Q;
	OS_EVENT           *lock;
	TIM_HandleTypeDef *pTECPWM;
	s16 target_t[PIDCTRL_NUM];//目标温度 0.1
	s16 current_t[TEMP_ID_NUMS];//当前温度 0.1
	s16 duty[PIDCTRL_NUM];//tec pwm占空比
}_app_temp_t;

void AppTempInit (void);

#endif

