#ifndef __AD_TASK_H__
#define __AD_TASK_H__

#include "bsp_ad7124.h"

enum _ad7124_status {
	AD7124_IDLE,
//	AD7124_MEASURE_AVDD,
//	AD7124_MEASURE_AVDD_OK,
//	AD7124_DETECT_SENSOR,
//	AD7124_MEASURE_SENSOR,
	AD7124_MEASURE_TEMP,
};

typedef struct  _ad7124
{
	ad7124_dev_t *pdev;
	u8 channel_last;
	u8 channel;
	u8 status;
	u8 busy;
}_ad7124_t;

void AD_TaskInit(void);
void AD_Task(void);
#endif

