#ifndef __PD_DataProcess_H__
#define __PD_DataProcess_H__

#include "includes.h"

typedef struct _pd_maxmin	{//PD最大值 最小值 均值
	u8 idx;
	u16 min[2];
	u16 max[2];
	u16 aver;
}_pd_maxmin_t;

typedef struct _pd_data	{//PD数据结构体
	u8 coll_enable;//是否采集标志
	u8 ch;
	u16 HoleThreshold;//有效孔位置的PD阀值
	_pd_maxmin_t templatehole;
}_pd_data_t;

extern _pd_data_t gPD_Data;

void PD_DataInit(void);
void StartCollTemplateHolePD(void);
void StopCollTemplateHolePD(void);
void CalcTemplateHolePDAver(void);
void PD_DataCollect(u16 ad_vol, u8 pd_ch);

#endif

