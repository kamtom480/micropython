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

#include <nuttx/i2c/i2c_master.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"

typedef struct _machine_hw_i2c_obj_t {
    mp_obj_base_t base;
    FAR struct i2c_master_s* i2c_dev;
    int8_t port;
    uint32_t freq;
} machine_hw_i2c_obj_t;

STATIC machine_hw_i2c_obj_t machine_hw_i2c_obj[1];

STATIC void machine_hw_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "I2C(%u, freq=%u)", self->port, self->freq);
}

STATIC void machine_hw_i2c_init(machine_hw_i2c_obj_t *self, int8_t port, uint32_t freq) {
    if (port != -1  && port != self->port) {
        self->port = port;
    }

    if (!self->i2c_dev) {
        self->i2c_dev = cxd56_i2cbus_initialize(port);
        if (!self->i2c_dev) {
            mp_raise_ValueError("Failed to initialize I2C bus");
        }
    }

    if (freq != -1 && freq != self->freq) {
        self->freq = freq;
    }
}

STATIC int machine_hw_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    machine_hw_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    struct i2c_msg_s msg;
    int ret;

    if (!self->i2c_dev) {
        mp_raise_msg(&mp_type_OSError, "transfer on deinitialized I2C");
    }

    int transfer_ret = 0;
    for (; n--; ++bufs) {
        size_t len = bufs->len;
        uint8_t *buf = bufs->buf;
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            // read bytes from the slave into the given buffer(s)
            msg.frequency = self->freq;
            msg.addr      = addr;
            msg.flags     = I2C_M_READ | (n == 0 ? 0 : I2C_M_NOSTOP);
            msg.buffer    = buf;
            msg.length    = len;
            ret = I2C_TRANSFER(self->i2c_dev, &msg, 1);
            if (ret != 0) {
                return ret;
            }
        } else {
            // write bytes from the given buffer(s) to the slave
            msg.frequency = self->freq;
            msg.addr      = addr;
            msg.flags     = (n == 0 ? 0 : I2C_M_NOSTOP);
            msg.buffer    = buf;
            msg.length    = len;
            ret = I2C_TRANSFER(self->i2c_dev, &msg, 1);
            if (ret < 0) {
                return ret;
            } else {
                // nack received, stop sending
                n = 0;
                break;
            }
            ++transfer_ret; // count the number of acks
        }
    }

    return transfer_ret;
}

mp_obj_t machine_hw_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    enum { ARG_port, ARG_scl, ARG_sda, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_port, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hw_i2c_obj_t *self;
    if (args[ARG_port].u_int == I2CDEV_PORT_0) {
        self = &machine_hw_i2c_obj[0];
    } else {
        mp_raise_ValueError("only I2C0 is available");
    }
    self->base.type = &machine_hw_i2c_type;

    if (args[ARG_freq].u_int != I2C_SPEED_STANDARD &&
        args[ARG_freq].u_int != I2C_SPEED_FAST) {
        mp_raise_ValueError("only 100kHz and 400kHz are supported"); 
    }

    // here we would check the scl/sda pins and configure them, but it's not supported
    if (args[ARG_scl].u_obj != MP_OBJ_NULL || args[ARG_sda].u_obj != MP_OBJ_NULL) {
        mp_raise_ValueError("explicit choice of scl/sda is not supported");
    }

    // initialise the I2C peripheral
    machine_hw_i2c_init(self, args[ARG_port].u_int, args[ARG_freq].u_int);

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_machine_i2c_p_t machine_hw_i2c_p = {
    .transfer = machine_hw_i2c_transfer,
};

const mp_obj_type_t machine_hw_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_hw_i2c_print,
    .make_new = machine_hw_i2c_make_new,
    .protocol = &machine_hw_i2c_p,
    .locals_dict = (mp_obj_dict_t *) &mp_machine_soft_i2c_locals_dict,
};
