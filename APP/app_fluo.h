#ifndef __APP_FLUO_H__
#define __APP_FLUO_H__

#include "includes.h"

typedef struct _Fluo_t {
//	OS_EVENT            *Sem;
    OS_EVENT            *Mbox;
}Fluo_t;

void AppFluoInit(void);
#endif

