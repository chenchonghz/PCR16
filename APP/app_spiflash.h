#ifndef _APP_SPIFLASH_H
#define _APP_SPIFLASH_H

#include "includes.h"

typedef struct _spiflash	{
	OS_EVENT           *MSG_Q;
	//FATFS fatfs;
}_spiflash_t;

extern _spiflash_t spiflash;
void WriteLogByTask(message_pkt_t *Src);
void AppSpiFlashInit(void);

#endif
