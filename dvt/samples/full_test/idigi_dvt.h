#ifndef IDIGI_DVT_H
#define IDIGI_DVT_H

typedef enum
{
    dvt_case_fw_download_denied,
    dvt_case_fw_invalid_size,
    dvt_case_fw_invalid_version,
    dvt_case_fw_unauthenticated,
    dvt_case_fw_not_allowed,
    dvt_case_fw_configured_to_reject,
    dvt_case_fw_encountered_error,
    dvt_case_fw_user_abort,
    dvt_case_fw_device_error,
    dvt_case_fw_invalid_offset,
    dvt_case_fw_invalid_data,
    dvt_case_fw_hardware_error,
    dvt_case_last
} dvt_cases_t;

typedef enum
{
    dvt_state_init,
    dvt_state_fw_download_progress,
    dvt_state_fw_download_complete,
    dvt_state_reset_called,
    dvt_state_request_start,
    dvt_state_request_progress,
    dvt_state_request_complete,
    dvt_state_response_complete,
    dvt_state_request_error
} dvt_state_t;

typedef enum
{
    dvt_case_put_request_ds_default,
    dvt_case_put_request_ds_append,
    dvt_case_put_request_ds_archive,
    dvt_case_put_request_ds_both,
    dvt_case_put_request_ds_zero,
    dvt_case_put_request_ds_busy,
    dvt_case_put_request_ds_cancel_start,
    dvt_case_put_request_ds_cancel_middle,
    dvt_case_put_request_ds_timeout
} dvt_ds_test_cases_t;

#define MAX_FILE_PATH_SIZE  32

typedef struct
{
    dvt_ds_test_cases_t test_case;
    dvt_state_t state;
    char file_path[MAX_FILE_PATH_SIZE];
    idigi_data_service_put_request_t header;
    char * file_buf;
    size_t file_size;
    size_t bytes_sent;
} dvt_ds_t;

typedef struct
{
    uint32_t target;
    uint32_t version;
    dvt_state_t state;
    char * description;
    char * name_spec;
    char * file_name;
    size_t file_size;
} dvt_data_t;

extern dvt_ds_t  data_service_info;

#endif

