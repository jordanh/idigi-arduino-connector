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

extern int app_os_malloc(size_t const size, void ** ptr);
extern void app_os_free(void * const ptr);

typedef struct 
{
    char dirname[256];
    char direntry_name[256];

} app_dir_data_t;


static idigi_callback_status_t app_process_file_error(void * response)
{
    idigi_callback_status_t status = idigi_callback_continue;
    idigi_file_response_t * response_data = response;

    response_data->errnum = errno;

    switch(response_data->errnum)
    {
        case EACCES:
        case EPERM:
        case EROFS:
        case ELOOP:
            response_data->error = idigi_file_system_permision_denied;
            break;

        case ENOMEM:
        case EMFILE:
        case ENFILE:
            response_data->error = idigi_file_system_out_of_memory;
            break;

        case ENOENT:
        case EBADF:
        case EPIPE:
        case EISDIR:
            response_data->error = idigi_file_system_path_not_found;
            break;

        case EINVAL:
        case ENAMETOOLONG:
            response_data->error = idigi_file_system_invalid_parameter;
            break;

        case EAGAIN:
            status = idigi_callback_busy;
            break;

        default:
            response_data->error = idigi_file_system_fatal_error;
            break;
    }
    return status;
}


static int app_convert_file_open_mode(int mode)
{
#if (IDIGI_O_RDONLY == O_RDONLY) && (IDIGI_O_WRONLY == O_WRONLY) && (IDIGI_O_RDWR == O_RDWR) && \
    (IDIGI_O_CREAT == O_CREAT)   && (IDIGI_O_APPEND == O_APPEND) && (IDIGI_O_TRUNC == O_TRUNC)

    return mode;
#else
    int result = 0;

    if (mode & IDIGI_O_RDONLY) result |= O_RDONLY;
    if (mode & IDIGI_O_WRONLY) result |= O_WRONLY;
    if (mode & IDIGI_O_RDWR)   result |= O_RDWR;
    if (mode & IDIGI_O_APPEND) result |= O_APPEND;
    if (mode & IDIGI_O_CREAT)  result |= O_CREAT;
    if (mode & IDIGI_O_TRUNC)  result |= O_TRUNC;

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

idigi_callback_status_t app_process_file_strerror(idigi_file_error_hint_request_t const * const request_data, idigi_file_response_t * response_data)
{
    size_t hint_size = 0;
     
    if (request_data->errnum != 0)
    {
        char *err_str = strerror(request_data->errnum);
        char *data_ptr = response_data->data.data_ptr;
    
        hint_size = strnlen(err_str, response_data->size_in_bytes - 1) + 1;
        memcpy(data_ptr, err_str, hint_size);
        data_ptr[hint_size] = '\0';
    }

    response_data->size_in_bytes = hint_size;

    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_stat(idigi_file_path_request_t const * const request_data, idigi_file_response_t * response_data)
{
    struct stat statbuf;
    idigi_file_stat_t * pstat = &response_data->data.stat;
    idigi_callback_status_t status = idigi_callback_continue;

    int result = stat(request_data->path, &statbuf);

    APP_DEBUG("stat for %s returned %d, filesize %ld\n", request_data->path, result, statbuf.st_size);

    if (result < 0)
    {
        status = app_process_file_error(response_data);
        goto done;
    }
        
    pstat->flags         = 0;
    pstat->file_size     = statbuf.st_size;
    pstat->last_modified = statbuf.st_mtime;

    if (S_ISDIR(statbuf.st_mode))
       pstat->flags |= IDIGI_FILE_IS_DIR;
    else
    if (S_ISREG(statbuf.st_mode))
       pstat->flags |= IDIGI_FILE_IS_REG;
    
done:
    return status;
}

idigi_callback_status_t app_process_file_opendir(idigi_file_path_request_t const * const request_data, idigi_file_response_t * response_data)
{

    idigi_callback_status_t status = idigi_callback_continue;
    app_dir_data_t *app_dir_data;
    DIR * dirp;

    errno = 0;
    dirp = opendir(request_data->path);

    APP_DEBUG("opendir for %s returned %p\n", request_data->path, (void *) dirp);

    if (dirp == NULL)
    {
        status = app_process_file_error(response_data);
        if (response_data->error == idigi_file_system_noerror)
             response_data->error = idigi_file_system_path_not_found;
        goto done;
    }
    
    {
        void * ptr;
        int result  = app_os_malloc(sizeof *app_dir_data, &ptr);

        if (result == 0 && ptr != NULL)
        {
            app_dir_data = ptr;
            strcpy(app_dir_data->dirname, request_data->path);
            app_dir_data->direntry_name[0] = '\0';
        
            response_data->context = app_dir_data;
            response_data->data.dir_handle = app_dir_data->dirname;
        }
        else
        {
            APP_DEBUG("app_process_file_opendir: malloc fails %s\n", request_data->path);
            response_data->error = idigi_file_system_out_of_memory;
        }
    }
    closedir(dirp);
    APP_DEBUG("closedir for %s\n", request_data->path);

done:
    return status;
}

idigi_callback_status_t app_process_file_closedir(idigi_file_dir_request_t const * const request_data, idigi_file_response_t * response_data)
{
    UNUSED_ARGUMENT(request_data);

    if (response_data->context != NULL)
    {
        app_os_free(response_data->context);
    }
    return idigi_callback_continue;
}

idigi_callback_status_t app_process_file_readdir(idigi_file_dir_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_dir_data_t *app_dir_data = response_data->context;
    struct dirent * direntp;
    DIR *dirp;

    if ((app_dir_data == NULL) || (request_data->dir_handle != app_dir_data->dirname))
    {
        status = idigi_callback_abort;
        goto done;
    }
    errno = 0;
    dirp = opendir((char const *)request_data->dir_handle);

    APP_DEBUG("opendir for %s returned %p\n",(char const *) request_data->dir_handle, (void *) dirp);

    if (dirp == NULL)
    {
        status = app_process_file_error(response_data);
        if (response_data->error == idigi_file_system_noerror)
             response_data->error = idigi_file_system_path_not_found;

        app_dir_data->direntry_name[0] = '\0';
        goto done;
    }
    
    do
    {
       errno = 0;
       direntp = readdir(dirp);

       if (direntp != NULL)
       {
           /* skip "." & ".." */
           if (strcmp(direntp->d_name, ".") == 0)
               continue;

           if (strcmp(direntp->d_name, "..") == 0)
               continue;

           /* 1st entry */
           if (app_dir_data->direntry_name[0] == '\0')
               break;

           /* if skip all entries, returned in prev. calls */
           if (strcmp(app_dir_data->direntry_name, direntp->d_name) == 0)
           {
               errno = 0;
               direntp = readdir(dirp);
               break;
           }
       }
    } while (direntp != NULL);

    if (direntp == NULL)
        APP_DEBUG("readdir returned NULL\n");
    else
         APP_DEBUG("readdir returned %s\n",  direntp->d_name);


    if (direntp == NULL)
    {
        app_dir_data->direntry_name[0] = '\0';

        if (errno != 0)
        {
            status = app_process_file_error(response_data);
        }
        goto done;
    }
    strcpy(app_dir_data->direntry_name,   direntp->d_name);
    strcpy(response_data->data.data_ptr, direntp->d_name);
    response_data->size_in_bytes = strlen(direntp->d_name) + 1;

    closedir(dirp);
    APP_DEBUG("closedir\n");

done:
    return status;
}


idigi_callback_status_t app_process_file_open(idigi_file_path_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int mode = app_convert_file_open_mode(request_data->mode);
    int fd;

    response_data->context = NULL;

    if (mode & O_CREAT)
    {
       fd = open(request_data->path, mode, 0664);
    }
    else
    {
       fd = open(request_data->path, mode);
    }

    APP_DEBUG("Open %s, %d, returned %d\n", request_data->path, mode, fd);

    if (fd < 0)
    {
        status = app_process_file_error(response_data);
    }

    response_data->data.fd = fd;

    return status;
}


idigi_callback_status_t app_process_file_lseek(idigi_file_offset_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int origin = app_convert_lseek_origin(request_data->origin);

    long int offset = lseek(request_data->fd, request_data->offset, origin);

    APP_DEBUG("lseek fd %d, offset %ld, origin %d returned %ld\n", request_data->fd, request_data->offset, 
                                                request_data->origin, offset);
    response_data->data.offset = offset;

    if (offset < 0)
    {
        status = app_process_file_error(response_data);
    }

    return status;
}

idigi_callback_status_t app_process_file_ftruncate(idigi_file_offset_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int result = ftruncate(request_data->fd, request_data->offset);

    APP_DEBUG("ftruncate %d, %ld returned %d\n", request_data->fd, request_data->offset, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data);
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
        status = app_process_file_error(response_data);
    }

    return status;
}

idigi_callback_status_t app_process_file_read(idigi_file_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    int result = read(request_data->fd, response_data->data.data_ptr, response_data->size_in_bytes);

    APP_DEBUG("read %d, %u, returned %d\n", request_data->fd, response_data->size_in_bytes, result);
 
    if (result < 0)
    {
        status = app_process_file_error(response_data);
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

    APP_DEBUG("write %d, %u, returned %d\n", request_data->fd, request_data->size_in_bytes, result);
 
    if (result < 0)
    {
        status = app_process_file_error(response_data);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    return status;
}

idigi_callback_status_t app_process_file_close(idigi_file_request_t const * const request_data, idigi_file_response_t * response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int result = close(request_data->fd);

    APP_DEBUG("close %d returned %d\n", request_data->fd, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data);
    }

    return status;
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

        case idigi_file_system_error:
            APP_DEBUG("Received file system error from the server\n");
            break;

        default:
            APP_DEBUG("Unsupported file system request %d\n", request);
    }

    return status;
}

