#include "idigi_data.h"

typedef struct {
	uint32_t                target;
    uint32_t                version;
    uint32_t                code_size;
    uint8_t                 data_service_enabled;
    char                    * name_spec;
    char                    * description;
    idigi_data_request_t    * data_service_request;
    
} firmware_list_t;

extern firmware_list_t* firmware_list;
extern uint8_t firmware_list_count;
