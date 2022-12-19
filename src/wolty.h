/*
 * wolty.h
 *
 *  Created on: 25 mar 2022
 *      Author: andrzej
 */

#ifndef WOLTY_H_
#define WOLTY_H_

#include <stdint.h>

typedef enum
{
    EWolt_Twilight,
//    EWolt_InternalTemp,
//    EWolt_VSYS,
} EWolt_t;

void wolty_init(void);
void wolty_deinit(void);
uint16_t wolty_read(EWolt_t what);

//float wolty_internalTemperature(void);
//float wolty_vsys(void);
float wolty_twilight(void);

#endif /* WOLTY_H_ */
