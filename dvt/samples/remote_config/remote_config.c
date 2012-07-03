/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#include <malloc.h>
#include "idigi_types.h"
#include "idigi_api.h"
#include "idigi_config.h"
#include "remote_config.h"
#include "idigi_remote.h"
#include "platform.h"
#include <string.h>

int32_t init_signed = 0;
uint32_t init_unsigned = 1;
float init_float = 0.1;
char * init_string = "default\0";
char * init_ipv4 = "0.0.0.0\0";
char * init_fqdnv4 = "test.idigi.com\0";
char * init_datetime = "2012-12-21T00:00:00Z\0";

void * ptr = 0;

#define arrplen(a) sizeof(a)/sizeof(setting_data_t)

typedef struct {
    void ** group_data;
    unsigned int group_id;
    unsigned int index;
    unsigned int capacity;
    unsigned int count;
} setting_data_t;

typedef struct {
    setting_data_t ** setting_data;
    unsigned int capacity;
    unsigned int count;
} rci_data_t;

rci_data_t rci_data[] = { 
    { // setting data
        NULL,
        0,
        0 
    },
    { // state data
        NULL,
        0,
        0
    }
};

typedef struct {
    void * group_context;
} remote_group_session_t;

void * get_setting_data(idigi_remote_group_request_t const * const request, 
    const void * def, int length);
void add_setting_data(idigi_remote_group_request_t const * const request, 
    const void * data, int length);

void * get_setting_data(idigi_remote_group_request_t const * const request, 
    const void * def, int length){

    unsigned int group_id = request->group.id;
    unsigned int index = request->group.index;
    unsigned int element_id = request->element.id;

    rci_data_t * r_data = &rci_data[request->group.type];

    unsigned int i;
    for(i = group_id; i < r_data->count; i++){
        setting_data_t * s_data = r_data->setting_data[i];
        if(s_data != NULL && 
            s_data->group_id == group_id && 
            s_data->index == index &&
            element_id < s_data->count &&
            s_data->group_data[element_id] != NULL){
            return s_data->group_data[element_id];
        }
    }
    
    // Data doesn't exist, add it.
    add_setting_data(request, def, length);
    return get_setting_data(request, def, length);
}

void add_setting_data(idigi_remote_group_request_t const * const request, 
    const void * data, int length){

    unsigned int group_id = request->group.id;
    unsigned int index = request->group.index;
    unsigned int element_id = request->element.id;

    APP_DEBUG("Setting with length %d.\n", length);

    rci_data_t * r_data = &rci_data[request->group.type];

    // Init setting data if not set.
    //APP_DEBUG("Setting Data for %d:%d.\n", group_id, element_id);
    if(r_data->setting_data == NULL){
        r_data->setting_data = (setting_data_t **)malloc(10*sizeof(setting_data_t));
        r_data->capacity = 10;
    }

    // If group id extends size of setting_data grow it.
    if(r_data->count == r_data->capacity){ // resize array
        r_data->capacity = r_data->capacity + 10;
        int new_size = r_data->count * sizeof(setting_data_t);
        r_data->setting_data = (setting_data_t **)realloc(r_data->setting_data, new_size);
    }

    // Init group data if not set.
    setting_data_t * s_data = NULL;
    unsigned int i;
    for(i = group_id; i < r_data->count; i++){
        s_data = r_data->setting_data[i];
        if(s_data != NULL && 
            s_data->group_id == group_id && 
            s_data->index == index){
            // Found setting data;
            break;
        }
        else{
            s_data = NULL;
        }
    }

    // Allocate Setting Data if not found.
    if(s_data == NULL){
        s_data =(setting_data_t *)malloc(sizeof(setting_data_t));
        r_data->setting_data[i] = s_data;
        r_data->count = r_data->count + 1; 
        s_data->group_id = group_id;
        s_data->index = index;
        s_data->group_data = (void **)malloc(10*sizeof(ptr));
        s_data->capacity = 10;
        s_data->count = 0;
    }

    while(s_data->count == s_data->capacity || 
        element_id >= s_data->capacity){ // resize array
        s_data->capacity += 10;
        unsigned int new_size = s_data->capacity * sizeof(ptr);
        s_data->group_data = (void **)realloc(s_data->group_data, new_size);
    }

    void * data_ptr = malloc(length);
    memcpy(data_ptr, data, length);
    s_data->group_data[element_id] = data_ptr;
    s_data->count += 1; 
}

static idigi_callback_status_t app_rci_group_process(
    idigi_remote_group_request_t const * const request,
    idigi_remote_group_response_t * const response);

static idigi_callback_status_t app_rci_group_get(
    idigi_remote_group_request_t const * const request,
    idigi_remote_group_response_t * const response);

static idigi_callback_status_t app_rci_group_set(
    idigi_remote_group_request_t const * const request,
    idigi_remote_group_response_t * const response);



static idigi_callback_status_t app_rci_group_process(
    idigi_remote_group_request_t const * const request,
    idigi_remote_group_response_t * const response){

    UNUSED_ARGUMENT(response);
    UNUSED_ARGUMENT(request);

    if(request->action == idigi_remote_action_query){
        return app_rci_group_get(request, response);
    }
    else{
        return app_rci_group_set(request, response);
    }
}

static idigi_callback_status_t app_rci_group_get(
    idigi_remote_group_request_t const * const request,
    idigi_remote_group_response_t * const response){
    
    void * data;

    switch(request->element.type){
        case idigi_element_type_string:
        case idigi_element_type_multiline_string:
        case idigi_element_type_password:
            data = get_setting_data(request, init_string, 
                sizeof(char)*strlen(init_string));
            response->element_data.element_value->string_value = data;
            break;
        case idigi_element_type_int32:
            data = get_setting_data(request, &init_signed, sizeof(int32_t));
            response->element_data.element_value->signed_integer_value = *(int32_t *)data;
            break;
        case idigi_element_type_uint32:
        case idigi_element_type_hex32:
        case idigi_element_type_0xhex:
            data = get_setting_data(request, &init_unsigned, 
                sizeof(uint32_t));
            response->element_data.element_value->unsigned_integer_value = *(uint32_t *)data; 
            break;
        case idigi_element_type_float:
            data = get_setting_data(request, &init_float, sizeof(float));
            response->element_data.element_value->float_value = init_float;
            break;
        case idigi_element_type_enum:
        case idigi_element_type_on_off:
        case idigi_element_type_boolean:
            data = get_setting_data(request, &init_unsigned, sizeof(uint32_t));
            response->element_data.element_value->unsigned_integer_value = *(uint32_t *)data;
            break;
        case idigi_element_type_ipv4:
            data = get_setting_data(request, init_ipv4, 
                sizeof(char)*(strlen(init_ipv4)+1));
            response->element_data.element_value->string_value = data;
            break;
        case idigi_element_type_fqdnv4:
        case idigi_element_type_fqdnv6:
            data =  get_setting_data(request, init_fqdnv4, 
                sizeof(char)*(strlen(init_fqdnv4)+1));
            response->element_data.element_value->string_value = data;
            break;
        case idigi_element_type_datetime:
            data = get_setting_data(request, init_datetime, 
                sizeof(char)*(strlen(init_datetime)+1));
            response->element_data.element_value->string_value = data;
            break;
        default:
            APP_DEBUG("Unknown Type.\n");
    }

    return idigi_callback_continue;
}

static idigi_callback_status_t app_rci_group_set(
    idigi_remote_group_request_t const * const request,
    idigi_remote_group_response_t * const response){

    UNUSED_ARGUMENT(response);


    switch(request->element.type){
        case idigi_element_type_string:
        case idigi_element_type_multiline_string:
        case idigi_element_type_password:
        case idigi_element_type_ipv4:
        case idigi_element_type_fqdnv4:
        case idigi_element_type_fqdnv6:
        case idigi_element_type_datetime:
            add_setting_data(request, request->element.value->string_value, 
                sizeof(char)*(strlen(request->element.value->string_value)+1));
            break;
        case idigi_element_type_int32:
            add_setting_data(request,
                &request->element.value->signed_integer_value, 
                sizeof(request->element.value->signed_integer_value));
        case idigi_element_type_uint32:
        case idigi_element_type_hex32:
        case idigi_element_type_0xhex:
            add_setting_data(request,
                &request->element.value->unsigned_integer_value, 
                sizeof(request->element.value->unsigned_integer_value));
            break;
        case idigi_element_type_float:
            add_setting_data(request, 
                &request->element.value->float_value, 
                sizeof(request->element.value->float_value));
            break;
        case idigi_element_type_enum:
        case idigi_element_type_on_off:
        case idigi_element_type_boolean:
            add_setting_data(request,
                &request->element.value->boolean_value, 
                sizeof(request->element.value->boolean_value));
            break;
    }
    

    return idigi_callback_continue;
}

idigi_callback_status_t app_remote_config_handler(
    idigi_remote_config_request_t const request,
    void const * const request_data, size_t const request_length,
    void * response_data, size_t * const response_length){

    idigi_callback_status_t status = idigi_callback_continue;

    UNUSED_ARGUMENT(request_length);
    UNUSED_ARGUMENT(response_length);

    switch(request){
        case idigi_remote_config_session_start:
        case idigi_remote_config_action_start:
        case idigi_remote_config_group_start:
        case idigi_remote_config_group_end:
        case idigi_remote_config_action_end:
        case idigi_remote_config_session_end:
        case idigi_remote_config_session_cancel:
            // do nothing here.
            break;
        case idigi_remote_config_group_process:
            status = app_rci_group_process(request_data, response_data);
            break;
        default:
            APP_DEBUG("Request Unknown: %d.\n", request);
            ASSERT(0);
            break;
    }

    return status;
}