/*
 * pytka.h
 *
 *  Created on: 25 mar 2022
 *      Author: andrzej
 */

#ifndef PYTKA_H_
#define PYTKA_H_


/*
 *  Raspberry pi pico pinout
 *
 *                     +------+ USB +-----+
 *  UART0_TX <--  GP0 -|- 1   +-----+  40-|- VBAT
 *  UART0_RX -->  GP1 -|- 2            39-|- VSYS
 *                GND -|- 3            38-|- GND
 *  LIGHT_MN <--  GP2 -|- 4            37-|- EN
 *  LIGHT_WC <--  GP3 -|- 5            36-|- 3.3V
 *       L3R <--  GP4 -|- 6            35-|- ADC_VREF
 *       L3G <--  GP5 -|- 7            34-|- GP28       <-- SENS_L
 *                GND -|- 8            33-|- AGND
 *       L3B <--  GP6 -|- 9     TOP    32-|- GP27
 *       L2R <--  GP7 -|-10            31-|- GP26
 *  UART1_TX <--  GP8 -|-11            30-|- RUN/RESET
 *  UART1_RX -->  GP9 -|-12            29-|- GP22       <-- BUTTON
 *                GND -|-13            28-|- GND
 *       L2G <-- GP10 -|-14            27-|- GP21
 *       L2B <-- GP11 -|-15            26-|- GP20
 *       L1R <-- GP12 -|-16            25-|- GP19
 *       L1G <-- GP13 -|-17            24-|- GP18
 *                GND -|-18            23-|- GND
 *       L1B <-- GP14 -|-19            22-|- GP17
 *   MP3P_ON <-- GP15 -|-20    hdbg    21-|- GP16
 *                     +------------------+
 *
 *  ==Interface list==
 *
 *  UART1 : MP3 player
 *
 *
 *  ==Pin list==
 *
 *  Debug:
 *  1  UART_TX,
 *  2  UART_RX
 *
 *  MP3 player:
 *  11 UART1_TX
 *  12 UART1_RX
 *  20 MP3P_ON
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
 * Twilight sensor:
 *  34 SENS_L
 *
 * Wake button:
 *  29 BUTTON
 *
 */

/* pin naming convention:
 *
 * PIN_<TARGET-DEVICE>_<UC_PERIPHERAL>_<DIRECTION>_<NAME>
 * */

/* GPIO connected to MP3 player*/
#define PIN_MP3P_GP_O_ON    15
#define PIN_MP3P_UART_O_TX   8
#define PIN_MP3P_UART_I_RX   9

/* MP3 player UART interface*/
#define UART_IF_MP3P    uart1

/* GPIO connected to LEDs*/
#define PIN_LIGHT_GP_O_N_MAIN 2
#define PIN_LIGHT_GP_O_WC     3

#define PIN_LIGHT_GP_O_L3R      4
#define PIN_LIGHT_GP_O_L3G      5
#define PIN_LIGHT_GP_O_L3B      6
#define PIN_LIGHT_GP_O_L2R      7
#define PIN_LIGHT_GP_O_L2G     10
#define PIN_LIGHT_GP_O_L2B     11
#define PIN_LIGHT_GP_O_L1R     12
#define PIN_LIGHT_GP_O_L1G     13
#define PIN_LIGHT_GP_O_L1B     14

/* Analog input GPIOs */
#define PIN_VD_ADC_I_SENSL   28
#define PIN_VD_ADC_I_VSYS    29
#define PIN_TEMP2_GP_IO_DATA  2

/* ADC channel inputs */
#define ADC_INPUT_SENSL        2
#define ADC_INPUT_VSYS         3
#define ADC_INPUT_INTERNAL_T   4

/* Wakeup button GPIO */
#define PIN_WAKEUP_GP_I_BUTTON  22

#endif /* PYTKA_H_ */
