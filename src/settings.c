/*
 * settings.c
 *
 *  Created on: 20 gru 2022
 *      Author: andrzej
 */

#include "settings.h"

#include <stdio.h>
#include "hardware/rtc.h"

PiernikSettings_t g_settings =
{
        .time_valid = 0,
        .enable_lights_start = {16, 0},
        .enable_lights_end = {23, 10},
        .wc_too = 0,
        .sense_dark = 0,
        .sense_bright = 0,
        .sens_threshold_dark = 0,
        .sens_threshold_bright = 0,
        .react_delay = 5,
        .min_on = 0,
        .min_off = 0,
        .max_on = 0,

        .alarm_count = 0,
        .alarms =
        {
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
        },
};

PiernikSettings_t * getSettings()
{
    return &g_settings;
}

void settingsParseTime(const char * input)
{
    int new_year, new_mon, new_day, new_hour, new_min, new_sec;
    int new_s_h, new_s_m, new_e_h, new_e_m;

    int conv = sscanf(input, "%d_%d_%d_%d_%d_%d_%d:%d_%d:%d_%d",
            &new_year, &new_mon, &new_day, &new_hour,
            &new_min, &new_sec, &new_s_h, &new_s_m,
            &new_e_h, &new_e_m);

    if (conv == 10)
    {
        if (new_year > 3000)
        {
            new_year /= 10;
            g_settings.wc_too = 1;
        }
        else
        {
            g_settings.wc_too = 0;
        }

        datetime_t nt = {
                .year = new_year,
                .month = new_mon + 1,
                .day = new_day,
                .hour = new_hour,
                .min = new_min,
                .sec = new_sec,
                .dotw = 1,
        };
        rtc_set_datetime(&nt);

        g_settings.enable_lights_start.h = new_s_h;
        g_settings.enable_lights_start.m = new_s_m;
        g_settings.enable_lights_end.h = new_e_h;
        g_settings.enable_lights_end.m = new_e_m;
        g_settings.time_valid = 1;
    }
}
