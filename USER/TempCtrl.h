#ifndef __TEMPCTRL_H__
#define __TEMPCTRL_H__

#include "includes.h"
#include "ad7124.h"

#define  	TEMP_ID1       uCH_2
#define  	TEMP_ID2       uCH_1
#define		TEMP_ID3       uCH_0
#define		TEMP_ID4       uCH_3


//#define TEMP_ID_MIN 			TEMP_ID1
//#define TEMP_ID_MAX 			TEMP_ID4
#define TEMP_ID_NUMS       	4

//typedef struct _tempctrl	{
//	u16 cur_vol;
//	u16 cur_temp;
//}_tempctrl_t;

void CalcTemperature(u32 Rx, s32 *ptemp);

#endif

