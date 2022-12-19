/*
 * takie_czasy.c
 *
 *  Created on: 26 mar 2022
 *      Author: andrzej
 */

#include "hardware/rtc.h"

#include "takie_czasy.h"

#include <stdio.h>

#define EPOCH_START_YEAR   2005
#define EPOCH_START_MONTH  4
#define EPOCH_START_DAY    2
#define EPOCH_START_HOUR   21
#define EPOCH_START_MINUTE 37
#define EPOCH_START_SECOND 0

static bool is_dt_valid(const datetime_t * dt)
{
    return dt->year > EPOCH_START_YEAR + 5;
}

static dptimestamp_t date_to_epoch(const datetime_t * dt)
{
    dptimestamp_t ts = 23599380U;
    uint32_t days = 0U;
    uint32_t hours;
    uint32_t minutes;

    for (uint32_t y = EPOCH_START_YEAR + 1U; y < dt->year; ++y)
    {
        if (0 == (y & 3))
        {
            days += 366U;
        }
        else
        {
            days += 365U;
        }
    }

    for (uint32_t m = 1; m < dt->month; ++m)
    {
        switch (m)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            days += 31U;
            break;
        case 2:
            if (0 == (dt->year & 3))
            {
                days += 29U;
            }
            else
            {
                days += 28U;
            }
            break;
        default:
            days += 30U;
        }
    }

    days += (dt->day - 1U);
    hours = days * 24U + dt->hour;
    minutes = hours * 60U + dt->min;
    ts += 60U * minutes + dt->sec;

    return ts;
}

static void epoch_to_date(dptimestamp_t ts, datetime_t * dt)
{
    uint32_t remains = ts - 23599380U;

    dt->year = EPOCH_START_YEAR + 1;

    while (1)
    {
        if (0 == (dt->year & 3))
        {
            if (remains < 31622400U)
            {
                break;
            }
            remains -= 31622400U;
        }
        else
        {
            if (remains < 31536000U)
            {
                break;
            }
            remains -= 31536000U;
        }
        dt->year += 1;
    }

    dt->month = 1;

    while (1)
    {
        uint32_t sim;

        switch (dt->month)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            sim = 2678400U;
            break;
        case 2:
            if (0 == (dt->year & 3))
            {
                sim = 2505600U;
            }
            else
            {
                sim = 2419200U;
            }
            break;
        default:
            sim = 2592000U;
        }

        if (remains < sim)
        {
            break;
        }
        remains -= sim;
        dt->month += 1;
    }

    dt->day = 1 + (remains / 86400);
    remains = remains % 86400;
    dt->dotw = 1;
    dt->hour = remains / 3600;
    remains = remains % 3600;
    dt->min = remains / 60;
    dt->sec = remains % 60;
}


void czasy_init(void)
{
    datetime_t zero = {
            .year = EPOCH_START_YEAR,
            .month = EPOCH_START_MONTH,
            .day = EPOCH_START_DAY,
            .hour = EPOCH_START_HOUR,
            .min = EPOCH_START_MINUTE,
            .sec = EPOCH_START_SECOND,
            .dotw = 1,
    };
    rtc_init();
    rtc_set_datetime(&zero);
}

int czasy_get(dptimestamp_t * ts)
{
    datetime_t tera;
    bool success = rtc_get_datetime(&tera);

    if (success)
    {
        printf("%d.%d.%d %d:%d:%d\n",
                (int) tera.year, (int) tera.month, (int) tera.day,
                (int) tera.hour, (int) tera.min, (int) tera.sec);

        if (is_dt_valid(&tera))
        {
            *ts = date_to_epoch(&tera);

            printf("  %u\n", *ts);
            datetime_t jeszczeraz;
            epoch_to_date(*ts, &jeszczeraz);
            printf("  %d.%d.%d %d:%d:%d\n",
                    (int) jeszczeraz.year, (int) jeszczeraz.month, (int) jeszczeraz.day,
                    (int) jeszczeraz.hour, (int) jeszczeraz.min, (int) jeszczeraz.sec);

            return STA_CZASY_OK;
        }

        return STA_CZASY_INVALID;
    }

    return STA_CZASY_RTC_NOT_RUNNING;
}

dptimestamp_t czasy_getUnchecked(void)
{
    datetime_t tera;
    bool success = rtc_get_datetime(&tera);
    return date_to_epoch(&tera);
}

int czasy_set(dptimestamp_t ts)
{
    datetime_t dt;

    if (ts < 23599380U)
    {
        return STA_CZASY_INVALID;
    }

    epoch_to_date(ts, &dt);

    if (rtc_set_datetime(&dt))
    {
        return STA_CZASY_OK;
    }

    return STA_CZASY_RTC_ARG_ERROR;
}
