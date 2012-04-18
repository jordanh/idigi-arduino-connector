/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 *
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 * =======================================================================
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "idigi_api.h"
#include "platform.h"

#ifndef APP_MIN_VALUE
#define APP_MIN_VALUE(a,b) (((a)<(b))?(a):(b))
#endif

extern int app_os_malloc(size_t const size, void ** ptr);
extern void app_os_free(void * const ptr);

typedef struct
{
    DIR         * dirp;
    struct dirent dir_entry;

} app_dir_data_t;

static idigi_callback_status_t app_process_file_error(idigi_file_error_data_t * error_data, int errnum)
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


static int app_convert_file_open_mode(int oflag)
{
#if (IDIGI_O_RDONLY == O_RDONLY) && (IDIGI_O_WRONLY == O_WRONLY) && (IDIGI_O_RDWR == O_RDWR) && \
    (IDIGI_O_CREAT == O_CREAT)   && (IDIGI_O_APPEND == O_APPEND) && (IDIGI_O_TRUNC == O_TRUNC)

    return oflag;
#else
    int result = 0;

    if (oflag & IDIGI_O_RDONLY) result |= O_RDONLY;
    if (oflag & IDIGI_O_WRONLY) result |= O_WRONLY;
    if (oflag & IDIGI_O_RDWR)   result |= O_RDWR;
    if (oflag & IDIGI_O_APPEND) result |= O_APPEND;
    if (oflag & IDIGI_O_CREAT)  result |= O_CREAT;
    if (oflag & IDIGI_O_TRUNC)  result |= O_TRUNC;

    return result;
#endif
}

static int app_convert_lseek_origin(int origin)
{
#if (IDIGI_SEEK_SET == SEEK_SET) && (IDIGI_SEEK_CUR == SEEK_CUR) && (IDIGI_SEEK_END == SEEK_END)

    return origin;
#else
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

idigi_callback_status_t app_process_file_strerror(void const * const request_data, 
                                                  idigi_file_data_response_t * response_data)
{
    size_t strerr_size = 0;

    UNUSED_ARGUMENT(request_data);

    idigi_file_error_data_t * error_data = response_data->error;
    int errnum = (int) error_data->errnum;

    if (errnum != 0)
    {
        char * err_str = strerror(errnum);
        char * ptr = response_data->data_ptr;
     
        strerr_size = strnlen(err_str, response_data->size_in_bytes - 1) + 1;
        memcpy(ptr, err_str, strerr_size);
        ptr[strerr_size] = '\0';
    }

    response_data->size_in_bytes = strerr_size;

    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t const * const request_data,
                                                   idigi_file_response_t * response_data)
{
    APP_DEBUG("Message Error %d\n", (int) request_data->message_status);

    if (response_data->user_context != NULL)
    {
        APP_DEBUG("The %p user_context should be freed here!\n", response_data->user_context);
    }

    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_hash(idigi_file_path_request_t const * const request_data,
                                              idigi_file_data_response_t * response_data)
{
    UNUSED_ARGUMENT(request_data);

    memset(response_data->data_ptr, 0, response_data->size_in_bytes);
    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_stat(idigi_file_stat_request_t const * const request_data,
                                              idigi_file_stat_response_t * response_data)
{
    struct stat statbuf;
    idigi_file_stat_t * pstat = &response_data->statbuf;
    idigi_callback_status_t status = idigi_callback_continue;

    int result = stat(request_data->path, &statbuf);

    APP_DEBUG("stat for %s returned %d, filesize %ld\n", request_data->path, result, statbuf.st_size);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
        goto done;
    }
        
    pstat->flags         = 0;
    pstat->file_size     = statbuf.st_size;
    pstat->last_modified = statbuf.st_mtime;
    pstat->hash_alg      = idigi_file_hash_none;

    if (S_ISDIR(statbuf.st_mode))
       pstat->flags |= IDIGI_FILE_IS_DIR;
    else
    if (S_ISREG(statbuf.st_mode))
       pstat->flags |= IDIGI_FILE_IS_REG;
    
done:
    return status;
}

idigi_callback_status_t app_process_file_opendir(idigi_file_path_request_t const * const request_data,
                                                 idigi_file_open_response_t * response_data)
{

    idigi_callback_status_t status = idigi_callback_continue;
    idigi_file_error_data_t * error_data = response_data->error;
    DIR * dirp;

    errno = 0;
    dirp = opendir(request_data->path);

    if (dirp != NULL)
    {
        void           * ptr;

        int result  = app_os_malloc(sizeof (app_dir_data_t), &ptr);

        if (result == 0 && ptr != NULL)
        {
            app_dir_data_t * dir_data = ptr;
            response_data->handle = ptr;

            dir_data->dirp = dirp;
            APP_DEBUG("opendir for %s returned %p\n", request_data->path, (void *) dirp);
        }
        else
        {
            APP_DEBUG("app_process_file_opendir: malloc fails %s\n", request_data->path);

            error_data->error_status = idigi_file_out_of_memory;
            error_data->errnum = (void *) ENOMEM; 
            closedir(dirp);
        }
    }
    else
        status = app_process_file_error(response_data->error, errno);

    return status;
}

idigi_callback_status_t app_process_file_closedir(idigi_file_request_t const * const request_data,
                                                  idigi_file_response_t * response_data)
{
    app_dir_data_t * dir_data = request_data->handle;

    APP_DEBUG("closedir %p\n", (void *) dir_data->dirp);

    closedir(dir_data->dirp);
    app_os_free(dir_data);

    if (response_data->user_context != NULL)
    {
        APP_DEBUG("The %p user_context should be freed here!\n", response_data->user_context);
    }
    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_readdir(idigi_file_request_t const * const request_data,
                                                 idigi_file_data_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_dir_data_t * dir_data = request_data->handle;
    struct dirent  * result;
    int rc;

    // Read next directory entry, skip "." and ".."
    do
    {
        rc = readdir_r(dir_data->dirp, &dir_data->dir_entry, &result);

        if (result != NULL && rc == 0)
        {
            // if not "." or "..", we are done
            if ((strcmp(result->d_name, ".") != 0) && (strcmp(result->d_name, "..") != 0))
                break;
        }
    } while(result != NULL && rc == 0);

    // error
    if (rc != 0)
    {
        status = app_process_file_error(response_data->error, rc);
        APP_DEBUG("readdir_r returned %d\n", rc);
        goto done;
    }

    // finished with the directory
    if (result == NULL)
    {
        APP_DEBUG("No more directory entries %d\n", rc);
        response_data->size_in_bytes = 0;
        goto done;
    }

    // read valid entry
    {
        size_t name_len = strlen(result->d_name) + 1;
        size_t buffer_size = response_data->size_in_bytes;

        APP_DEBUG("readdir_r returned directory %s\n", result->d_name);

        if(name_len <= buffer_size)
        {
            strcpy((char *) response_data->data_ptr, result->d_name);
            response_data->size_in_bytes = name_len;
        }
        else
        {
            idigi_file_error_data_t * error_data = response_data->error;

            ASSERT(0);
            APP_DEBUG("directory name too long\n");
            error_data->error_status = idigi_file_out_of_memory;
            error_data->errnum = (void *) ENAMETOOLONG;
        }
    }

done:
    return status;
}


idigi_callback_status_t app_process_file_open(idigi_file_open_request_t const * const request_data,
                                              idigi_file_open_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int oflag = app_convert_file_open_mode(request_data->oflag);
    int fd;

    response_data->user_context = NULL;

    // 0664 = read,write owner + read,write group + read others
    fd = open(request_data->path, oflag, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH); 

    APP_DEBUG("Open %s, %d, returned %d\n", request_data->path, oflag, fd);

    if (fd < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }

    response_data->handle = (void *) fd;

    return status;
}


idigi_callback_status_t app_process_file_lseek(idigi_file_lseek_request_t const * const request_data,
                                               idigi_file_lseek_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int origin = app_convert_lseek_origin(request_data->origin);
    int fd = (int) request_data->handle;

    long int offset = lseek(fd, request_data->offset, origin);

    APP_DEBUG("lseek fd %d, offset %ld, origin %d returned %ld\n", fd, request_data->offset, 
                                                request_data->origin, offset);
    response_data->offset = offset;

    if (offset < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }

    return status;
}

idigi_callback_status_t app_process_file_ftruncate(idigi_file_ftruncate_request_t const * const request_data,
                                                   idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int fd = (int) request_data->handle;

    int result = ftruncate(fd, request_data->length);

    APP_DEBUG("ftruncate %d, %ld returned %d\n", fd, request_data->length, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }

    return status;
}

idigi_callback_status_t app_process_file_rm(idigi_file_path_request_t const * const request_data,
                                            idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int result = unlink(request_data->path);

    APP_DEBUG("unlink %s returned %d\n", request_data->path, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }

    return status;
}

idigi_callback_status_t app_process_file_read(idigi_file_request_t const * const request_data,
                                              idigi_file_data_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int fd = (int) request_data->handle;
 
    int result = read(fd, response_data->data_ptr, response_data->size_in_bytes);

    APP_DEBUG("read %d, %zu, returned %d\n", fd, response_data->size_in_bytes, result);
 
    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

idigi_callback_status_t app_process_file_write(idigi_file_write_request_t const * const request_data,
                                               idigi_file_write_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int fd = (int) request_data->handle;

    int result = write(fd, request_data->data_ptr, request_data->size_in_bytes);

    APP_DEBUG("write %d, %zu, returned %d\n", fd, request_data->size_in_bytes, result);
 
    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

idigi_callback_status_t app_process_file_close(idigi_file_request_t const * const request_data, 
                                               idigi_file_response_t * response_data)
{
    int fd = (int) request_data->handle;
    int result = close(fd);

    APP_DEBUG("close %d returned %d\n", fd, result);

    if (result < 0 && errno == EIO)
    {
        idigi_file_error_data_t * error_data = response_data->error;

        error_data->errnum = (void *) EIO;
        error_data->error_status = idigi_file_unspec_error;
    }
    if (response_data->user_context != NULL)
    {
        APP_DEBUG("The %p user_context should be freed here!\n", response_data->user_context);
    }

    return idigi_callback_continue;
}

idigi_callback_status_t app_file_system_handler(idigi_data_service_request_t const request,
                                                void const * request_data, size_t const request_length,
                                                void * response_data, size_t * const response_length)
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

        case idigi_file_system_ftruncate:
            status = app_process_file_ftruncate(request_data, response_data);
            break;

        case idigi_file_system_rm:
            status = app_process_file_rm(request_data, response_data);
            break;

        case idigi_file_system_stat:
            app_process_file_stat(request_data, response_data);
            break;

        case idigi_file_system_opendir:
            app_process_file_opendir(request_data, response_data);
            break;

        case idigi_file_system_readdir:
            app_process_file_readdir(request_data, response_data);
            break;

        case idigi_file_system_closedir:
            app_process_file_closedir(request_data, response_data);
            break;

        case idigi_file_system_strerror:
            app_process_file_strerror(request_data, response_data);
            break;

        case idigi_file_system_hash:
            app_process_file_hash(request_data, response_data);
            break;

        case idigi_file_system_msg_error:
            app_process_file_msg_error(request_data, response_data);
            break;

        default:
            APP_DEBUG("Unsupported file system request %d\n", request);
    }

    return status;
}

