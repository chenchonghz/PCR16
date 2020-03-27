#ifndef _bsp_rtc_h_
#define _bsp_rtc_h_

#include "includes.h"

void bsp_rtc_set_time(const u8 year, const u8 mon,const u8 wday,
                                const u8 mday, const u8 hour,
                                const u8 min, const u8 sec);
void bsp_printf_rtctime(void);
void bsp_rtc_init(void);
//void bsp_rtc_get_time(RTC_TimeTypeDef *pTime, RTC_DateTypeDef *pDate);								
								
#endif
