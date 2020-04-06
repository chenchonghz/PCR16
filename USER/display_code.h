#ifndef __DISPLAY_CODE_H__
#define __DISPLAY_CODE_H__

#include "includes.h"
#include "app_display.h"

extern const char Code_Warning[][12];
extern const char Code_Message[][12];
	
void SaveUIEditInfor(void);
void UUIDBackup(void);
void DisplayUIIDAndBackup(u8 id);
void DisplayBackupUIID(void);
void DisplayMessageUI(char *pbuf);
void DisplayEditUI(void);
void DisplayKeyboardUI(void);
void DisplayStepUI(void);
void DisplayStageUI(void);
void DisplayTempProgramUI(u8 clear_s);
void DisplaMenuUI(void);
void DisplayLabUI(void);
void DisplayQiTingLab(void);
void PaintTriangle(u16 x, u16 y, u16 w, u16 h);
void PaintTriangleInBasicGraph(_UI_t *pUI,  u16 x, u16 y, u16 w, u16 h);
#endif

