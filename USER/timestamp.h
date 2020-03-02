#ifndef _timestam_h_
#define _timestam_h_

#include <stdint.h>

typedef struct rtc_time
{
    int tm_sec;//seconds 0-61
    int tm_min;//minutes 1-59
    int tm_hour;//hours 0-23
    int tm_mday;//day of the month 1-31
    int tm_mon;//months since jan 0-11
    int tm_year;//years since 1900
    int tm_wday;//days since Sunday, 0-6
    int tm_yday;//days since Jan 1, 0-365
    int tm_isdst;//Daylight Saving time indicator
} RTC_TIME_ST;

void timestamp_to_time(int64_t time, struct rtc_time *tm);

int64_t time_to_timestamp(const unsigned int year0, const unsigned int mon0,
                                const unsigned int day, const unsigned int hour,
                                const unsigned int min, const unsigned int sec);
#endif
