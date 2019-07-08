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

#include <sys/stat.h>
#include <dirent.h>

#include "py/objstr.h"
#include "py/runtime.h"
#include "genhdr/mpversion.h"
#include "py/mphal.h"

extern const mp_obj_type_t mp_fat_vfs_type;

STATIC const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC MP_DEFINE_ATTRTUPLE(
    os_uname_info_obj,
    os_uname_info_fields,
    5,
    (mp_obj_t)&os_uname_info_sysname_obj,
    (mp_obj_t)&os_uname_info_nodename_obj,
    (mp_obj_t)&os_uname_info_release_obj,
    (mp_obj_t)&os_uname_info_version_obj,
    (mp_obj_t)&os_uname_info_machine_obj
);

STATIC mp_obj_t os_uname(void) {
    return (mp_obj_t)&os_uname_info_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

STATIC mp_obj_t os_listdir(size_t n_args, const mp_obj_t *args) {
    char *path_in;
    struct stat st;
    const char *full_path;
    mp_obj_t dir_list = mp_obj_new_list(0, NULL);

    if (n_args == 1) {
        path_in = mp_obj_str_get_str(args[0]);
    } else {
        path_in = NULL;
    }

    full_path = get_full_path(path_in);

    if (stat(full_path, &st) < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "File does not exist"));
    }

    if (S_ISDIR(st.st_mode)) {
        DIR *dirp;

        // Open the directory
        dirp = opendir(full_path);

        if (dirp == NULL) {
        // Failed to open the directory
        }

        // Read each directory entry
        for (; ; ) {
            struct dirent *entryp = readdir(dirp);
            if (entryp == NULL) {
                // Finished with this directory
                break;
            }
            mp_obj_list_append(dir_list, mp_obj_new_str(entryp->d_name, strlen(entryp->d_name)));
        }

        // Close the directory
        closedir(dirp);
    } else {
        mp_obj_list_append(dir_list, mp_obj_new_str(path_in, strlen(path_in)));
    }

    free(full_path);

    return dir_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

STATIC mp_obj_t os_mkdir(mp_obj_t path_in) {
    const char *full_path;

    full_path = get_full_path(mp_obj_str_get_str(path_in)); 

    if (full_path != NULL) {
        if (mkdir(full_path, 0777) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Cannot create directory"));
        }

        free(full_path);
    }

    return mp_const_none;
 }
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

STATIC mp_obj_t os_rmdir(mp_obj_t path_in) {
    const char *full_path;

    full_path = get_full_path(mp_obj_str_get_str(path_in)); 

    if (full_path != NULL) {
        if (rmdir(full_path) < 0) {
           nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Cannot remove directory"));
        }

        free(full_path);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

STATIC mp_obj_t os_chdir(mp_obj_t path_in) {
    const char *full_path;

    full_path = get_full_path(mp_obj_str_get_str(path_in)); 

    if (full_path != NULL) {
        if (chdir(full_path) < 0) {
           nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Cannot change directory"));
        }

        free(full_path);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

STATIC mp_obj_t os_getcwd(void) {
    const char *pwd;

    pwd = getenv("PWD");

    return mp_obj_new_str(pwd, strlen(pwd));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

STATIC mp_obj_t os_remove(mp_obj_t path_in) {
    const char *full_path;

    full_path = get_full_path(mp_obj_str_get_str(path_in)); 

    if (full_path != NULL) {
        if (unlink(full_path) < 0) {
           nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Cannot remove file"));
        }

        free(full_path);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

STATIC mp_obj_t os_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path, *new_path;

    old_path = get_full_path(mp_obj_str_get_str(old_path_in));
    new_path = get_full_path(mp_obj_str_get_str(new_path_in)); 

    if (old_path != NULL && new_path != NULL) {
        if (rename(old_path, new_path) < 0) {
           nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Cannot change file name"));
        }

        free(old_path);
        free(new_path);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(os_rename_obj, os_rename);

STATIC mp_obj_t os_stat(mp_obj_t path_in) {
    const char *full_path;
    struct stat st;

    full_path = get_full_path(mp_obj_str_get_str(path_in));

    if (full_path != NULL) {
        if (stat(full_path, &st) < 0) {
           nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Cannot find file"));
        }

        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
        t->items[0] = MP_OBJ_NEW_SMALL_INT(st.st_mode);
        t->items[1] = MP_OBJ_NEW_SMALL_INT(0);
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
        t->items[3] = MP_OBJ_NEW_SMALL_INT(1);
        t->items[4] = MP_OBJ_NEW_SMALL_INT(0);
        t->items[5] = MP_OBJ_NEW_SMALL_INT(0);
        t->items[6] = MP_OBJ_NEW_SMALL_INT(st.st_size);
        t->items[7] = MP_OBJ_NEW_SMALL_INT(st.st_atime);
        t->items[8] = MP_OBJ_NEW_SMALL_INT(st.st_mtime);
        t->items[9] = MP_OBJ_NEW_SMALL_INT(st.st_ctime);

        free(full_path);
        return MP_OBJ_FROM_PTR(t);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

STATIC const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },
    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&os_uname_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&os_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&os_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&os_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&os_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&os_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&os_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&os_stat_obj) },
};
STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t uos_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
