#ifndef __APP_DISPLAY_H__
#define __APP_DISPLAY_H__

#include "includes.h"
#include "DaCai_ProtoAnalysis.h"
#include "DaCai_API.h"
#include "LIFO_buffer.h"

typedef struct _appdisplay	{
	OS_EVENT		*sem;
	OS_EVENT        *MSG_Q;
	_DACAI_t	*pDaCai;
	_UI_t *pUI;
}_appdisplay_t;

//extern LIFOBUFF_T ScreenIDLIFO;
extern _appdisplay_t appdis;
void AppDisplayInit(void);
void StartAppDisplay(message_pkt_t *msg);

#endif
