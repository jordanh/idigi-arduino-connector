#include "idigi_api.h"

#define DEVICE_ID_LENGTH    16
#define VENDOR_ID_LENGTH    4
#define MAC_ADDR_LENGTH     6

#define DEBUG_PRINTF(...)   DBPRINTF(__VA_ARGS__)

typedef int bool;
#define true 1
#define false 0

typedef struct __attribute__((__packed__))
{
    uint32_t                start_time;
    uint8_t                 vendor_id[VENDOR_ID_LENGTH];
    uint16_t                tx_keepalive;
    uint16_t                rx_keepalive;
    uint16_t                wait_count;
    uint8_t                 phone_number;
    uint32_t                link_speed;
    char *                  device_type;
    char *                  server_url;
    idigi_connection_type_t connection_type;
    idigi_network_handle_t  socket_fd;
    int                     connected;
} IDIGI_CONFIG;

extern IDIGI_CONFIG idigi_config;

void sleep(unsigned int);

idigi_callback_status_t idigi_config_callback(idigi_config_request_t const request_id,
                                            const void * const request_data,
                                            void * response_data,
                                            size_t * const response_length);

idigi_callback_status_t idigi_os_callback(idigi_os_request_t const request_id,
                                            const void * const request_data,
                                            void * response_data);

idigi_callback_status_t idigi_network_callback(idigi_network_request_t request,
                                            const void * const request_data,
                                            void * response_data,
                                            size_t * response_length);