#ifndef __DISPLAY_CODE_H__
#define __DISPLAY_CODE_H__

#include "includes.h"
#include "app_display.h"

extern const char Code_Warning[][12];
extern const char Code_Message[][12];
	
void SaveUIEditInfor(void);
void DisplayUIIDAndBackup(u8 id);
void DisplayBackupUIID(void);
void DisplayMessageUI(char *pbuf);
void DisplayEditUI(void);
void DisplayKeyboardUI(void);
#endif

