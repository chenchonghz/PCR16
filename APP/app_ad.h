#ifndef __APP_AD_H__
#define __APP_AD_H__

#include "bsp_ad7124.h"
#include "ad7124.h"

typedef struct _app_ad	{
	u16 wait_t;
}_app_ad_t;

void AppADInit (void);
void StartAppADTask(void);
void StopAppADTask(void);
#endif

