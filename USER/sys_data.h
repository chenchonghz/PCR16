#ifndef __SYS_DATA_H
#define __SYS_DATA_H

#include "includes.h"

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


extern tlsf_t UserMem;

void SysDataInit(void);

#endif
