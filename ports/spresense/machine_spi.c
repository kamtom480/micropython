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

#include <nuttx/spi/spi.h>

#include "py/runtime.h"
#include "extmod/machine_spi.h"
#include "py/mphal.h"
#include "modmachine.h"

STATIC uint8_t machine_hw_spi_reverse_bits(uint8_t data)
{
    return (((data & 0x01) << 7) | ((data & 0x02) << 5) | ((data & 0x04) << 3) | ((data & 0x08) << 1) |
            ((data & 0x10) >> 1) | ((data & 0x20) >> 3) | ((data & 0x40) >> 5) | ((data & 0x80) >> 7));
}

#define lsb2msb(d)  machine_hw_spi_reverse_bits(d) // Reverse bits (LSB to MSB)
#define msb2lsb(d)  machine_hw_spi_reverse_bits(d) // Reverse bits (MSB to LSB)

typedef struct _machine_hw_spi_obj_t {
    mp_obj_base_t base;
    FAR struct spi_dev_s* spi_dev;
    int8_t port;
    int32_t baudrate;
    int8_t polarity;
    int8_t phase;
    int8_t bits;
    int8_t firstbit;
} machine_hw_spi_obj_t;

STATIC machine_hw_spi_obj_t machine_hw_spi_obj[2];

STATIC void machine_hw_spi_init_internal(
    machine_hw_spi_obj_t    *self,
    int8_t                  port,
    int32_t                 baudrate,
    int8_t                  polarity,
    int8_t                  phase,
    int8_t                  bits,
    int8_t                  firstbit) {
    int8_t mode;

    if (port != -1  && port != self->port) {
        self->port = port;
    }

    if (!self->spi_dev) {
        self->spi_dev = cxd56_spibus_initialize(port);
        if (!self->spi_dev) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to initialize SPI bus"));
        }

        self->firstbit = MICROPY_PY_MACHINE_SPI_MSB;
    }

    if (baudrate != -1 && baudrate != self->baudrate) {
        self->baudrate = baudrate;
        SPI_SETFREQUENCY(self->spi_dev, baudrate);
    }

    if (polarity != -1 && bits != self->bits) {
        self->polarity = polarity;
    }

    if (phase != -1 && bits != self->bits) {
        self->phase =  phase;
    }

    if (self->polarity == 0) {
        if (self->phase == 0) {
            mode = SPIDEV_MODE0;
        } else {
            mode = SPIDEV_MODE1;
        }
    } else {
        if (self->phase == 0) {
            mode = SPIDEV_MODE2;
        } else {
            mode = SPIDEV_MODE3;
        }
    }

    SPI_SETMODE(self->spi_dev, mode);

    if (bits != -1 && bits != self->bits) {
        self->bits = bits;
        SPI_SETBITS(self->spi_dev, bits);
    }

    if (firstbit != -1 && firstbit != self->firstbit) {
        self->firstbit = firstbit;
    }
}

STATIC void machine_hw_spi_deinit(mp_obj_base_t *self_in) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
}

STATIC void machine_hw_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->spi_dev) {
        mp_raise_msg(&mp_type_OSError, "transfer on deinitialized SPI");
        return;
    }

    if (self->firstbit == MICROPY_PY_MACHINE_SPI_LSB) {
        uint8_t *p = (uint8_t *)src;
        for (size_t i = 0; i < len; ++i, ++p) {
            *p = lsb2msb(*p);
        }
    }

    SPI_EXCHANGE(self->spi_dev, src, dest, len);

    if (self->firstbit == MICROPY_PY_MACHINE_SPI_LSB) {
        uint8_t *p = (uint8_t *)dest;
        for (size_t i = 0; i < len; ++i, ++p) {
            *p = msb2lsb(*p);
        }
    }
}

/******************************************************************************/
// MicroPython bindings for hw_spi

STATIC void machine_hw_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(port=%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u)",
              self->port, self->baudrate, self->polarity,
              self->phase, self->bits, self->firstbit);
}

STATIC void machine_hw_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    enum { ARG_port, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_port,     MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    machine_hw_spi_init_internal(self, args[ARG_port].u_int, args[ARG_baudrate].u_int,
                                 args[ARG_polarity].u_int, args[ARG_phase].u_int, args[ARG_bits].u_int,
                                 args[ARG_firstbit].u_int);
}

mp_obj_t machine_hw_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_port, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_port,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hw_spi_obj_t *self;
    if (args[ARG_port].u_int == SPIDEV_PORT_4) {
        self = &machine_hw_spi_obj[0];
    } else {
        self = &machine_hw_spi_obj[1];
    }
    self->base.type = &machine_hw_spi_type;

    // here we would check the sck/mosi/miso pins and configure them, but it's not implemented
    if (args[ARG_sck].u_obj != MP_OBJ_NULL
        || args[ARG_mosi].u_obj != MP_OBJ_NULL
        || args[ARG_miso].u_obj != MP_OBJ_NULL) {
        mp_raise_ValueError("explicit choice of sck/mosi/miso is not implemented");
    }

    machine_hw_spi_init_internal(
        self,
        args[ARG_port].u_int,
        args[ARG_baudrate].u_int,
        args[ARG_polarity].u_int,
        args[ARG_phase].u_int,
        args[ARG_bits].u_int,
        args[ARG_firstbit].u_int);

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_machine_spi_p_t machine_hw_spi_p = {
    .init = machine_hw_spi_init,
    .deinit = machine_hw_spi_deinit,
    .transfer = machine_hw_spi_transfer,
};

const mp_obj_type_t machine_hw_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hw_spi_print,
    .make_new = machine_hw_spi_make_new,
    .protocol = &machine_hw_spi_p,
    .locals_dict = (mp_obj_dict_t *) &mp_machine_spi_locals_dict,
};
