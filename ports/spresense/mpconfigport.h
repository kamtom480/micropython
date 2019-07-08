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

// options to control how MicroPython is built

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER             (1)

#define MICROPY_QSTR_BYTES_IN_HASH          (1)
#define MICROPY_ALLOC_PATH_MAX              (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (16)
#define MICROPY_EMIT_X64                    (0)
#define MICROPY_EMIT_THUMB                  (0)
#define MICROPY_EMIT_INLINE_THUMB           (0)
#define MICROPY_COMP_MODULE_CONST           (0)
#define MICROPY_COMP_CONST                  (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN    (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN    (0)
#define MICROPY_MEM_STATS                   (0)
#define MICROPY_DEBUG_PRINTERS              (0)
#define MICROPY_ENABLE_GC                   (0)
#define MICROPY_GC_ALLOC_THRESHOLD          (0)
#define MICROPY_REPL_EVENT_DRIVEN           (0)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_HELPER_LEXER_UNIX           (0)
#define MICROPY_ENABLE_SOURCE_LINE          (0)
#define MICROPY_MODULE_WEAK_LINKS           (1)
#define MICROPY_ENABLE_DOC_STRING           (0)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_ASYNC_AWAIT              (0)
#define MICROPY_PY_BUILTINS_BYTEARRAY       (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS   (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (0)
#define MICROPY_PY_BUILTINS_ENUMERATE       (0)
#define MICROPY_PY_BUILTINS_FILTER          (0)
#define MICROPY_PY_BUILTINS_FROZENSET       (0)
#define MICROPY_PY_BUILTINS_REVERSED        (0)
#define MICROPY_PY_BUILTINS_SET             (0)
#define MICROPY_PY_BUILTINS_SLICE           (0)
#define MICROPY_PY_BUILTINS_PROPERTY        (0)
#define MICROPY_PY_BUILTINS_MIN_MAX         (0)
#define MICROPY_PY_BUILTINS_STR_COUNT       (0)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO   (0)
#define MICROPY_PY___FILE__                 (0)
#define MICROPY_PY_GC                       (0)
#define MICROPY_PY_ARRAY                    (0)
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_COLLECTIONS              (0)
#define MICROPY_PY_MATH                     (0)
#define MICROPY_PY_CMATH                    (0)
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)
#define MICROPY_PY_IO_FILEIO                (1)
#define MICROPY_PY_IO_BYTESIO               (1)
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_STRUCT                   (0)
#define MICROPY_MODULE_FROZEN_MPY           (0)
#define MICROPY_CPYTHON_COMPAT              (0)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_NONE)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_NONE)
#define MICROPY_PY_UTIME_MP_HAL             (1)

#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SPI_MSB          (0)
#define MICROPY_PY_MACHINE_SPI_LSB          (1)
#define MICROPY_PY_MACHINE_SPI_MAKE_NEW     machine_hw_spi_make_new
#define MICROPY_PY_MACHINE_I2C              (1)
#define MICROPY_PY_MACHINE_I2C_MAKE_NEW     machine_hw_i2c_make_new

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t utime_module;
extern const struct _mp_obj_module_t uos_module;

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_machine), (mp_obj_t)&mp_module_machine }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_utime), (mp_obj_t)&utime_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uos), (mp_obj_t)&uos_module }, \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&utime_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_os), (mp_obj_t)&uos_module }, \

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "SPRESENSE"
#define MICROPY_HW_MCU_NAME "cxd56"
#define MICROPY_PY_SYS_PLATFORM "nuttx"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];