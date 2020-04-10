#ifndef __DACAI_TOUCHEVENT_H__
#define __DACAI_TOUCHEVENT_H__

#include "includes.h"

#define	T_BUTTON_NUM		6

u8 TempButtonClick(u16 touch_x, u16 touch_y);
void TempButtonCheckOn(u8 id);

extern u8 TempButtonState[T_BUTTON_NUM-1];
extern u8 TempButtonPressID;
#endif
