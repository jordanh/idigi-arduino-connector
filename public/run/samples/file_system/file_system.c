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

static idigi_callback_status_t app_process_file_error(idigi_file_error_data_t * error_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    error_data->errnum = errno;

    switch(error_data->errnum)
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
		case ENOSYS:
		case ENOTDIR:
		case EISDIR:
		case EBADF:
            error_data->error_status = idigi_file_path_not_found;
            break;

        case EINVAL:
            error_data->error_status = idigi_file_invalid_parameter;
            break;

        case EAGAIN:
            error_data->errnum = 0;
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

idigi_callback_status_t app_process_file_strerror(void const * const request_data, idigi_file_response_t * response_data)
{
    size_t strerr_size = 0;

    UNUSED_ARGUMENT(request_data);

    idigi_file_error_data_t * error_data = response_data->error;

    if (error_data->errnum != 0)
    {
        char * err_str = strerror(error_data->errnum);
        char * ptr = response_data->data_ptr;
     
        strerr_size = strnlen(err_str, response_data->size_in_bytes - 1) + 1;
        memcpy(ptr, err_str, strerr_size);
        ptr[strerr_size] = '\0';
    }

    response_data->size_in_bytes = strerr_size;

    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t const * const request_data, idigi_file_response_t * response_data)
{
    APP_DEBUG("Message Error %d\n", (int) request_data->message_status);

    if (response_data->user_context != NULL)
    {
        APP_DEBUG("The %p user_context should be freed here!\n", response_data->user_context);
    }

    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_hash(idigi_file_path_request_t const * const request_data, idigi_file_response_t * response_data)
{
    UNUSED_ARGUMENT(request_data);

    if (response_data->size_in_bytes > 0)
        memset(response_data->data_ptr, 0, response_data->size_in_bytes);

    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_stat(idigi_file_stat_request_t const * const request_data, idigi_file_stat_response_t * response_data)
{
    struct stat statbuf;
    idigi_file_stat_t * pstat = response_data->stat_ptr;
    idigi_callback_status_t status = idigi_callback_continue;

    int result = stat(request_data->path, &statbuf);

    APP_DEBUG("stat for %s returned %d, filesize %ld\n", request_data->path, result, statbuf.st_size);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error);
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

idigi_callback_status_t app_process_file_opendir(idigi_file_path_request_t const * const request_data, idigi_file_dir_response_t * response_data)
{

    idigi_callback_status_t status = idigi_callback_continue;
    idigi_file_error_data_t * error_data = response_data->error;
    DIR * dirp;

    errno = 0;
    dirp = opendir(request_data->path);

    APP_DEBUG("opendir for %s returned %p\n", request_data->path, (void *) dirp);

    if (dirp == NULL)
    {
        status = app_process_file_error(response_data->error);

        if (error_data->error_status == idigi_file_noerror)
            error_data->error_status = idigi_file_path_not_found;
        goto done;
    }
    
    {
        void * ptr;
        int result  = app_os_malloc(sizeof(struct dirent), &ptr);

        if (result == 0 && ptr != NULL)
        {
            idigi_file_dir_data_t *dir_data = response_data->dir_data;

            dir_data->dir_handle = dirp;
            dir_data->dir_entry  = ptr;
        }
        else
        {
            APP_DEBUG("app_process_file_opendir: malloc fails %s\n", request_data->path);

            error_data->error_status = idigi_file_out_of_memory;
            error_data->errnum     = ENOMEM; 
            closedir(dirp);
            APP_DEBUG("closedir for %s\n", request_data->path);
        }
    }

done:
    return status;
}

idigi_callback_status_t app_process_file_closedir(idigi_file_dir_data_t const * const request_data, idigi_file_response_t * response_data)
{
    UNUSED_ARGUMENT(response_data);

    APP_DEBUG("closedir for %p\n", request_data->dir_handle);
    closedir((DIR *) request_data->dir_handle);
    
    if (request_data->dir_entry != NULL)
        app_os_free(request_data->dir_entry);

    if (response_data->user_context != NULL)
    {
        APP_DEBUG("The %p user_context should be freed here!\n", response_data->user_context);
    }
    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_readdir(idigi_file_dir_data_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    struct dirent * direntp = request_data->dir_entry;
    DIR           * dirp    = request_data->dir_handle;
    struct dirent * result;
    int read_again = 0;
    int rc;

    errno = 0;

    do
    {
       read_again = 0; 

       rc = readdir_r(dirp, direntp, &result);

       if (rc == 0 && result != NULL)
       {
           /* skip "." & ".." */
           if (strcmp(result->d_name, ".") == 0)
              read_again = 1;
           else
           if (strcmp(result->d_name, "..") == 0)
              read_again = 1;

       }
    } while (read_again != 0);


    if (result == NULL)
        APP_DEBUG("readdir returned NULL\n");
    else
        APP_DEBUG("readdir returned %s\n",  result->d_name);

    if (rc != 0)
       status = app_process_file_error(response_data->error);

    if (result == NULL || rc != 0)
    {
        char *entry_name = response_data->data_ptr;
        *entry_name = '\0';
         response_data->size_in_bytes = 1;
        goto done;
    }

    {
        size_t name_len = strlen(result->d_name) + 1;
        size_t buffer_size = response_data->size_in_bytes;

        if(name_len <= buffer_size)
        {

            strcpy((char *) response_data->data_ptr, result->d_name);
            response_data->size_in_bytes = name_len;
        }
        else
        {
            ASSERT(0);
            errno = ENAMETOOLONG;
            status = app_process_file_error(response_data->error);
        }
    }

done:
    return status;
}


idigi_callback_status_t app_process_file_open(idigi_file_open_request_t const * const request_data, idigi_file_open_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int oflag = app_convert_file_open_mode(request_data->oflag);
    int fd;

    response_data->user_context = NULL;

    if (oflag & O_CREAT)
    {
       fd = open(request_data->path, oflag, 0664);
    }
    else
    {
       fd = open(request_data->path, oflag);
    }

    APP_DEBUG("Open %s, %d, returned %d\n", request_data->path, oflag, fd);

    if (fd < 0)
    {
        status = app_process_file_error(response_data->error);
    }

    *response_data->fd_ptr = fd;

    return status;
}


idigi_callback_status_t app_process_file_lseek(idigi_file_lseek_request_t const * const request_data, idigi_file_lseek_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int origin = app_convert_lseek_origin(request_data->origin);

    long int offset = lseek(request_data->fd, request_data->offset, origin);

    APP_DEBUG("lseek fd %d, offset %ld, origin %d returned %ld\n", request_data->fd, request_data->offset, 
                                                request_data->origin, offset);
    *response_data->offset_ptr = offset;

    if (offset < 0)
    {
        status = app_process_file_error(response_data->error);
    }

    return status;
}

idigi_callback_status_t app_process_file_ftruncate(idigi_file_ftruncate_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int result = ftruncate(request_data->fd, request_data->length);

    APP_DEBUG("ftruncate %d, %ld returned %d\n", request_data->fd, request_data->length, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error);
    }

    return status;
}

idigi_callback_status_t app_process_file_rm(idigi_file_path_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int result = unlink(request_data->path);

    APP_DEBUG("unlink %s returned %d\n", request_data->path, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error);
    }

    return status;
}

idigi_callback_status_t app_process_file_read(idigi_file_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
 
    int result = read(request_data->fd, response_data->data_ptr, response_data->size_in_bytes);

    APP_DEBUG("read %d, %zu, returned %d\n", request_data->fd, response_data->size_in_bytes, result);
 
    if (result < 0)
    {
        status = app_process_file_error(response_data->error);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

idigi_callback_status_t app_process_file_write(idigi_file_write_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int result = write(request_data->fd, request_data->data_ptr, request_data->size_in_bytes);

    APP_DEBUG("write %d, %zu, returned %d\n", request_data->fd, request_data->size_in_bytes, result);
 
    if (result < 0)
    {
        status = app_process_file_error(response_data->error);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

idigi_callback_status_t app_process_file_close(idigi_file_request_t const * const request_data, idigi_file_response_t * response_data)
{
    int result = close(request_data->fd);

    APP_DEBUG("close %d returned %d\n", request_data->fd, result);

    if (result < 0 && errno == EIO)
    {
        response_data->error->errnum = EIO;
        response_data->error->error_status = idigi_file_unspec_error;
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

