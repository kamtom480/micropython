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

#include "pin_def.h"
#include <sys/types.h>

void mp_hal_init(void);
const char *get_full_path(const char *path);

#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_name(p) (p)
#define mp_hal_pin_obj_t uint8_t
#define mp_hal_get_pin_obj(o) machine_pin_get_id(o)
void mp_hal_pin_od_low(mp_hal_pin_obj_t pin);
void mp_hal_pin_od_high(mp_hal_pin_obj_t pin);
void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin);
void mp_hal_pin_input(mp_hal_pin_obj_t pin);
void mp_hal_pin_output(mp_hal_pin_obj_t pin);
int mp_hal_pin_read(mp_hal_pin_obj_t pin);
void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v);

#define mp_hal_delay_us_fast(p) mp_hal_delay_us(p)

#define SPIDEV_PORT_4     (4)
#define SPIDEV_PORT_5     (5)

#define I2CDEV_PORT_0     (0)

#define UART_2            (2)

#define arrayLength(a)      (sizeof(a) / sizeof((a)[0]))
#define arrayForEach(a, i)  for (unsigned int i = 0; i < arrayLength(a); ++i)
