/*
 * takie_czasy.h
 *
 *  Created on: 26 mar 2022
 *      Author: andrzej
 */

#ifndef TAKIE_CZASY_H_
#define TAKIE_CZASY_H_

#include <stdint.h>

#define STA_CZASY_OK                 0
#define STA_CZASY_INVALID           -1
#define STA_CZASY_RTC_NOT_RUNNING   -2
#define STA_CZASY_RTC_ARG_ERROR     -3

typedef uint32_t dptimestamp_t;

void czasy_init(void);
int czasy_get(dptimestamp_t * ts);
dptimestamp_t czasy_getUnchecked(void);
int czasy_set(dptimestamp_t ts);

#endif /* TAKIE_CZASY_H_ */
