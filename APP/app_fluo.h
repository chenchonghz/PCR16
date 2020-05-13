#ifndef __APP_FLUO_H__
#define __APP_FLUO_H__

#include "includes.h"

typedef struct _Fluo_t {
//	OS_EVENT            *Sem;
    OS_EVENT            *Mbox;
	OS_EVENT            *CalcMbox;
}Fluo_t;

extern Fluo_t fluo;

void AppFluoInit(void);

#endif

