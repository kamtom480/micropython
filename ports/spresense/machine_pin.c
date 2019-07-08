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

#include <stdio.h>
#include <string.h>

#include <arch/board/board.h>
#include <arch/chip/pin.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "extmod/virtpin.h"

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    mp_hal_pin_obj_t pin;
} machine_pin_obj_t;

STATIC const machine_pin_obj_t machine_pin_obj[] = {
    {{&machine_pin_type}, PIN_D00, PIN_UART2_RXD},
    {{&machine_pin_type}, PIN_D01, PIN_UART2_TXD},
    {{&machine_pin_type}, PIN_D02, PIN_HIF_IRQ_OUT},
    {{&machine_pin_type}, PIN_D03, PIN_PWM3},
    {{&machine_pin_type}, PIN_D04, PIN_SPI2_MOSI},
    {{&machine_pin_type}, PIN_D05, PIN_PWM1},
    {{&machine_pin_type}, PIN_D06, PIN_PWM0},
    {{&machine_pin_type}, PIN_D07, PIN_SPI3_CS1_X},
    {{&machine_pin_type}, PIN_D08, PIN_SPI2_MISO},
    {{&machine_pin_type}, PIN_D09, PIN_PWM2},
    {{&machine_pin_type}, PIN_D10, PIN_SPI4_CS_X},
    {{&machine_pin_type}, PIN_D11, PIN_SPI4_MOSI},
    {{&machine_pin_type}, PIN_D12, PIN_SPI4_MISO},
    {{&machine_pin_type}, PIN_D13, PIN_SPI4_SCK},
    {{&machine_pin_type}, PIN_D14, PIN_I2C0_BDT},
    {{&machine_pin_type}, PIN_D15, PIN_I2C0_BCK},
    {{&machine_pin_type}, PIN_D16, PIN_EMMC_DATA0},
    {{&machine_pin_type}, PIN_D17, PIN_EMMC_DATA1},
    {{&machine_pin_type}, PIN_D18, PIN_I2S0_DATA_OUT},
    {{&machine_pin_type}, PIN_D19, PIN_I2S0_DATA_IN},
    {{&machine_pin_type}, PIN_D20, PIN_EMMC_DATA2},
    {{&machine_pin_type}, PIN_D21, PIN_EMMC_DATA3},
    {{&machine_pin_type}, PIN_D22, PIN_SEN_IRQ_IN},
    {{&machine_pin_type}, PIN_D23, PIN_EMMC_CLK},
    {{&machine_pin_type}, PIN_D24, PIN_EMMC_CMD},
    {{&machine_pin_type}, PIN_D25, PIN_I2S0_LRCK},
    {{&machine_pin_type}, PIN_D26, PIN_I2S0_BCK},
    {{&machine_pin_type}, PIN_D27, PIN_UART2_CTS},
    {{&machine_pin_type}, PIN_D28, PIN_UART2_RTS},
    {{&machine_pin_type}, PIN_LED0, PIN_I2S1_BCK},
    {{&machine_pin_type}, PIN_LED1, PIN_I2S1_LRCK},
    {{&machine_pin_type}, PIN_LED2, PIN_I2S1_DATA_IN},
    {{&machine_pin_type}, PIN_LED3, PIN_I2S1_DATA_OUT},
};

STATIC mp_hal_pin_obj_t pin_convert(mp_hal_pin_obj_t pin)
{
    mp_hal_pin_obj_t new_pin;
    machine_pin_obj_t *pin_obj;
    if (0 <= pin && pin < MP_ARRAY_SIZE(machine_pin_obj)) {
        pin_obj = (machine_pin_obj_t*)&machine_pin_obj[pin];
        new_pin = pin_obj->pin;
    } else {
        new_pin = PIN_NOT_ASSIGNED;
    }
    
    return new_pin;
}

void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    board_gpio_write(_pin, -1);
    board_gpio_config(_pin, 0, true, true, PIN_FLOAT);
}

void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    board_gpio_write(_pin, -1);
    board_gpio_config(_pin, 0, false, true, PIN_FLOAT);
}

int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    return board_gpio_read(_pin);
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, int value) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    value = (value == LOW ? LOW : HIGH);

    board_gpio_write(_pin, value);
}

void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    board_gpio_write(_pin, -1);
    board_gpio_config(_pin, 0, false, true, PIN_FLOAT);
    board_gpio_write(_pin, LOW);
}

void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    board_gpio_write(_pin, -1);
    board_gpio_config(_pin, 0, true, true, PIN_PULLUP);
}

void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    mp_hal_pin_obj_t _pin = pin_convert(pin);
    if (_pin == PIN_NOT_ASSIGNED) {
        mp_raise_ValueError("invalid pin");
    }

    board_gpio_write(_pin, -1);
    board_gpio_config(_pin, 0, false, true, PIN_FLOAT);
    board_gpio_write(_pin, LOW);
}

uint8_t machine_pin_get_id(mp_obj_t pin_in) {
    if (mp_obj_get_type(pin_in) != &machine_pin_type) {
        mp_raise_ValueError("expecting a pin");
    }
    machine_pin_obj_t *self = pin_in;
    return self->id;
}

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);;
    mp_printf(print, "Pin(%u)", self->id);
}

// pin.init(mode, pull=None, *, value)
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t _pin = pin_convert(self->id);
    if (_pin == PIN_NOT_ASSIGNED)
        return mp_const_none;

    bool input;
    bool highdrive = true; // always use high drive current
    int  pull;

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t pin_io_mode = mp_obj_get_int(args[ARG_mode].u_obj);
        input = (pin_io_mode == INPUT ? true : false);
    }

    // configure pull
    if (args[ARG_pull].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        int mode = 0;
        if (args[ARG_pull].u_obj != mp_const_none) {
            mode = mp_obj_get_int(args[ARG_pull].u_obj);
        }
        pull = (mode == PIN_PULLUP ? PIN_PULLUP : PIN_PULLDOWN);
    }

    // disable output
    board_gpio_write(_pin, -1);
    board_gpio_config(_pin, 0, input, highdrive, pull);

    // set initial value (do this before configuring mode/pull)
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        mp_hal_pin_write(self->id, mp_obj_is_true(args[ARG_value].u_obj));
    }

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted pin object
    int wanted_pin = mp_obj_get_int(args[0]);
    const machine_pin_obj_t *self = NULL;
    if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj)) {
        self = (machine_pin_obj_t*)&machine_pin_obj[wanted_pin];
    }
    if (self == NULL || self->base.type == NULL) {
        mp_raise_ValueError("invalid pin");
    }

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self->id));
    } else {
        // set pin
        mp_hal_pin_write(self->id, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.off()
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_write(self->id, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_write(self->id, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(PIN_PULLUP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(PIN_PULLDOWN) },
};

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
            return mp_hal_pin_read(self->id);
        }
        case MP_PIN_WRITE: {
            mp_hal_pin_write(self->id, arg);
            return 0;
        }
    }
    return -1;
}

STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC const mp_pin_p_t pin_pin_p = {
  .ioctl = pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = machine_pin_print,
    .make_new = mp_pin_make_new,
    .call = machine_pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_t)&machine_pin_locals_dict,
};