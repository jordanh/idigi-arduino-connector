#include "idigi_app.h"

idigi_callback_status_t idigi_data_service_callback(idigi_data_service_request_t request,
                                                  void const * request_data){
    idigi_callback_status_t status = idigi_callback_continue;

    switch (request)
    {
    case idigi_data_service_send_complete:
    {
        idigi_data_send_t const * send_info = request_data;

        UNUSED_PARAMETER(send_info);
        DEBUG_PRINTF("Handle: %d, status: %d, sent: %d bytes\n", send_info->session_id, send_info->status, send_info->bytes_sent);
        break;
    }

    case idigi_data_service_error:
    {
        idigi_data_error_t const * error_block = request_data;

        UNUSED_PARAMETER(error_block);
        DEBUG_PRINTF("Handle: %d, error: %d\n", error_block->session_id, error_block->error);
        break;
    }

    case idigi_data_service_response:
    {
        idigi_data_response_t const * response = request_data;
        char * data = (char *)response->message.value;

        UNUSED_PARAMETER(response);

        data[response->message.size] = '\0';
        DEBUG_PRINTF("Handle: %d, status: %d, message: %s\n", response->session_id, response->status, data);
        break;
    }

    default:
        break;

    }

    return status;
}
