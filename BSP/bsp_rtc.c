#include "bsp_rtc.h"
//#include "rtc.h"

//#define	RTC_BKP_VAL		0x25A5A
//void bsp_rtc_init(void)	
//{
//	hrtc.Instance = RTC;
//	
//	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)==RTC_BKP_VAL)	{	
//		SYS_PRINTF("\r\nRTC BKP REG OK");		
//		return;
//	}
//	SYS_PRINTF("RTC Init");
//	MX_RTC_Init();
//	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BKP_VAL);
//}

//void bsp_rtc_set_time(const u8 year, const u8 mon,const u8 wday,
//                                const u8 mday, const u8 hour,
//                                const u8 min, const u8 sec)
//{
//	RTC_TimeTypeDef sTime = {0};
//	RTC_DateTypeDef sDate = {0};
//	
//	sTime.Hours = hour;
//	sTime.Minutes = min;
//	sTime.Seconds = sec;
//	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
//	sDate.WeekDay = wday;
//	sDate.Month = mon;
//	sDate.Date = mday;
//	sDate.Year = year;
//	SYS_PRINTF("Set RTC");
//	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
//}

//void bsp_rtc_get_time(RTC_TimeTypeDef *pTime, RTC_DateTypeDef *pDate)
//{
////	RTC_TimeTypeDef sTime = {0};
////	RTC_DateTypeDef sDate = {0};
//	
//	HAL_RTC_GetTime(&hrtc, pTime ,RTC_FORMAT_BIN);
//	HAL_RTC_GetDate(&hrtc, pDate, RTC_FORMAT_BIN);
//}
//	
//void bsp_printf_rtctime(void)
//{
//	RTC_TimeTypeDef sTime = {0};
//	RTC_DateTypeDef sDate = {0};
//	
////	HAL_RTC_GetTime(&hrtc, &sTime ,RTC_FORMAT_BIN);
////	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
//	bsp_rtc_get_time(&sTime,&sDate);
//	
//	SYS_PRINTF("%u/%u/%u  %u:%u:%u",(sDate.Year+2000),(sDate.Month),(sDate.Date),	\
//	(sTime.Hours),(sTime.Minutes),(sTime.Seconds));
//}




