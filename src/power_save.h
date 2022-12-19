/*
 * power_save.h
 *
 *  Created on: 26 mar 2022
 *      Author: andrzej
 */

#ifndef POWER_SAVE_H_
#define POWER_SAVE_H_

#include <stdint.h>

typedef enum
{
    EWakeup_Unknown = 0,
    EWakeup_RTC,
    EWakeup_Button,
} EWakeupReason_t;

void power_save_init(void);
EWakeupReason_t power_save_sleep(uint32_t seconds);

#endif /* POWER_SAVE_H_ */
