/*
 * settings.h
 *
 *  Created on: 20 gru 2022
 *      Author: andrzej
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>

#define ALARM_TYPE_WC     1
#define ALARM_TYPE_DISCO  2
#define ALARM_TYPE_ALARM  3

typedef struct {
    uint8_t h;
    uint8_t m;
} ootime_t;

typedef struct {
    uint8_t type;
    ootime_t tim;
    uint8_t repeat_days;
    uint16_t param1;
    uint16_t param2;
} alarm_t;

typedef struct
{
    uint8_t time_valid;
    ootime_t enable_lights_start;
    ootime_t enable_lights_end;
    uint8_t wc_too;
    uint8_t sense_dark;
    uint8_t sense_bright;
    uint16_t sens_threshold_dark;
    uint16_t sens_threshold_bright;
    uint16_t react_delay;
    uint16_t min_on;
    uint16_t min_off;
    uint16_t max_on;

    uint8_t alarm_count;
    alarm_t alarms [8];
} PiernikSettings_t;

PiernikSettings_t * getSettings();

void settingsParseTime(const char * input);

#endif /* SETTINGS_H_ */
