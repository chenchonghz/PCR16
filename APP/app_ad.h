#ifndef __APP_AD_H__
#define __APP_AD_H__

#include "bsp_ad7124.h"
#include "ad7124.h"

typedef struct _app_ad	{
	OS_EVENT           *sem;
	u8 rflag;
	u16 wait_t;
}_app_ad_t;

extern _app_ad_t app_ad;

void AppADInit (void);
void StartAppADTask(void);
void StopAppADTask(void);
#endif

