/*
 * lights.c
 *
 *  Created on: 17 gru 2022
 *      Author: andrzej
 */
#include <stdio.h>

#include "lights.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"

#include "pytka.h"

/*
 *  Raspberry pi pico pinout
 *
 *                     +------+ USB +-----+
 *                GP0 -|- 1   +-----+  40-|- VBAT
 *                GP1 -|- 2            39-|- VSYS
 *                GND -|- 3            38-|- GND
 *  LIGHT_MN <--  GP2 -|- 4            37-|- EN
 *  LIGHT_WC <--  GP3 -|- 5            36-|- 3.3V
 *       L3R <--  GP4 -|- 6            35-|- ADC_VREF
 *       L3G <--  GP5 -|- 7            34-|- GP28
 *                GND -|- 8            33-|- AGND
 *       L3B <--  GP6 -|- 9     TOP    32-|- GP27
 *       L2R <--  GP7 -|-10            31-|- GP26
 *                GP8 -|-11            30-|- RUN/RESET
 *                GP9 -|-12            29-|- GP22
 *                GND -|-13            28-|- GND
 *       L2G <-- GP10 -|-14            27-|- GP21
 *       L2B <-- GP11 -|-15            26-|- GP20
 *       L1R <-- GP12 -|-16            25-|- GP19
 *       L1G <-- GP13 -|-17            24-|- GP18
 *                GND -|-18            23-|- GND
 *       L1B <-- GP14 -|-19            22-|- GP17
 *               GP15 -|-20    hdbg    21-|- GP16
 *                     +------------------+
 *
 *  ==Pin list==
 *
 *  Main light:
 *  4  LIGHT_MN
 *
 *  WC light:
 *  5  LIGHT_WC
 *
 *  RGB disco lights:
 *  6  L3R
 *  7  L3G
 *  9  L3B
 *  10 L2R
 *  14 L2G
 *  15 L2B
 *  16 L1R
 *  17 L1G
 *  19 L1B
 *
 */

#define PIN_SALON   PIN_LIGHT_GP_O_N_MAIN
#define PIN_WC      PIN_LIGHT_GP_O_WC

#define PIN_L3R     PIN_LIGHT_GP_O_L3R
#define PIN_L3G     PIN_LIGHT_GP_O_L3G
#define PIN_L3B     PIN_LIGHT_GP_O_L3B
#define PIN_L2R     PIN_LIGHT_GP_O_L2R
#define PIN_L2G     PIN_LIGHT_GP_O_L2G
#define PIN_L2B     PIN_LIGHT_GP_O_L2B
#define PIN_L1R     PIN_LIGHT_GP_O_L1R
#define PIN_L1G     PIN_LIGHT_GP_O_L1G
#define PIN_L1B     PIN_LIGHT_GP_O_L1B

typedef enum {
    ELedOff,
    ELedOn,
    ELedPwm,
} ELedState;

typedef enum {
    ELightsNop,
} ELightsRequest;

typedef enum {
    ELightsIdle,
} ELightsState;

static struct {
    int valid;
    ELightsState gotoState;
} s_request = {0};

static struct {
    ELightsState state;
    ELedState ledState[ELedCount];
} s_state = {.state = ELightsIdle};

static const unsigned LED_PINS [ELedCount] =
{
        PIN_SALON, // ELedMain
        PIN_WC   , // ELedWc,
        PIN_L3R  , // ELed1R,
        PIN_L3G  , // ELed1G,
        PIN_L3B  , // ELed2B,
        PIN_L2R  , // ELed2R,
        PIN_L2G  , // ELed2G,
        PIN_L2B  , // ELed2B,
        PIN_L1R  , // ELed3R,
        PIN_L1G  , // ELed3G,
        PIN_L1B  , // ELed3B,
};

static inline void gpled_init(ELed led, unsigned ival)
{
    unsigned gpn = LED_PINS[led];

    gpio_init(gpn);
    gpio_set_dir(gpn, GPIO_OUT);
    lights_onoff(led, ival);
}

static inline void gpled_toggle(ELed led)
{
    unsigned gpn = LED_PINS[led];
    ELedState state = s_state.ledState[led];

    state ^= 1;

    lights_onoff(led, state);
}

void lights_init(void)
{
    gpled_init(ELedMain, 0);
    gpled_init(ELedWc, 0);

    gpled_init(ELed3R, 0);
    gpled_init(ELed3G, 0);
    gpled_init(ELed3B, 0);
    gpled_init(ELed2R, 0);
    gpled_init(ELed2G, 0);
    gpled_init(ELed2B, 0);
    gpled_init(ELed1R, 0);
    gpled_init(ELed1G, 0);
    gpled_init(ELed1B, 0);

    s_state.state = ELightsIdle;
    s_request.valid = 0;
}

void lights_cycle(void)
{

}

void lights_off(void)
{
    lights_onoff(ELedMain, 0);
    lights_onoff(ELedWc, 0);

    lights_onoff(ELed3R, 0);
    lights_onoff(ELed3G, 0);
    lights_onoff(ELed3B, 0);
    lights_onoff(ELed2R, 0);
    lights_onoff(ELed2G, 0);
    lights_onoff(ELed2B, 0);
    lights_onoff(ELed1R, 0);
    lights_onoff(ELed1G, 0);
    lights_onoff(ELed1B, 0);

    s_state.state = ELightsIdle;
    s_request.valid = 0;
}

void lights_onoff(ELed led, unsigned val)
{
    unsigned gpn = LED_PINS[led];

    if (PIN_LIGHT_GP_O_N_MAIN == gpn)
    {
        gpio_put(gpn, !val);
    }
    else
    {
        gpio_put(gpn, val);
    }

    s_state.ledState[led] = val ? ELedOn : ELedOff;
}

void lights_cmd(const char * txt)
{
    if (txt[0] == 'l')
    {
        if (txt[1] == 's')
        {
            printf("L: toggle salon\n");
            gpled_toggle(ELedMain);
        }
        else if (txt[1] == 'w')
        {
            printf("L: toggle WC\n");
            gpled_toggle(ELedWc);
        }
    }
}

void lights_blink_ip_address(uint32_t ip4a)
{
    ip4a >>= 24;
    uint8_t hundrets = ip4a / 100;
    uint8_t tens = (ip4a % 100) / 10;
    uint8_t ones = ip4a % 10;
    uint8_t i;

    printf("Blink last octet of IP address: %d\n", (int) ip4a);
    printf("  red    %d   blinks\n", (int) hundrets);
    printf("  green   %d  blinks\n", (int) tens);
    printf("  blue     %d blinks\n", (int) ones);

    for (i = 0; i < hundrets; ++i)
    {
        lights_onoff(ELed1R, 1);
        vTaskDelay(300);
        lights_onoff(ELed1R, 0);
        vTaskDelay(700);
    }

    vTaskDelay(1000);

    for (i = 0; i < tens; ++i)
    {
        lights_onoff(ELed1G, 1);
        vTaskDelay(300);
        lights_onoff(ELed1G, 0);
        vTaskDelay(700);
    }

    vTaskDelay(1000);

    for (i = 0; i < ones; ++i)
    {
        lights_onoff(ELed1B, 1);
        vTaskDelay(300);
        lights_onoff(ELed1B, 0);
        vTaskDelay(700);
    }
}


