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
#include <time.h>
#include "idigi_api.h"
#include "platform.h"

#if defined APP_ENABLE_MD5
#include <openssl/md5.h>

#define APP_MD5_BUFFER_SIZE 1024

typedef struct
{
    MD5_CTX md5;
    char buf[APP_MD5_BUFFER_SIZE];
    unsigned int flags;
    int fd;

} app_md5_ctx;
#endif

#ifndef APP_MIN_VALUE
#define APP_MIN_VALUE(a,b) (((a)<(b))?(a):(b))
#endif

typedef struct
{
    DIR * dirp;
    struct dirent dir_entry;

} app_dir_data_t;

#define DVT_FS_BUSY_OFFSET      0
#define DVT_FS_START_OFFSET     1
#define DVT_FS_MIDDLE_OFFSET    2
#define DVT_FS_END_OFFSET       3
#define DVT_FS_TIMEOUT_OFFSET   4

#define DVT_FS_TIMEOUT_SECS     70

typedef enum
{
    dvt_fs_case_offset_busy,
    dvt_fs_case_offset_start,
    dvt_fs_case_offset_middle,
    dvt_fs_case_offset_end,
    dvt_fs_case_offset_timeout,
    dvt_fs_case_offset_COUNT
} dvt_fs_case_offset_t;

typedef enum
{
    dvt_fs_state_at_start,
    dvt_fs_state_at_middle,
    dvt_fs_state_at_end
} dvt_fs_state_t;

typedef enum
{
    dvt_fs_error_none,
    dvt_fs_error_get_busy,
    dvt_fs_error_get_start,
    dvt_fs_error_get_middle,
    dvt_fs_error_get_end,
    dvt_fs_error_get_timeout,
    dvt_fs_error_put_busy,
    dvt_fs_error_put_start,
    dvt_fs_error_put_middle,
    dvt_fs_error_put_end,
    dvt_fs_error_put_timeout,
    dvt_fs_error_ls_busy,
    dvt_fs_error_ls_start,
    dvt_fs_error_ls_middle,
    dvt_fs_error_ls_end,
    dvt_fs_error_ls_timeout,
    dvt_fs_error_ls_invalid_hash,
    dvt_fs_error_rm_timeout,
    dvt_fs_error_COUNT
} dvt_fs_error_cases_t;

typedef struct
{
    char const * fpath;
    dvt_fs_error_cases_t error_case;
}dvt_fs_error_entry_t;

static dvt_fs_error_entry_t dvt_fs_error_list[] =
{
    {"dvt_fs_get_error.test", dvt_fs_error_get_busy},
    {"dvt_fs_put_error.test", dvt_fs_error_put_busy},
    {"./public", dvt_fs_error_ls_busy},
    {"./public/step", dvt_fs_error_ls_start},
    {"./public/run", dvt_fs_error_ls_end},
    {"./public/run/samples", dvt_fs_error_ls_middle},
    {"./public/run/platforms", dvt_fs_error_ls_timeout},
    {"./public/include", dvt_fs_error_ls_invalid_hash},
    {"dvt_fs_rm_tout.test", dvt_fs_error_rm_timeout}
};

static size_t const  dvt_fs_error_list_count  = asizeof(dvt_fs_error_list);
static dvt_fs_error_cases_t dvt_current_error_case = dvt_fs_error_none;
static dvt_fs_state_t dvt_current_state = dvt_fs_state_at_start;

static void update_error_case(char const * const path)
{
    size_t i;

    dvt_current_error_case = dvt_fs_error_none;
    for (i = 0; i < dvt_fs_error_list_count; i++)
    {
        if (!strcmp(path, dvt_fs_error_list[i].fpath))
        {
            dvt_current_error_case = dvt_fs_error_list[i].error_case;
            break;
        }
    }
    dvt_current_state = dvt_fs_state_at_start;
}

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

    if (oflag & IDIGI_O_RDONLY) result |= O_RDONLY;
    if (oflag & IDIGI_O_WRONLY) result |= O_WRONLY;
    if (oflag & IDIGI_O_RDWR)   result |= O_RDWR;
    if (oflag & IDIGI_O_APPEND) result |= O_APPEND;
    if (oflag & IDIGI_O_CREAT)  result |= O_CREAT;
    if (oflag & IDIGI_O_TRUNC)  result |= O_TRUNC;

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
    long int errnum = (long int) error_data->errnum;

    if (errnum != 0)
    {
        char * err_str = strerror(errnum);
        char * ptr = response_data->data_ptr;

        strerr_size = APP_MIN_VALUE(strlen(err_str) + 1, response_data->size_in_bytes);
        memcpy(ptr, err_str, strerr_size);
        ptr[strerr_size - 1] = '\0';
    }

    response_data->size_in_bytes = strerr_size;

    return idigi_callback_continue;
}


#if defined APP_ENABLE_MD5
static app_md5_ctx * app_allocate_md5_ctx(unsigned int const flags, idigi_file_error_data_t * const error_data)
{
    app_md5_ctx * ctx = malloc(sizeof *ctx);

    if (ctx != NULL)
    {
        ctx->flags = flags;
        ctx->fd    = -1;
    }
    else
    {
        app_process_file_error(error_data, ENOMEM);
        APP_DEBUG("app_allocate_md5_ctx: malloc fails\n");
    }
    return ctx;
}

static idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t const * const request_data,
                                                          idigi_file_response_t * const response_data)
{
    UNUSED_ARGUMENT(request_data);
    APP_DEBUG("Message Error %d\n", (int) request_data->message_status);

    // All application resources, used in the session, must be released in this callback
    if (response_data->user_context != NULL)
    {
        app_md5_ctx * ctx = response_data->user_context;

        if (ctx->fd >= 0)
            close(ctx->fd);

        free(response_data->user_context);
        response_data->user_context = NULL;
    }
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_file_hash(idigi_file_path_request_t const * const request_data,
                                                     idigi_file_data_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_md5_ctx * ctx = response_data->user_context;
    int ret;

    if (ctx == NULL)
    {   
        ASSERT(0);
        goto error;
    }

    if (ctx->fd < 0)
    {
        ctx->fd = open(request_data->path, O_RDONLY);
        APP_DEBUG("Open %s, returned %d\n", request_data->path, ctx->fd);

        if (ctx->fd < 0)
        {
            ASSERT(0);
            goto error;
        }

        MD5_Init(&ctx->md5);
    }

    while ((ret = read (ctx->fd, ctx->buf, sizeof ctx->buf)) > 0)
    {
        MD5_Update(&ctx->md5, ctx->buf, ret);
    }
    if (ret == -1 && errno == EAGAIN)
    {
        status = idigi_callback_busy;
        goto done;
    }

    APP_DEBUG("Close %d\n", ctx->fd);
    close (ctx->fd);
    ctx->fd = -1;

    if (ret == 0)
    {
        MD5_Final (response_data->data_ptr, &ctx->md5);
        goto done;
    }

error:
    memset(response_data->data_ptr, 0, response_data->size_in_bytes);

done:
    if (ctx != NULL && status == idigi_callback_continue)
    {
        // free md5 context here,  if ls was issued a single file
        if ((ctx->flags & IDIGI_FILE_IS_DIR) == 0)
        {
            free(response_data->user_context);
            response_data->user_context = NULL;
        }
    }
    return status;
}
#else

static idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t const * const request_data,
                                                          idigi_file_response_t * const response_data)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(response_data);
    APP_DEBUG("Message Error %d\n", (int) request_data->message_status);

    // All application resources, used in the session, must be released in this callback
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_file_hash(idigi_file_path_request_t const * const request_data,
                                                     idigi_file_data_response_t * const response_data)
{
    UNUSED_ARGUMENT(request_data);

    // app_process_file_hash() should not be called if APP_ENABLE_MD5 is not defined
    ASSERT(0)

    memset(response_data->data_ptr, 0, response_data->size_in_bytes);
    return idigi_callback_continue;
}
#endif

static idigi_callback_status_t app_process_file_stat(idigi_file_stat_request_t const * const request_data,
                                                     idigi_file_stat_response_t * const response_data)
{
    struct stat statbuf;
    idigi_file_stat_t * pstat = &response_data->statbuf;
    idigi_callback_status_t status = idigi_callback_continue;

    int const result = stat(request_data->path, &statbuf);

    APP_DEBUG("stat for %s returned %d, filesize %ld\n", request_data->path, result, statbuf.st_size);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
        goto done;
    }

    pstat->flags = 0;
    pstat->file_size = statbuf.st_size;
    pstat->last_modified = statbuf.st_mtime;
    pstat->hash_alg = idigi_file_hash_none;

    if (S_ISDIR(statbuf.st_mode))
       pstat->flags |= IDIGI_FILE_IS_DIR;
    else
    if (S_ISREG(statbuf.st_mode))
       pstat->flags |= IDIGI_FILE_IS_REG;

#if defined APP_ENABLE_MD5

    if (request_data->hash_alg != idigi_file_hash_none)
    {
        update_error_case(request_data->path);
        if (dvt_current_error_case == dvt_fs_error_ls_invalid_hash)
        {
            pstat->hash_alg = idigi_file_hash_crc32;
            goto done;
        }
    }

    /*
     * If ls was issued for a directory
     * - app_process_file_stat() is called with the requested hash algorithm once for this directory.
     * - app_process_file_stat() is called with idigi_file_hash_none for each directory entry.
     */
    switch (request_data->hash_alg)
    {
        case idigi_file_hash_best:
        case idigi_file_hash_md5:
            if (pstat->flags != 0)
            {
                pstat->hash_alg = idigi_file_hash_md5;
                if (response_data->user_context == NULL)
                    response_data->user_context = app_allocate_md5_ctx(pstat->flags, response_data->error);
            }
            break;


        default:
            break;
    }
#endif
done:
    return status;
}

static idigi_callback_status_t app_process_file_opendir(idigi_file_path_request_t const * const request_data,
                                                        idigi_file_open_response_t * const response_data)
{

    idigi_callback_status_t status = idigi_callback_continue;
    DIR * dirp;

    errno = 0;
    dirp = opendir(request_data->path);
    update_error_case(request_data->path);

    if ((dirp != NULL) && (dvt_current_error_case == dvt_fs_error_ls_start))
    {
       closedir(dirp);
       response_data->error->errnum = (void *) EACCES;
       response_data->error->error_status = idigi_file_permision_denied;
       goto done;
    }

    if (dirp != NULL)
    {
        app_dir_data_t * dir_data = malloc(sizeof *dir_data);

        if (dir_data != NULL)
        {
            response_data->handle = dir_data;

            dir_data->dirp = dirp;
            APP_DEBUG("opendir for %s returned %p\n", request_data->path, (void *) dirp);
            update_error_case(request_data->path);

        }
        else
        {
            APP_DEBUG("app_process_file_opendir: malloc fails %s\n", request_data->path);
            status = app_process_file_error(response_data->error, ENOMEM);
            closedir(dirp);
        }
    }
    else
        status = app_process_file_error(response_data->error, errno);

done:
    return status;
}

static idigi_callback_status_t app_process_file_closedir(idigi_file_request_t const * const request_data,
                                                         idigi_file_response_t * const response_data)
{
    app_dir_data_t * dir_data = request_data->handle;
    UNUSED_ARGUMENT(response_data);

    ASSERT(dir_data != NULL);
    APP_DEBUG("closedir %p\n", (void *) dir_data->dirp);

    closedir(dir_data->dirp);
    free(dir_data);

    // All application resources, used in the session, must be released in this callback

#if defined APP_ENABLE_MD5
    if (response_data->user_context != NULL)
    {
        // free md5 context here, if ls was issued a directory
        free(response_data->user_context);
        response_data->user_context = NULL;
    }
#endif
    return idigi_callback_continue;
}

static idigi_callback_status_t app_process_file_readdir(idigi_file_request_t const * const request_data,
                                                        idigi_file_data_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    app_dir_data_t * dir_data = request_data->handle;
    struct dirent  * result;

    switch (dvt_current_error_case)
    {
        case dvt_fs_error_ls_busy:
            if (dvt_current_state < dvt_fs_state_at_end)
            {
                status = idigi_callback_busy;
                goto done;
            }
            break;

        case dvt_fs_error_ls_middle:
            if (dvt_current_state == dvt_fs_state_at_middle)
            {
                response_data->error->errnum = (void *) EINVAL;
                response_data->error->error_status = idigi_file_invalid_parameter;
                goto done;
            }
            break;

        case dvt_fs_error_ls_end:
            if (dvt_current_state == dvt_fs_state_at_end)
            {
                response_data->error->errnum = (void *) ENOMEM;
                response_data->error->error_status = idigi_file_out_of_memory;
                goto done;
            }
            break;

        case dvt_fs_error_ls_timeout:
            {
                static time_t dvt_start_time;
                time_t dvt_current_time;

                if (dvt_current_state == dvt_fs_state_at_start)
                    time(&dvt_start_time);
            
                time(&dvt_current_time);

                if ((dvt_current_time - dvt_start_time) < DVT_FS_TIMEOUT_SECS)
                    status = idigi_callback_busy;
                else
                    response_data->error->error_status = idigi_file_user_cancel;
                goto done;
            }
        default:
            break;
    }

    // Read next directory entry, skip "." and ".."
    int rc = readdir_r(dir_data->dirp, &dir_data->dir_entry, &result);

    while(rc == 0 && result != NULL)
    {
        // if not "." or "..", we are done
        if ((strcmp(result->d_name, ".") != 0) && (strcmp(result->d_name, "..") != 0))
            break;

        rc = readdir_r(dir_data->dirp, &dir_data->dir_entry, &result);
    }

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
        size_t const name_len = strlen(result->d_name);

        APP_DEBUG("readdir_r returned directory %s\n", result->d_name);

        if(name_len < response_data->size_in_bytes)
        {
            strcpy(response_data->data_ptr, result->d_name);
            response_data->size_in_bytes = name_len + 1;
        }
        else
        {
            ASSERT(0);
            APP_DEBUG("directory name too long\n");
            status = app_process_file_error(response_data->error, ENAMETOOLONG);
        }
    }

done:
    if (dvt_current_state < dvt_fs_state_at_end)
        dvt_current_state++;
    return status;
}


static idigi_callback_status_t app_process_file_open(idigi_file_open_request_t const * const request_data,
                                                     idigi_file_open_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    int const oflag = app_convert_file_open_mode(request_data->oflag);

    // 0664 = read,write owner + read,write group + read others
    long int const fd = open(request_data->path, oflag, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

    APP_DEBUG("Open %s, %d, returned %ld\n", request_data->path, oflag, fd);
    update_error_case(request_data->path);

    if (fd < 0)
    {
        status = app_process_file_error(response_data->error, errno);
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

    APP_DEBUG("lseek fd %ld, offset %ld, origin %d returned %ld\n", fd, request_data->offset,
                                                request_data->origin, offset);
    response_data->offset = offset;

    if (offset < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }
    else
    {
        if (dvt_current_error_case != dvt_fs_error_none && 
            request_data->origin == IDIGI_SEEK_SET)
        {
            APP_DEBUG("Offset %ld\n",request_data->offset);
            if (request_data->offset < dvt_fs_case_offset_COUNT)
                dvt_current_error_case += request_data->offset;
            else
            {
                APP_DEBUG("Unexpected offset %ld for error case %d\n", request_data->offset, dvt_current_error_case);
            }
        }
    }

    return status;
}

static idigi_callback_status_t app_process_file_ftruncate(idigi_file_ftruncate_request_t const * const request_data,
                                                          idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;

    int const result = ftruncate(fd, request_data->length);

    APP_DEBUG("ftruncate %ld, %ld returned %d\n", fd, request_data->length, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }

    return status;
}

static idigi_callback_status_t app_process_file_rm(idigi_file_path_request_t const * const request_data,
                                                   idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    update_error_case(request_data->path);
    static int cleanup = 0;

    if ((dvt_current_error_case == dvt_fs_error_rm_timeout) && (cleanup == 0))
    {
        static time_t dvt_start_time = 0;
        time_t dvt_current_time;

        if (dvt_start_time == 0)
        {
            time(&dvt_start_time);
        }

        time(&dvt_current_time);

        if ((dvt_current_time - dvt_start_time) < DVT_FS_TIMEOUT_SECS)
        {
             status = idigi_callback_busy;
        }
        else
        {
            dvt_start_time = 0;
            response_data->error->error_status = idigi_file_user_cancel;
            cleanup = 1;
        }
        goto done;
    }
    cleanup = 0;

    int const result = unlink(request_data->path);

    APP_DEBUG("unlink %s returned %d\n", request_data->path, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
    }

done:
    return status;
}

static idigi_callback_status_t app_process_file_read(idigi_file_request_t const * const request_data,
                                                     idigi_file_data_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;

        static time_t dvt_start_time;
        time_t dvt_current_time;

    switch (dvt_current_error_case)
    {
        case dvt_fs_error_get_busy:
            if (dvt_current_state < dvt_fs_state_at_end)
            {
                status = idigi_callback_busy;
                goto done;
            }
            break;

        case dvt_fs_error_get_start:
            if (dvt_current_state == dvt_fs_state_at_start)
            {
                response_data->error->errnum = (void *) EACCES;
                response_data->error->error_status = idigi_file_permision_denied;
                goto done;
            }
            break;

        case dvt_fs_error_get_middle:
            if (dvt_current_state == dvt_fs_state_at_middle)
            {
                response_data->error->errnum = (void *) EINVAL;
                response_data->error->error_status = idigi_file_invalid_parameter;
                goto done;
            }
            break;

        case dvt_fs_error_get_end:
            if (dvt_current_state == dvt_fs_state_at_end)
            {
                response_data->error->errnum = (void *) ENOMEM;
                response_data->error->error_status = idigi_file_out_of_memory;
                goto done;
            }
            break;

        case dvt_fs_error_get_timeout:
            if (dvt_current_state == dvt_fs_state_at_start)
                time(&dvt_start_time);
            
            time(&dvt_current_time);

            if ((dvt_current_time - dvt_start_time) < DVT_FS_TIMEOUT_SECS)
                status = idigi_callback_busy;
            else
                response_data->error->error_status = idigi_file_user_cancel;
            goto done;

        default:
            break;
    }

    int const result = read(fd, response_data->data_ptr, response_data->size_in_bytes);

    APP_DEBUG("read %ld, %zu, returned %d\n", fd, response_data->size_in_bytes, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    if (dvt_current_state < dvt_fs_state_at_end)
        dvt_current_state++;
    return status;
}

static idigi_callback_status_t app_process_file_write(idigi_file_write_request_t const * const request_data,
                                                      idigi_file_write_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;
    static time_t dvt_start_time;
    time_t dvt_current_time;


    switch (dvt_current_error_case)
    {
    case dvt_fs_error_put_busy:

            if (dvt_current_state < dvt_fs_state_at_middle)
            {
                status = idigi_callback_busy;
                goto done;
            }
            break;

        case dvt_fs_error_put_start:
            if (dvt_current_state == dvt_fs_state_at_start)
            {
                response_data->error->errnum = (void *) EACCES;
                response_data->error->error_status = idigi_file_permision_denied;
                goto done;
            }
            break;

        case dvt_fs_error_put_middle:
            if (dvt_current_state == dvt_fs_state_at_middle)
            {
                response_data->error->errnum = (void *) EINVAL;
                response_data->error->error_status = idigi_file_invalid_parameter;
                goto done;
            }
            break;

        case dvt_fs_error_put_end:
            if (dvt_current_state == dvt_fs_state_at_end)
            {
                response_data->error->errnum = (void *) ENOMEM;
                response_data->error->error_status = idigi_file_out_of_memory;
                goto done;
            }
            break;

    case dvt_fs_error_put_timeout:
            if (dvt_current_state == dvt_fs_state_at_start)
                time(&dvt_start_time);
            
            time(&dvt_current_time);

            if ((dvt_current_time - dvt_start_time) < DVT_FS_TIMEOUT_SECS)
                status = idigi_callback_busy;
            else
                response_data->error->error_status = idigi_file_user_cancel;
            goto done;

        default:
            break;
    }


    int const result = write(fd, request_data->data_ptr, request_data->size_in_bytes);

    APP_DEBUG("write %ld, %zu, returned %d\n", fd, request_data->size_in_bytes, result);

    if (result < 0)
    {
        status = app_process_file_error(response_data->error, errno);
        goto done;
    }

    response_data->size_in_bytes = result;

done:
    if (dvt_current_state < dvt_fs_state_at_end)
        dvt_current_state++;
    return status;
}

static idigi_callback_status_t app_process_file_close(idigi_file_request_t const * const request_data,
                                                      idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    long int const fd = (long int) request_data->handle;
    int const result = close(fd);

    APP_DEBUG("close %ld returned %d\n", fd, result);

    if (result < 0 && errno == EIO)
    {
        status = app_process_file_error(response_data->error, EIO);
    }

    // All application resources, used in the session, must be released in this callback

    return status;
}

idigi_callback_status_t app_file_system_handler(idigi_data_service_request_t const request,
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

        case idigi_file_system_ftruncate:
            status = app_process_file_ftruncate(request_data, response_data);
            break;

        case idigi_file_system_rm:
            status = app_process_file_rm(request_data, response_data);
            break;

        case idigi_file_system_stat:
            status = app_process_file_stat(request_data, response_data);
            break;

        case idigi_file_system_opendir:
            status = app_process_file_opendir(request_data, response_data);
            break;

        case idigi_file_system_readdir:
            status = app_process_file_readdir(request_data, response_data);
            break;

        case idigi_file_system_closedir:
            status = app_process_file_closedir(request_data, response_data);
            break;

        case idigi_file_system_strerror:
            status = app_process_file_strerror(response_data);
            break;

        case idigi_file_system_hash:
            status = app_process_file_hash(request_data, response_data);
            break;

        case idigi_file_system_msg_error:
            status = app_process_file_msg_error(request_data, response_data);
            break;

        default:
            status = idigi_callback_unrecognized;
            APP_DEBUG("Unsupported file system request %d\n", request);
    }

    return status;
}

