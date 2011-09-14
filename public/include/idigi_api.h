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
#ifndef _IDIGI_API_H
#define _IDIGI_API_H

#include "idigi_types.h"

#define IDIGI_PORT       3197
#define IDIGI_SSL_PORT   3199

typedef enum {
   idigi_success,
   idigi_init_error,
   idigi_configuration_error,
   idigi_invalid_data_size,
   idigi_invalid_data_range,
   idigi_invalid_payload_packet,
   idigi_keepalive_error,
   idigi_server_overload,
   idigi_bad_version,
   idigi_invalid_packet,
   idigi_exceed_timeout,
   idigi_unsupported_security,
   idigi_invalid_data,
   idigi_server_disconnected,
   idigi_connect_error,
   idigi_receive_error,
   idigi_send_error,
   idigi_close_error,
   idigi_device_terminated,
   idigi_service_busy,
   idigi_invalid_response,
} idigi_status_t;

typedef enum {
    idigi_class_config,
    idigi_class_network,
    idigi_class_operating_system,
    idigi_class_firmware,
    idigi_class_data_service
} idigi_class_t;

typedef enum {
    idigi_config_device_id,
    idigi_config_vendor_id,
    idigi_config_device_type,
    idigi_config_server_url,
    idigi_config_connection_type,
    idigi_config_mac_addr,
    idigi_config_link_speed,
    idigi_config_phone_number,
    idigi_config_tx_keepalive,
    idigi_config_rx_keepalive,
    idigi_config_wait_count,
    idigi_config_ip_addr,
    idigi_config_error_status,
    idigi_config_firmware_facility,
    idigi_config_data_service,

} idigi_config_request_t;

typedef enum {
    idigi_network_connect,
    idigi_network_send,
    idigi_network_receive,
    idigi_network_close,
    idigi_network_disconnected,
    idigi_network_reboot

} idigi_network_request_t;

typedef enum {
    idigi_os_malloc,
    idigi_os_free,
    idigi_os_system_up_time,
} idigi_os_request_t;

typedef enum {
    idigi_firmware_target_count,
    idigi_firmware_version,
    idigi_firmware_code_size,
    idigi_firmware_description,
    idigi_firmware_name_spec,
    idigi_firmware_download_request,
    idigi_firmware_binary_block,
    idigi_firmware_download_complete,
    idigi_firmware_download_abort,
    idigi_firmware_target_reset,
} idigi_firmware_request_t;

typedef enum {
    idigi_data_service_send_complete,
    idigi_data_service_response,
    idigi_data_service_device_request,
    idigi_data_service_device_response,
    idigi_data_service_error    
} idigi_data_service_request_t;

typedef enum {
    idigi_initiate_terminate,
    idigi_initiate_data_service
} idigi_initiate_request_t;

typedef enum {
   idigi_lan_connection_type,
   idigi_wan_connection_type
} idigi_connection_type_t;

typedef enum  {
    idigi_callback_continue,
    idigi_callback_busy,
    idigi_callback_abort,
    idigi_callback_unrecognized
} idigi_callback_status_t;

typedef enum {
   idigi_fw_success,
   idigi_fw_download_denied,
   idigi_fw_download_invalid_size,
   idigi_fw_download_invalid_version,
   idigi_fw_download_unauthenticated,
   idigi_fw_download_not_allowed,
   idigi_fw_download_configured_to_reject,
   idigi_fw_encountered_error,
   idigi_fw_user_abort,
   idigi_fw_device_error,
   idigi_fw_invalid_offset,
   idigi_fw_invalid_data,
   idigi_fw_hardware_error
} idigi_fw_status_t;

typedef enum {
   idigi_fw_download_success,
   idigi_fw_download_checksum_mismatch,
   idigi_fw_download_not_complete
} idigi_fw_download_complete_status_t;

typedef union {
   idigi_config_request_t config_request;
   idigi_network_request_t network_request;
   idigi_os_request_t os_request;
   idigi_firmware_request_t firmware_request;
   idigi_data_service_request_t data_service_request;
} idigi_request_t;

#define idigi_handle_t void *

typedef struct  {
    idigi_class_t class_id;
    idigi_request_t request_id;
    idigi_status_t status;
} idigi_error_status_t;

typedef struct  {
   idigi_network_handle_t *  network_handle;
    uint8_t const * buffer;
    size_t length;
    unsigned timeout;
} idigi_write_request_t;

typedef struct  {
   idigi_network_handle_t * network_handle;
    uint8_t * buffer;
    size_t length;
    unsigned timeout;
} idigi_read_request_t;

typedef struct {
   unsigned timeout;
   uint8_t target;
} idigi_fw_config_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t version;
    uint32_t code_size;
    char * desc_string;
    char * file_name_spec;
    char * filename;
} idigi_fw_download_request_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t offset;
    uint8_t * data;
    size_t length;
} idigi_fw_image_data_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    uint32_t code_size;
    uint32_t checksum;
} idigi_fw_download_complete_request_t;

typedef struct {
    uint32_t version;
    uint32_t calculated_checksum;
    idigi_fw_download_complete_status_t status;
} idigi_fw_download_complete_response_t;

typedef struct {
    unsigned timeout;
    uint8_t target;
    idigi_fw_status_t status;
} idigi_fw_download_abort_t;

#define IDIGI_DATA_REQUEST_START        0x0001
#define IDIGI_DATA_REQUEST_LAST         0x0002
#define IDIGI_DATA_REQUEST_ARCHIVE      0x0004
#define IDIGI_DATA_REQUEST_COMPRESSED   0x0008
#define IDIGI_DATA_REQUEST_APPEND       0x0010

typedef struct
{
    uint8_t const * value;
    uint8_t size;
} idigi_data_block_t;

typedef struct
{
    uint8_t * data;
    size_t size;
} idigi_data_payload_t;

typedef struct
{
    void * session;
    uint16_t flag;
    idigi_data_block_t path;
    idigi_data_block_t content_type;
    idigi_data_payload_t payload;
} idigi_data_request_t;

typedef struct
{
    uint16_t session_id;
    uint8_t  status;
    idigi_data_block_t message;
} idigi_data_response_t;

typedef struct
{
    uint16_t session_id;
    idigi_status_t status;
    size_t bytes_sent;
} idigi_data_send_t;

typedef struct
{
    uint16_t session_id;
} idigi_data_session_t;

#define IDIGI_DATA_GET_SESSION_ID(session)    ((idigi_data_session_t *)session)->session_id

typedef enum 
{
    idigi_msg_error_none,
    idigi_msg_error_fatal,
    idigi_msg_error_invalid_opcode,
    idigi_msg_error_format,
    idigi_msg_error_session_in_use,
    idigi_msg_error_unknown_session,
    idigi_msg_error_compression_failure,
    idigi_msg_error_decompression_failure,
    idigi_msg_error_memory,
    idigi_msg_error_send,
    idigi_msg_error_cancel,
    idigi_msg_error_busy,
    idigi_msg_error_ack,
    idigi_msg_error_timeout,
    idigi_msg_error_no_service,
    idigi_msg_error_count
} idigi_msg_error_t;

typedef struct
{
    uint16_t session_id;
    idigi_msg_error_t error;
} idigi_data_error_t;

typedef enum {
    idigi_data_service_success,
    idigi_data_service_not_handled
} idigi_ds_status_t;

typedef struct {
    uint16_t session_id;
    char const * target;
    uint8_t const * data;
    size_t data_length;
    uint16_t flag;
    void * user_context;
} idigi_ds_device_request_t;

typedef struct
{
    uint16_t session_id;
    idigi_ds_status_t  status;
    uint8_t * data;
    size_t data_length;
    uint16_t flag;
} idigi_ds_device_response_t;

typedef idigi_callback_status_t (* idigi_callback_t) (idigi_class_t const class_id, idigi_request_t const request_id,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

idigi_handle_t idigi_init(idigi_callback_t const callback);

idigi_status_t idigi_step(idigi_handle_t const handle);

idigi_status_t idigi_run(idigi_handle_t const handle);

idigi_status_t idigi_initiate_action(idigi_handle_t const handle, idigi_initiate_request_t const request,
                                     void const * const request_data, void * const response_data);

#endif /* _IDIGI_API_H */
