/*
 * lights.h
 *
 *  Created on: 17 gru 2022
 *      Author: andrzej
 */

#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <stdint.h>

typedef enum {
    ELedMain,
    ELedWc,
    ELed1R,
    ELed1G,
    ELed1B,
    ELed2R,
    ELed2G,
    ELed2B,
    ELed3R,
    ELed3G,
    ELed3B,

    ELedCount,
} ELed;


void lights_init(void);
void lights_cycle(void);
void lights_off(void);

void lights_cmd(const char * txt);
void lights_blink_ip_address(uint32_t ip4a);
void lights_onoff(ELed led, unsigned val);


#endif /* LIGHTS_H_ */
