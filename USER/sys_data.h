#ifndef __SYS_DATA_H
#define __SYS_DATA_H

#include "sys_bits.h"
#include "sys_types.h"
#include "sys_defs.h"
#include "tlsf.h"

#define	FILE_NAME_LEN		50
#define	LOG_FILE_NAME		"Pcr16Log.txt"//"PerfusionLog.txt"

typedef struct _syserror	{
	BIT8 Y1;
	BIT8 Y2;
	//BIT8 Y3;
}_syserror_t;

typedef struct _sys_status	{
	volatile u32 state;
}_sys_status_t;

typedef struct _sys_data	{
	s8 HeatCoverTemp;//热盖温度
	s8 Temp1;//
	s8 Temp2;//
	s16 PD_1;//PD传感器1
	s16 PD_2;//PD传感器2
}_sys_data_t;

extern _sys_data_t SysData;
extern tlsf_t UserMem;

void SysDataInit(void);

#endif
