/*
 * plaja.c
 *
 *  Created on: 10 gru 2022
 *      Author: andrzej
 */

#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "plaja.h"
#include "pytka.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#define PIN_ONOFF  PIN_MP3P_GP_O_ON
#define PIN_RX     PIN_MP3P_UART_I_RX
#define PIN_TX     PIN_MP3P_UART_O_TX

static void mp3read(unsigned timeout)
{
    unsigned passed = 0;
    unsigned read = 0;

    do
    {
        while (uart_is_readable(UART_IF_MP3P))
        {
            char r = uart_getc(UART_IF_MP3P);
            printf(" %02X", (unsigned) r);
            ++read;
        }
        sleep_ms(1);
        passed += 1U;
    } while (passed < timeout);

    if (read) {printf("\n");}
}

static uint16_t checksum(const uint8_t * data, uint32_t length)
{
    uint16_t sum = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        sum += data[i];
    }

    return 0xFFFFU - sum + 1U;
}

static void dbg_print_tx(const uint8_t * msg, uint32_t len)
{
    printf("TX:");
    for (uint32_t i = 0; i < len; ++i)
    {
        printf(" %02X", (unsigned) msg[i]);
    }
    printf("\n");
}

static void sendMessage(uint8_t command, uint16_t parameter,
        uint8_t feedback, uint32_t responseReadTime)
{
    uint8_t buffer [16];
    uint32_t len = 0;
    uint16_t cksum;

    buffer[len++] = 0x7EU;
    buffer[len++] = 0xFFU;
    buffer[len++] = 0x06U;
    buffer[len++] = command;
    buffer[len++] = feedback ? 0x01U : 0x00U;
    buffer[len++] = parameter >> 8;
    buffer[len++] = parameter & 0xFF;

    cksum = checksum(&buffer[1], len - 1U);

    buffer[len++] = cksum >> 8;
    buffer[len++] = cksum & 0xFF;
    buffer[len++] = 0xEFU;

    dbg_print_tx(buffer, len);
    uart_write_blocking(UART_IF_MP3P, buffer, len);

    if (responseReadTime)
    {
        mp3read(responseReadTime);
    }
}

void plaja_init(void)
{
    gpio_init(PIN_ONOFF);
    gpio_set_dir(PIN_ONOFF, GPIO_OUT);
    gpio_put(PIN_ONOFF, 0);

    uart_init(UART_IF_MP3P, 9600);
    gpio_set_function(PIN_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_RX, GPIO_FUNC_UART);
}

void plaja_set_on(bool enable)
{
    gpio_put(PIN_ONOFF, enable ? 1 : 0);

    if (enable)
    {
        mp3read(1000);
    }
}

void plaja_action(EPlayaRequest_t request, uint16_t parameter)
{
    sendMessage((uint8_t) request, parameter, 0, 300);
}


