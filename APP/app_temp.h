#ifndef __APP_TEMP_H__
#define __APP_TEMP_H__

#include "includes.h"
#include "TempCtrl.h"

typedef struct _app_temp	{
	OS_EVENT           *MSG_Q;
	OS_EVENT           *lock;
	s16 target_t[TEMP_ID_NUMS];//目标温度 0.01
	s16 current_t[TEMP_ID_NUMS];//当前温度 0.01
}_app_temp_t;

void AppTempInit (void);

#endif

