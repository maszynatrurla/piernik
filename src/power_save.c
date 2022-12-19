/*
 * power_save.c
 *
 *  Created on: 26 mar 2022
 *      Author: andrzej
 */

#include "power_save.h"
#include "pytka.h"

#include <stdio.h>

#include "pico/critical_section.h"
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/xosc.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/sync.h"
#include "hardware/structs/scb.h"
#include "hardware/structs/rosc.h"

typedef enum {
    DORMANT_SOURCE_NONE,
    DORMANT_SOURCE_XOSC,
    DORMANT_SOURCE_ROSC
} dormant_source_t;

static dormant_source_t _dormant_source;
static int awoken = 1;
static int button;
static int rain;

inline static void rosc_clear_bad_write(void) {
    hw_clear_bits(&rosc_hw->status, ROSC_STATUS_BADWRITE_BITS);
}

inline static bool rosc_write_okay(void) {
    return !(rosc_hw->status & ROSC_STATUS_BADWRITE_BITS);
}

inline static void rosc_write(io_rw_32 *addr, uint32_t value) {
    rosc_clear_bad_write();
    assert(rosc_write_okay());
    *addr = value;
    assert(rosc_write_okay());
};

static void rosc_disable(void) {
    uint32_t tmp = rosc_hw->ctrl;
    tmp &= (~ROSC_CTRL_ENABLE_BITS);
    tmp |= (ROSC_CTRL_ENABLE_VALUE_DISABLE << ROSC_CTRL_ENABLE_LSB);
    rosc_write(&rosc_hw->ctrl, tmp);
    // Wait for stable to go away
    while(rosc_hw->status & ROSC_STATUS_STABLE_BITS);
}

static void sleep_run_from_dormant_source(dormant_source_t dormant_source) {
    _dormant_source = dormant_source;

    // FIXME: Just defining average rosc freq here.
    uint src_hz = (dormant_source == DORMANT_SOURCE_XOSC) ? XOSC_MHZ * MHZ : 6.5 * MHZ;
    uint clk_ref_src = (dormant_source == DORMANT_SOURCE_XOSC) ?
                       CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC :
                       CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH;

    // CLK_REF = XOSC or ROSC
    clock_configure(clk_ref,
                    clk_ref_src,
                    0, // No aux mux
                    src_hz,
                    src_hz);

    // CLK SYS = CLK_REF
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
                    0, // Using glitchless mux
                    src_hz,
                    src_hz);

    // CLK USB = 0MHz
    clock_stop(clk_usb);

    // CLK ADC = 0MHz
    clock_stop(clk_adc);

    // CLK RTC = ideally XOSC (12MHz) / 256 = 46875Hz but could be rosc
    uint clk_rtc_src = (dormant_source == DORMANT_SOURCE_XOSC) ?
                       CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC :
                       CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH;

    clock_configure(clk_rtc,
                    0, // No GLMUX
                    clk_rtc_src,
                    src_hz,
                    46875);

    // CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select and enable
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    src_hz,
                    src_hz);

    pll_deinit(pll_sys);
    pll_deinit(pll_usb);

    // Assuming both xosc and rosc are running at the moment
    if (dormant_source == DORMANT_SOURCE_XOSC) {
        // Can disable rosc
        rosc_disable();
    } else {
        // Can disable xosc
        xosc_disable();
    }

    // Reconfigure uart with new clocks
    setup_default_uart();
}

static void wakeup_callback(void)
{
    awoken = 1;
}

#define PIN_BUTTON PIN_WAKEUP_GP_I_BUTTON

static void wakeup_pin_callback(uint gpio, uint32_t events)
{
    if (gpio == PIN_BUTTON)
    {
        button = 1;
    }
}


void power_save_init(void)
{
    gpio_init(PIN_BUTTON);
    gpio_set_dir(PIN_BUTTON, GPIO_IN);
    gpio_set_irq_enabled(PIN_BUTTON, 4, true);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON, 4, true, wakeup_pin_callback);
}

static EWakeupReason_t power_save_sleep_internal(uint32_t seconds)
{
    critical_section_t crit_sec;

    critical_section_init(&crit_sec);

    uint32_t scb_orig = scb_hw->scr;
    uint32_t clock0_orig = clocks_hw->sleep_en0;
    uint32_t clock1_orig = clocks_hw->sleep_en1;
    datetime_t dt;

    uart_default_tx_wait_blocking();
    sleep_run_from_dormant_source(DORMANT_SOURCE_XOSC);

    rtc_get_datetime(&dt);
    dt.dotw = -1;
    uint32_t acc = dt.sec + seconds;
    dt.sec = (acc % 60);
    acc = acc / 60;
    if (acc == 0) goto alarm_set;

    acc += dt.min;
    dt.min = (acc % 60);
    acc = acc / 60;
    if (acc == 0) goto alarm_set;

    acc += dt.hour;
    dt.hour = (acc % 24);
    acc = acc / 24;
    if (acc == 0) goto alarm_set;

    acc += (dt.day - 1);
    switch (dt.month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        dt.day = 1 + acc % 31;
        acc = acc / 31;
        break;
    case 2:
        if (0 == (dt.year & 3))
        {
            dt.day = 1 + acc % 29;
            acc = acc / 29;
            break;
        }
        else
        {
            dt.day = 1 + acc % 28;
            acc = acc / 28;
            break;
        }
        break;
    default:
        dt.day = 1 + acc % 30;
        acc = acc / 30;
        break;
    }
    if (acc == 0) goto alarm_set;

    acc += (dt.month - 1);
    dt.month = acc % 12 + 1;
    acc = acc / 12;
    dt.year += acc;


alarm_set:
    critical_section_enter_blocking(&crit_sec);
    awoken = 0;
    button = 0;
    rain = 0;

    // Turn off all clocks when in sleep mode except for RTC
    clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
    clocks_hw->sleep_en1 = 0x0;

    rtc_set_alarm(&dt, wakeup_callback);

    uint save = scb_hw->scr;
    // Enable deep sleep at the proc
    scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

    critical_section_exit(&crit_sec);

    // Go to sleep
    __wfi();


    //Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);

    //reset procs back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    //reset clocks
    clocks_init();
    setup_default_uart();

    rtc_disable_alarm();

    critical_section_deinit(&crit_sec);

    if (awoken)
    {
        return EWakeup_RTC;
    }
    if (button)
    {
        return EWakeup_Button;
    }

    return EWakeup_Unknown;
}

EWakeupReason_t power_save_sleep(uint32_t seconds)
{
        return power_save_sleep_internal(seconds);
}

