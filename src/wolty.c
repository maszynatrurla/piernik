/*
 * wolty.c
 *
 *  Created on: 25 mar 2022
 *      Author: andrzej
 */

#include "wolty.h"
#include "pytka.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"

/*
 *  Raspberry pi pico pinout
 *
 *       +------+ USB +-----+
 *      -|- 1   +-----+  40-|-
 *      -|- 2            39-|-
 *      -|- 3            38-|- GND
 *      -|- 4            37-|-
 *      -|- 5            36-|-
 *      -|- 6            35-|-
 *      -|- 7            34-|- GP28 <- ADC2 - SENS_L
 *      -|- 8            33-|-
 *      -|- 9     TOP    32-|-
 *      -|-10            31-|-
 *      -|-11            30-|-
 *      -|-12            29-|-
 *      -|-13            28-|-
 *      -|-14            27-|-
 *      -|-15            26-|-
 *      -|-16            25-|-
 *      -|-17            24-|-
 *      -|-18            23-|-
 *      -|-19            22-|-
 *      -|-20    hdbg    21-|-
 *       +------------------+
 */

#define PIN_SOLAR   PIN_VD_ADC_I_SENSL
#define PIN_VSYS    PIN_VD_ADC_I_VSYS

static float wolty_convert(uint16_t raw, float vref)
{
    float factor = vref / (1 << 12);
    return factor * ((float) raw);
}

void wolty_init(void)
{
    adc_init();
    adc_gpio_init(PIN_SOLAR);
    // adc_gpio_init(PIN_VSYS);
    // adc_set_temp_sensor_enabled(true);
}

void wolty_deinit(void)
{
    // adc_set_temp_sensor_enabled(false);
}

uint16_t wolty_read(EWolt_t what)
{
    switch (what)
    {
    case EWolt_Twilight:
        adc_select_input(ADC_INPUT_SENSL);
        break;
//    case EWolt_InternalTemp:
//        adc_select_input(ADC_INPUT_INTERNAL_T);
//        break;
//    case EWolt_VSYS:
//        adc_select_input(ADC_INPUT_VSYS);
//        break;
    default:
        return 0xFFFFU;
    }

    return adc_read();
}

//float wolty_internalTemperature(void)
//{
//    uint16_t raw = wolty_read(EWolt_InternalTemp);
//    float voltage = wolty_convert(raw, 3.3f);
//    return 27.f - (voltage - 0.706f) / 0.001721f;
//}
//
//float wolty_vsys(void)
//{
//    return wolty_convert(wolty_read(EWolt_VSYS), 3.3f) * 3.f;
//}

float wolty_twilight(void)
{
    return wolty_convert(wolty_read(EWolt_Twilight), 3.3f);
}
