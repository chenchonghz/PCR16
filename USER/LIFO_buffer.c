#include <string.h>
#include "LIFO_buffer.h"

/**
 * @def		RB_VHEAD(rb)
 * volatile typecasted head index
 */
#define RB_VHEAD(rb)              (*(volatile INT32U *) &(rb)->head)

/**
 * @def		RB_VTAIL(rb)
 * volatile typecasted tail index
 */
#define RB_VTAIL(rb)              (*(volatile INT32U *) &(rb)->tail)
	
#define RB_INDH(rb)                ((rb)->head & ((rb)->count - 1))
//#define RB_INDT(rb)                ((rb)->tail & ((rb)->count - 1))

static int RingBuffer_GetCount(LIFOBUFF_T *LifoBuff)
{
	return RB_VHEAD(LifoBuff);
}

static int LifoBuffer_IsFull(LIFOBUFF_T *LifoBuff)
{
	return (RingBuffer_GetCount(LifoBuff) >= LifoBuff->count);
}

static int LifoBuffer_IsEmpty(LIFOBUFF_T *LifoBuff)
{
	return RB_VHEAD(LifoBuff) == 0;
}

/* Initialize ring buffer */
int LIFOBuffer_Init(LIFOBUFF_T *LifoBuff, void *buffer, int itemSize, int count)
{
	LifoBuff->dat = buffer;
	LifoBuff->count = count;
	LifoBuff->itemSz = itemSize;
	LifoBuff->head = LifoBuff->tail = 0;

	return 1;
}

/* Insert a single item into Ring Buffer */
int LIFOBuffer_Insert(LIFOBUFF_T *LifoBuff, const void *dat)
{
	INT8U *ptr = LifoBuff->dat;

	/* We cannot insert when queue is full */
	if (LifoBuffer_IsFull(LifoBuff))
		return 0;

	ptr += RB_INDH(LifoBuff) * LifoBuff->itemSz;
	memcpy(ptr, dat, LifoBuff->itemSz);
	LifoBuff->head++;

	return 1;
}

/* Pop single item from Ring Buffer */
int LIFOBuffer_Pop(LIFOBUFF_T *LifoBuff, void *dat)
{
	INT8U *ptr = LifoBuff->dat;

	/* We cannot pop when queue is empty */
	if (LifoBuffer_IsEmpty(LifoBuff))
		return 0;

//	ptr += RB_INDT(LifoBuff) * LifoBuff->itemSz;
//	memcpy(dat, ptr, LifoBuff->itemSz);
//	LifoBuff->tail++;
	LifoBuff->head--;
	ptr += RB_INDH(LifoBuff) * LifoBuff->itemSz;
	memcpy(dat, ptr, LifoBuff->itemSz);

	return 1;
}

