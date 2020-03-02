#include "timestamp.h"

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)


static const uint8_t rtc_days_in_month[] =
{
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static uint8_t is_leap_year(uint32_t year)
{
    return (!(year % 4) && (year % 100)) || !(year % 400);
}

static int32_t rtc_month_days(uint32_t month, uint32_t year)
{
    return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

void timestamp_to_time(int64_t time, struct rtc_time *tm)
{
    unsigned int month, year;
    int days;

    days = time / 86400;
    time -= (unsigned int) days * 86400;

    /* day of the week, 1970-01-01 was a Thursday */
    tm->tm_wday = (days + 4) % 7;

    year = 1970 + days / 365;
    days -= (year - 1970) * 365
            + LEAPS_THRU_END_OF(year - 1)
            - LEAPS_THRU_END_OF(1970 - 1);
    if (days < 0)
    {
        year -= 1;
        days += 365 + is_leap_year(year);
    }
    tm->tm_year = year - 1900;
    tm->tm_yday = days + 1;

    for (month = 0; month < 11; month++)
    {
        int newdays;

        newdays = days - rtc_month_days(month, year);
        if (newdays < 0)
            break;
        days = newdays;
    }
    tm->tm_mon = month;
    tm->tm_mday = days + 1;

    tm->tm_hour = time / 3600;
    time -= tm->tm_hour * 3600;
    tm->tm_min = time / 60;
    tm->tm_sec = time - tm->tm_min * 60;
}

int64_t time_to_timestamp(const unsigned int year0, const unsigned int mon0,
                                const unsigned int day, const unsigned int hour,
                                const unsigned int min, const unsigned int sec)
{
    uint32_t mon = mon0, year = year0;

    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int32_t) (mon -= 2))
    {
        mon += 12;	/* Puts Feb last since it has leap day */
        year -= 1;
    }

    return ((((int64_t)
              (year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) + year * 365 - 719499
             ) * 24 + hour /* now have hours */
            ) * 60 + min /* now have minutes */
           ) * 60 + sec; /* finally seconds */
}

static int32_t sys_time_zone = 28800; //default time zone, 8 hours
int32_t RTC_get_time_zone(void)
{
    return sys_time_zone;
}

