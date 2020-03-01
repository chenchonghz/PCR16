#ifndef __DACAI_API_H__
#define __DACAI_API_H__

#include "DaCai_ProtoAnalysis.h"

typedef struct _UI	{
	s16 screen_id;//µ±Ç°»­Ãæid
	s16 sub_screen_id;
	u8 ctrl_id;
	s8 button_id;
	s8 index;
	char *pdata;
	u16 datlen;
}_UI_t;

extern _UI_t UI;

void DaCaiAPI_Init(void);
void DaCai_CheckDevice(void);
void DaCai_SwitchUI(_UI_t *pUI);
void DaCai_DynamicCtrl(_UI_t *pUI, u8 flag);
void DaCai_UpdateTXT(_UI_t *pUI);
void DaCai_GetScreenID(void);
void DaCai_StandbyMode(u8 state);
void DaCai_ButtonCtrl(_UI_t *pUI,u8 state);
void DaCai_ChooseCtrl(_UI_t *pUI);
void DaCai_SetCursorPos(_UI_t *pUI,u8 state);
void DaCai_DisKeyboard(u16 x,u16 y);
void DaCai_TimeGet(void);
void DaCai_TimeModeSet(void);
void DaCai_CtrlONOFF(_UI_t *pUI, u8 flag);
void DaCai_SwitchSubUI(_UI_t *pUI);
void DaCai_UpdateMultiButton(_UI_t *pUI,u8 *pButtonID, u8 *pVal, u8 num);
void DaCai_UpdateMultiTXT(_UI_t *pUI,_MultiTXT **pMultiTXT, u8 num);
void DaCai_ScreenSaveMode(u8 enable, u16 standy_t, u8 standy_brightness, u8 brightness);
void DaCai_AdjustBrightness(u8 brightness);
void DaCai_ClearScreenAuto(u8 enable);
void DaCai_ClearScreen(u8 screen);
void DaCai_ClearTXT(_UI_t *pUI);
void DaCai_SetProgressForecolor(_UI_t *pUI,u8 R,u8 G);
void DaCai_SetTXTtwinkle(_UI_t *pUI,u16 period);
void DaCai_EnterStandby(void);
void DaCai_ExitStandby(void);

#endif
