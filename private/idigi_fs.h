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
    int       fd;
    uint32_t  bytes_done;
    uint32_t  data_length;
    uint32_t  offset;
    char    * path;
    void    * dir_handle;
    uint32_t  last_modified;

    uint32_t  flags;
    void    * user_context;
    file_system_opcode_t        opcode;
    idigi_file_error_code_t     error;
    int errnum;

} file_system_context_t;


#define IDIGI_FILE_STATE_TRUNC   0x01

#define FILE_OPCODE_BYTES           1

#define FsIsBitSet(flag, bit)  (((flag) & (bit)) == (bit))
#define FsBitSet(flag, bit)    ((flag) |= (bit))
#define FsBitClear(flag, bit)  ((flag) &= ~(bit))

#define fileIsDir(flags)    FsIsBitSet(flags, IDIGI_FILE_IS_DIR)
#define fileSetDir(flags)   FsBitSet(flags,   IDIGI_FILE_IS_DIR)
#define fileClearDir(flags) FsBitClear(flags, IDIGI_FILE_IS_DIR) 

#define fileIsReg(flags)    FsIsBitSet(flags, IDIGI_FILE_IS_REG)
#define fileSetReg(flags)   FsBitSet(flags,   IDIGI_FILE_IS_REG)
#define fileClearReg(flags) FsBitClear(flags, IDIGI_FILE_IS_REG) 

#define fileIsTrunc(flags)    FsIsBitSet(flags, IDIGI_FILE_STATE_TRUNC)
#define fileSetTrunc(flags)   FsBitSet(flags,   IDIGI_FILE_STATE_TRUNC)
#define fileClearTrunc(flags) FsBitClear(flags, IDIGI_FILE_STATE_TRUNC) 

#define fs_dont_continue(status, error_code) (status != idigi_callback_continue || error_code != idigi_file_system_noerror)
#define fs_continue(status, error_code) (status == idigi_callback_continue && error_code == idigi_file_system_noerror)


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
     file_system_context_t * context = session->service_context;
     msg_service_data_t * service_data = service_request->need_data;

     uint8_t * fs_error_response = service_data->data_ptr;
     size_t    header_bytes = record_bytes(fs_error_response_header);
     uint8_t   error_code = context->error;

     ASSERT(error_code > idigi_file_system_noerror && error_code <= idigi_file_system_permision_denied);

     error_code--;

     message_store_u8(fs_error_response, opcode, fs_error_opcode);
     message_store_u8(fs_error_response, error_code, error_code);

     service_data->length_in_bytes = header_bytes;

     if (context->errnum != 0)
     {
        idigi_file_error_hint_request_t request; 
        idigi_file_response_t           response;
        size_t response_length = sizeof response;

        size_t buffer_size = MIN_VALUE(service_data->length_in_bytes - header_bytes, UCHAR_MAX);
        idigi_request_t const request_id;

        request_id.file_system_request = idigi_file_system_strerror;
        request.errnum = context->errnum;

        response.data.data_ptr = fs_error_response + header_bytes;
        response.size_in_bytes = buffer_size;

        response.context = context->user_context;

        idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                       &request, sizeof request, &response, &response_length);

        context->user_context = response.context;

        if (response_length != sizeof response || response.size_in_bytes > buffer_size)
        {
            /* wrong size returned and let's cancel the request */
            notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
            set_file_system_service_error(service_request, idigi_msg_error_cancel);
        }
        message_store_u8(fs_error_response, error_hint_len, (uint8_t) response.size_in_bytes);

        service_data->length_in_bytes += response.size_in_bytes;
     }
     MsgSetLastData(service_data->flags);
}


static idigi_callback_status_t call_file_system_user(idigi_data_t * const idigi_ptr,
                                                    msg_service_request_t * const service_request, 
                                                    idigi_file_system_request_t fs_request_id,
                                                    void const * request,
                                                    size_t       request_length,
                                                    idigi_file_response_t * response)
{
    idigi_callback_status_t status;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_msg_error_t message_status = idigi_msg_error_none;

    idigi_request_t request_id;
    size_t response_length = sizeof *response;
    size_t response_data_size = response->size_in_bytes;

    request_id.file_system_request = fs_request_id;

    response->context = context->user_context;
    response->error   = idigi_file_system_noerror;
    response->errnum   = 0;

    status = idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                            request, request_length, response, &response_length);

    context->user_context = response->context;
 
    switch (status)
    {
        case idigi_callback_continue:
            if (response_length != sizeof *response || response->size_in_bytes > response_data_size)
            {
                /* wrong size returned and let's cancel the request */
                message_status = idigi_msg_error_cancel;
                notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
                break;
            }
            context->error  = response->error;
            context->errnum = response->errnum;
            break;

        case idigi_callback_busy:
            break;

        case idigi_callback_unrecognized:
        case idigi_callback_abort: 
            message_status = idigi_msg_error_cancel;
            break;
    }

    if (message_status != idigi_msg_error_none)
        set_file_system_service_error(service_request, message_status);

    return status;
}

static idigi_callback_status_t call_file_stat_user(idigi_data_t * const idigi_ptr, 
                                                   msg_service_request_t * const service_request,
                                                   char const * path)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t   status;
    idigi_file_path_request_t request; 
    idigi_file_response_t     response;

    request.path  = path;
    response.size_in_bytes = sizeof response.data.stat;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_stat, 
                                    &request,  sizeof request, 
                                    &response);

    if (fs_continue(status, context->error))
    {
        idigi_file_stat_t * pstat = &response.data.stat;

        context->data_length   = (uint32_t) pstat->file_size;
        context->last_modified = pstat->last_modified;
        context->flags         = pstat->flags;
    }

    return status;
}

static idigi_callback_status_t call_file_opendir_user(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request,
                                                       char const * path)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t   status;
    idigi_file_path_request_t request; 
    idigi_file_response_t     response;

    request.path  = path;
    response.size_in_bytes = sizeof (long *);

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_opendir, 
                                    &request,  sizeof request, 
                                    &response);

    if (fs_continue(status, context->error))
    {
        context->dir_handle = response.data.dir_handle;
    }

    return status;
}

static idigi_callback_status_t call_file_readdir_user(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request,
                                                       char * path,
                                                       size_t buffer_size)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t  status;
    idigi_file_dir_request_t request; 
    idigi_file_response_t    response;

    request.dir_handle      = context->dir_handle;

    response.data.data_ptr       = path;
    response.size_in_bytes  = buffer_size;

    *path = '\0';

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_readdir, 
                                    &request,  sizeof request, 
                                    &response);
    return status;
}

static idigi_callback_status_t call_file_closedir_user(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t  status;
    idigi_file_dir_request_t request; 
    idigi_file_response_t    response;

    request.dir_handle  = context->dir_handle;
    response.size_in_bytes = 0;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_closedir, 
                                    &request,  sizeof request, 
                                    &response);

    context->dir_handle = NULL;

    return status;
}


static idigi_callback_status_t call_file_open_user(idigi_data_t * const idigi_ptr, 
                                                   msg_service_request_t * const service_request,
                                                   char const * path, 
                                                   int mode)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t      status;
    idigi_file_path_request_t    request; 
    idigi_file_response_t        response;

    request.path = path;
    request.mode     = mode;
    response.data.fd = -1;
    response.size_in_bytes = sizeof response.data.fd;
    
    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_open, 
                                   &request,  sizeof request, 
                                   &response);
    context->fd = response.data.fd;
    return status;
}

static idigi_callback_status_t call_file_close_user(idigi_data_t * const idigi_ptr,
                                                    msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;

    if (context->fd >= 0)
    {
        idigi_file_request_t  request;  
        idigi_file_response_t response;

        request.fd = context->fd;
        response.size_in_bytes = 0;

        status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_close, 
                                       &request, sizeof request, 
                                       &response);

        if (status != idigi_callback_busy)
        {
            context->fd = -1;
        }
    }
    return status;
}

static idigi_callback_status_t call_file_lseek_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request, 
                                                     long int   offset_in,
                                                     int        origin,
                                                     long int * offset_out)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;

    idigi_callback_status_t  status;
    idigi_file_offset_request_t request;  
    idigi_file_response_t response;
    
    request.fd = context->fd;
    request.offset = offset_in;
    request.origin = origin;

    response.data.offset   = -1;
    response.size_in_bytes = sizeof response.data.offset;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_lseek, 
                                   &request, sizeof request, 
                                   &response);
    *offset_out = response.data.offset;

    return status;
}


static idigi_callback_status_t call_file_ftruncate_user(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request) 
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t  status;
    idigi_file_offset_request_t request;  
    idigi_file_response_t       response;

    request.fd     = context->fd;
    request.offset = context->offset;
    response.size_in_bytes = 0;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_ftruncate, 
                                   &request, sizeof request, 
                                   &response);

    return status;
}

static idigi_callback_status_t call_file_rm_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request,
                                                     char const * path) 
{
    idigi_callback_status_t status;
    idigi_file_path_request_t  request; 
    idigi_file_response_t   response;

    request.path = path;
    response.size_in_bytes = 0;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_rm, 
                                   &request, sizeof request, 
                                   &response);

    return status;
}

static idigi_callback_status_t call_file_read_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request, 
                                                     void   * buffer,
                                                     size_t * buffer_size)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status;
    idigi_file_request_t  request; 
    idigi_file_response_t response;

    request.fd = context->fd;

    response.data.data_ptr = buffer;
    response.size_in_bytes = *buffer_size;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_read, 
                                   &request, sizeof request, 
                                   &response);

    *buffer_size = response.size_in_bytes;

    return status;
}

static idigi_callback_status_t call_file_write_user(idigi_data_t * const idigi_ptr,
                                                     msg_service_request_t * const service_request,
                                                     void const * buffer,
                                                     size_t     * bytes_done)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status;
    idigi_file_write_request_t request;  
    idigi_file_response_t   response;

    request.fd     = context->fd;
    request.data_ptr = buffer;
    request.size_in_bytes  = *bytes_done;
    response.size_in_bytes = *bytes_done;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_write, 
                                   &request, sizeof request, 
                                   &response);

    *bytes_done = response.size_in_bytes;

    if (status == idigi_callback_continue && context->error == idigi_file_system_noerror)
    {
        if (response.size_in_bytes == 0 && request.size_in_bytes != 0)
        {
            status = idigi_callback_busy;
        }
    }

    return status;
}

static size_t parse_file_path(void const * const path_ptr, size_t const buffer_size)
{
    char const * path = path_ptr;
    size_t path_len = strnlen(path, buffer_size - 1);

    if (path_len != 0 && path[path_len] == '\0')
    {
        path_len++;
    }

    ASSERT(path_len > 0);

    return path_len;
}

static size_t parse_file_get_header(uint8_t const * const header_ptr, size_t const buffer_size, file_system_context_t * const context)
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
    uint8_t const *fs_get_request = header_ptr + FILE_OPCODE_BYTES;
    size_t  header_len = record_bytes(fs_get_request_header) + FILE_OPCODE_BYTES;

    size_t len = parse_file_path(fs_get_request, buffer_size - header_len);
 
    if (len != 0)
    {
        fs_get_request += len;
        context->offset      = message_load_be32(fs_get_request, offset);
        context->data_length = message_load_be32(fs_get_request, length);
        len   += header_len;
    }
    return len;
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
    file_system_context_t * context = session->service_context;
    
    status = call_file_lseek_user(idigi_ptr, service_request, 0, IDIGI_SEEK_END, &ret);

    if (fs_dont_continue(status, context->error))
        goto done;

    if (ret == -1 || (uint32_t) ret < context->offset)
    {
        context->error = idigi_file_system_invalid_parameter;
        goto done;
    }

    status = call_file_lseek_user(idigi_ptr, service_request, (long int ) context->offset, IDIGI_SEEK_SET, &ret);

    if (fs_dont_continue(status, context->error))
        goto done;

    if (ret == -1)
        context->error = idigi_file_system_invalid_parameter;

done:
    return status;
}

static idigi_callback_status_t process_file_get_request(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t * service_data = service_request->have_data;

    if (parse_file_get_header(service_data->data_ptr, service_data->length_in_bytes, context) == 0)
    {
        context->error = idigi_file_system_request_format_error;
        goto done;
    }
    if (context->fd == -1)
    {
        char const * path = service_data->data_ptr;
        path += FILE_OPCODE_BYTES;

        status = call_file_open_user(idigi_ptr, service_request, path, IDIGI_O_RDONLY);

        if (fs_dont_continue(status, context->error))
            goto done;
    }

    status = set_file_position(idigi_ptr, service_request);

done:
    return status;
}

static idigi_callback_status_t process_file_get_response(idigi_data_t * const idigi_ptr,
                                                          msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t * service_data = service_request->need_data;

    if (context->error != idigi_file_system_noerror)
        goto error;
    
    {
        idigi_bool_t last_msg = idigi_false;
        uint8_t * data_ptr = service_data->data_ptr;

        size_t buffer_size = service_data->length_in_bytes;
        size_t bytes_read = 0;
        size_t bytes_to_read;

        service_data->length_in_bytes = 0;

        if (MsgIsStart(service_data->flags))
        {
            *data_ptr++ = fs_get_response_opcode;
            buffer_size--;
            service_data->length_in_bytes++;
        }

        bytes_to_read = MIN_VALUE(buffer_size, context->data_length - context->bytes_done);

        while (bytes_to_read > 0)
        {
            size_t cnt = bytes_to_read;
            status = call_file_read_user(idigi_ptr, service_request, data_ptr, &cnt);

            if (status == idigi_callback_busy)
            {
                if (bytes_read > 0)
                {
                    /* Return what's read already */
                    status = idigi_callback_continue;
                }
                break;
            }

            if (status != idigi_callback_continue)
                goto close_file;

            /* user returned an error on 1st response, send error message 
               Can't return an error after part of the file has been returned
             */
            if (context->error != idigi_file_system_noerror && MsgIsStart(service_data->flags))
                goto error;

            if (cnt == 0 || context->error != idigi_file_system_noerror)
            {
                context->error = idigi_file_system_noerror;
                last_msg = idigi_true;
                break;
            }

            data_ptr += cnt;
            bytes_to_read -= cnt;
            bytes_read += cnt;
        }
        service_data->length_in_bytes += bytes_read;
        context->bytes_done += bytes_read;

        if (context->data_length == context->bytes_done) 
            last_msg = idigi_true;

        if (last_msg)
        {
            MsgSetLastData(service_data->flags);
            goto close_file;
        }
        goto done;
    }

error:
    format_file_error_msg(idigi_ptr, service_request);

close_file:
    call_file_close_user(idigi_ptr, service_request);

done:
    return status;
}

static idigi_callback_status_t process_file_rm_request(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t * service_data = service_request->have_data;
    char const * path = service_data->data_ptr;
    path += FILE_OPCODE_BYTES;

    if (parse_file_path(path, service_data->length_in_bytes - FILE_OPCODE_BYTES) == 0) 
    {
        context->error = idigi_file_system_request_format_error;
        goto done;
    }
    status = call_file_rm_user(idigi_ptr, service_request, path);

done:
    return status;
}

static idigi_callback_status_t process_file_response_nodata(idigi_data_t * const idigi_ptr,
                                                         msg_service_request_t * const service_request,
                                                         file_system_opcode_t opcode)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t * service_data = service_request->need_data;
    UNUSED_PARAMETER(idigi_ptr);

    if (context->error == 0)
    {
        uint8_t * data_ptr = service_data->data_ptr;
        *data_ptr = opcode;
        service_data->length_in_bytes = sizeof(*data_ptr);
        MsgSetLastData(service_data->flags);
    }
    else
    {
        format_file_error_msg(idigi_ptr, service_request);
    }
    return idigi_callback_continue;
}

static size_t parse_file_put_header(uint8_t const * const header_ptr, 
                                    size_t const buffer_size, 
                                    file_system_context_t * const context)
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
    size_t  header_len = record_bytes(fs_put_request_header) + FILE_OPCODE_BYTES;

    size_t len = parse_file_path(fs_put_request, buffer_size - header_len);

    if (len != 0)
    {
        fs_put_request += len;
        context->flags  = message_load_u8(fs_put_request, flags);
        context->offset = message_load_be32(fs_put_request, offset);
        len   += header_len;
    }

    return len;
}


static idigi_callback_status_t process_file_put_request(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t    * service_data = service_request->have_data;
    uint8_t               * data_ptr = service_data->data_ptr;
    size_t                  bytes_to_write = service_data->length_in_bytes;
    size_t                  bytes_written = 0;

    if (context->error != idigi_file_system_noerror)
        goto close_file;

    if (MsgIsStart(service_data->flags))
    {
        size_t header_len = parse_file_put_header(data_ptr, service_data->length_in_bytes, context); 
        if (header_len == 0)
        {
            context->error = idigi_file_system_request_format_error;
            goto done;
        }

        if (context->fd == -1)
        {
            int open_flags = IDIGI_O_WRONLY | IDIGI_O_CREAT;   

            if (fileIsTrunc(context->flags))
            {
                if (context->offset == 0)
                {
                    open_flags |= IDIGI_O_TRUNC;
                    fileClearTrunc(context->flags);
                }
            }

            status = call_file_open_user(idigi_ptr, service_request, (char const *) data_ptr + FILE_OPCODE_BYTES, open_flags);
            if (fs_dont_continue(status, context->error))
                goto done;
        }
        status = set_file_position(idigi_ptr, service_request);
        if (status == idigi_callback_busy)
            goto done;

        if (fs_dont_continue(status, context->error))
           goto close_file;

        data_ptr       += header_len;
        bytes_to_write -= header_len;
    }

    if (context->bytes_done > bytes_to_write)
    {
        /* messaging problem with the buffer */
        set_file_system_service_error(service_request, idigi_msg_error_format);
        status = idigi_callback_abort;
        ASSERT_GOTO(idigi_false, close_file);
    }

    data_ptr       += context->bytes_done;
    bytes_to_write -= context->bytes_done;

    while(bytes_to_write > 0)
    {
        size_t cnt = bytes_to_write;
        status = call_file_write_user(idigi_ptr, service_request, data_ptr, &cnt);

        if (status == idigi_callback_busy)
        {
            context->bytes_done += bytes_written;
            context->offset  += bytes_written;
            goto done;
        }
        if (fs_dont_continue(status, context->error))
           goto close_file;

        data_ptr       += cnt;
        bytes_to_write -= cnt;
        bytes_written  += cnt;
    }
    context->bytes_done = 0;
    context->offset  += bytes_written;

    if (!MsgIsLastData(service_data->flags))
        goto done;

    if (fileIsTrunc(context->flags))
    {
        status = call_file_ftruncate_user(idigi_ptr, service_request);
        if (fs_dont_continue(status, context->error))
            goto close_file;

        fileClearTrunc(context->flags);
    }

close_file:
    status = call_file_close_user(idigi_ptr, service_request);

done:
    return status;
}

static size_t parse_file_ls_header(uint8_t const * const header_ptr, size_t const buffer_size)
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

    size_t len = parse_file_path(data_ptr, buffer_size - header_len);
    if (len != 0)
    {
        len += header_len;
    }

    return len += header_len;
}

static size_t format_file_ls_response_header(uint8_t * data_ptr)
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
    message_store_u8(fs_ls_response, hash_alg,   idigi_file_hash_none);
    message_store_u8(fs_ls_response, hash_bytes, 0);

    return record_bytes(fs_ls_response_header);
}

static size_t ls_resp_header_size(void)
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
                                      uint8_t * data_ptr)
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


    uint8_t * fs_ls_response = data_ptr;
    uint8_t flags = fileIsDir(context->flags) ? IDIGI_FILE_IS_DIR : 0;
    size_t result;

    message_store_u8(fs_ls_response,   flags, flags);
    message_store_be32(fs_ls_response, last_modified, context->last_modified);
    result = record_bytes(fs_ls_response_dir);

    if (!fileIsDir(context->flags))
    {
        fs_ls_response += result;
        message_store_be32(fs_ls_response, size, context->data_length);
        result += record_bytes(fs_ls_response_file);
    }

    return result;
}

static idigi_callback_status_t file_store_path(idigi_data_t * const idigi_ptr, char const *path, file_system_context_t * context)
{
    size_t path_len = strlen(path) + 1;
    void *ptr;

    idigi_callback_status_t status = malloc_data(idigi_ptr, path_len, &ptr);
    if (status != idigi_callback_continue)
        goto done;

    context->path = ptr;
    memcpy(context->path, path, path_len);

done:
    return status;
}


static idigi_callback_status_t process_file_ls_request(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t    * service_data = service_request->have_data;
    char const * path = service_data->data_ptr;
    path += FILE_OPCODE_BYTES;
    
    if (context->error != idigi_file_system_noerror)
        goto done;

    if (parse_file_ls_header(service_data->data_ptr, service_data->length_in_bytes) == 0)
    {
        context->error = idigi_file_system_request_format_error;
        goto done;
    }
    
    status = call_file_stat_user(idigi_ptr, service_request, path);
    if (fs_dont_continue(status, context->error))
        goto done;

    if (fileIsDir(context->flags))
    {
        status = call_file_opendir_user(idigi_ptr, service_request, path);
    }
    else
    {
        status = file_store_path(idigi_ptr, path, context);
    }
        
done:
    return status;
}

static idigi_callback_status_t process_file_ls_response(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;
    msg_service_data_t    * service_data = service_request->need_data;
    uint8_t * data_ptr = service_data->data_ptr;
    size_t buffer_size = service_data->length_in_bytes;
    size_t    resp_len    = 0;

    if (context->error != idigi_file_system_noerror)
        goto error;

    if (MsgIsStart(service_data->flags))
    {
        resp_len = format_file_ls_response_header(data_ptr);
        buffer_size -= resp_len;
        data_ptr    += resp_len;

        if (context->dir_handle == NULL)
        {
            /* ls for a single file */
            size_t path_len = strlen(context->path) + 1;
            if ((path_len + ls_resp_header_size()) > buffer_size)
            {
                context->error = idigi_file_system_out_of_memory;
                goto done;
            }
            memcpy(data_ptr, context->path, path_len);
            resp_len += path_len;
            resp_len += format_file_ls_response(context, data_ptr + path_len);
            service_data->length_in_bytes = resp_len;
            MsgSetLastData(service_data->flags);
            goto done;
        }
    }
    {
        char * path = (char *) data_ptr;

        context->flags = 0;
 
        status = call_file_readdir_user(idigi_ptr, service_request, path, buffer_size - ls_resp_header_size());

        if (status != idigi_callback_continue)
           goto close_dir;

        if (context->error != idigi_file_system_noerror)
        {
           if (MsgIsStart(service_data->flags))
                goto error;
           *path = '\0';
        }

        if (*path == '\0')
        {
            MsgSetLastData(service_data->flags);
            service_data->length_in_bytes = resp_len;
            goto close_dir;
        }
        status = call_file_stat_user(idigi_ptr, service_request, path);
        if (fs_continue(status, context->error))
        {
            size_t path_len = strlen(path) + 1;

            resp_len += path_len;
            resp_len += format_file_ls_response(context, data_ptr + path_len);
            service_data->length_in_bytes = resp_len;
            goto done;
        }
    }

error:
    /* add hint for idigi_file_system_out_of_memory */
    format_file_error_msg(idigi_ptr, service_request);

close_dir:
    call_file_closedir_user(idigi_ptr, service_request);

done:
    return status;
}



static idigi_callback_status_t process_file_error_request(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_abort;
    idigi_debug("file_system_error_callback\n");

    UNUSED_PARAMETER(idigi_ptr);
    UNUSED_PARAMETER(service_request);

    return status;
}

idigi_callback_status_t allocate_file_context(idigi_data_t * const idigi_ptr, file_system_context_t ** result)
{
    file_system_context_t * context = NULL;
    idigi_callback_status_t status;

    void *ptr;

    status = malloc_data(idigi_ptr, sizeof *context, &ptr);
    if (status != idigi_callback_continue)
        goto done;

    context = ptr;

    context->fd                 = -1;
    context->errnum             = 0;
    context->bytes_done         = 0;
    context->flags              = 0;
    context->user_context       = NULL;
    context->dir_handle         = NULL;
    context->path               = NULL;
    context->last_modified      = 0;
    context->error              = idigi_file_system_noerror;

done:
    *result = context;
    return status;
}

static idigi_callback_status_t file_system_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status = idigi_callback_abort;
    msg_service_data_t * service_data = service_request->have_data;

    if (MsgIsStart(service_data->flags))
    {
        if (context == NULL)
        {
            status = allocate_file_context(idigi_ptr, &context);
            if (status != idigi_callback_continue)
                goto done;
        }
        session->service_context = context;
        context->opcode = (file_system_opcode_t ) *((uint8_t *)service_data->data_ptr);
    }
    else 
    if (context == NULL) 
    {
        set_file_system_service_error(service_request, idigi_msg_error_format);
        ASSERT_GOTO(idigi_false, done);
    }

    if (context->opcode != fs_put_request_opcode && !MsgIsStart(service_data->flags))
    {
        /* don't support request in >1 message */
        context->error = idigi_file_system_request_format_error;
        goto done;
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

        case fs_error_opcode:
            status = process_file_error_request(idigi_ptr, service_request);
            break;

        default:
            set_file_system_service_error(service_request, idigi_msg_error_format);
            ASSERT_GOTO(idigi_false, done);
            break;
    }

done:
    return status;
}

static idigi_callback_status_t file_system_response_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status = idigi_callback_abort;

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
            set_file_system_service_error(service_request, idigi_msg_error_format);
            ASSERT_GOTO(idigi_false, done);
            break;
    }

done:
    return status;
}

static idigi_callback_status_t file_system_free_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    
    if (context != NULL)
    {
        if (context->path != NULL)
        {
            free_data(idigi_ptr, context->path);
        }

        free_data(idigi_ptr, context);
    }

    return idigi_callback_continue;
}

static idigi_callback_status_t file_system_error_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    UNUSED_PARAMETER(idigi_ptr);
    UNUSED_PARAMETER(service_request);
    /* TODO: add processing of this type */
    idigi_debug("file_system_get_callback: msg_service_type_error\n");

    return idigi_callback_continue;
}


static idigi_callback_status_t file_system_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
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