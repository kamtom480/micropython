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
#include <fcntl.h>

#include <nuttx/drivers/pwm.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t pwm_num;
    uint8_t pwm_pin;
    uint8_t active;
    int pwm_fd;
    struct pwm_info_s info;
} machine_pwm_obj_t;

STATIC const machine_pwm_obj_t machine_pwm_obj[] = {
    {{&machine_pwm_type}, 0, PIN_PWM_0, 0, -1, {.frequency = 0, .duty = 0}},
    {{&machine_pwm_type}, 1, PIN_PWM_1, 0, -1, {.frequency = 0, .duty = 0}},
    {{&machine_pwm_type}, 2, PIN_PWM_2, 0, -1, {.frequency = 0, .duty = 0}},
    {{&machine_pwm_type}, 3, PIN_PWM_3, 0, -1, {.frequency = 0, .duty = 0}},
};

STATIC const char* machine_pwm_devpath[] = {
    { "/dev/pwm0" },
    { "/dev/pwm1" },
    { "/dev/pwm2" },
    { "/dev/pwm3" },
};

STATIC void machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%u", self->pwm_num);
    if (self->active) {
        mp_printf(print, ", freq=%u, duty=%u", self->info.frequency, self->info.duty);
    }
    mp_printf(print, ")");
}

STATIC void machine_pwm_init_helper(machine_pwm_obj_t *self,
        size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->active = 1;

    if (self->pwm_fd < 0) {
        self->pwm_fd = open(machine_pwm_devpath[self->pwm_num], O_RDONLY);
        if (self->pwm_fd < 0) {
            mp_raise_ValueError("Failed to open pwm device");
        }
    }

    if (args[ARG_freq].u_int != -1) {
        self->info.frequency = args[ARG_freq].u_int;
    }

    if (args[ARG_duty].u_int != -1) {
        self->info.duty = args[ARG_duty].u_int;
    }

    if (self->active == 0 && self->info.frequency > 0 && self->info.duty > 0) {
        if (ioctl(self->pwm_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&self->info)) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to set characteristics"));
            return;
        }

        if (ioctl(self->pwm_fd, PWMIOC_START, 0) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to start PWM"));
            return;
        }
    }
}

STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    uint8_t pin_id = machine_pin_get_id(args[0]);

    machine_pwm_obj_t *self = NULL;
    arrayForEach(machine_pwm_obj, i) {
        if (pin_id == machine_pwm_obj[i].pwm_pin) {
            self = &machine_pwm_obj[i];
        }
    }

    if (!self) {
        mp_raise_ValueError("PWM is not available on this pin"); 
    }

    // start the PWM running for this channel
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_pwm_init(size_t n_args,
        const mp_obj_t *args, mp_map_t *kw_args) {
    machine_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pwm_init_obj, 1, machine_pwm_init);

STATIC mp_obj_t machine_pwm_deinit(mp_obj_t self_in) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_deinit_obj, machine_pwm_deinit);

STATIC mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(self->info.frequency);
    }

    // set
    self->info.frequency = mp_obj_get_int(args[1]);
    if (self->active == 0 && self->info.frequency > 0 && self->info.duty > 0) {
        if (ioctl(self->pwm_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&self->info)) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to set characteristics"));
            return;
        }

        if (ioctl(self->pwm_fd, PWMIOC_START, 0) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to start PWM"));
            return;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freq_obj, 1, 2, machine_pwm_freq);

STATIC mp_obj_t machine_pwm_duty(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int duty;

    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(self->info.duty);
    }

    // set
    self->info.duty = mp_obj_get_int(args[1]);
    if (self->active == 0 && self->info.frequency > 0 && self->info.duty > 0) {
        if (ioctl(self->pwm_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&self->info)) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to set characteristics"));
            return;
        }

        if (ioctl(self->pwm_fd, PWMIOC_START, 0) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to start PWM"));
            return;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_obj, 1, 2, machine_pwm_duty);

STATIC const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&machine_pwm_duty_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

const mp_obj_type_t machine_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = machine_pwm_print,
    .make_new = machine_pwm_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_pwm_locals_dict,
};
