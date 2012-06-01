/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

typedef enum
{
    fs_get_request_opcode = 1,
    fs_get_response_opcode,
    fs_put_request_opcode,
    fs_put_response_opcode,
    fs_ls_request_opcode,
    fs_ls_response_opcode,
    fs_rm_request_opcode,
    fs_rm_response_opcode,
    fs_error_opcode = 200
} file_system_opcode_t;


typedef struct
{
    void * handle;
    void * user_context;
    idigi_file_error_data_t error;
    union
    {
        struct
        {
            uint32_t bytes_done;
            uint32_t data_length;
            uint32_t offset;
        } f;
        struct
        {
            char * path;
            size_t path_len;
            size_t file_size;
            uint32_t last_modified;
            idigi_file_hash_algorithm_t hash_alg;
        } d;
    }data;
    file_system_opcode_t opcode;
    idigi_callback_status_t status;
    uint8_t flags;
    uint8_t state;

} file_system_context_t;


#define IDIGI_FILE_TRUNC            0x01
#define IDIGI_LSEEK_DONE            0x02
#define IDIGI_FILE_ERROR_HINT       0x80

#define FILE_STATE_NONE             0
#define FILE_STATE_READDIR_DONE     1
#define FILE_STATE_STAT_DONE        2
#define FILE_STATE_CLOSING          3

#define FILE_OPCODE_BYTES           1

static const char file_str_etoolong[] = "File path too long";
static const char file_str_invalid_offset[] = "Invalid offset";
static const char file_str_request_format_error[] = "Request format error";

#define FsIsBitSet(flag, bit)  (((flag) & (bit)) == (bit))
#define FsBitSet(flag, bit)    ((flag) |= (bit))
#define FsBitClear(flag, bit)  ((flag) &= ~(bit))

#define FileIsDir(context)    FsIsBitSet(context->flags, IDIGI_FILE_IS_DIR)
#define FileIsReg(context)    FsIsBitSet(context->flags, IDIGI_FILE_IS_REG)

#define FileNeedTrunc(context)  FsIsBitSet(context->flags, IDIGI_FILE_TRUNC)
#define FileClearTrunc(context) FsBitClear(context->flags, IDIGI_FILE_TRUNC) 

#define FileLseekDone(context)    FsIsBitSet(context->flags, IDIGI_LSEEK_DONE)
#define FileSetLseekDone(context) FsBitSet(context->flags, IDIGI_LSEEK_DONE) 

#define FileHasErrorHint(context) FsIsBitSet(context->flags, IDIGI_FILE_ERROR_HINT) 
#define FileSetErrorHint(context) FsBitSet(context->flags, IDIGI_FILE_ERROR_HINT) 

#define FileSetState(context, s) (context->state = s)
#define FileGetState(context)    (context->state)


#define fileOperationSuccess(status, context) (status==idigi_callback_continue && context->error.error_status==idigi_file_noerror)

static void set_file_system_service_error(msg_service_request_t * const service_request, idigi_msg_error_t const msg_error)
{
    service_request->error_value = msg_error;
    service_request->service_type = msg_service_type_error;
}

static void format_file_error_msg(idigi_data_t * const idigi_ptr,
                                  msg_service_request_t * const service_request)
{
     /* 1st message so let's parse message-start packet:
      *
      * File System Get request format:
      *  ------------------------------------------------------
      * |   0    |     +1     |  +1               |    +N      | 
      *  ------------------------------------------------------
      * | Opcode | Error code | Error Hint Length | Error hint | 
      *  ------------------------------------------------------
      *
      */
     enum  {
         field_define(fs_error_response, opcode,         uint8_t),
         field_define(fs_error_response, error_code,     uint8_t),
         field_define(fs_error_response, error_hint_len, uint8_t),
         record_end(fs_error_response_header)
     };

     msg_session_t * const session = service_request->session;
     file_system_context_t * const context = session->service_context;
     msg_service_data_t * const service_data = service_request->need_data;

     size_t const header_bytes = record_bytes(fs_error_response_header);
     size_t const buffer_size  = MIN_VALUE(service_data->length_in_bytes - header_bytes, UCHAR_MAX);
     uint8_t * fs_error_response = service_data->data_ptr;
     uint8_t error_code  = context->error.error_status;
     uint8_t error_hint_len = 0;

     /* don't send an error response, of thession is canceled */
     if (error_code == idigi_file_user_cancel)
         goto done;

     error_code -= idigi_file_user_cancel;

     message_store_u8(fs_error_response, opcode, fs_error_opcode);
     message_store_u8(fs_error_response, error_code, error_code);

     service_data->length_in_bytes = header_bytes;
     MsgSetLastData(service_data->flags);

     if (context->error.errnum == NULL)
         goto done;

     if (FileHasErrorHint(context))
     {
        error_hint_len = MIN_VALUE(strlen(context->error.errnum), buffer_size);
        memcpy(fs_error_response + header_bytes, context->error.errnum, error_hint_len);
     }
     else
     {
        idigi_file_data_response_t response;
        size_t response_length = sizeof response;

        idigi_request_t request_id;
        request_id.file_system_request = idigi_file_system_strerror;

        response.error    = &context->error;
        response.data_ptr = fs_error_response + header_bytes;
        response.size_in_bytes = buffer_size;

        response.user_context = context->user_context;

        idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                       NULL, 0, &response, &response_length);

        context->user_context = response.user_context;

        if (response_length != sizeof response || response.size_in_bytes > buffer_size)
        {
            /* wrong size returned and let's cancel the request */
            notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
            set_file_system_service_error(service_request, idigi_msg_error_cancel);
        }
        else
        {
            error_hint_len = response.size_in_bytes;
        }
     }
     message_store_u8(fs_error_response, error_hint_len, error_hint_len);

     if (error_hint_len != 0)
        service_data->length_in_bytes += error_hint_len;

     MsgSetLastData(service_data->flags);
done:
     return;
}

static idigi_callback_status_t call_file_system_user(idigi_data_t * const idigi_ptr,
                                                    msg_service_request_t * const service_request,
                                                    idigi_file_system_request_t const fs_request_id,
                                                    void const * const request_data,
                                                    size_t const request_length,
                                                    void * const response_data,
                                                    size_t const response_length)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_file_response_t * const response = response_data;
    void const * const old_errnum = context->error.errnum;
    idigi_msg_error_t  msg_error = idigi_msg_error_none;
    size_t response_length_in = response_length;
    idigi_callback_status_t status;
    idigi_request_t request_id;

    request_id.file_system_request = fs_request_id;

    response->user_context = context->user_context;
    response->error = &context->error;

    status = idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                            request_data, request_length, response, &response_length_in);

    context->user_context = response->user_context;

 
    switch (status)
    {
        case idigi_callback_continue:
            if (response_length != response_length_in)
            {
                /* wrong size returned and let's cancel the request */
                msg_error = idigi_msg_error_cancel;
                notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
            }
            else
            if (context->error.error_status == idigi_file_user_cancel)
                msg_error = idigi_msg_error_cancel;
            break;

        case idigi_callback_busy:
            context->error.errnum = old_errnum;
            break;

        case idigi_callback_unrecognized:
            status = idigi_callback_continue; /* fall through */

        case idigi_callback_abort:
            msg_error = idigi_msg_error_cancel;
            break;
    }

    if (msg_error != idigi_msg_error_none)
    {
        context->error.error_status = idigi_file_user_cancel;
        set_file_system_service_error(service_request, msg_error);
    }
    context->status = status;
    return status;
}

static size_t file_hash_size(idigi_file_hash_algorithm_t const hash_alg)
{
    size_t result;

    switch(hash_alg)
    {
        case idigi_file_hash_md5:
            result = 16;
            break;
        case idigi_file_hash_crc32:
            result = 4;
            break;
        default:
            result = 0;
    }
    return result;
}

static idigi_callback_status_t call_file_stat_user(idigi_data_t * const idigi_ptr, 
                                                   msg_service_request_t * const service_request,
                                                   char const * const path,
                                                   idigi_file_hash_algorithm_t const hash_alg)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_file_stat_request_t request; 
    idigi_file_stat_response_t response;
    idigi_callback_status_t status;

    response.statbuf.flags = 0;
    response.statbuf.hash_alg = idigi_file_hash_none;

    request.path = path;
    request.hash_alg = hash_alg;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_stat,
                                    &request,  sizeof request,
                                    &response, sizeof response);

    if (fileOperationSuccess(status, context))
    {
        context->data.d.file_size = response.statbuf.file_size;
        context->data.d.last_modified = response.statbuf.last_modified;
        context->flags = response.statbuf.flags;
    
        switch(hash_alg)
        {
            /* don't overwrite existing value for directory */
            case idigi_file_hash_none:
                 goto done;

            case idigi_file_hash_best:
                switch(response.statbuf.hash_alg)
                {
                    case idigi_file_hash_md5:
                    case idigi_file_hash_crc32:
                    case idigi_file_hash_none:
                        context->data.d.hash_alg = response.statbuf.hash_alg;
                        break;
                    default:
                        context->data.d.hash_alg = idigi_file_hash_none;
                        break;
                }
                break;

            default:
                if (hash_alg != response.statbuf.hash_alg)
                    context->data.d.hash_alg = idigi_file_hash_none;
                break;
        }

        if (context->data.d.hash_alg != idigi_file_hash_none)
        {
            if (!FileIsDir(context) && !FileIsReg(context))
                context->data.d.hash_alg = idigi_file_hash_none;
        }
    }
done:
    return status;
}

static idigi_callback_status_t call_file_opendir_user(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request,
                                                       char const * const path)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_file_path_request_t request; 
    idigi_file_open_response_t response;
    idigi_callback_status_t   status;

    request.path = path;
    response.handle = NULL;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_opendir, 
                                    &request,  sizeof request,
                                    &response, sizeof response);

    if (fileOperationSuccess(status, context))
    {
        context->handle = response.handle;
        if (context->handle == NULL)
        {
            idigi_request_t request_id;
            request_id.file_system_request = idigi_file_system_opendir;

            context->error.error_status = idigi_file_user_cancel;
            notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data);
        }
    }

    return status;
}

static idigi_callback_status_t call_file_readdir_user(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request,
                                                       char * const path,
                                                       size_t const buffer_size)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_file_request_t request; 
    idigi_file_data_response_t response;
    idigi_callback_status_t  status;
 
    request.handle = context->handle;
    response.data_ptr = path;
    response.size_in_bytes = buffer_size;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_readdir, 
                                    &request,  sizeof request,
                                    &response, sizeof response);
    if (response.size_in_bytes > buffer_size)
    {
        /* wrong size returned and let's cancel the request */
        idigi_request_t request_id;
        request_id.file_system_request = idigi_file_system_readdir;
        ASSERT(0);
        context->error.error_status = idigi_file_user_cancel;
        set_file_system_service_error(service_request, idigi_msg_error_cancel);
        notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
        goto done;
    }

    if (fileOperationSuccess(status, context))
    {
        if (response.size_in_bytes == 0)
            *path = '\0';
    }
done:
    return status;
}

static idigi_callback_status_t call_file_close_user(idigi_data_t * const idigi_ptr,
                                                  msg_service_request_t * const service_request,
                                                  idigi_file_system_request_t const fs_request_id)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status = context->status;

    if (context->handle != NULL)
    {
        idigi_file_error_data_t const old_error_data = context->error;
        idigi_file_request_t request;
        idigi_file_response_t response;

        request.handle = context->handle;
        FileSetState(context, FILE_STATE_CLOSING);
 
        status = call_file_system_user(idigi_ptr, service_request, fs_request_id,
                                        &request,  sizeof request,
                                        &response, sizeof response);

        if (status != idigi_callback_busy)
        {
            context->handle = NULL;

            /* Don't overwrite existing abort status in close */
            if (context->status != idigi_callback_continue)
                status = context->status;

            /* Don't overwrite existing error status in close */
            if (old_error_data.error_status != idigi_file_noerror)
                context->error = old_error_data;
        }
    }
    return status;
}

static idigi_callback_status_t call_file_hash_user(idigi_data_t * const idigi_ptr, 
                                                   msg_service_request_t * const service_request,
                                                   char const * const path, 
                                                   uint8_t * const hash_ptr)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;

    idigi_file_stat_request_t request;
    idigi_file_data_response_t response;

    if (context->data.d.hash_alg == idigi_file_hash_none || FileIsDir(context))
        goto done;
    
    response.size_in_bytes = file_hash_size(context->data.d.hash_alg);

    memset(hash_ptr, 0, response.size_in_bytes);

    if (!FileIsReg(context))
        goto done;

    request.path = path;
    request.hash_alg = context->data.d.hash_alg;
    response.data_ptr = hash_ptr;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_hash,
                                   &request,  sizeof request,
                                   &response, sizeof response);
done:
    return status;
}

static idigi_callback_status_t call_file_open_user(idigi_data_t * const idigi_ptr, 
                                                   msg_service_request_t * const service_request,
                                                   char const * const path, 
                                                   int const oflag)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t      status;
    idigi_file_open_request_t    request;
    idigi_file_open_response_t   response;

    request.path  = path;
    request.oflag = oflag;
    response.handle = NULL;
    
    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_open, 
                                   &request,  sizeof request,
                                   &response, sizeof response);

    if (fileOperationSuccess(status, context))
        context->handle = response.handle;

    return status;
}

static idigi_callback_status_t call_file_lseek_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request, 
                                                     long int const  offset_in,
                                                     int  const origin,
                                                     long int * const offset_out)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;

    idigi_callback_status_t status;
    idigi_file_lseek_request_t request;
    idigi_file_lseek_response_t response;
    
    request.handle = context->handle;
    request.offset = offset_in;
    request.origin = origin;
    response.offset = -1;
    *offset_out = -1;
 
    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_lseek, 
                                   &request, sizeof request,
                                   &response, sizeof response);
    *offset_out = response.offset;

    return status;
}


static idigi_callback_status_t call_file_ftruncate_user(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request) 
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status;
    idigi_file_ftruncate_request_t request;
    idigi_file_response_t response;

    request.handle = context->handle;
    request.length = context->data.f.offset;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_ftruncate, 
                                   &request, sizeof request,
                                   &response, sizeof response);

    return status;
}

static idigi_callback_status_t call_file_rm_user(idigi_data_t * const idigi_ptr,
                                                 msg_service_request_t * const service_request,
                                                 char const * const path) 
{
    idigi_callback_status_t status;
    idigi_file_path_request_t request; 
    idigi_file_response_t response;

    request.path = path;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_rm,
                                   &request, sizeof request,
                                   &response, sizeof response);

    return status;
}

static idigi_callback_status_t call_file_read_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request,
                                                     void   * const buffer,
                                                     size_t * const buffer_size)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status;
    idigi_file_request_t request;
    idigi_file_data_response_t response;

    request.handle = context->handle;
    response.data_ptr = buffer;
    response.size_in_bytes = *buffer_size;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_read,
                                   &request, sizeof request,
                                   &response, sizeof response);

    if (response.size_in_bytes > *buffer_size)
    {
        /* wrong size returned and let's cancel the request */
        idigi_request_t request_id;
        request_id.file_system_request = idigi_file_system_read;

        ASSERT(0);
        context->error.error_status = idigi_file_user_cancel;
        set_file_system_service_error(service_request, idigi_msg_error_cancel);
        notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
        goto done;
    }

    *buffer_size = response.size_in_bytes;

done:
    return status;
}

static idigi_callback_status_t call_file_write_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request,
                                                     void const * const buffer,
                                                     size_t * const bytes_done)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status;
    idigi_file_write_request_t request;
    idigi_file_write_response_t response;

    request.handle = context->handle;
    request.data_ptr = buffer;
    request.size_in_bytes = *bytes_done;
    response.size_in_bytes = *bytes_done;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_write,
                                   &request, sizeof request,
                                   &response, sizeof response);

    if (fileOperationSuccess(status, context))
    {
        *bytes_done = response.size_in_bytes;

        if (response.size_in_bytes == 0 && request.size_in_bytes != 0)
        {
            status = idigi_callback_busy;
        }
    }

    return status;
}

static size_t parse_file_path(file_system_context_t * const context, 
                              void const * const path_ptr, 
                              size_t const buffer_size)
{
    char const * const path = path_ptr;
    size_t path_len = strlen(path);

    if (path_len > buffer_size)
        path_len = 0;
    else
    if (path_len != 0 && path[path_len] == '\0')
    {
        path_len++;
    }

    if (path_len == 0)
    {
        ASSERT(idigi_false);
        context->error.error_status = idigi_file_request_format_error;
        context->error.errnum = file_str_request_format_error;
        FileSetErrorHint(context);
    }

    return path_len;
}

static size_t parse_file_get_header(file_system_context_t * const context, 
                                    uint8_t const * const header_ptr, 
                                    size_t const buffer_size)
{
    
    /* 1st message so let's parse message-start packet:
     *
     * File System Get request format:
     *  -------------------------------------
     * |   0    |   N    |  +4    |    +4    | 
     *  --------------------------------------
     * | Opcode | Path   | Offset | Length   | 
     *  -------------------------------------
     *
     */

    enum fs_get_request {
        field_define(fs_get_request, offset, uint32_t),
        field_define(fs_get_request, length, uint32_t),
        record_end(fs_get_request_header)
    };
    uint8_t const * fs_get_request = header_ptr + FILE_OPCODE_BYTES;
    size_t  const header_len = record_bytes(fs_get_request_header) + FILE_OPCODE_BYTES;

    size_t len = parse_file_path(context, fs_get_request, buffer_size - header_len);
 
    if (len != 0)
    {
        fs_get_request += len;
        context->data.f.offset = message_load_be32(fs_get_request, offset);
        context->data.f.data_length = message_load_be32(fs_get_request, length);
        len += header_len;
    }
    return len;
}

static idigi_callback_status_t process_file_get_request(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    msg_service_data_t * const service_data = service_request->have_data;
    idigi_callback_status_t status = idigi_callback_continue;

    if (parse_file_get_header(context, service_data->data_ptr, service_data->length_in_bytes) == 0)
        goto done;

    if (context->handle == NULL)
    {
        char const * path = service_data->data_ptr;
        path += FILE_OPCODE_BYTES;

        status = call_file_open_user(idigi_ptr, service_request, path, IDIGI_O_RDONLY);
    }
done:
    return status;
}

static idigi_callback_status_t set_file_position(idigi_data_t * const idigi_ptr,
                                                 msg_service_request_t * const service_request)

{
    idigi_callback_status_t status;
    long int ret;

    /* Check that offset is inside the file
       Some systems crash, when trying to set offset outside the file 
     */
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    
    status = call_file_lseek_user(idigi_ptr, service_request, 0, IDIGI_SEEK_END, &ret);
    if (status == idigi_callback_busy || !fileOperationSuccess(status, context))
        goto done;

    if (ret == -1 || (uint32_t) ret < context->data.f.offset)
    {
        ret = -1;
        goto error;
    }

    status = call_file_lseek_user(idigi_ptr, service_request, (long int ) context->data.f.offset, IDIGI_SEEK_SET, &ret);
    if (status == idigi_callback_busy || !fileOperationSuccess(status, context))
        goto done;

error:
    if (ret == -1)
    {
        context->error.error_status = idigi_file_invalid_parameter;
        context->error.errnum = file_str_invalid_offset;
        FileSetErrorHint(context);
    }

done:
    return status;
}


static idigi_callback_status_t process_file_get_response(idigi_data_t * const idigi_ptr,
                                                          msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    msg_service_data_t * const service_data = service_request->need_data;
    idigi_callback_status_t status = idigi_callback_continue;

    if ((context->error.error_status != idigi_file_noerror) || (FileGetState(context) == FILE_STATE_CLOSING))
       goto close_file;

    {
        uint8_t * data_ptr = service_data->data_ptr;

        size_t buffer_size = service_data->length_in_bytes;
        size_t bytes_read = 0;
        size_t bytes_to_read;

        if (MsgIsStart(service_data->flags))
        {
           if (!FileLseekDone(context) && (context->data.f.offset != 0))
           {
                status = set_file_position(idigi_ptr, service_request);
                if (status == idigi_callback_busy)
                    goto done;

                if (!fileOperationSuccess(status, context))
                    goto close_file;

                FileSetLseekDone(context);
           }

           *data_ptr++ = fs_get_response_opcode;
           buffer_size--;
        }
        /* bytes to read in this callback */
        bytes_to_read = MIN_VALUE(buffer_size, context->data.f.data_length - context->data.f.bytes_done);

        while (bytes_to_read > 0)
        {
            size_t cnt = bytes_to_read;
            status = call_file_read_user(idigi_ptr, service_request, data_ptr, &cnt);

            if (status == idigi_callback_busy)
            {
                if (bytes_read > 0)
                    status = idigi_callback_continue;  /* Return what's read already */
                break;
            }
            if (!fileOperationSuccess(status, context))
                goto close_file;

            if (cnt > 0) 
            {
                data_ptr += cnt;
                bytes_to_read -= cnt;
                bytes_read += cnt;
            }
            else
            {
                bytes_to_read = 0;
                MsgSetLastData(service_data->flags);
            }
        }
        context->data.f.bytes_done += bytes_read;
        service_data->length_in_bytes = bytes_read;

        if (MsgIsStart(service_data->flags))
            service_data->length_in_bytes++; /* opcode */

        if (context->data.f.data_length == context->data.f.bytes_done)
            MsgSetLastData(service_data->flags);

        if (!MsgIsLastData(service_data->flags))
            goto done;
    }

close_file:
    status = call_file_close_user(idigi_ptr, service_request, idigi_file_system_close);
    if (status == idigi_callback_busy)
        goto done;

    if ((context->error.error_status > idigi_file_user_cancel) &&
        (status == idigi_callback_continue))
    {
        /* not 1st response - too late to send error code */
        if (!MsgIsStart(service_data->flags))
            set_file_system_service_error(service_request, idigi_msg_error_cancel);
        else
            format_file_error_msg(idigi_ptr, service_request);
    }

done:
    return status;
}

static idigi_callback_status_t process_file_rm_request(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    msg_service_data_t * const service_data = service_request->have_data;
    char const * path = service_data->data_ptr;
    idigi_callback_status_t status = idigi_callback_continue;
    
    path += FILE_OPCODE_BYTES;

    if (parse_file_path(session->service_context, path, service_data->length_in_bytes - FILE_OPCODE_BYTES) == 0) 
        goto done;

    status = call_file_rm_user(idigi_ptr, service_request, path);

done:
    return status;
}

static idigi_callback_status_t process_file_response_nodata(idigi_data_t * const idigi_ptr,
                                                         msg_service_request_t * const service_request,
                                                         file_system_opcode_t const opcode)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    msg_service_data_t * const service_data = service_request->need_data;

    if (context->error.error_status == idigi_file_noerror)
    {
        uint8_t * const data_ptr = service_data->data_ptr;
        
        *data_ptr = opcode;

        service_data->length_in_bytes = sizeof *data_ptr;
        MsgSetLastData(service_data->flags);
    }
    else
    {
        format_file_error_msg(idigi_ptr, service_request);
    }
    return idigi_callback_continue;
}

static size_t parse_file_put_header(file_system_context_t * const context, 
                                    uint8_t const * const header_ptr, 
                                    size_t const buffer_size)
{
    /* 
     *
     * File System Put request format:
     *  --------------------------------------------
     * |   0    |   N    |  +1   |  +4    |   N     | 
     *  --------------------------------------------
     * | Opcode | Path   | Flags | Offset | Payload | 
     *  --------------------------------------------
     *
     */

    enum {
        field_define(fs_put_request, flags,  uint8_t),
        field_define(fs_put_request, offset, uint32_t),
        record_end(fs_put_request_header)
    };
    uint8_t const *fs_put_request = header_ptr + FILE_OPCODE_BYTES;
    size_t  const header_len = record_bytes(fs_put_request_header) + FILE_OPCODE_BYTES;

    size_t len = parse_file_path(context, fs_put_request, buffer_size - header_len);

    if (len != 0)
    {
        fs_put_request += len;
        context->flags  |= message_load_u8(fs_put_request, flags);
        context->data.f.offset = message_load_be32(fs_put_request, offset);
        len   += header_len;
    }

    return len;
}


static idigi_callback_status_t process_file_put_request(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;

    if ((context->error.error_status != idigi_file_noerror) || (FileGetState(context) == FILE_STATE_CLOSING))
       goto close_file;

    {
        msg_service_data_t * const service_data = service_request->have_data;
        uint8_t const * data_ptr = service_data->data_ptr;
        size_t bytes_to_write = service_data->length_in_bytes;
        size_t bytes_written = 0;

        if (MsgIsStart(service_data->flags))
        {
            size_t header_len = parse_file_put_header(context, data_ptr, service_data->length_in_bytes); 
            if (header_len == 0)
                goto done;

            if (context->handle == NULL)
            {
                int open_flags = IDIGI_O_WRONLY | IDIGI_O_CREAT;
                char const * path = service_data->data_ptr;
                path += FILE_OPCODE_BYTES;

                if (FileNeedTrunc(context))
                {
                    if (context->data.f.offset == 0)
                    {
                        open_flags |= IDIGI_O_TRUNC;
                        FileClearTrunc(context);
                    }
                }

                status = call_file_open_user(idigi_ptr, service_request, path, open_flags);
                if (context->handle == NULL)
                    goto done;
            }

            if (!FileLseekDone(context) && (context->data.f.offset != 0))
            {
                status = set_file_position(idigi_ptr, service_request);
                if (status == idigi_callback_busy)
                    goto done;

                if (!fileOperationSuccess(status, context))
                    goto close_file;

                FileSetLseekDone(context);
            }
            data_ptr  += header_len;
            bytes_to_write -= header_len;
        }

        if (context->data.f.bytes_done > bytes_to_write)
        {
            idigi_request_t request_id;
            request_id.file_system_request = idigi_file_system_write;

            context->error.error_status = idigi_file_user_cancel;
            notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
            set_file_system_service_error(service_request, idigi_msg_error_cancel);
            
            ASSERT_GOTO(idigi_false, close_file);
        }

        data_ptr += context->data.f.bytes_done;
        bytes_to_write -= context->data.f.bytes_done;

        while(bytes_to_write > 0)
        {
            size_t cnt = bytes_to_write;
            status = call_file_write_user(idigi_ptr, service_request, data_ptr, &cnt);

            if (status == idigi_callback_busy)
            {
                context->data.f.bytes_done += bytes_written;
                context->data.f.offset  += bytes_written;
                goto done;
            }
            if (!fileOperationSuccess(status, context))
               goto close_file;

            data_ptr       += cnt;
            bytes_to_write -= cnt;
            bytes_written  += cnt;
        }
        context->data.f.bytes_done = 0;
        context->data.f.offset  += bytes_written;

        if (!MsgIsLastData(service_data->flags))
            goto done;

        if (FileNeedTrunc(context))
        {
            status = call_file_ftruncate_user(idigi_ptr, service_request);
            if (status == idigi_callback_busy)
                goto done;
        }
    }
close_file:
    status = call_file_close_user(idigi_ptr, service_request, idigi_file_system_close);

done:
    return status;
}

static size_t parse_file_ls_header(file_system_context_t * const context,
                                   uint8_t const * const header_ptr, 
                                   size_t const buffer_size)
{
    /* 
     * File System Ls request format:
     *  ----------------------------
     * |   0    |   N    |   1      | 
     *  ----------------------------
     * | Opcode | Path   | hash alg |  
     *  ----------------------------
     */
    uint8_t const * data_ptr = header_ptr + FILE_OPCODE_BYTES;
    const size_t header_len = 1 + FILE_OPCODE_BYTES;
    idigi_file_hash_algorithm_t hash_alg;

    size_t len = parse_file_path(context, data_ptr, buffer_size - header_len);
    if (len != 0)
    {
        data_ptr += len;
        len += header_len;

        hash_alg = *data_ptr;

        switch (hash_alg)
        {
            case idigi_file_hash_crc32:
            case idigi_file_hash_md5:
            case idigi_file_hash_best:
            case idigi_file_hash_none:
                context->data.d.hash_alg = hash_alg;
                break;

            default:
                ASSERT(idigi_false);
                context->error.error_status = idigi_file_request_format_error;
                context->error.errnum = file_str_request_format_error;
                FileSetErrorHint(context);

                len = 0;
        }
    }
    return len;
}

static size_t format_file_ls_response_header(idigi_file_hash_algorithm_t const hash_alg, 
                                             size_t const hash_size, 
                                             uint8_t * const data_ptr)
{
   /* 
     * File System Ls response header format:
     *  --------------------------------
     * |   1    |    1     |    1       | 
     *  --------------------------------
     * | opcode | hash_alg | hash_bytes |
     *  --------------------------------
     */ 

    enum {
        field_define(fs_ls_response, opcode,     uint8_t),
        field_define(fs_ls_response, hash_alg,   uint8_t),
        field_define(fs_ls_response, hash_bytes, uint8_t),
        record_end(fs_ls_response_header)
    };

    uint8_t * fs_ls_response = data_ptr;

    message_store_u8(fs_ls_response, opcode,     fs_ls_response_opcode);
    message_store_u8(fs_ls_response, hash_alg,   hash_alg);
    message_store_u8(fs_ls_response, hash_bytes, hash_size);

    return record_bytes(fs_ls_response_header);
}

static size_t file_ls_resp_header_size(void)
{
    /* 
     * File System Ls request format:
     *  ----------------------------
     * |   1    |     4    |   0/4  | 
     *  ----------------------------
     * | flags  | last     | size   | 
     * |        | modified |        |
     *  ----------------------------
     */

    enum  {
        field_define(fs_ls_response, flags,  uint8_t),
        field_define(fs_ls_response, last_modified, uint32_t),
        field_define(fs_ls_response, size, uint32_t),
        record_end(fs_ls_response_file)
    };
    return record_bytes(fs_ls_response_file);
}

static size_t format_file_ls_response(file_system_context_t const * context,
                                      char const * path,
                                      size_t const path_len,
                                      uint8_t * const data_ptr)
{
    /* 
     * File System Ls request format:
     *  ----------------------------
     * |   1    |     4    |   0/4  | 
     *  ----------------------------
     * | flags  | last     | size   | 
     * |        | modified |        |
     *  ----------------------------
     */

    enum  {
        field_define(fs_ls_response, flags,  uint8_t),
        field_define(fs_ls_response, last_modified, uint32_t),
        record_end(fs_ls_response_dir)
    };
    enum  {
        field_define(fs_ls_response, size, uint32_t),
        record_end(fs_ls_response_file)
    };

    uint8_t * fs_ls_response = data_ptr + path_len;
    uint8_t const flags = FileIsDir(context) ? IDIGI_FILE_IS_DIR : 0;
    size_t result;

    memcpy(data_ptr, path, path_len);

    message_store_u8(fs_ls_response,   flags, flags);
    message_store_be32(fs_ls_response, last_modified, context->data.d.last_modified);
    result = path_len + record_bytes(fs_ls_response_dir);

    if (!FileIsDir(context))
    {
        fs_ls_response += record_bytes(fs_ls_response_dir);
        message_store_be32(fs_ls_response, size, context->data.d.file_size);
        result += record_bytes(fs_ls_response_file);
    }

    return result;
}

static idigi_callback_status_t file_store_path(idigi_data_t * const idigi_ptr, 
                                               file_system_context_t * const context, 
                                               char const * const path)
{
    idigi_callback_status_t status = idigi_callback_continue;
    size_t path_len = strlen(path);
    void *ptr;

    if (FileIsDir(context) && path[path_len - 1] != '/')
    {
        path_len++;
    }

    if (path_len >= IDIGI_FILE_SYSTEM_MAX_PATH_LENGTH)
    {
        ASSERT(idigi_false);
        context->error.error_status = idigi_file_out_of_memory;
        context->error.errnum = file_str_etoolong;
        FileSetErrorHint(context);
        goto done;
    }

    status = malloc_data(idigi_ptr, IDIGI_FILE_SYSTEM_MAX_PATH_LENGTH, &ptr);
    if (status != idigi_callback_continue)
        goto done;

    context->data.d.path = ptr;
    memcpy(context->data.d.path, path, path_len + 1);

    if (FileIsDir(context) && (path[path_len - 1] != '/'))
    {
        context->data.d.path[path_len - 1] = '/';
        context->data.d.path[path_len] = '\0';
    }
    context->data.d.path_len = path_len;

done:
    return status;
}

static idigi_callback_status_t process_file_ls_request(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    msg_service_data_t    * const service_data = service_request->have_data;
    char const * path = service_data->data_ptr;
    idigi_callback_status_t status = idigi_callback_continue;

    path += FILE_OPCODE_BYTES;
    
    if (context->error.error_status != idigi_file_noerror)
        goto done;

    if (parse_file_ls_header(context, service_data->data_ptr, service_data->length_in_bytes) == 0)
        goto done;
     
    if (FileGetState(context) < FILE_STATE_STAT_DONE)
    {
        status = call_file_stat_user(idigi_ptr, service_request, path, context->data.d.hash_alg);
        if ((status == idigi_callback_busy) || !fileOperationSuccess(status, context))
            goto done;

        FileSetState(context, FILE_STATE_STAT_DONE);
    }
    if (FileIsDir(context))
    {
        status = call_file_opendir_user(idigi_ptr, service_request, path);
        if ((status == idigi_callback_busy) || !fileOperationSuccess(status, context))
            goto done;
        
        /* to read next dir entry */
        FileSetState(context, FILE_STATE_NONE);
    }
    
    /* call_file_stat_user must be done before store path, to strcat '/' to dir path */ 
    status = file_store_path(idigi_ptr, context, path);
            
done:
    return status;
}

static idigi_callback_status_t process_file_ls_response(idigi_data_t * const idigi_ptr, 
                                                        msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    msg_service_data_t    * const service_data = service_request->need_data;
    idigi_callback_status_t status = idigi_callback_continue;

    if ((context->error.error_status != idigi_file_noerror) || (FileGetState(context) == FILE_STATE_CLOSING))
       goto close_dir;
    {
        uint8_t * data_ptr = service_data->data_ptr;
        size_t buffer_size = service_data->length_in_bytes;
        size_t    resp_len    = 0;

        size_t const header_len = file_ls_resp_header_size();
        size_t hash_len   = file_hash_size(context->data.d.hash_alg);
        char * file_path;
        size_t file_path_len;

        if (MsgIsStart(service_data->flags))
        {
            resp_len = format_file_ls_response_header(context->data.d.hash_alg, hash_len, data_ptr);
            data_ptr += resp_len;
            buffer_size -= resp_len;
        }

        if (context->handle == NULL)
        {
            /* ls command was issued for a single file */
            file_path_len = context->data.d.path_len + 1;
            file_path     = context->data.d.path;

            if ((file_path_len + header_len + hash_len) > buffer_size)
            {
                context->error.error_status = idigi_file_out_of_memory;
                context->error.errnum = file_str_etoolong;
                FileSetErrorHint(context);
                ASSERT_GOTO(idigi_false, close_dir);
            }

            if (hash_len != 0)   
            {
                uint8_t * const hash_ptr = data_ptr + file_path_len + header_len;
                status = call_file_hash_user(idigi_ptr, service_request, file_path, hash_ptr);
                if (status == idigi_callback_busy)
                    goto done;

                if (!fileOperationSuccess(status, context))
                    goto close_dir;

                resp_len += hash_len;
            }

            MsgSetLastData(service_data->flags);
        }
        else
        {
            /* ls command was issued for a directory */
            file_path = context->data.d.path + context->data.d.path_len;

            if (FileGetState(context) < FILE_STATE_READDIR_DONE)
            {
                /* read next dir entry */
                size_t const path_max = MIN_VALUE((buffer_size - (header_len + hash_len)),
                                            (IDIGI_FILE_SYSTEM_MAX_PATH_LENGTH - context->data.d.path_len));
            
                status = call_file_readdir_user(idigi_ptr, service_request, file_path, path_max);
                if (status == idigi_callback_busy)
                    goto done;

                if (!fileOperationSuccess(status, context))
                    goto close_dir;

                if (*file_path == '\0')
                {
                    /* all entries processed */
                    MsgSetLastData(service_data->flags);
                    service_data->length_in_bytes = resp_len;
                    goto close_dir;
                }
                FileSetState(context, FILE_STATE_READDIR_DONE);
            }

            file_path_len = strlen(file_path) + 1;

            if (FileGetState(context) < FILE_STATE_STAT_DONE)
            {
                if ((context->data.d.path_len + file_path_len) > IDIGI_FILE_SYSTEM_MAX_PATH_LENGTH)
                {
                    context->error.error_status = idigi_file_out_of_memory;
                    context->error.errnum = file_str_etoolong;
                    FileSetErrorHint(context);
                    ASSERT_GOTO(idigi_false, close_dir);
                }
                /* strcat file name to after directory path */
                memcpy(context->data.d.path + context->data.d.path_len, file_path, file_path_len);

                status = call_file_stat_user(idigi_ptr, service_request, context->data.d.path, idigi_file_hash_none);
                if (status == idigi_callback_busy)
                    goto done;

                if (!fileOperationSuccess(status, context))
                    goto close_dir;
 
                FileSetState(context, FILE_STATE_STAT_DONE);
            }
            if (FileIsDir(context))
                hash_len = 0;
 
            if (hash_len != 0)
            {
                uint8_t * const hash_ptr = data_ptr + file_path_len + header_len;

                status = call_file_hash_user(idigi_ptr, service_request, context->data.d.path, hash_ptr);
                if (status == idigi_callback_busy)
                    goto done;

                if (!fileOperationSuccess(status, context))
                    goto close_dir;

                resp_len += hash_len;
            }

            /* to read next dir entry */
            FileSetState(context, FILE_STATE_NONE);
        }
        resp_len += format_file_ls_response(context, file_path, file_path_len, data_ptr);
        service_data->length_in_bytes = resp_len;
        goto done;
    }

close_dir:
    if (context->handle != NULL)
        status = call_file_close_user(idigi_ptr, service_request, idigi_file_system_closedir);

    if (context->error.error_status > idigi_file_user_cancel && 
        status == idigi_callback_continue)
    {
         /* not 1st response - too late to send error code */
        if (!MsgIsStart(service_data->flags))
            set_file_system_service_error(service_request, idigi_msg_error_cancel);
        else
        {
            format_file_error_msg(idigi_ptr, service_request);
        }
    }
done:
    return status;
}


static idigi_callback_status_t allocate_file_context(idigi_data_t * const idigi_ptr,
                                              file_system_opcode_t const opcode,
                                              file_system_context_t * * const result)
{
    file_system_context_t * context = NULL;
    idigi_callback_status_t status;

    void * ptr;

    status = malloc_data(idigi_ptr, sizeof *context, &ptr);
    if (status != idigi_callback_continue)
        goto done;

    context = ptr;

    context->handle = NULL;
    context->user_context = NULL;
    context->flags = 0;
    context->state = FILE_STATE_NONE;
    context->error.errnum = NULL;
    context->error.error_status = idigi_file_noerror;

    if (opcode == fs_ls_request_opcode)
    {
        context->data.d.path = NULL;
    }
    else
    {
        context->data.f.bytes_done = 0;
    }

done:
    *result = context;
    return status;
}

static idigi_callback_status_t file_system_request_callback(idigi_data_t * const idigi_ptr, 
                                                            msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    msg_service_data_t * const service_data = service_request->have_data;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;

    if (MsgIsStart(service_data->flags))
    {
        if (context == NULL)
        {
            uint8_t const * const ptr = service_data->data_ptr;
            const file_system_opcode_t opcode = (file_system_opcode_t) *ptr;

            status = allocate_file_context(idigi_ptr, opcode, &context);
            if (status != idigi_callback_continue)
                goto done;
            session->service_context = context;
            context->opcode = opcode;
        }
    }
    else 
    {
        if (context == NULL) 
        {
            set_file_system_service_error(service_request, idigi_msg_error_format);
            ASSERT_GOTO(idigi_false, done);
        }
    }

    if (context->opcode != fs_put_request_opcode && !MsgIsStart(service_data->flags))
    {
        /* don't support request in >1 message */
        context->error.error_status = idigi_file_request_format_error;
        context->error.errnum = file_str_request_format_error;
        FileSetErrorHint(context);
        ASSERT_GOTO(idigi_false, done);
    }

    switch (context->opcode)
    {
        case fs_get_request_opcode:            
            status = process_file_get_request(idigi_ptr, service_request);
            break;

        case fs_put_request_opcode:            
            status = process_file_put_request(idigi_ptr, service_request);
            break;

        case fs_rm_request_opcode:            
            status = process_file_rm_request(idigi_ptr, service_request);
            break;

        case fs_ls_request_opcode:            
            status = process_file_ls_request(idigi_ptr, service_request);
            break;

        default:
            context->error.error_status = idigi_file_request_format_error;
            context->error.errnum = file_str_request_format_error;
            FileSetErrorHint(context);

            ASSERT_GOTO(idigi_false, done);
    }

done:
    return status;
}

static idigi_callback_status_t file_system_response_callback(idigi_data_t * const idigi_ptr, 
                                                             msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;

    if (context == NULL)
    {
        set_file_system_service_error(service_request, idigi_msg_error_unknown_session);
        ASSERT_GOTO(idigi_false, done);
    }


    switch (context->opcode)
    {
        case fs_get_request_opcode:            
            status = process_file_get_response(idigi_ptr, service_request);
            break;

        case fs_put_request_opcode:            
            status = process_file_response_nodata(idigi_ptr, service_request, fs_put_response_opcode);
            break;

        case fs_rm_request_opcode:            
            status = process_file_response_nodata(idigi_ptr, service_request, fs_rm_response_opcode);
            break;

        case fs_ls_request_opcode:            
            status = process_file_ls_response(idigi_ptr, service_request);
            break;

        default:
            set_file_system_service_error(service_request, idigi_msg_error_unknown_session);
            ASSERT_GOTO(idigi_false, done);
            break;
    }

done:
    return status;
}

static idigi_callback_status_t file_system_free_callback(idigi_data_t * const idigi_ptr, 
                                                         msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    
    if (context != NULL)
    {
        if (context->opcode == fs_ls_request_opcode && context->data.d.path != NULL)
        {
            free_data(idigi_ptr, context->data.d.path);
        }

        free_data(idigi_ptr, context);
    }

    return idigi_callback_continue;
}

static idigi_callback_status_t file_system_error_callback(idigi_data_t * const idigi_ptr, 
                                                          msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * const context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;

    idigi_file_error_request_t request;
    idigi_file_data_response_t response;
    idigi_request_t request_id;

    size_t response_length = sizeof response;
    
    request_id.file_system_request = idigi_file_system_msg_error;

    request.message_status = service_request->error_value;
    response.data_ptr = NULL;
    response.size_in_bytes = 0;
    response.user_context = context == NULL ? NULL : context->user_context;

    idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                   &request, sizeof request, &response, &response_length);

    if (context != NULL)
    {
        context->user_context = response.user_context;

        if (context->handle != NULL)
        {
            idigi_file_system_request_t fs_request_id = context->opcode == fs_ls_request_opcode ?
                                            idigi_file_system_closedir : idigi_file_system_close;

            status = call_file_close_user(idigi_ptr, service_request, fs_request_id);
            response.user_context = context->user_context;

            if (status == idigi_callback_busy)
                goto done;
        }
    }

done:
    return idigi_callback_continue;
}


static idigi_callback_status_t file_system_callback(idigi_data_t * const idigi_ptr, 
                                                    msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_abort;

    ASSERT_GOTO(idigi_ptr != NULL, done);
    ASSERT_GOTO(service_request != NULL, done);
    ASSERT_GOTO(service_request->session != NULL, done);

    switch (service_request->service_type)
    {
    case msg_service_type_have_data:
        status = file_system_request_callback(idigi_ptr, service_request);
        break;

    case msg_service_type_need_data:
        status = file_system_response_callback(idigi_ptr, service_request);
        break;

    case msg_service_type_error:
        status = file_system_error_callback(idigi_ptr, service_request);
        break;

    case msg_service_type_free:
        status = file_system_free_callback(idigi_ptr, service_request);
        break;

    default:
        status = idigi_callback_unrecognized;
        ASSERT(idigi_false);
        break;
    }

done:
    return status;
}

static idigi_callback_status_t idigi_facility_file_system_cleanup(idigi_data_t * const idigi_ptr)
{
    return msg_cleanup_all_sessions(idigi_ptr,  msg_service_id_file);
}

static idigi_callback_status_t idigi_facility_file_system_delete(idigi_data_t * const data_ptr)
{
    return msg_delete_facility(data_ptr, msg_service_id_file);
}

static idigi_callback_status_t idigi_facility_file_system_init(idigi_data_t * const data_ptr, unsigned int const facility_index)
{
    return msg_init_facility(data_ptr, facility_index, msg_service_id_file, file_system_callback);
}
