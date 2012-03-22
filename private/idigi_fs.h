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
    size_t    path_len;
    uint32_t  last_modified;
    size_t    file_size;
    idigi_file_dir_data_t dir;

    void    * user_context;
    idigi_file_hash_algorithm_t hash_alg;
    file_system_opcode_t        opcode;
    idigi_file_error_data_t     error;
    uint8_t  flags;
    uint8_t  state;

} file_system_context_t;


#define IDIGI_FILE_TRUNC            0x01

#define FILE_STATE_NONE                0
#define FILE_STATE_READDIR_DONE        1
#define FILE_STATE_STAT_DONE           2

#define FILE_OPCODE_BYTES           1

#define FILE_STR_ETOOLONG "File name too long"

#define FsIsBitSet(flag, bit)  (((flag) & (bit)) == (bit))
#define FsBitSet(flag, bit)    ((flag) |= (bit))
#define FsBitClear(flag, bit)  ((flag) &= ~(bit))

#define FileIsDir(context)    FsIsBitSet(context->flags, IDIGI_FILE_IS_DIR)
#define FileIsReg(context)    FsIsBitSet(context->flags, IDIGI_FILE_IS_REG)

#define FileNeedTrunc(context)  FsIsBitSet(context->flags, IDIGI_FILE_TRUNC)
#define FileClearTrunc(context) FsBitClear(context->flags, IDIGI_FILE_TRUNC) 

#define FileSetState(context, s) (context->state = s)
#define FileGetState(context)    (context->state)

#define fileSessionError(status, context)  (status>idigi_callback_busy || context->error.error_code!=idigi_file_system_noerror)

static void set_file_system_service_error(msg_service_request_t * const service_request, idigi_msg_error_t const msg_error)
{
    service_request->error_value = msg_error;
    service_request->service_type = msg_service_type_error;
}

static void format_file_error_msg(idigi_data_t * const idigi_ptr,
                                  msg_service_request_t * const service_request,
                                  char const * hint)
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
     uint8_t   error_code = context->error.error_code;
     size_t    buffer_size = MIN_VALUE(service_data->length_in_bytes - header_bytes, UCHAR_MAX);
     uint8_t   error_hint_len = 0;

     ASSERT(error_code > idigi_file_system_noerror);

     /* don't send an error response, of thession is canceled */
     if (error_code == idigi_file_system_user_cancel)
         goto done;

     error_code -= idigi_file_system_user_cancel;

     message_store_u8(fs_error_response, opcode, fs_error_opcode);
     message_store_u8(fs_error_response, error_code, error_code);

     service_data->length_in_bytes = header_bytes;

     if (hint != NULL)
     {
        error_hint_len = MIN_VALUE(buffer_size, strlen(hint) + 1);
        memcpy(fs_error_response + header_bytes, hint, error_hint_len);

     }
     else
     if (context->error.errnum != 0)
     {
        idigi_file_request_t            request; 
        idigi_file_response_t           response;
        size_t response_length = sizeof response;

        idigi_request_t request_id;
        request_id.file_system_request = idigi_file_system_strerror;

        response.error   = &context->error;
        response.data_ptr = fs_error_response + header_bytes;
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
        else
        {
            error_hint_len = (uint8_t) response.size_in_bytes;
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
                                                    msg_service_request_t * service_request, 
                                                    idigi_file_system_request_t fs_request_id,
                                                    void const * request,
                                                    size_t       request_length,
                                                    void *       response_data)
{
    idigi_callback_status_t status;
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_file_response_t * response = response_data;
    size_t buffer_size = response->size_in_bytes;
    idigi_msg_error_t  msg_error = idigi_msg_error_none;

    idigi_request_t request_id;
    size_t response_length = sizeof *response;

    request_id.file_system_request = fs_request_id;

    response->context = context->user_context;
    response->error   = &context->error;

    status = idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                            request, request_length, response, &response_length);

    context->user_context = response->context;
 
    switch (status)
    {
        case idigi_callback_continue:
            if (response_length != sizeof *response || response->size_in_bytes > buffer_size)
            {
                /* wrong size returned and let's cancel the request */
                msg_error = idigi_msg_error_cancel;
                notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
                break;
            }
            if (context->error.error_code == idigi_file_system_user_cancel)
            {
                msg_error = idigi_msg_error_cancel;
                break;
            }
            break;

        case idigi_callback_busy:
            break;

        case idigi_callback_unrecognized:
            status = idigi_callback_continue; /* fall through */

        case idigi_callback_abort: 
            msg_error = idigi_msg_error_cancel;
            break;
    }

    if (msg_error != idigi_msg_error_none)
    {
        set_file_system_service_error(service_request, msg_error);
    }
    return status;
}

static size_t file_hash_size(idigi_file_hash_algorithm_t hash_alg)
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
                                                   char const * path,
                                                   idigi_file_hash_algorithm_t hash_alg)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t     status;
    idigi_file_path_request_t   request; 
    idigi_file_stat_response_t  response;

    idigi_file_stat_t     stat;

    stat.flags    = 0;
    stat.hash_alg = 0;

    request.path     = path;
    request.hash_alg = hash_alg;

    response.stat_ptr       = &stat;
    response.size_in_bytes  = sizeof stat;
 
    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_stat, 
                                    &request,  sizeof request, 
                                    &response);

    context->file_size     = stat.file_size;
    context->last_modified = stat.last_modified;
    context->flags         = stat.flags;
    
    /* don't overwrite */
    if (hash_alg == idigi_file_hash_none ||
        status == idigi_callback_busy    || 
        fileSessionError(status, context))
    {
        goto done;
    }

    /* asked for best context->hash_alg, if asked for none */
    if (hash_alg == idigi_file_hash_best)
    {
        if (stat.hash_alg == idigi_file_hash_best)
            context->hash_alg = idigi_file_hash_none;
        else
            context->hash_alg = stat.hash_alg;
    }
    else
    /* asked for crc32, md5, use it or none */
    if (stat.hash_alg != context->hash_alg)
       context->hash_alg = idigi_file_hash_none;

done:
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
    idigi_file_dir_response_t response;

    request.path        = path;
    response.dir_data   = &context->dir;
    response.size_in_bytes = sizeof context->dir;

    context->dir.dir_handle     = NULL;
    context->dir.dir_entry      = NULL;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_opendir, 
                                    &request,  sizeof request, 
                                    &response);

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
    idigi_file_dir_data_t    request; 
    idigi_file_response_t    response;
 
    request = context->dir;
    response.data_ptr      = path;
    response.size_in_bytes = buffer_size;

    *path = '\0';

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_readdir, 
                                    &request,  sizeof request, 
                                    &response);

    if (response.size_in_bytes == 0)
        *path = '\0';

    return status;
}

static idigi_callback_status_t call_file_closedir_user(idigi_data_t * const idigi_ptr, 
                                                       msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t  status;
    idigi_file_dir_data_t    request; 
    idigi_file_response_t    response;

    request = context->dir;
    response.data_ptr = NULL;
    response.size_in_bytes = 0;
 
    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_closedir, 
                                    &request,  sizeof request, 
                                    &response);

    context->dir.dir_handle = NULL;
    context->dir.dir_entry  = NULL;

    return status;
}

static idigi_callback_status_t call_file_hash_user(idigi_data_t * const idigi_ptr, 
                                                   msg_service_request_t * const service_request,
                                                   char const * path, 
                                                   uint8_t * hash_ptr)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t      status = idigi_callback_continue;

    idigi_file_path_request_t request; 
    idigi_file_response_t     response;

    size_t hash_size = file_hash_size(context->hash_alg);

    if (context->hash_alg == idigi_file_hash_none || FileIsDir(context))
        goto done;

    memset(hash_ptr, 0, hash_size);

    if (!FileIsReg(context))
        goto done;

    request.path     = path;
    request.hash_alg = context->hash_alg;

    response.data_ptr      = hash_ptr;
    response.size_in_bytes = hash_size;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_hash, 
                                   &request,  sizeof request, 
                                   &response);
done:
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
    idigi_file_open_response_t   response;

    request.path = path;
    request.mode = mode;

    response.fd_ptr = &context->fd;
    response.size_in_bytes = sizeof context->fd;
    
    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_open, 
                                   &request,  sizeof request, 
                                   &response);
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
        response.data_ptr = NULL;
        response.size_in_bytes = 0;

        status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_close, 
                                       &request, sizeof request, 
                                       &response);
        context->fd = -1;
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
    idigi_file_lseek_response_t response;
    
    request.fd = context->fd;
    request.offset = offset_in;
    request.origin = origin;

    *offset_out = -1;
    response.offset_ptr = offset_out;
    response.size_in_bytes = sizeof *offset_out;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_lseek, 
                                   &request, sizeof request, 
                                   &response);

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
    
    response.data_ptr = NULL;
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
    idigi_file_response_t      response;

    request.path = path;
    response.data_ptr = NULL;
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

    response.data_ptr = buffer;
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
    idigi_file_response_t      response;

    request.fd  = context->fd;
    request.data_ptr       = buffer;
    request.size_in_bytes  = *bytes_done;

    response.data_ptr = NULL;
    response.size_in_bytes = *bytes_done;

    status = call_file_system_user(idigi_ptr, service_request, idigi_file_system_write, 
                                   &request, sizeof request, 
                                   &response);

    if (status == idigi_callback_continue && !fileSessionError(status, context))
    {
        *bytes_done = response.size_in_bytes;

        if (response.size_in_bytes == 0 && request.size_in_bytes != 0)
        {
            status = idigi_callback_busy;
        }
    }

    return status;
}

static size_t parse_file_path(file_system_context_t * context, void const * const path_ptr, size_t const buffer_size)
{
    char const * path = path_ptr;
    size_t path_len = strnlen(path, buffer_size - 1);

    if (path_len != 0 && path[path_len] == '\0')
    {
        path_len++;
    }

    if (path_len == 0)
    {
        ASSERT(idigi_false);
        context->error.error_code = idigi_file_system_request_format_error;
    }

    return path_len;
}

static size_t parse_file_get_header(file_system_context_t * context, uint8_t const * const header_ptr, size_t const buffer_size)
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

    size_t len = parse_file_path(context, fs_get_request, buffer_size - header_len);
 
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
    if (status == idigi_callback_busy || fileSessionError(status, context))
        goto done;

    if (ret == -1 || (uint32_t) ret < context->offset)
    {
        context->error.error_code = idigi_file_system_invalid_parameter;
        goto done;
    }

    status = call_file_lseek_user(idigi_ptr, service_request, (long int ) context->offset, IDIGI_SEEK_SET, &ret);
    if (status == idigi_callback_busy || fileSessionError(status, context))
        goto done;

    if (ret == -1)
    {
        context->error.error_code = idigi_file_system_invalid_parameter;
    }

done:
    return status;
}

static idigi_callback_status_t process_file_get_request(idigi_data_t * const idigi_ptr,
                                                        msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t * service_data = service_request->have_data;

    if (parse_file_get_header(context, service_data->data_ptr, service_data->length_in_bytes) == 0)
        goto done;

    if (context->fd == -1)
    {
        char const * path = service_data->data_ptr;
        path += FILE_OPCODE_BYTES;

        status = call_file_open_user(idigi_ptr, service_request, path, IDIGI_O_RDONLY);
    }
    if (context->fd >= 0)
    {
        status = set_file_position(idigi_ptr, service_request);

        if (fileSessionError(status, context))
            call_file_close_user(idigi_ptr, service_request);
    }
done:
    return status;
}

static idigi_callback_status_t process_file_get_response(idigi_data_t * const idigi_ptr,
                                                          msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session = service_request->session;
    file_system_context_t * context = session->service_context;

    if (context->error.error_code != idigi_file_system_noerror)
        goto error;
    
    {
        msg_service_data_t * service_data = service_request->need_data;
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
        /* bytes to read in this callback */
        bytes_to_read = MIN_VALUE(buffer_size, context->data_length - context->bytes_done);

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

            if (context->error.error_code != idigi_file_system_noerror)
            {
                /* 1st response - can send error */
                if (MsgIsStart(service_data->flags))
                    goto error;

                /* sent part of response already - cancel the session */
                set_file_system_service_error(service_request, idigi_msg_error_cancel);
                goto close_file;
            }
            if (fileSessionError(status, context))
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
        service_data->length_in_bytes += bytes_read;
        context->bytes_done += bytes_read;

        if (context->data_length == context->bytes_done) 
            MsgSetLastData(service_data->flags);
 
        if (MsgIsLastData(service_data->flags))
             goto close_file;

        goto done;
    }

error:
    format_file_error_msg(idigi_ptr, service_request, NULL);

close_file:
    call_file_close_user(idigi_ptr, service_request);

done:
    return status;
}

static idigi_callback_status_t process_file_rm_request(idigi_data_t * const idigi_ptr,
                                                       msg_service_request_t * const service_request)
{
    idigi_callback_status_t status = idigi_callback_continue;
    msg_session_t * session = service_request->session;
    msg_service_data_t * service_data = service_request->have_data;
    char const * path = service_data->data_ptr;
    
    path += FILE_OPCODE_BYTES;

    if (parse_file_path(session->service_context, path, service_data->length_in_bytes - FILE_OPCODE_BYTES) == 0) 
        goto done;

    status = call_file_rm_user(idigi_ptr, service_request, path);

done:
    return status;
}

static idigi_callback_status_t process_file_response_nodata(idigi_data_t * const idigi_ptr,
                                                         msg_service_request_t * const service_request,
                                                         file_system_opcode_t opcode)
{
    msg_session_t * session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t * service_data = service_request->need_data;

    if (context->error.error_code == idigi_file_system_noerror)
    {
        uint8_t * data_ptr = service_data->data_ptr;
        
        *data_ptr = opcode;

        service_data->length_in_bytes = sizeof *data_ptr;
        MsgSetLastData(service_data->flags);
    }
    else
    {
        format_file_error_msg(idigi_ptr, service_request, NULL);
    }
    return idigi_callback_continue;
}

static size_t parse_file_put_header(file_system_context_t * context, uint8_t const * const header_ptr, size_t const buffer_size)
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

    size_t len = parse_file_path(context, fs_put_request, buffer_size - header_len);

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
    msg_session_t * session = service_request->session;
    file_system_context_t * context = session->service_context;

    if (fileSessionError(status, context))
        goto close_file;

    {
        msg_service_data_t    * service_data = service_request->have_data;
        uint8_t               * data_ptr = service_data->data_ptr;
        size_t                  bytes_to_write = service_data->length_in_bytes;
        size_t                  bytes_written = 0;

        if (MsgIsStart(service_data->flags))
        {
            size_t header_len = parse_file_put_header(context, data_ptr, service_data->length_in_bytes); 
            if (header_len == 0)
                goto done;

            if (context->fd == -1)
            {
                int open_flags = IDIGI_O_WRONLY | IDIGI_O_CREAT;   

                if (FileNeedTrunc(context))
                {
                    if (context->offset == 0)
                    {
                        open_flags |= IDIGI_O_TRUNC;
                        FileClearTrunc(context);
                    }
                }

                status = call_file_open_user(idigi_ptr, service_request, (char const *) data_ptr + FILE_OPCODE_BYTES, open_flags);
            }
            if (context->fd >= 0)
                status = set_file_position(idigi_ptr, service_request);

            if (status == idigi_callback_busy)
                goto done;

            if (fileSessionError(status, context))
               goto close_file;

            data_ptr       += header_len;
            bytes_to_write -= header_len;
        }

        if (context->bytes_done > bytes_to_write)
        {
            idigi_request_t request_id;
            request_id.file_system_request = idigi_file_system_write;

            notify_error_status(idigi_ptr->callback, idigi_class_file_system, request_id, idigi_invalid_data_size);
            set_file_system_service_error(service_request, idigi_msg_error_cancel); 
            
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
            if (fileSessionError(status, context))
               goto close_file;

            data_ptr       += cnt;
            bytes_to_write -= cnt;
            bytes_written  += cnt;
        }
        context->bytes_done = 0;
        context->offset  += bytes_written;

        if (!MsgIsLastData(service_data->flags))
            goto done;

        if (FileNeedTrunc(context))
        {
            status = call_file_ftruncate_user(idigi_ptr, service_request);
            if (status == idigi_callback_busy)
                goto done;

            if (fileSessionError(status, context))
                goto close_file;
        }
    }
close_file:
    call_file_close_user(idigi_ptr, service_request);

done:
    return status;
}

static size_t parse_file_ls_header(file_system_context_t *context,
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

        hash_alg = (idigi_file_hash_algorithm_t) *data_ptr;

        switch (hash_alg)
        {
            case idigi_file_hash_crc32:
            case idigi_file_hash_md5:
            case idigi_file_hash_best:
            case idigi_file_hash_none:
                context->hash_alg = hash_alg;
                break;

            default:
                ASSERT(idigi_false);
                context->error.error_code = idigi_file_system_request_format_error;
                len = 0;
        }
    }
    return len;
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
                                      size_t       path_len,
                                      uint8_t    * data_ptr)
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
    uint8_t flags = FileIsDir(context) ? IDIGI_FILE_IS_DIR : 0;
    size_t result;

    memcpy(data_ptr, path, path_len);

    message_store_u8(fs_ls_response,   flags, flags);
    message_store_be32(fs_ls_response, last_modified, context->last_modified);
    result = path_len + record_bytes(fs_ls_response_dir);

    if (!FileIsDir(context))
    {
        fs_ls_response += result;
        message_store_be32(fs_ls_response, size, context->file_size);
        result += record_bytes(fs_ls_response_file);
    }

    return result;
}

static idigi_callback_status_t file_store_path(idigi_data_t * const idigi_ptr, file_system_context_t * context, char const * path)
{
    idigi_callback_status_t status = idigi_callback_continue;
    size_t path_len = strlen(path);
    void *ptr;

    if (FileIsDir(context) && path[path_len - 1] != '/')
        path_len++;

    if (path_len >= IDIGI_MAX_PATH_LENGTH)
    {
        ASSERT(idigi_false);
        context->error.error_code = idigi_file_system_out_of_memory;
        goto done;
    }

    status = malloc_data(idigi_ptr, IDIGI_MAX_PATH_LENGTH, &ptr);
    if (status != idigi_callback_continue)
        goto done;

    context->path = ptr;
    strcpy(context->path, path);

    if (FileIsDir(context) && (path[path_len - 1] != '/'))
    {
        context->path[path_len - 1] = '/';
        context->path[path_len] = '\0';
    }
    context->path_len = path_len;

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
    
    if (context->error.error_code != idigi_file_system_noerror)
        goto done;

    if (parse_file_ls_header(context, service_data->data_ptr, service_data->length_in_bytes) == 0)
        goto done;
     
    if (FileGetState(context) < FILE_STATE_STAT_DONE)
    {
        status = call_file_stat_user(idigi_ptr, service_request, path, context->hash_alg);
        if (status == idigi_callback_busy || fileSessionError(status, context))
            goto done;

        FileSetState(context, FILE_STATE_STAT_DONE);
    }
    if (FileIsDir(context))
    {
        status = call_file_opendir_user(idigi_ptr, service_request, path);
        if (status == idigi_callback_busy || fileSessionError(status, context))
            goto done;
        
        /* to read next dir entry */
        FileSetState(context, FILE_STATE_NONE);
    }
    
    /* call_file_stat_user must be done before store path, to strcat '/' to dir path */ 
    status = file_store_path(idigi_ptr, context, path);
            
done:
    return status;
}

static idigi_callback_status_t process_file_ls_response(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    msg_service_data_t    * service_data = service_request->need_data;
    idigi_callback_status_t status = idigi_callback_continue;
    char * error_hint = NULL;

    if (context->error.error_code != idigi_file_system_noerror)
    {
        if (context->error.error_code == idigi_file_system_out_of_memory)
            error_hint = FILE_STR_ETOOLONG;
        goto error;
     }
 
    {
        uint8_t * data_ptr = service_data->data_ptr;
        size_t buffer_size = service_data->length_in_bytes;
        size_t    resp_len    = 0;

        size_t header_len = file_ls_resp_header_size();
        size_t hash_len   = file_hash_size(context->hash_alg);
        char * file_path;
        size_t file_path_len;

        if (MsgIsStart(service_data->flags))
        {
            resp_len = format_file_ls_response_header(data_ptr);
            buffer_size -= resp_len;
            data_ptr    += resp_len;
        }

        if (context->dir.dir_handle == NULL)
        {
            /* ls command was issued for a single file */
            file_path_len = context->path_len + 1;
            file_path     = context->path;

            if ((file_path_len + header_len + hash_len) > buffer_size)
            {
                context->error.error_code = idigi_file_system_out_of_memory;
                error_hint = FILE_STR_ETOOLONG;
                ASSERT_GOTO(idigi_false, error);
            }

            if (hash_len != 0)   
            {
                uint8_t * hash_ptr = data_ptr + file_path_len + header_len;
                status = call_file_hash_user(idigi_ptr, service_request, file_path, hash_ptr);
                if (status == idigi_callback_busy)
                    goto done;

                if (fileSessionError(status, context))
                    goto error;

                resp_len += hash_len;
            }

            MsgSetLastData(service_data->flags);
        }
        else
        {
            /* ls command was issued for a directory */
            file_path = context->path + context->path_len;

            if (FileGetState(context) < FILE_STATE_READDIR_DONE)
            {
                /* read next dir entry */
                size_t path_max = MIN_VALUE(buffer_size - (header_len + hash_len), IDIGI_MAX_PATH_LENGTH - context->path_len);
            
                status = call_file_readdir_user(idigi_ptr, service_request, file_path, path_max);
                if (status == idigi_callback_busy)
                    goto done;

                if (fileSessionError(status, context))
                    goto error;

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
                if ((context->path_len + file_path_len) > IDIGI_MAX_PATH_LENGTH)
                {
                    context->error.error_code = idigi_file_system_out_of_memory;
                    error_hint = FILE_STR_ETOOLONG;
                    ASSERT_GOTO(idigi_false, error);
                }
                /* strcat file name to after directory path */
                memcpy(context->path + context->path_len, file_path, file_path_len);

                status = call_file_stat_user(idigi_ptr, service_request, context->path, idigi_file_hash_none);
                if (status == idigi_callback_busy)
                    goto done;

                if (fileSessionError(status, context))
                    goto error;
 
                FileSetState(context, FILE_STATE_STAT_DONE);
            }
            if (FileIsDir(context))
            {
                hash_len = 0;
            }

            if (hash_len != 0)   
            {
                uint8_t * hash_ptr = data_ptr + file_path_len + header_len;

                status = call_file_hash_user(idigi_ptr, service_request, context->path, hash_ptr);
                if (status == idigi_callback_busy)
                    goto done;

                if (fileSessionError(status, context))
                    goto error;

                resp_len += hash_len;
            }

            /* to read next dir entry */
            FileSetState(context, FILE_STATE_NONE);
        }
        resp_len += format_file_ls_response(context, file_path, file_path_len, data_ptr);
        service_data->length_in_bytes = resp_len;
        goto done;
    }

error:
    /* have a messaging error, already set in service_request */
    if (context->error.error_code <= idigi_file_system_user_cancel) 
        goto close_dir;

    /* have a file error: 
     *  - if 1sr response message, send error response
     *  - otherwise, can't send error response, cancel the session
     */
    if (MsgIsStart(service_data->flags))
        format_file_error_msg(idigi_ptr, service_request, error_hint);
    else
    if (service_request->error_value == idigi_msg_error_none)
        set_file_system_service_error(service_request, idigi_msg_error_cancel);

close_dir:
    if (context->dir.dir_handle != NULL)
        call_file_closedir_user(idigi_ptr, service_request);

done:
    return status;
}


idigi_callback_status_t allocate_file_context(idigi_data_t * const idigi_ptr, file_system_context_t ** result)
{
    file_system_context_t * context = NULL;
    idigi_callback_status_t status;

    void * ptr;

    status = malloc_data(idigi_ptr, sizeof *context, &ptr);
    if (status != idigi_callback_continue)
        goto done;

    context = ptr;

    context->fd                 = -1;
    context->bytes_done         = 0;

    context->path_len           = 0;
    context->path               = NULL;
    context->dir.dir_handle     = NULL;
    context->dir.dir_entry      = NULL;

    context->user_context       = NULL;
    context->flags              = 0;
    context->state              = 0;
    context->error.errnum       = 0;
    context->error.error_code   = idigi_file_system_noerror;

done:
    *result = context;
    return status;
}

static idigi_callback_status_t file_system_request_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
    idigi_callback_status_t status = idigi_callback_continue;
    msg_service_data_t * service_data = service_request->have_data;

    file_system_opcode_t opcode;

    if (MsgIsStart(service_data->flags))
    {
        uint8_t * ptr = service_data->data_ptr;
        opcode = (file_system_opcode_t) *ptr;

        if (context == NULL)
        {
            status = allocate_file_context(idigi_ptr, &context);
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
        opcode = context->opcode;
    }

    if (opcode != fs_put_request_opcode && !MsgIsStart(service_data->flags))
    {
        /* don't support request in >1 message */
        context->error.error_code = idigi_file_system_request_format_error;
        ASSERT_GOTO(idigi_false, done);
    }

    switch (opcode)
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
            context->error.error_code = idigi_file_system_request_format_error;
            ASSERT_GOTO(idigi_false, done);
    }

done:
    return status;
}

static idigi_callback_status_t file_system_response_callback(idigi_data_t * const idigi_ptr, msg_service_request_t * const service_request)
{
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;
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
    msg_session_t * const session = service_request->session;
    file_system_context_t * context = session->service_context;

    idigi_file_error_request_t request;
    idigi_file_response_t      response;
    idigi_request_t request_id;

    size_t response_length = sizeof response;
    
    request_id.file_system_request = idigi_file_system_error;

    request.message_status = session->error;
    response.data_ptr = NULL;
    response.size_in_bytes = 0;
    response.context = context->user_context;

    idigi_callback(idigi_ptr->callback, idigi_class_file_system, request_id,
                   &request, sizeof request, &response, &response_length);

    context->user_context = response.context;
    
    if (context != NULL)
    {
        if (context->fd >= 0)
            call_file_close_user(idigi_ptr, service_request);

        if (context->dir.dir_handle != NULL)
            call_file_closedir_user(idigi_ptr, service_request);
    }

    file_system_free_callback(idigi_ptr, service_request);

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