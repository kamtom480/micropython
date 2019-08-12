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

#define PIN_NOT_ASSIGNED    (0xFF)

// Pin type
#define PINTYPE_MASK    0xc0
#define PINTYPE_DIGITAL 0x00
#define PINTYPE_LED     0x40
#define PINTYPE_ANALOG  0x80
#define _DIGITAL_PIN(N) (uint8_t)(PINTYPE_DIGITAL|(N))
#define _LED_PIN(N)     (uint8_t)(PINTYPE_LED    |(N))
#define _ANALOG_PIN(N)  (uint8_t)(PINTYPE_ANALOG |(N))
#define _PIN_OFFSET(P)  ((P)&~PINTYPE_MASK)

// Digital pins - 0x0N
#define PIN_D00     _DIGITAL_PIN(0)
#define PIN_D01     _DIGITAL_PIN(1)
#define PIN_D02     _DIGITAL_PIN(2)
#define PIN_D03     _DIGITAL_PIN(3)
#define PIN_D04     _DIGITAL_PIN(4)
#define PIN_D05     _DIGITAL_PIN(5)
#define PIN_D06     _DIGITAL_PIN(6)
#define PIN_D07     _DIGITAL_PIN(7)
#define PIN_D08     _DIGITAL_PIN(8)
#define PIN_D09     _DIGITAL_PIN(9)
#define PIN_D10     _DIGITAL_PIN(10)
#define PIN_D11     _DIGITAL_PIN(11)
#define PIN_D12     _DIGITAL_PIN(12)
#define PIN_D13     _DIGITAL_PIN(13)
#define PIN_D14     _DIGITAL_PIN(14)
#define PIN_D15     _DIGITAL_PIN(15)
#define PIN_D16     _DIGITAL_PIN(16)
#define PIN_D17     _DIGITAL_PIN(17)
#define PIN_D18     _DIGITAL_PIN(18)
#define PIN_D19     _DIGITAL_PIN(19)
#define PIN_D20     _DIGITAL_PIN(20)
#define PIN_D21     _DIGITAL_PIN(21)
#define PIN_D22     _DIGITAL_PIN(22)
#define PIN_D23     _DIGITAL_PIN(23)
#define PIN_D24     _DIGITAL_PIN(24)
#define PIN_D25     _DIGITAL_PIN(25)
#define PIN_D26     _DIGITAL_PIN(26)
#define PIN_D27     _DIGITAL_PIN(27)
#define PIN_D28     _DIGITAL_PIN(28)

// LED - 0x4N
#define PIN_LED0    _LED_PIN(0)
#define PIN_LED1    _LED_PIN(1)
#define PIN_LED2    _LED_PIN(2)
#define PIN_LED3    _LED_PIN(3)

// Analog pins - 0x8N
#define PIN_A0      _ANALOG_PIN(0)
#define PIN_A1      _ANALOG_PIN(1)
#define PIN_A2      _ANALOG_PIN(2)
#define PIN_A3      _ANALOG_PIN(3)
#define PIN_A4      _ANALOG_PIN(4)
#define PIN_A5      _ANALOG_PIN(5)

#define PIN_PWM_0   PIN_D06
#define PIN_PWM_1   PIN_D05
#define PIN_PWM_2   PIN_D09
#define PIN_PWM_3   PIN_D03

#define LOW         0x0
#define HIGH        0x1
#define INPUT       0x0
#define OUTPUT      0x1