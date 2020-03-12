#ifndef __LIFO_BUFFER_H_
#define __LIFO_BUFFER_H_

#include    "os_cpu.h"

//后进先出 buffer

typedef struct {
	void *dat;
	int count;
	int itemSz;
	INT32U head;
	INT32U tail;
} LIFOBUFF_T;

	
int LIFOBuffer_Init(LIFOBUFF_T *LifoBuff, void *buffer, int itemSize, int count);
int LIFOBuffer_Insert(LIFOBUFF_T *LifoBuff, const void *dat);
int LIFOBuffer_Pop(LIFOBUFF_T *LifoBuff, void *dat);

#endif
