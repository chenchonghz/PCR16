#ifndef _APP_UDISK_H
#define _APP_UDISK_H

#include "includes.h"

typedef struct _udisk	{
	OS_EVENT           *Mbox;
	//FATFS fatfs;
//	u8 state;
}_appudisk_t;

extern _appudisk_t appudisk;
void AppUSBInit(void);

#endif
