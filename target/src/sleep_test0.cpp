/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ref: https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_dormant/hello_dormant.c
 */

#include "pico/stdlib.h"
#include "pico/sleep.h"

#include <stdio.h>

#define LED_PIN 25
#define WAKE_PIN 15

int main() {
    stdio_init_all();

    gpio_init(LED_PIN) ;
    gpio_set_dir(LED_PIN,  GPIO_OUT);
    gpio_set_dir(WAKE_PIN, GPIO_IN);

    gpio_put(LED_PIN, 1);
    sleep_ms(5000);

    printf("\n\nHello Dormant!\n");

    gpio_put(LED_PIN, 0);
    sleep_ms(500);

    printf("Switching to XOSC\n");

    uart_default_tx_wait_blocking();

    // UART will be reconfigured by sleep_run_from_xosc
    sleep_run_from_xosc();

    for (int i = 0; i < 10; ++i) {
        gpio_put(LED_PIN, 0);
        sleep_ms(50);
        gpio_put(LED_PIN, 1);
        sleep_ms(50);
    }

    // GG: this one is not printed in the serial console
    printf("Running from XOSC\n");
    stdio_flush();

    uart_default_tx_wait_blocking();

    // Go to sleep until we see a high edge on GPIO WAKE_PIN
    // GG: touch GPIO WAKE_PIN with a wire to wake up the pico
    sleep_goto_dormant_until_edge_high(WAKE_PIN);

    for (int i = 0; i < 20; ++i) {
        gpio_put(LED_PIN, 0);
        sleep_ms(25);
        gpio_put(LED_PIN, 1);
        sleep_ms(25);
    }

    // GG: this does nothing
    stdio_init_all();
    stdio_flush();

    uint i = 0;
    while (1) {
        // GG: we still don't see the prints, but the LED is blinking
        printf("XOSC awake %d\n", i++);
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);
    }

    return 0;
}
