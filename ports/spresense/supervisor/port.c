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
#include <sys/boardctl.h>

#include "supervisor/port.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/analogio/AnalogIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PWMOut.h"

safe_mode_t port_init(void) {
    boardctl(BOARDIOC_INIT, 0);
    return NO_SAFE_MODE;
}

void reset_cpu(void) {
    boardctl(BOARDIOC_RESET, 0);
}

void reset_port(void) {
#if CIRCUITPY_ANALOGIO
    analogin_reset();
#endif
#if CIRCUITPY_PULSEIO
    pulseout_reset();
    pwmout_reset();
#endif

    reset_all_pins();
}

void reset_board(void) {
}

void reset_to_bootloader(void) {
}

extern uint32_t _ebss;

// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}
