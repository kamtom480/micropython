/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <cxd56_clock.h>
#include <nuttx/config.h>
#include <sys/boardctl.h>

#include "py/mphal.h"

#define DELAY_CORRECTION    (700)
#define DELAY_INTERVAL      (50)

void mp_hal_init(void) {
    boardctl(BOARDIOC_INIT, 0);
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;

    int r = read(0, &c, 1);
    (void)r;

    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    int r = write(1, str, len);
    (void)r;
}

mp_uint_t mp_hal_ticks_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

mp_uint_t mp_hal_ticks_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void mp_hal_set_interrupt_char(char c) {
    return;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return cxd56_get_cpu_baseclk();
}

void mp_hal_delay_ms(uint32_t ms) {
    if (ms) {
        while (DELAY_INTERVAL < ms) {
            mp_hal_delay_us(DELAY_INTERVAL * 1000);
            ms -= DELAY_INTERVAL;
        }
        mp_hal_delay_us(ms * 1000);
    }
}

void mp_hal_delay_us(uint32_t us) {
    if (us) {
        unsigned long long ticks = mp_hal_ticks_cpu() / 1000000L * us;
        if (ticks < DELAY_CORRECTION) return; // delay time already used in calculation

        ticks -= DELAY_CORRECTION;
        ticks /= 6;
        // following loop takes 6 cycles
        do {
            __asm__ __volatile__("nop");
        } while(--ticks);
    }
}
