/*
 * Copyright (c) 1996-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include "iDigiFileSystem.h"
#include "iDigiConnector.h"

extern iDigiConnectorClass iDigi;

void iDigiFileSystem::enableSharing(uint8_t sdChipSelect, uint8_t ssPin)
{
	sdCS = sdChipSelect;
	this->ssPin = ssPin;

	pinMode(ssPin, OUTPUT);
	digitalWrite(ssPin, HIGH);

	DigiSD.begin(sdCS);
}

void iDigiFileSystem::disableSharing()
{

};

idigi_callback_status_t iDigiFileSystem::appReqHandler(idigi_file_system_request_t const request,
                                        void const * request_data, size_t const request_length,
                                        void * response_data, size_t * const response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch (request)
    {

        case idigi_file_system_open:
            status = app_process_file_open((idigi_file_open_request_t * const) request_data,
            	                           (idigi_file_open_response_t * const) response_data);
            break;

        case idigi_file_system_read:
            status = app_process_file_read((idigi_file_request_t * const) request_data,
            	                           (idigi_file_data_response_t * const) response_data);
            break;

        case idigi_file_system_write:
            status = app_process_file_write((idigi_file_write_request_t *) request_data,
            								(idigi_file_write_response_t *) response_data);
            break;

        case idigi_file_system_lseek:
            status = app_process_file_lseek((idigi_file_lseek_request_t * const) request_data,
            	                            (idigi_file_lseek_response_t * const) response_data);
            break;

        case idigi_file_system_close:
            status = app_process_file_close((idigi_file_request_t * const) request_data,
            	                            (idigi_file_response_t * const) response_data);
            break;

        case idigi_file_system_ftruncate:
            status = app_process_file_ftruncate((idigi_file_ftruncate_request_t * const) request_data,
            	                                (idigi_file_response_t * const) response_data);
            break;

        case idigi_file_system_rm:
            status = app_process_file_rm((idigi_file_path_request_t * const) request_data,
            	                         (idigi_file_response_t * const) response_data);
            break;

        case idigi_file_system_stat:
            status = app_process_file_stat((idigi_file_stat_request_t * const) request_data,
            	                           (idigi_file_stat_response_t * const) response_data);
            break;

        case idigi_file_system_opendir:
            status = app_process_file_opendir((idigi_file_path_request_t * const) request_data,
            	                              (idigi_file_open_response_t * const) response_data);
            break;
        case idigi_file_system_readdir:
            status = app_process_file_readdir((idigi_file_request_t * const) request_data, 
            	                              (idigi_file_data_response_t * const) response_data);
            break;

        case idigi_file_system_closedir:
            status = app_process_file_closedir((idigi_file_request_t * const) request_data,
            	                               (idigi_file_response_t * const) response_data);
            break;

        case idigi_file_system_strerror:
            status = app_process_file_strerror((idigi_file_data_response_t *) response_data);
            break;

        case idigi_file_system_msg_error:
            status = app_process_file_msg_error((idigi_file_error_request_t * const) request_data,
            	                                (idigi_file_response_t * const) response_data);
            break;

        default:
            APP_DEBUG("Unsupported file system request %d\n", request);
    }

    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_open(idigi_file_open_request_t * const request_data,
                                                     idigi_file_open_response_t * const response_data)
{
	idigi_callback_status_t status = idigi_callback_continue;

	SdFile *file = new SdFile;
	uint8_t filemode = 0;

    if (request_data->oflag & IDIGI_O_RDONLY) filemode |= O_RDONLY;
    if (request_data->oflag & IDIGI_O_WRONLY) filemode |= O_WRONLY;
    if (request_data->oflag & IDIGI_O_RDWR) filemode |= O_RDWR;
    if (request_data->oflag & IDIGI_O_APPEND) filemode |= O_APPEND;
    if (request_data->oflag & IDIGI_O_CREAT) filemode |= O_CREAT;
    if (request_data->oflag & IDIGI_O_TRUNC) filemode |= O_TRUNC;

    if ((request_data->oflag & (IDIGI_O_RDONLY | IDIGI_O_WRONLY)) == 0)
    	filemode |= O_RDONLY; 

    *file = DigiSD.open(request_data->path, filemode);

    if (!file->isOpen())
    {
        response_data->error->error_status = idigi_file_path_not_found;
    	return status;
    }

    APP_DEBUG("Open %s, req flag %d, flag %d, returned %p\n", request_data->path, request_data->oflag, filemode, (void *) file);

    response_data->handle = (void *) file;
    response_data->user_context = NULL;

    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_ftruncate(idigi_file_ftruncate_request_t * const request_data,
                                                          idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *file = (SdFile *) request_data->handle;

    uint8_t success = file->truncate((uint32_t) request_data->length);

    if (!success)
    {
        response_data->error->error_status = idigi_file_unspec_error;
    	return status;
    }

    APP_DEBUG("ftruncate %ld succeeded", request_data->length);

    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_rm(idigi_file_path_request_t * const request_data,
                                                   idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;

    bool const result = DigiSD.remove((char *) request_data->path);

    if (!result)
    {
    	response_data->error->error_status = idigi_file_unspec_error;
    }

    APP_DEBUG("unlink %s returned %d\n", request_data->path, (uint8_t) result);

    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_read(idigi_file_request_t * const request_data,
                                                     idigi_file_data_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *file = (SdFile *) request_data->handle;

    int const result = file->read(response_data->data_ptr, response_data->size_in_bytes);

    if (result < 0)
    {
        APP_DEBUG("read %p req %d, returned err %d\n", (void *) file, response_data->size_in_bytes, result);
        response_data->error->error_status = idigi_file_unspec_error;
        goto done;
    }

    APP_DEBUG("read %p req %d, returned %d\n", (void *) file, response_data->size_in_bytes, result);
    response_data->size_in_bytes = result;

done:
    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_write(idigi_file_write_request_t * const request_data,
                                                      idigi_file_write_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *file = (SdFile *) request_data->handle;

  	file->clearWriteError();
    size_t wrote = file->write(request_data->data_ptr, request_data->size_in_bytes);

    if (file->getWriteError())
    {
        response_data->error->error_status = idigi_file_unspec_error;
        APP_DEBUG("write attempt %d, returned err (wrote: %d)\n", request_data->size_in_bytes, wrote);
        goto done;
    }

    APP_DEBUG("write attempt %d, wrote: %d\n", request_data->size_in_bytes, wrote);
    response_data->size_in_bytes = wrote;

done:
    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_lseek(idigi_file_lseek_request_t * const request_data,
                                                      idigi_file_lseek_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *file = (SdFile *) request_data->handle;

    uint8_t result = 0;

    switch(request_data->origin)
    {
    case IDIGI_SEEK_SET:
        result = file->seekSet(request_data->offset);
        break;
    case IDIGI_SEEK_END:
        if (request_data->offset != 0)
        {
        	// not supported by underlying file-system implementation
        	response_data->error->error_status = idigi_file_invalid_parameter;
        	goto done;
        }
        result = file->seekEnd();
        break;
    case IDIGI_SEEK_CUR:
    default:
        result = file->seekCur(request_data->offset);
        break;
    }

    if (result)	// it's a boolean
    	response_data->offset = request_data->offset;
    else
    	response_data->offset = 0;

    APP_DEBUG("lseek offset %ld, origin %d returned %ld\n", 
                request_data->offset, request_data->origin, response_data->offset);
done:
    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_close(idigi_file_request_t * const request_data,
                                                      idigi_file_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *file = (SdFile *) request_data->handle;

    file->close();

    APP_DEBUG("close %p\n", (void *) file);
    free(file);

    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_opendir(idigi_file_path_request_t * const request_data,
                                                        idigi_file_open_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *dir = new SdFile;

    if (dir == NULL)
    {
    	response_data->error->error_status = idigi_file_out_of_memory;
    }

    *dir = DigiSD.open(request_data->path);

    if (!dir->isOpen() || !dir->isDir())
    {
    	response_data->error->error_status = idigi_file_path_not_found;
    	return status;
    }

    dir->rewind();

    response_data->handle = dir;
    APP_DEBUG("opendir for %s returned %p\n", request_data->path, (void *) dir);

    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_closedir(idigi_file_request_t * const request_data,
                                                         idigi_file_response_t * const response_data)
{
    SdFile *dir = (SdFile *) request_data->handle;

    APP_DEBUG("closedir %p\n", (void *) dir);

    if (dir->isOpen())
    	dir->close();
    free(dir);

    return idigi_callback_continue;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_readdir(idigi_file_request_t * const request_data,
                                                        idigi_file_data_response_t * const response_data)
{
    idigi_callback_status_t status = idigi_callback_continue;
    SdFile *dir = (SdFile *) request_data->handle;

    char fileName[DIGISD_MAX_FILENAME_LEN];
    SdFile entry = DigiSD.openNextFile(dir, fileName);

    if (!entry.isOpen())
    {
    	APP_DEBUG("No more directory entries\n");
        response_data->size_in_bytes = 0;
        goto done;
    }

    // read valid entry
    {
        size_t const name_len = strlen(fileName);

        APP_DEBUG("fileName is %s\n", fileName);

        if(name_len < response_data->size_in_bytes)
        {
            memcpy(response_data->data_ptr, fileName, name_len + 1);
            response_data->size_in_bytes = name_len + 1;
        }
        else
        {
            APP_DEBUG("directory entry name too long\n");
            response_data->error->error_status = idigi_file_out_of_memory;
        }
    }

    entry.close();

done:
    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_stat(idigi_file_stat_request_t * const request_data,
                                                     idigi_file_stat_response_t * const response_data)
{
    idigi_file_stat_t *pstat = &response_data->statbuf;
    idigi_callback_status_t status = idigi_callback_continue;

    SdFile entry = DigiSD.open(request_data->path);

    if (!entry.isOpen())
    {
     	APP_DEBUG("stat cannot open: %s\n", request_data->path);
        goto done;   	
    }

    pstat->last_modified = 0;					// Last modified date unsupported:
    pstat->hash_alg = idigi_file_hash_none;	// File hash not supported
    pstat->file_size = (size_t) entry.fileSize();
    pstat->flags = 0;
    if (entry.isDir())
    	pstat->flags |= IDIGI_FILE_IS_DIR;
    else
    	pstat->flags |= IDIGI_FILE_IS_REG;

    entry.close();

done:
    return status;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_strerror(idigi_file_data_response_t * response_data)
{
    size_t strerr_size = 0;

    idigi_file_error_data_t * error_data = response_data->error;
    long int errnum = (long int)error_data->errnum;

    if (errnum != 0)
    {
        char * ptr = (char *) response_data->data_ptr;
        strerr_size = snprintf(ptr, response_data->size_in_bytes, "error #: %ld", errnum);
    }

    response_data->size_in_bytes = strerr_size;

    return idigi_callback_continue;
}

idigi_callback_status_t iDigiFileSystem::app_process_file_msg_error(idigi_file_error_request_t * const request_data,
                                                          idigi_file_response_t * const response_data)
{
    UNUSED_ARGUMENT(request_data);
    UNUSED_ARGUMENT(response_data);
    APP_DEBUG("Message Error %d\n", request_data->message_status);

    // All application resources, used in the session, must be released in this callback
    return idigi_callback_continue;
}

