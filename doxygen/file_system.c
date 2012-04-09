/*! @page file_system File System
 *
 * @htmlinclude nav.html
 *
 * @section file_system_overview1 File System
 *
 *  -# @ref file_system_overview
 *  -# @ref file_system_context
 *  -# @ref file_system_term
 *  -# @ref file_system_open
 *  -# @ref file_system_lseek
 *  -# @ref file_system_read
 *  -# @ref file_system_write
 *  -# @ref file_system_ftruncate
 *  -# @ref file_system_close
 *  -# @ref file_system_rm
 *  -# @ref file_system_opendir
 *  -# @ref file_system_readdir
 *  -# @ref file_system_closedir
 *  -# @ref file_system_stat
 *  -# @ref file_system_hash
 *  -# @ref file_system_strerror
 *  -# @ref file_system_msg_error 
 *  -# @ref file_data_compression
 *
 * @section file_system_overview File System Overview
 *
 * The file system facility is an optional facility for applications to access files on the device 
 * remotely from the iDigi Device Cloud. The IIK invokes the application-defined callbacks  
 * to read from a file and to write to a file, to list files and directory entries, and obtain  
 * a file hash value.
 *
 * A typical application-defined callback sequence for sending file data to the iDigi Device Cloud would include:
 *  -# IIK calls application-defined @ref file_system_open with read-only access.
 *  -# IIK calls application-defined @ref file_system_lseek. 
 *  -# IIK calls application-defined @ref file_system_read number of times, until
 *     the requested data amount is retrieved or the end of the file is reached.
 *  -# IIK calls application-defined @ref file_system_close.
 *
 * A typical application-defined callback sequence for writing data received from the iDigi Device Cloud to a file 
 * would include:
 *  -# IIK calls application-defined @ref file_system_open with write-create access.
 *  -# IIK calls application-defined @ref file_system_lseek. 
 *  -# IIK calls application-defined @ref file_system_write number of times, untill all data,
 *     received from the iDigi Device Cloud, is written to the file.
 *  -# IIK might call application-defined @ref file_system_ftruncate, if requested by the iDigi Device Cloud.
 *  -# IIK calls application-defined @ref file_system_close.
 *
 * In order to remove a file:
 *  -# IIK calls application-defined @ref file_system_rm.
 *
 * A typical application-defined callback sequence to get listing for a single file would be:
 *  -# IIK calls application-defined @ref file_system_stat. 
 *  -# IIK calls application-defined @ref file_system_hash if the hash value must be returned for this file.
 *
 * A typical application-defined callback sequence to get a directory listing would be:
 *  -# IIK calls application-defined @ref file_system_stat and learns that the path is a directory.
 *  -# IIK calls application-defined @ref file_system_opendir.
 *  -# For each directory entry IIK invokes application-defined callbacks:
 *  -# @ref file_system_readdir.
 *  -# @ref file_system_stat.
 *  -# @ref file_system_hash, if the hash value must be returned for this file.
 *  -# When all of the directory entries are processed, IIK calls application-defined @ref file_system_closedir.
 *
 * @note See @ref file_system under Configuration to enable or disable file system.
 *
 * @section file_system_context Session Context
 *
 * All file system response sctuctures have the <b><i>user_context</i></b> field. This field is provided to 
 * the application to identify the session and store session data between callbacks. 
 *
 * All application session memory must be released in the last callback of the session, typically 
 * @ref file_system_close or @ref file_system_closedir. This callback will be invoked if the file or 
 * directory was opened successfully, even if the session had an error or was canceled by the user.
 *
 * @section file_system_term Session Termination and Error Processing
 * 
 * All file system response sctuctures have the pointer to @ref idigi_file_error_data_t structure, which contains:
 * @li error_status - error status of @ref idigi_file_error_status_t type. 
 * @li errnum - errno.
 * 
 * The callback should use @ref idigi_file_noerror status in case of successful file operation and
 * @ref idigi_file_user_cancel status to cancel the session. Any other error status will be used to send 
 * the File System error message to the iDigi Device Cloud.
 *
 * The callback can set errnum to the system errno value. It will be used later in the @ref file_system_strerror 
 * to send the error string to the iDigi Device Cloud. 
 *
 * Different scenarios for the session termination are described below.
 *
 * If the session is successful:
 *  -# IIK calls @ref file_system_close or @ref file_system_closedir if there is an open file or directory.
 *  -# IIK sends last response to the iDigi Device Cloud.
 *
 * The callback returns @ref idigi_callback_abort:
 *  -# IIK calls @ref file_system_close or @ref file_system_closedir if there is an open file or directory.
 *  -# IIK is aborted.
 *
 * The callback returns @ref idigi_callback_continue and sets error_status to @ref idigi_file_user_cancel:
 *  -# IIK calls @ref file_system_close or @ref file_system_closedir if there is an open file or directory.
 *  -# IIK canceles the session.
 *
 * The callback returns @ref idigi_callback_continue and sets error status and errnum:
 *  -# IIK calls @ref file_system_close or @ref file_system_closedir if there is an open file or directory.
 *  -# If IIK has already sent part of file or directory data, it calcels the session. This is due to he fact 
 *     that it can't differentiate an error response from part of the data response.
 *  -# Otherwise IIK calls @ref file_system_strerror and sends an error response to the iDigi Device Cloud.
 *
 * File system was notified of an error in the messaging layer:
 *  -# IIK calls @ref file_system_close or @ref file_system_closedir if there is an open file or directory.
 *  -# IIK calls @ref file_system_msg_error.
 *  -# IIK canceles the session.
 *
 * @section file_system_open Open a file callback
 *
 * Open a file for the spesified path. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_open @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_open_request_t @htmlonly structure:
 *   <p><b><i>path</i></b> - File path is a null-terminated string.
 *   <p><b><i>oflag</i></b> - Bitwise-inclusive OR of @endhtmlonly @ref file_open_flag_t @htmlonly flags.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_open_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_open_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks. 
 *   <p><b><i>size_in_bytes</i></b> - sizeof (int).
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>fd_ptr</i></b> - Pointer to memory where the callback places a file descriptor of an open file. This file 
 *                             descriptor will be used in subsequent callbacks to read from a file,
 *                             write to a file, set file position, truncate a file, and close a file.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_open_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>File opened successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_open)
 *    {
 *       status = app_process_file_open(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_open(idigi_file_open_request_t const * const request_data, 
 *                                               idigi_file_open_response_t * response_data)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 *    int oflag = 0; 
 *    int fd;
 *
 *    if (request_data->oflag & IDIGI_O_RDONLY) oflag |= O_RDONLY;
 *    if (request_data->oflag & IDIGI_O_WRONLY) oflag |= O_WRONLY;
 *    if (request_data->oflag & IDIGI_O_RDWR)   oflag |= O_RDWR;
 *    if (request_data->oflag & IDIGI_O_APPEND) oflag |= O_APPEND;
 *    if (request_data->oflag & IDIGI_O_CREAT)  oflag |= O_CREAT;
 *    if (request_data->oflag & IDIGI_O_TRUNC)  oflag |= O_TRUNC;
 *
 *    if (oflag & O_CREAT)
 *    {
 *       fd = open(request_data->path, oflag, 0664);
 *    }
 *    else
 *    {
 *       fd = open(request_data->path, oflag);
 *    }
 *    APP_DEBUG("Open %s, %d, returned %d\n", request_data->path, oflag, fd);
 *
 *    if (fd < 0)
 *    {
 *      error_data->errnum = errno;
 *      switch(error_data->errnum)
 *      {
 *          case EACCES:
 *          case EPERM:
 *              error_data->error_status = idigi_file_permision_denied;
 *              break;
 *          case ENOMEM:
 *              error_data->error_status = idigi_file_out_of_memory;
 *              break;
 *	    	case ENOENT:
 *	    	case EISDIR:
 *	    	case EBADF:
 *              error_data->error_status = idigi_file_path_not_found;
 *              break;
 *          case EINVAL:
 *              error_data->error_status = idigi_file_invalid_parameter;
 *              break;
 *          case EAGAIN:
 *              error_data->errnum = 0;
 *              status = idigi_callback_busy;
 *              break;
 *	    	case ENOSPC:
 *	    		error_data->error_status = idigi_file_insufficient_storage_space;
 *              break;
 *          default:
 *              error_data->errnum = idigi_file_unspec_error;
 *      }
 *    }
 *    *response_data->fd_ptr = fd;
 *
 *    return status;
 * }
 * @endcode
 *
 * @section file_system_lseek   Seek file position callback
 *
 * Set the file offset for a file, associated with the open file descriptor.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_lseek @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_lseek_request_t @htmlonly structure:
 *   <p><b><i>fd</i></b> - File descriptor of an open file.
 *   <p><b><i>offset</i></b> - Offset in bytes relative to origin.
 *   <p><b><i>origin</i></b> - IDIGI_SEEK_CUR, IDIGI_SEEK_SET, or IDIGI_SEEK_END, see
 *                             @endhtmlonly @ref idigi_file_seek_origin_t @htmlonly
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_lseek_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_lseek_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - sizeof (long int).
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>offset_ptr</i></b> - Pointer to memory where the callback places resulting file offset.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_lseek_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>File offset set successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_lseek)
 *    {
 *       status = app_process_file_lseek(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_lseek(idigi_file_lseek_request_t const * const request_data, 
 *                                                idigi_file_lseek_response_t * response_data)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 *    int origin;
 *    long int offset;
 *
 *    if (request_data->origin == IDIGI_SEEK_SET)
 *       origin = SEEK_SET;
 *    if (request_data->origin == IDIGI_SEEK_END)
 *       origin = SEEK_END;
 *    else
 *       origin = SEEK_CUR;
 *
 *    offset = lseek(request_data->fd, request_data->offset, origin);
 *
 *    APP_DEBUG("lseek fd %d, offset %ld, origin %d returned %ld\n", request_data->fd, 
 *               request_data->offset, request_data->origin, offset);
 *
 *    *response_data->offset_ptr = offset;
 *
 *    if (offset < 0)
 *    {
 *      error_data->errnum = errno;
 *          
 *      switch(error_data->errnum)
 *      {
 *          case EBADF:
 *              error_data->error_status = idigi_file_path_not_found;
 *              break;
 *          case EINVAL:
 *              error_data->error_status = idigi_file_invalid_parameter;
 *              break;
 *	    	case ENOSPC:
 *          case EFBIG:
 *	    		error_data->error_status = idigi_file_insufficient_storage_space;
 *              break;
 *          case EAGAIN:
 *              error_data->errnum = 0;
 *              status = idigi_callback_busy;
 *              break;
 *          default:
 *              error_data->errnum = idigi_file_unspec_error;
 *      } 
 *    }
 *    return status;
 * }
 *
 * @endcode
 *
 * @section file_system_read    Read file data callback
 *
 * Read data from a file, associated with the open file descriptor.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_read @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_request_t @htmlonly structure:
 *   <p><b><i>fd</i></b> - File descriptor of an open file.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - [IN] Size of the memory buffer. [OUT] Number of bytes
 *                                    retrieved from the file and written to the memory buffer.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - Pointer to memory where the callback writes data.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Reading from a file succeded or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_read)
 *    {
 *       status = app_process_file_read(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_read(idigi_file_request_t const * const request_data, 
 *                                               idigi_file_response_t * response_data)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 * 
 *    int result = read(request_data->fd, response_data->data_ptr, response_data->size_in_bytes);
 *
 *    APP_DEBUG("read %d, %u, returned %d\n", request_data->fd, response_data->size_in_bytes, result);
 * 
 *    if (result >= 0)
 *    {
 *        response_data->size_in_bytes = result;
 *    }
 *    else
 *    if (result < 0)
 *    {
 *      error_data->errnum = errno;
 *          
 *      switch(error_data->errnum)
 *      {
 *          case EBADF:
 *              error_data->error_status = idigi_file_path_not_found;
 *              break;
 *          case EINVAL:
 *              error_data->error_status = idigi_file_invalid_parameter;
 *              break;
 *          case EAGAIN:
 *              error_data->errnum = 0;
 *              status = idigi_callback_busy;
 *              break;
 *          default:
 *              error_data->errnum = idigi_file_unspec_error;
 *      } 
 *    }
 *    return status;
 * } 
 *
 * @endcode
 *
 * @section file_system_write   Write file data callback
 *
 * Write data from to a file, associated with the open file descriptor.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_write @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_write_request_t @htmlonly structure:
 *   <p><b><i>fd</i></b> - File descriptor of an open file.
 *   <p><b><i>data_ptr</i></b> - Pointer to data to write to the file.
 *   <p><b><i>size_in_bytes</i></b> - Number of data bytes to write to the file.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_write_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - Number of bytes actually written to the file.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - NULL.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Writing to a file succeded or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_write)
 *    {
 *       status = app_process_file_write(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_write(idigi_file_write_request_t const * const request_data, 
 *                                                idigi_file_response_t * response_data)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 * 
 *    int result = write(request_data->fd, request_data->data_ptr, request_data->size_in_bytes);
 *
 *    APP_DEBUG("write %d, %u, returned %d\n", request_data->fd, response_data->size_in_bytes, result);
 * 
 *    if (result >= 0)
 *    {
 *        response_data->size_in_bytes = result;
 *    }
 *    else
 *    if (result < 0)
 *    {
 *      error_data->errnum = errno;
 *          
 *      switch(error_data->errnum)
 *      {
 *          case EBADF:
 *              error_data->error_status = idigi_file_path_not_found;
 *              break;
 *          case EINVAL:
 *              error_data->error_status = idigi_file_invalid_parameter;
 *              break;
 *          case EAGAIN:
 *              error_data->errnum = 0;
 *              status = idigi_callback_busy;
 *              break;
 *          default:
 *              error_data->errnum = idigi_file_unspec_error;
 *      } 
 *    }
 *    return status;
 * } 
 *
 * @endcode
 *
 * @section file_system_ftruncate   Truncate a file callback
 *
 *
 * Truncate a file to a specified length.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_ftruncate @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_ftruncate_request_t @htmlonly structure:
 *   <p><b><i>fd</i></b> - File descriptor of an open file.
 *   <p><b><i>length</i></b> - Length to truncate file to.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_ftruncate_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - 0.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - NULL.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>File truncated successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_ftruncate)
 *    {
 *       status = app_process_file_ftruncate(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_ftruncate(idigi_file_ftruncate_request_t const * const request_data, 
 *                                                     idigi_file_response_t * response_data)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 * 
 *    int result = ftruncate(request_data->fd, request_data->offset);
 *
 *    APP_DEBUG("ftruncate %d, %ld returned %d\n", request_data->fd, request_data->offset, result);
 * 
 *    if (result < 0)
 *    {
 *      error_data->errnum = errno;
 *          
 *      switch(error_data->errnum)
 *      {
 *          case EBADF:
 *          case EINVAL:
 *          case EFBIG:
 *              error_data->error_status = idigi_file_invalid_parameter;
 *              break;
 *          case EAGAIN:
 *              error_data->errnum = 0;
 *              status = idigi_callback_busy;
 *              break;
 *          default:
 *              error_data->errnum = idigi_file_unspec_error;
 *      } 
 *    }
 *    return status;
 * } 
 *
 * @endcode
 *
 * @section file_system_close   Close a file callback
 *
 * Close a file, associated with the open file descriptor.
 *
 * This callback must free all memory, allocated during the file system session.
 *
 * @note IIK invokes this callback only once for any open file, the callback must not return 
 * @ref idigi_callback_busy.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_close @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_request_t @htmlonly structure:
 *   <p><b><i>fd</i></b> - File descriptor of an open file.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Any allocated memory must be freed in this callback.
 *   <p><b><i>size_in_bytes</i></b> - 0.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - NULL.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>File closed successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly 
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_close)
 *    {
 *       status = app_process_file_close(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_close(idigi_file_request_t const * const request_data, 
 *                                                idigi_file_response_t * response_data)
 * {
 *     int result = close(request_data->fd);
 *     idigi_file_error_data_t * error_data = response_data->error;
 * 
 *     APP_DEBUG("close %d returned %d\n", request_data->fd, result);
 * 
 *     if (result < 0 && errno == EIO)
 *     {
 *         error_data->errnum = EIO;
 *         error_data->error_status = idigi_file_unspec_error;
 *     }
 *     if (response_data->user_context != NULL)
 *     {
 *         // Session data must be freed here
 *     }
 *     return idigi_callback_continue;
 *  } 
 *
 * @endcode
 *
 * @section file_system_rm      Remove a file callback
 *
 * Remove a file for the spesified path. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_rm @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_path_request_t @htmlonly structure:
 *   <p><b><i>path</i></b> - File path is a null-terminated string.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_path_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - 0.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - NULL.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>File removed successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_rm)
 *    {
 *       status = app_process_file_rm(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_rm(idigi_file_path_request_t const * const request_data, 
 *                                             idigi_file_response_t * response_data)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 *
 *     // Posix function to remove a file
 *     int result = unlink(request_data->path);
 *
 *     APP_DEBUG("unlink %s returned %d\n", request_data->path, result);
 *
 *     if (result < 0)
 *     {
 *          error_data->errnum = errno;
 *          switch(error_data->errnum)
 *          {
 *              case EACCES:
 *              case EPERM:
 *                  error_data->error_status = idigi_file_permision_denied;
 *                  break;
 *              case ENOMEM:
 *                  error_data->error_status = idigi_file_out_of_memory;
 *                  break;
 *	        	case ENOENT:
 *	        	case EISDIR:
 *	        	case EBADF:
 *                  error_data->error_status = idigi_file_path_not_found;
 *                  break;
 *              case EINVAL:
 *                  error_data->error_status = idigi_file_invalid_parameter;
 *                  break;
 *              case EAGAIN:
 *                  error_data->errnum = 0;
 *                  status = idigi_callback_busy;
 *                  break;
 *              default:
 *                  error_data->errnum = idigi_file_unspec_error;
 *          }
 *     }
 *     return status;
 *  } 
 *
 * @endcode
 *
 * @section file_system_opendir     Open a directory callback
 *
 * Open a directory for the spesified path. 
 * 
 * If the callback has successfully opened a directory, it should fill in @ref idigi_file_dir_data_t structure 
 * of the @ref idigi_file_dir_response_t which will be used in future callbacks to read directory entries and close the directory.
 *
 * The directory handle dir_data->dir_handle of an open directory must not be NULL. @ref file_system_readdir and 
 * @ref file_system_closedir will use this handle to identify the directory. 
 *
 * The directory entry handle dir_data->dir_entry can be used to identify the directory entry from one 
 * @ref file_system_readdir to another. 
 *
 * The application can allocate memory to store the directory entry data in @ref file_system_opendir and
 * initialize the directory entry handle to point to this memory. Any memory, allocated for the directory handle 
 * and the directory entry handle must be freed in @ref file_system_closedir.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_opendir @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_path_request_t @htmlonly structure:
 *   <p><b><i>path</i></b> - File path is a null-terminated string.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_path_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_dir_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - sizeof *dir_data.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                              where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                              and errno in case of an error.
 *   <p><b><i>dir_data</i></b> - Pointer to a @endhtmlonly @ref idigi_file_dir_data_t @htmlonly structure:
 *      <p><b><i> - dir_handle</i></b> - Directory handle.
 *      <p><b><i> - dir_entry</i></b>  - Directory entry handle. 
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_dir_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Directory opened successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_opendir)
 *    {
 *       status = app_process_file_opendir(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_opendir(idigi_file_path_request_t const * const request_data, 
 *                                                  idigi_file_dir_response_t * response_data)
 * {
 * 
 *    idigi_callback_status_t status = idigi_callback_continue;
 *    idigi_file_error_data_t * error_data = response_data->error;
 *    DIR * dirp;
 * 
 *     errno = 0;
 *     dirp = opendir(request_data->path);
 *
 *     APP_DEBUG("opendir for %s returned %p\n", request_data->path, (void *) dirp);
 *
 *     if (dirp != NULL)
 *     {
 *        // Allocate memory for struct dirent to be used in readdir_r
 *        // in a idigi_file_system_readdir callback:
 *        // readdir_r((DIR *) dir_data->dir_handle, (struct dirent *) dir_data->dir_entry, &result);
 *
 *        void * ptr;
 *        int result  = app_os_malloc(sizeof(struct dirent), &ptr);
 * 
 *        if (result == 0 && ptr != NULL)
 *        {
 *             idigi_file_dir_data_t *dir_data = response_data->dir_data;
 * 
 *             dir_data->dir_handle = dirp; 
 *             dir_data->dir_entry  = ptr;
 *        }
 *         else
 *         {
 *             error_data->error_status = idigi_file_out_of_memory;
 *             error_data->errnum     = ENOMEM; 
 *             closedir(dirp);
 *
 *             APP_DEBUG("app_process_file_opendir: malloc fails %s\n", request_data->path);
 *             APP_DEBUG("closedir for %s\n", request_data->path);
 *         }
 *    }
 *    else
 *    {
 *          error_data->errnum = errno;
 *          switch(error_data->errnum)
 *          {
 *             case EACCES:
 *                  error_data->error_status = idigi_file_permision_denied;
 *                 break;
 *              case ENOMEM:
 *                  error_data->error_status = idigi_file_out_of_memory;
 *                  break;
 *	        	case ENOENT:
 * 	        	case ENOTDIR:
 *                 error_data->error_status = idigi_file_path_not_found;
 *                  break;
 *              case EINVAL:
 *                  error_data->error_status = idigi_file_invalid_parameter;
 *                 break;
 *              case EAGAIN:
 *                 error_data->errnum = 0;
 *                  status = idigi_callback_busy;
 *                  break;
 *              default:
 *                 error_data->errnum = idigi_file_unspec_error;
 *          }
 *    }
 *    return status;
 * }
 *
 * @endcode
 *
 * @section file_system_readdir     Read next directory entry callback
 *
 * Read the next directory entry, specified by the directory handle and the directory entry handle, 
 * returned in the @ref file_system_opendir. Return the directory entry name.
 *
 * The directory entry name is a null-terminated character string. The callback must use '\0' when  
 * all directory entries have been processed.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_readdir @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_dir_data_t @htmlonly structure:
 *   <p><b><i>dir_handle</i></b> - Directory handle. 
 *   <p><b><i>dir_entry</i></b> - Directory entry handle.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_dir_data_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - [IN] Size of the memory buffer. [OUT] Length of the ditectory name, 
 *                                      including ending '\0'.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                              where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                              and errno in case of an error.
 *   <p><b><i>data_ptr</i></b> - Pointer to memory where the callback writes the directory name. 
 *                               Callback must use an empty string "" if it could not read the directory entry. 
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Next directory entry returned or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_readdir)
 *    {
 *       status = app_process_file_readdir(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_readdir(idigi_file_dir_data_t const * const request_data, 
 *                                                  idigi_file_response_t * response_data)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     struct dirent * direntp = request_data->dir_entry;
 *     DIR           * dirp    = request_data->dir_handle;
 *     idigi_file_error_data_t * error_data = response_data->error;
 *     struct dirent * result;
 *     int rc;
 * 
 *     // This sample does not skip "." and "..", but you might consider doing so.
 * 
 *     errno = 0;
 *     rc = readdir_r(dirp, direntp, &result);
 * 
 *     if (result == NULL)
 *         APP_DEBUG("readdir_t: result = NULL\n");
 *     else
 *         APP_DEBUG("readdir_r: directory %s\n",  result->d_name);
 * 
 *     if (result != NULL && rc == 0)
 *     {
 *         if((strlen(result->d_name) < response_data->size_in_bytes)
 *         {
 *
 *             strcpy((char *) response_data->data_ptr, result->d_name);
 *            response_data->size_in_bytes = strlen(result->d_name) + 1;
 *         }
 *         else
 *         {
 *             error_data->errnum = ENAMETOOLONG;
 *             error_data->error_status = idigi_file_unspec_error;
 *         }
 *     }
 *     else
 *     if (rc != 0)
 *     {
 *           error_data->errnum = errno;
 *           switch(error_data->errnum)
 *           {
 * 	          case ENOENT:
 *               case EBADF:
 *                  error_data->error_status = idigi_file_path_not_found;
 *                  break;
 *               case EAGAIN:
 *                  error_data->errnum = 0;
 *                   status = idigi_callback_busy;
 *                   break;
 *               case EOVERFLOW:
 *               default:
 *                 error_data->errnum = idigi_file_unspec_error;
 *           }
 *    }
 *    return status;
 * }
 *
 * @endcode
 *
 * @section file_system_closedir    Close a directory callback
 *
 * Close a directory, specified by the directory handle, returned in the @ref file_system_opendir.
 *
 * This callback must free all memory, allocated during the file system session.
 *
 * @note IIK invokes this callback only once for any opene directory, the callback must not return 
 * @ref idigi_callback_busy.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_closedir @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_dir_data_t @htmlonly structure:
 *   <p><b><i>dir_handle</i></b> - Directory handle. 
 *   <p><b><i>dir_entry</i></b>  - Directory entry handle. Must be freed in this callback,
 *                                 if previously allocated.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_dir_data_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Any allocated memory must be freed in this callback.
 *   <p><b><i>size_in_bytes</i></b> - 0.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                              where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                              and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - NULL.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Directory closed successfully or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_closedir)
 *    {
 *       status = app_process_file_closedir(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_closedir(idigi_file_dir_data_t const * const request_data, 
 *                                                   idigi_file_response_t * response_data)
 * {
 *    UNUSED_ARGUMENT(response_data);
 *
 *    APP_DEBUG("closedir for %p\n", request_data->dir_handle);
 *    closedir((DIR *) request_data->dir_handle);
 *    
 *    if (request_data->dir_entry != NULL)
 *        app_os_free(request_data->dir_entry);
 *
 *    if (response_data->user_context != NULL)
 *    {
 *      // Session data must be freed here
 *   }
 *   return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section file_system_stat        Get file status callback
 *
 * Get file status for the named file, specified by the path.
 *
 * The named file might be a regular file, directory, or neither. 
 *
 * When the iDigi Device Cloud requests the directory listing, it suggests 
 * the hash algorithm for the whole directory. The returned actual hash 
 * algorithm will apply to regular files only, as directory entries are processed.
 * When IIK will call @ref file_system_stat callback for each directory entry, 
 * it would no more ask for the hash algorithm (using @ref idigi_file_hash_none).
 *
 * When the iDigi Device Cloud requests the file listing, it suggests 
 * the hash algorithm for this file. The returned actual hash be used only if 
 * the path names a regular file.
 *
 * If the suggested hash algorithm is @ref idigi_file_hash_best, the actual must be
 * @ref idigi_file_hash_md5, @ref idigi_file_hash_crc32, or @ref idigi_file_hash_none.
 *
 * If the suggested hash algorithm is @ref idigi_file_hash_md5, or @ref idigi_file_hash_crc32,
 * the actual must be the same or @ref idigi_file_hash_none.
 *
 * If the suggested hash algorithm is @ref idigi_file_hash_none, the actual must be
 * @ref idigi_file_hash_none.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_stat @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_stat_request_t @htmlonly structure:
 *   <p><b><i>path</i></b> - File path is a null-terminated string.
 *   <p><b><i>hash_alg</i></b> - Suggested hash algorithm of @endhtmlonly @ref idigi_file_hash_algorithm_t @htmlonly type.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_stat_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_stat_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - sizeof *stat_ptr.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>stat_ptr</i></b> - Pointer to a @endhtmlonly @ref idigi_file_stat_t @htmlonly structure where 
 *                            the callback writes file status data.
 *      <p><b><i> - last_modified</i></b> - Last modified time (seconds since 1970). If not supported, use 0.
 *      <p><b><i> - file_size</i></b> - File size in bytes.
 *      <p><b><i> - hash_alg</i></b> - Actual hash algorithm: @endhtmlonly @ref idigi_file_hash_crc32 @htmlonly,
 *                  @endhtmlonly @ref idigi_file_hash_md5 @htmlonly, or @endhtmlonly @ref idigi_file_hash_none @htmlonly,
 *      <p><b><i> - flags</i></b> - Non-zero if the file is a regular file or a directory: @endhtmlonly @ref file_stat_flag_t @htmlonly.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_stat_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Status information returned or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_stat)
 *    {
 *       status = app_process_file_stat(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_stat(idigi_file_stat_request_t const * const request_data, 
 *                                             idigi_file_stat_response_t * response_data)
 * {
 *     struct stat statbuf;
 *     idigi_file_stat_t * pstat = response_data->stat_ptr;
 *     idigi_callback_status_t status = idigi_callback_continue;
 *     idigi_file_error_data_t * error_data = response_data->error;
 * 
 *     int result = stat(request_data->path, &statbuf);
 * 
 *     APP_DEBUG("stat for %s returned %d, filesize %ld\n", request_data->path, result, statbuf.st_size);
 * 
 *     if (result == 0)
 *     {
 *         pstat->flags         = 0;
 *         pstat->file_size     = statbuf.st_size;
 *         pstat->last_modified = statbuf.st_mtime;
 *         pstat->hash_alg      = idigi_file_hash_none;
 * 
 *         if (S_ISDIR(statbuf.st_mode))
 *            pstat->flags |= IDIGI_FILE_IS_DIR;
 *         else
 *         if (S_ISREG(statbuf.st_mode))
 *            pstat->flags |= IDIGI_FILE_IS_REG;
 *     }
 *     else
 *     {
 *         error_data->errnum = errno;
 *         switch(error_data->errnum)
 *         {
 *          case EACCES:
 *              error_data->error_status = idigi_file_permision_denied;
 *               break;
 *           case ENAMETOOLONG:
 *               error_data->error_status = idigi_file_out_of_memory;
 *               break;
 *	        	case ENOENT:
 *               error_data->error_status = idigi_file_path_not_found;
 *               break;
 *	        	case ENOTDIR:
 *	        	case EBADF:
 *           case EINVAL:
 *               error_data->error_status = idigi_file_invalid_parameter;
 *               break;
 *           case EAGAIN:
 *               error_data->errnum = 0;
 *               status = idigi_callback_busy;
 *               break;
 *           default:
 *               error_data->errnum = idigi_file_unspec_error;
 *         }
 *    }
 *    return status;
 * }
 *
 * @endcode
 *
 * @section file_system_hash        Get file hash value callback
 * 
 * Get file hash value for the specified path.
 *
 * If the application callback needs more than one pass to calculate the hash value,
 * it can save data for the next pass in response_data->user_context and return @ref idigi_callback_busy. 
 * The callback will return @ref idigi_callback_continue when it completes hash calculations. 
 *
 * If the callback encounters an error while reading a file it should 
 * return zero hash value, rather than set an error status. Setting an error status will result
 * in premature termination of the session that is getting a directory listing.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_hash @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_stat_request_t @htmlonly structure:
 *   <p><b><i>path</i></b> - File path is a null-terminated string.
 *   <p><b><i>hash_alg</i></b> - Hash algorithm @endhtmlonly @ref idigi_file_hash_md5 @htmlonly
 *                               or @endhtmlonly @ref idigi_file_hash_crc32 @htmlonly.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_stat_request_t @htmlonly structure</td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user to be used on subsequent callbacks.
 *   <p><b><i>size_in_bytes</i></b> - Size of memory buffer for hash value: 16 bytes for md5, 4 bytes for crc32.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure
 *                            where the callback writes @endhtmlonly @ref idigi_file_error_status_t @htmlonly
 *                            and errno in case of file error.
 *   <p><b><i>data_ptr</i></b> - Pointer to  memory to write hash value.</td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Status information returned or error has occured</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Busy. The callback will be repeated.
 * </td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_hash)
 *    {
 *       status = app_process_file_hash(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_hash(idigi_file_stat_request_t const * const request_data, 
 *                                             idigi_file_response_t * response_data)
 * {
 *     idigi_callback_status_t status = idigi_callback_continue;
 *
 *     switch(request_data->hash_alg)
 *     {
 *          case idigi_file_hash_crc32:
 *              status = app_calc_crc32(request_data->path, response_data);
 *              break;
 *
 *          case idigi_file_hash_md5:
 *              status = app_calc_md5(request_data->path, response_data);
 *              break;
 *
 *          default:
 *              ASSERT(0);
 *     }
 *     return status;
 * }
 *
 * @endcode
 *
 * @section file_system_strerror    Get error description callback
 *
 * Get error description string to send to the iDigi Device Cloud.
 *
 * IIK invokes this callback if an earlier callback has set an error status and   
 * non-zero errnum in @ref idigi_file_error_data_t structure of the response.
 *
 * IIK calls the @ref file_system_close or the @ref file_system_closedir
 * prior to calling the @ref file_system_strerror. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_strerror @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th> <td> NULL </td>
 * </tr>
 * <tr>
 * <td>request_length</td> <td> 0 </td>
 * </tr>
 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - - Set by a user in an earlier callback.
 *   <p><b><i>size_in_bytes</i></b> - [IN] Size of the memory buffer. 
 *                                      [OUT] Length of the error description string, including ending '\0'.
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure.
 *      <p><b><i>- error_status</i></b> - [IN] Error status of @endhtmlonly @ref idigi_file_error_status_t @htmlonly type
 *                                        . 
 *      <p><b><i>- errnum</i></b> - [IN] errno. Used as an input for an error description string. 
 *   <p><b><i>data_ptr</i></b> - Pointer to memory where the callback writes the error description string.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Continue</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_strerror)
 *    {
 *       status = app_process_file_strerror(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 * idigi_callback_status_t app_process_file_strerror(void const * const request_data, 
 *                                                   idigi_file_response_t * response_data)
 * {
 *     size_t strerr_size = 0;
 *
 *    UNUSED_ARGUMENT(request_data);
 * 
 *    idigi_file_error_data_t * error_data = response_data->error;
 * 
 *    if (error_data->errnum != 0)
 *    {
 *         char * err_str = strerror(error_data->errnum);
 *        char * ptr = response_data->data_ptr;
 *      
 *        strerr_size = strnlen(err_str, response_data->size_in_bytes - 1) + 1;
 *        memcpy(ptr, err_str, strerr_size);
 *         ptr[strerr_size] = '\0';
 *    }
 * 
 *    response_data->size_in_bytes = strerr_size;
 *
 *     return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section file_system_msg_error   Inform of a messaging error callback
 *
 * Inform the application callback of an error in the messaging layer.
 *
 * IIK calls the @ref file_system_close or the @ref file_system_closedir
 * prior to calling the @ref file_system_msg_error. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_file_system @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_file_system_msg_error @htmlonly</td>
 * </tr>
 * <tr>
 * <th>request_data</th>
 * <td> [IN] pointer to @endhtmlonly @ref idigi_file_error_request_t @htmlonly structure:
 *   <p><b><i>message_status</i></b> - @endhtmlonly @ref idigi_msg_error_t @htmlonly.
 * </td></tr>
 * <tr>
 * <td>request_length</td>
 * <td> [IN] Size of @endhtmlonly @ref idigi_file_error_request_t @htmlonly structure</td>
 * </tr>

 * <tr>
 * <th>response_data</th>
 * <td> [OUT] pointer to @endhtmlonly @ref idigi_file_response_t @htmlonly structure:
 *   <p><b><i>user_context</i></b> - Set by a user in an earlier callback.
 *   <p><b><i>size_in_bytes</i></b> - 0. 
 *   <p><b><i>error</i></b> - Pointer to a @endhtmlonly @ref idigi_file_error_data_t @htmlonly structure.
 *   <p><b><i>data_ptr</i></b> - NULL.
 * </td></tr>
 * <tr>
 * <td>response_length</td>
 * <td>[OUT] Size of @endhtmlonly @ref idigi_file_response_t @htmlonly structure</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Continue</td>
 * </tr>
 * </table>
 * @endhtmlonly
 * 
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_d,
 *                                            void const * request_data, size_t const request_length,
 *                                            void * response_data, size_t * const response_length)
 * {
 *    idigi_callback_status_t status = idigi_callback_continue;
 *
 *    if (class_id == idigi_class_file_system && request_id.file_system_request == idigi_file_system_msg_error)
 *    {
 *       status = app_process_file_msg_error(request_data, response_data);
 *    }
 *    return status;  
 * }
 *
 *  idigi_callback_status_t app_process_file_msg_error(idigi_file_error_request_t const * const request_data, 
 *                                                     idigi_file_response_t * response_data)
 * {
 *     APP_DEBUG("Message Error %d\n", (int) request_data->message_status);
 * 
 *     if (response_data->user_context != NULL)
 *     {
 *         // Any file, opened to caclulate hash value, must be closed here.
 *         // Session data must be freed here
 *     }
 * 
 *    return idigi_callback_continue;
 * }
 *
 * @endcode
 *
 * @section file_data_compression Optional Data Compression Support
 * The IIK has an optional Data Compression switch that reduces the amount of network traffic.  This option requires applications
 * to link with the zlib library and add the zlib header file (zlib.h) to the IIK include path.
 *
 * @note Enabling this option greatly increases the application code size and memory required to execute.
 *
 * If your application requires Data compression, but your development environment does not include the zlib library,
 * you will need to download and build the library.  The zlib home page is located at: http://zlib.net/.   Instructions
 * on how to build zlib are provided with the package.
 *
 * @note The zlib library is required only if your application enables the @ref IDIGI_COMPRESSION "Data Compression switch".
 *
 */
