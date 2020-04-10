#include "DaCai_TouchEvent.h"
#include "app_display.h"

#define	TOUCH_W		150
#define	TOUCH_H		200


 struct touch_area_t	{
	u16 x1;
	u16 y1;
	u16 x2;
	u16 y2;
};
const struct touch_area_t	touch_area[T_BUTTON_NUM]	=	{
	[0]	=	{
		.x1 = 17,
		.y1	= 143,
		.x2 = 17+TOUCH_W,
		.y2 = 102+TOUCH_H,
	},
	[1]	=	{
		.x1 = 171,
		.y1	= 143,
		.x2 = 171+TOUCH_W,
		.y2 = 102+TOUCH_H,
	},
	[2]	=	{
		.x1 = 325,
		.y1	= 143,
		.x2 = 325+TOUCH_W,
		.y2 = 102+TOUCH_H,
	},
	[3]	=	{
		.x1 = 479,
		.y1	= 143,
		.x2 = 479+TOUCH_W,
		.y2 = 102+TOUCH_H,
	},
	[4]	=	{
		.x1 = 633,
		.y1	= 143,
		.x2 = 633+TOUCH_W,
		.y2 = 102+TOUCH_H,
	},
	[5]	=	{
		.x1 = 70,
		.y1	= 21,
		.x2 = 173,
		.y2 = 21+60,
	},
};
u32 LaseClickTim[T_BUTTON_NUM]={0};
u8 TempButtonClick(u16 touch_x, u16 touch_y)
{
//	static u8 touchcnt[T_BUTTON_NUM];
	u8 i;
	s32 time_diff;
	
	for(i=0;i<T_BUTTON_NUM;i++)	{
		if(touch_x>touch_area[i].x1 && touch_x<touch_area[i].x2 && touch_y>touch_area[i].y1 && touch_y<touch_area[i].y2)	{
//			touchcnt[i] ++;
//			if(touchcnt[i]>=2)	
//			{
//				touchcnt[i] = 0;
				time_diff = OSTimeGet() - LaseClickTim[i];
				if(time_diff<0)	{
					time_diff += 0xffffffff;
				}
				if(time_diff<=200)
					return 0xff;
				else if(time_diff<1000&&time_diff>200)	{//双击间隔要求小于1s
					i |= 0x80;//双击
				}
//				else	{
//					i |= 0x80;//双击
//				}
				LaseClickTim[i] = OSTimeGet();
				return i;//单击
//			}	
		}
//		else
//			touchcnt[i] = 0;		
	}
	return 0xff;
}

u8 TempButtonState[T_BUTTON_NUM-1]={0};
u8 TempButtonPressID=0xff;
void TempButtonCheckOn(u8 id)
{
	u8 i;
	_MultiDat icon[T_BUTTON_NUM-1];
	
	for(i=0;i<T_BUTTON_NUM-1;i++)	{
		icon[i].len = 2;
		icon[i].id = 10 + i;
		if(i==id)	{
			TempButtonState[i] = !TempButtonState[i];
		}
		else	{
			TempButtonState[i] = 0;
		}
		icon[i].buf[0] = 0;
		icon[i].buf[1] = TempButtonState[i];
	}
	DaCai_UpdateMultiIcon(appdis.pUI, icon, i);
	TempButtonPressID = id;
}

