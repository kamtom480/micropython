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

#include <fcntl.h>

#include <arch/chip/scu.h>
#include <arch/chip/adc.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint8_t adc_num;
    int adc_fd;
} machine_adc_obj_t;

STATIC const machine_adc_obj_t machine_adc_obj[] = {
    {{&machine_adc_type}, 0, -1},
    {{&machine_adc_type}, 1, -1},
    {{&machine_adc_type}, 2, -1},
    {{&machine_adc_type}, 3, -1},
    {{&machine_adc_type}, 4, -1},
    {{&machine_adc_type}, 5, -1},
};

STATIC const char* machine_adc_devpath[] = {
    { "/dev/lpadc0" },
    { "/dev/lpadc1" },
    { "/dev/lpadc2" },
    { "/dev/lpadc3" },
    { "/dev/hpadc0" },
    { "/dev/hpadc1" },
};

STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    // get adc id
    mp_int_t adc_num = mp_obj_get_int(args[0]);
    if (adc_num > 6) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Analog%d does not exist", adc_num));
    }

    machine_adc_obj_t *self;
    self = &machine_adc_obj[adc_num];

    if (self->adc_fd < 0) {
        self->adc_fd = open(machine_adc_devpath[adc_num], O_RDONLY);
        if (self->adc_fd < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to open adc device"));
        }

        // SCU FIFO overwrite
        if (ioctl(self->adc_fd, SCUIOC_SETFIFOMODE, 1) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to set SCU FIFO mode"));
        }

        // ADC FIFO size
        if (ioctl(self->adc_fd, ANIOC_CXD56_FIFOSIZE, 2) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to set ADC FIFO size"));
        }

        // start ADC
        if (ioctl(self->adc_fd, ANIOC_CXD56_START, 0) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to start ADC"));
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADC Pin %u", self->adc_num);
}

STATIC mp_obj_t machine_adc_read(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int16_t sample;
    int nbytes;

    do {
        nbytes = read(self->adc_fd, &sample, sizeof(sample));
        if (nbytes < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to read analog"));
            break;
        }
    } while (nbytes == 0);

    sample = (sample - SHRT_MIN) * (1023 - 0) / (SHRT_MAX - SHRT_MIN) + 0;

    return MP_OBJ_NEW_SMALL_INT(sample);
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_obj, machine_adc_read);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_adc_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = machine_adc_print,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_t)&machine_adc_locals_dict,
};
