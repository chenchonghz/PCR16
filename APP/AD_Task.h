#ifndef __AD_TASK_H__
#define __AD_TASK_H__

#include "bsp_ad7124.h"

typedef struct  _ad7124
{
	ad7124_dev_t *pdev;
	u8 channel_last;
	u8 channel;
	u8 status;
	u8 busy;
}_ad7124_t;

void AD_TaskInit(void);

#endif

