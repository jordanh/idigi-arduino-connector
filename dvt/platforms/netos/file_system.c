/*
 * Copyright (c) 2011, 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"
#include "netosIo.h"
#include "bsp_api.h"
#include "sysAccess.h"
#include "bsp.h"

#include "idigi_config.h"
#include "idigi_debug.h"
#include "idigi_api.h"
#include "platform.h"


#if !defined IDIGI_FILE_SYSTEM 
#error "Replace IDIGI_NO_FILE_SYSTEM with IDIGI_FILE_SYSTEM in idigi_config.h to run this sample" 
#endif

#ifndef APP_MIN_VALUE
#define APP_MIN_VALUE(a,b) (((a)<(b))?(a):(b))
#endif


static idigi_callback_status_t app_process_file_error(idigi_file_error_data_t * const error_data, long int const errnum)
{
    idigi_callback_status_t status = idigi_callback_continue;

    error_data->errnum = (void *) errnum;

    switch(errnum)
    {
        case EACCES:
        case EPERM:
        case EROFS:
            error_data->error_status = idigi_file_permision_denied;
            break;

        case ENOMEM:
        case ENAMETOOLONG:
            error_data->error_status = idigi_file_out_of_memory;
            break;

        case ENOENT:
        case ENODEV:
        case EBADF:
            error_data->error_status = idigi_file_path_not_found;
            break;

        case EINVAL:
        case ENOSYS:
        case ENOTDIR:
        case EISDIR:
            error_data->error_status = idigi_file_invalid_parameter;
            break;

#if EAGAIN != EWOULDBLOCK
        case EWOULDBLOCK:
#endif
        case EAGAIN:
            status = idigi_callback_busy;
            break;

        case ENOSPC:
            error_data->error_status = idigi_file_insufficient_storage_space;
            break;

        default:
            error_data->error_status = idigi_file_unspec_error;
            break;
    }
    return status;
}

static int app_convert_file_open_mode(int const oflag)
{
#if (IDIGI_O_RDONLY == O_RDONLY) && (IDIGI_O_WRONLY == O_WRONLY) && (IDIGI_O_RDWR == O_RDWR) && \
    (IDIGI_O_CREAT == O_CREAT)   && (IDIGI_O_APPEND == O_APPEND) && (IDIGI_O_TRUNC == O_TRUNC)

    return oflag;
#else
    int result = 0;

    if (oflag & IDIGI_O_WRONLY) result |= O_WRONLY;
    if (oflag & IDIGI_O_RDWR)   result |= O_RDWR;
    if (oflag & IDIGI_O_APPEND) result |= O_APPEND;
    if (oflag & IDIGI_O_CREAT)  result |= O_CREAT;
    if (oflag & IDIGI_O_TRUNC)  result |= O_TRUNC;

    if ((oflag & (IDIGI_O_WRONLY | IDIGI_O_RDWR)) == 0)
        result |= O_RDONLY;

    return result;
#endif
}

static int app_convert_lseek_origin(int const origin)
{
#if (IDIGI_SEEK_SET == SEEK_SET) && (IDIGI_SEEK_CUR == SEEK_CUR) && (IDIGI_SEEK_END == SEEK_END)

    return origin;
#else
    int result;

    switch(origin)
    {
    case IDIGI_SEEK_SET:
        result = SEEK_SET;
        break;
    case IDIGI_SEEK_END:
        result = SEEK_END;
        break;
    case IDIGI_SEEK_CUR:
    default:
        result = SEEK_CUR;
        break;
    }

    return result;
#endif
}

static idigi_callback_status_t app_process_file_strerror(idigi_file_data_response_t * response_data)
{
    size_t strerr_size = 0;

    idigi_file_error_data_t * error_data = response_data->error;
    long int errnum = (long int)error_data->errnum;

    if (errnum != 0)
    {
        char * err_str = strerror(errnum);
        char * ptr = response_data->data_ptr;

        strerr_size = APP_MIN_VALUE(strlen(err_str), response_data->size_in_bytes);
        memcpy(ptr, err_str, strerr_size);
    }

    response_data->size_in_bytes = strerr_size;

    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t const * const request_data,
                                                          idigi_file_response_t * const response_data)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(response_data);
    APP_DEBUG("Message Error %d\n", request_data->message_status);

    // All application resources, used in the session, must be released in this callback
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_file_open(idigi_file_open_request_t const * const request_data,
                                                     idigi_file_open_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int const oflag = app_convert_file_open_mode(request_data->oflag);

    // 0664 = read,write owner + read,write group + read others
    long int const fd = open(request_data->path, oflag, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    int errnum = getErrno(); // In NET+OS APP_DEBUG clears errno

    APP_DEBUG("Open %s, %d, returned %ld\n", request_data->path, oflag, fd);

    if (fd < 0)
    {
        APP_DEBUG("errno %d\n", errnum);
        status = app_process_file_error(response_data->error, errnum);
    }

    response_data->handle = (void *) fd;
    response_data->user_context = NULL;

    return status;
}


static idigi_callback_status_t app_process_file_lseek(idigi_file_lseek_request_t const * const request_data,
                                                      idigi_file_lseek_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int const origin = app_convert_lseek_origin(request_data->origin);
    long int const fd = (long int) request_data->handle;

    long int const offset = lseek(fd, request_data->offset, origin);
    int errnum = getErrno(); // In NET+OS APP_DEBUG clears errno
    

    APP_DEBUG("lseek fd %ld, offset %ld, origin %d returned %ld\n", fd, request_data->offset,
                                                request_data->origin, offset);
    response_data->offset = offset;

    if (offset < 0)
    {
        APP_DEBUG("errno %d\n", errnum);
        status = app_process_file_error(response_data->error, errnum);
    }

    return status;
}

static idigi_callback_status_t app_process_file_rm(idigi_file_path_request_t const * const request_data,
                                                   idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int const result = unlink(request_data->path);
    int errnum = getErrno(); // In NET+OS APP_DEBUG clears errno

    APP_DEBUG("unlink %s returned %d\n", request_data->path, result);

    if (result < 0)
    {
        APP_DEBUG("errno %d\n", errnum);
        status = app_process_file_error(response_data->error, errnum);
    }

    return status;
}

static idigi_callback_status_t app_process_file_read(idigi_file_request_t const * const request_data,
                                                     idigi_file_data_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;

    int const result = read(fd, response_data->data_ptr, response_data->size_in_bytes);
    int errnum = getErrno(); // In NET+OS APP_DEBUG clears errno

    APP_DEBUG("read %ld, %zu, returned %d\n", fd, response_data->size_in_bytes, result);

    if (result < 0)
    {
        APP_DEBUG("errno %d\n", errnum);
        status = app_process_file_error(response_data->error, errnum);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

static idigi_callback_status_t app_process_file_write(idigi_file_write_request_t const * const request_data,
                                                      idigi_file_write_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;

    int const result = write(fd, request_data->data_ptr, request_data->size_in_bytes);
    int errnum = getErrno(); // In NET+OS APP_DEBUG clears errno

    APP_DEBUG("write %ld, %zu, returned %d\n", fd, request_data->size_in_bytes, result);

    if (result < 0)
    {
        APP_DEBUG("errno %d\n", errnum);
        status = app_process_file_error(response_data->error, errnum);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

static idigi_callback_status_t app_process_file_close(idigi_file_request_t const * const request_data,
                                                      idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;
    int const result = close(fd);
    int errnum = getErrno(); // In NET+OS APP_DEBUG clears errno

    APP_DEBUG("close %ld returned %d\n", fd, result);

    if (result < 0 && errnum == EIO)
    {
        APP_DEBUG("errno %d\n", errnum);
        status = app_process_file_error(response_data->error, EIO);
    }

    // All application resources, used in the session, must be released in this callback

    return status;
}

idigi_callback_status_t app_file_system_handler(idigi_file_system_request_t const request,
                                                void const * const request_data, size_t const request_length,
                                                void * const response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {
        case idigi_file_system_open:
            status = app_process_file_open(request_data, response_data);
            break;

        case idigi_file_system_read:
            status = app_process_file_read(request_data, response_data);
            break;

        case idigi_file_system_write:
            status = app_process_file_write(request_data, response_data);
            break;

        case idigi_file_system_lseek:
            status = app_process_file_lseek(request_data, response_data);
            break;

        case idigi_file_system_close:
            status = app_process_file_close(request_data, response_data);
            break;

        case idigi_file_system_rm:
            status = app_process_file_rm(request_data, response_data);
            break;

        case idigi_file_system_msg_error:
            status = app_process_file_msg_error(request_data, response_data);
            break;

        case idigi_file_system_strerror:
            status = app_process_file_strerror(response_data);
            break;

        case idigi_file_system_stat:
        case idigi_file_system_opendir:
        case idigi_file_system_readdir:
        case idigi_file_system_closedir:
        case idigi_file_system_hash:
        case idigi_file_system_ftruncate:
        default:
            APP_DEBUG("Unsupported file system request %d\n", request);
            status = app_process_file_error(((idigi_file_response_t *)response_data)->error, ENOTSUP);
    }

    return status;
}

