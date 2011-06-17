typedef struct {
    uint32_t    version;
    uint32_t    code_size;
    char        * name_spec;
    char        * description;
} firmware_list_t;

extern firmware_list_t* firmware_list;
extern uint8_t firmware_list_count;
