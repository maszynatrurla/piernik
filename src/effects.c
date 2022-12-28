/*
 * effects.c
 *
 *  Created on: 17 gru 2022
 *      Author: andrzej
 */
#include "effects.h"
#include "lights.h"
#include "plaja.h"
#include "pytka.h"

#include <stdio.h>

#include "hardware/timer.h"
#include "hardware/gpio.h"

/* Pause time between effect phases [ms].
 * In general equal to the time length sound effect. */

#define WC_ENTER_TLEN   6000    /**< Length of door creaking. */
#define WC_PEE_TLEN     13000   /**< Length of stream sound. */
#define WC_FLUSH_TLEN   5000    /**< Length of flushing sound. */
#define WC_EXIT_TLEN    5000    /**< Pause until light is turned off. */

#define DISCO_TLEN      60000   /**< How long disco effect is taking place. */

#define ALARM_TLEN      15000   /**< How long alarm effect is taking place. */


#define PIN_BUTTON PIN_WAKEUP_GP_I_BUTTON

/**
 * Substates of go-to-WC effect.
 */
typedef enum
{
    ESWcIdle,
    ESWcEnter,
    ESWcPee,
    ESWcFlush,
    ESWcLightsOff,
} EWcState;

/**
 * Substates of disco effect.
 */
typedef enum
{
    ESDiscoIdle,
    ESDiscoOn,
} EDiscoState;

/**
 * Substates of alarm effect.
 */
typedef enum
{
    ESAlarmIdle,
    ESAlarmOn,
} EAlarmState;

/**
 * Effect state structure.
 */
static struct
{
    struct {
        EWcState state;
        uint32_t ts;
    } wc;

    struct {
        EDiscoState state;
        uint32_t ts;
    } disco;

    struct {
        EAlarmState state;
        uint32_t ts;
    } alarm;

    int request_wc;
    int request_disco;
    int request_alarm;

    int request_button;
    unsigned button_c;
} s_effect;

static uint32_t time_now_ms(void)
{
    return time_us_64() / 1000;
}

static void do_wc(void)
{
    switch (s_effect.wc.state)
    {
        case ESWcIdle:
        {
            lights_onoff(ELedWc, 1);
            plaja_action(PLAJA_CMD_PLAY_INDX, TRACK_DOOR_CREAK);
            s_effect.wc.ts = time_now_ms();
            s_effect.wc.state = ESWcEnter;
            break;
        }
        case ESWcEnter:
        {
            uint32_t tnow = time_now_ms();
            if (tnow > (s_effect.wc.ts + WC_ENTER_TLEN))
            {
                plaja_action(PLAJA_CMD_PLAY_INDX, TRACK_PEE);
                s_effect.wc.ts = tnow;
                s_effect.wc.state = ESWcPee;
            }
            break;
        }
        case ESWcPee:
        {
            uint32_t tnow = time_now_ms();
            if (tnow > (s_effect.wc.ts + WC_PEE_TLEN))
            {
                plaja_action(PLAJA_CMD_PLAY_INDX, TRACK_TOILET_FLUSH);
                s_effect.wc.ts = tnow;
                s_effect.wc.state = ESWcFlush;
            }
            break;
        }
        case ESWcFlush:
        {
            uint32_t tnow = time_now_ms();
            if (tnow > (s_effect.wc.ts + WC_FLUSH_TLEN))
            {
                s_effect.wc.ts = tnow;
                lights_onoff(ELedWc, 0);
                s_effect.wc.state = ESWcLightsOff;
            }
            break;
        }
        case ESWcLightsOff:
        {
            uint32_t tnow = time_now_ms();
            if (tnow > (s_effect.wc.ts + WC_EXIT_TLEN))
            {
                s_effect.wc.state = ESWcIdle;
                s_effect.request_wc = 0;
            }
            break;
        }
        default:
        {
            s_effect.wc.state = ESWcIdle;
            s_effect.request_wc = 0;
            break;
        }
    }
}

static void do_disco_lights(uint32_t tnow)
{
    uint32_t s = (tnow / 1000) % 3;
    uint32_t ms = tnow % 1000;

    if (ms < 200)
    {
        if (0 == s)
        {
            lights_onoff(ELed1B, 1);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 1);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
        else if (1 == s)
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 1);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 1);
            lights_onoff(ELed3R, 0);
        }
        else
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 1);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 1);
        }
    }
    else if (ms < 500)
    {
        if (0 == s)
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 1);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
        else if (1 == s)
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 1);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
        else
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 1);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
    }
    else if (ms < 800)
    {
        if (0 == s)
        {
            lights_onoff(ELed1B, 1);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 1);
            lights_onoff(ELed2B, 1);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 1);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
        else if (1 == s)
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 1);
            lights_onoff(ELed1R, 1);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 1);
            lights_onoff(ELed2R, 1);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
        else
        {
            lights_onoff(ELed1B, 1);
            lights_onoff(ELed1G, 1);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 1);
            lights_onoff(ELed2G, 1);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 0);
        }
    }
    else
    {
        if (0 == s)
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 1);
            lights_onoff(ELed2G, 0);
            lights_onoff(ELed2R, 1);
            lights_onoff(ELed3B, 1);
            lights_onoff(ELed3G, 0);
            lights_onoff(ELed3R, 1);
        }
        else if (1 == s)
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 1);
            lights_onoff(ELed2G, 1);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3B, 1);
            lights_onoff(ELed3G, 1);
            lights_onoff(ELed3R, 0);
        }
        else
        {
            lights_onoff(ELed1B, 0);
            lights_onoff(ELed1G, 0);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2B, 0);
            lights_onoff(ELed2G, 1);
            lights_onoff(ELed2R, 1);
            lights_onoff(ELed3B, 0);
            lights_onoff(ELed3G, 1);
            lights_onoff(ELed3R, 1);
        }
    }
}

static void stop_disco_lights(void)
{
    lights_onoff(ELed1B, 0);
    lights_onoff(ELed1G, 0);
    lights_onoff(ELed1R, 0);
    lights_onoff(ELed2B, 0);
    lights_onoff(ELed2G, 0);
    lights_onoff(ELed2R, 0);
    lights_onoff(ELed3B, 0);
    lights_onoff(ELed3G, 0);
    lights_onoff(ELed3R, 0);
}

static void do_disco(void)
{
    switch (s_effect.disco.state)
    {
        case ESDiscoIdle:
        {
            plaja_action(PLAJA_CMD_VOL_SET, 24);
            plaja_action(PLAJA_CMD_PLAY_INDX, TRACK_BANIA_U_CYGANA);
            lights_onoff(ELedMain, 0);
            s_effect.disco.ts = time_now_ms();
            s_effect.disco.state = ESDiscoOn;
            break;
        }
        case ESDiscoOn:
        {
            uint32_t tnow = time_now_ms();
            do_disco_lights(tnow);
            if (tnow > (s_effect.disco.ts + DISCO_TLEN))
            {
                plaja_action(PLAJA_CMD_STOP, 0);
                plaja_action(PLAJA_CMD_VOL_SET, 30);
                stop_disco_lights();
                s_effect.disco.state = ESDiscoIdle;
                s_effect.request_disco = 0;
            }
            break;
        }
        default:
        {
            s_effect.disco.state = ESDiscoIdle;
            s_effect.request_disco = 0;
            break;
        }
    }
}

static void do_alarm(void)
{
    switch (s_effect.alarm.state)
    {
    case ESAlarmIdle:
        plaja_action(PLAJA_CMD_VOL_SET, 24);
        plaja_action(PLAJA_CMD_PLAY_INDX, TRACK_CAR_ALARM);
        s_effect.alarm.ts = time_now_ms();
        s_effect.alarm.state = ESAlarmOn;
        break;
    case ESAlarmOn:
    {
        uint32_t tnow = time_now_ms();

        if (tnow > (s_effect.alarm.ts + ALARM_TLEN))
        {
            plaja_action(PLAJA_CMD_STOP, 0);
            plaja_action(PLAJA_CMD_VOL_SET, 30);
            lights_onoff(ELed1R, 0);
            lights_onoff(ELed2R, 0);
            lights_onoff(ELed3R, 0);
            s_effect.alarm.state = ESAlarmIdle;
            s_effect.request_alarm = 0;
        }
        else
        {
            uint32_t hms = tnow % 500;

            lights_onoff(ELed1R, (hms < 250) ? 1 : 0);
            lights_onoff(ELed2R, (hms < 250) ? 0 : 1);
            lights_onoff(ELed3R, (hms < 250) ? 1 : 0);
        }
        break;
    }
    default:
        s_effect.alarm.state = ESAlarmIdle;
        s_effect.request_alarm = 0;
        break;
    }
}

static void do_button(void)
{
    if (s_effect.request_alarm || s_effect.request_disco || s_effect.request_wc)
    {
        effects_deinit();
        stop_disco_lights();
        plaja_action(PLAJA_CMD_STOP, 0);
        plaja_action(PLAJA_CMD_VOL_SET, 30);
    }
    else
    {
        ++s_effect.button_c;

        switch(s_effect.button_c & 3)
        {
        case 0:
            lights_onoff(ELedMain, 0);
            lights_onoff(ELedWc, 0);
            break;
        case 1:
            lights_onoff(ELedMain, 1);
            lights_onoff(ELedWc, 0);
            break;
        case 2:
            lights_onoff(ELedMain, 1);
            lights_onoff(ELedWc, 1);
            break;
        case 3:
            lights_onoff(ELedMain, 0);
            lights_onoff(ELedWc, 1);
            break;
        default:
            break;
        }
    }
}

static void button_callback(uint gpio, uint32_t events)
{
    if (gpio == PIN_BUTTON)
    {
        s_effect.request_button = 1;
    }
}

void effects_init(void)
{
    s_effect.wc.state = ESWcIdle;
    s_effect.disco.state = ESDiscoIdle;
    s_effect.request_disco = 0;
    s_effect.request_wc = 0;
    s_effect.request_alarm = 0;

    s_effect.request_button = 0;
    s_effect.button_c = 0;
    gpio_init(PIN_BUTTON);
    gpio_set_dir(PIN_BUTTON, GPIO_IN);
    gpio_set_irq_enabled(PIN_BUTTON, 4, true);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON, 4, true, button_callback);
}

void effects_cycle(void)
{
    if (s_effect.request_button)
    {
        do_button();
        s_effect.request_button = 0;
    }
    if (s_effect.request_wc)
    {
        do_wc();
    }
    if (s_effect.request_disco)
    {
        do_disco();
    }
    if (s_effect.request_alarm)
    {
        do_alarm();
    }
}

void effects_deinit(void)
{
    effects_init();
}

void effects_go_to_wc(void)
{
    printf("E: WC");
    if (!s_effect.request_wc)
    {
        printf(" start\n");
        s_effect.request_wc = 1;
    }
    else
    {
        printf(" stop\n");
        s_effect.wc.state = ESWcIdle;
        s_effect.request_wc = 0;
    }
}

void effects_disco(void)
{
    printf("E: disco");
    if (!s_effect.request_disco)
    {
        printf(" start\n");
        s_effect.request_disco = 1;
    }
    else
    {
        printf(" stop\n");
        plaja_action(PLAJA_CMD_STOP, 0);
        plaja_action(PLAJA_CMD_VOL_SET, 30);
        stop_disco_lights();
        s_effect.disco.state = ESDiscoIdle;
        s_effect.request_disco = 0;
    }
}

void effects_alarm(void)
{
    printf("E: alarm");
    if (!s_effect.request_alarm)
    {
        printf(" start\n");
        s_effect.request_alarm = 1;
    }
    else
    {
        printf(" stop\n");
        plaja_action(PLAJA_CMD_STOP, 0);
        plaja_action(PLAJA_CMD_VOL_SET, 30);
        lights_onoff(ELed1R, 0);
        lights_onoff(ELed2R, 0);
        lights_onoff(ELed3R, 0);
        s_effect.alarm.state = ESAlarmIdle;
        s_effect.request_alarm = 0;
    }
}
