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
    dvt_case_put_request_no_flag,
    dvt_case_put_request_defaults,
    dvt_case_put_request_append,
    dvt_case_put_request_archive,
    dvt_case_put_request_both,
    dvt_case_put_request_busy,
    dvt_case_put_request_cancel_at_start,
    dvt_case_put_request_cancel_in_middle,
    dvt_case_put_request_timeout,
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
    dvt_state_stop
} dvt_state_t;

typedef struct
{
    idigi_data_service_put_request_t header;
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
    uint8_t * file_content;
    dvt_ds_t * ds_info;
} dvt_data_t;

extern dvt_data_t * dvt_current_ptr;
extern void cleanup_dvt_data(void);

#endif

