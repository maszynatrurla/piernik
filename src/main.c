/*
 * main.c
 *
 *  Created on: 29 lis 2022
 *      Author: andrzej
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"

#include "FreeRTOS.h"
#include "task.h"

#include "pytka.h"
#include "lights.h"
#include "credentials.h"
#include "wolty.h"
#include "power_save.h"
#include "takie_czasy.h"
#include "plaja.h"
#include "effects.h"

#include "httpserver.h"

#define WIFI_TASK_PRIORITY              ( tskIDLE_PRIORITY + 6UL )
#define EFFECTS_TASK_PRIORITY           ( tskIDLE_PRIORITY + 4UL )
#define LIGHTS_TASK_PRIORITY            ( tskIDLE_PRIORITY + 5UL )

void vApplicationMallocFailedHook(void)
{
    printf("Nie ma takiego alokowania\n");
    while(1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{
    printf("What a loser: %s\n", pcTaskName);
    while (1);
}

void effects_task(void *params)
{
    (void) params;

    while (1)
    {
        effects_cycle();
        vTaskDelay(10);
    }
}

void lights_task(void *params)
{
    (void) params;

    while (1)
    {
        lights_cycle();
        vTaskDelay(1000);
    }
}

void wifi_task(void *params)
{
    extern struct netif * netif_default;
    uint32_t myIpA;

    int i;

    if ((cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND)))
    {
        printf("Fail #1\n");
        return;
    }

    printf("initiailized\n");

    cyw43_arch_enable_sta_mode();

    printf("sta mode initialized\n");

    for (i = 0; i < 3; ++i)
    {
        if (cyw43_arch_wifi_connect_timeout_ms(
                wifi_ssid, wifi_pass,
                CYW43_AUTH_WPA2_AES_PSK,
                20000))
        {
            printf("FAIL #2\n");
            vTaskDelay(500);
        }
        else
        {
            break;
        }
    }
    if (3 == i)
    {
        while (1) {
            vTaskDelay(1000);
        }
    }

    printf("tera jusz\n");

    for (i = 0; i < 4; ++i)
    {
        if (netif_is_link_up(netif_default) && ip4_addr_isany_val(*netif_ip4_addr(netif_default)))
        {
            break;
        }
        vTaskDelay(500);
    }

    myIpA = netif_ip4_addr(netif_default)->addr;
    lights_blink_ip_address(myIpA);


    http_server_netconn_init();
    bool server_ok;

    do {
        server_ok = http_server_cycle();
    }
    while (server_ok);
    printf("usral sie ten server\n");

    http_server_netconn_deinit();

    while (1) {
        vTaskDelay(1000);
    }

    cyw43_arch_deinit();
}

void vLaunch(void)
{
    TaskHandle_t task1, task2, task3;
    xTaskCreate(wifi_task, "WifiThread", 4096, NULL, WIFI_TASK_PRIORITY, &task1);
    xTaskCreate(effects_task, "EffectsThread", 256, NULL, EFFECTS_TASK_PRIORITY, &task2);
    xTaskCreate(lights_task, "LightsThread", 256, NULL, LIGHTS_TASK_PRIORITY, &task3);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main()
{
    stdio_init_all();
    setup_default_uart();
    lights_init();
    effects_init();
    plaja_init();
    czasy_init();

    printf("Piernik!\n");

    vLaunch();

    // test sleep and button wakeup
#if 0
    czasy_init();
    power_save_init();

    sleep_ms(5000);

    while (1)
    {
        EWakeupReason_t wkup;

        printf("Going to sleep. Wakeup in 60s.\n");
        wkup = power_save_sleep(32);
        switch (wkup)
        {
        case EWakeup_Unknown:
            printf(" awoken ???\n");
            break;
        case EWakeup_RTC:
            printf(" awoken by RTC\n");
            break;
        case EWakeup_Button:
            printf(" awoken by button\n");
            break;
        default:
            printf(" awoken !!!???\n");
            break;
        }

        gpled_onoff(PIN_LIGHT_GP_O_N_MAIN, 1);
        gpled_onoff(PIN_LIGHT_GP_O_WC, 1);
        sleep_ms(3000);
        gpled_onoff(PIN_LIGHT_GP_O_N_MAIN, 0);
        gpled_onoff(PIN_LIGHT_GP_O_WC, 0);
    }


    // test twilight sensor
    wolty_init();

    while (1)
    {
        float value = wolty_twilight();
        printf("Twilight sensor: %-4.2fV\n", value);
        sleep_ms(500);
    }

#endif

    // test pwm
#if 0
    gpio_set_function(PIN_G, GPIO_FUNC_PWM);
    unsigned slice = pwm_gpio_to_slice_num(PIN_G);
    unsigned channel = pwm_gpio_to_channel(PIN_G);

    pwm_set_wrap(slice, 200);
    pwm_set_chan_level(slice, channel, 0);
    pwm_set_enabled(slice, true);

    while (1)
    {
        for (int i = 0; i <= 200; i+=1)
        {
            pwm_set_chan_level(slice, channel, i);
            sleep_ms(2);
        }
        for (int i = 199; i >= 0; i-=1)
        {
            pwm_set_chan_level(slice, channel, i);
            sleep_ms(2);
        }
    }
#endif

    return 0;
}
