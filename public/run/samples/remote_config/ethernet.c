/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
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
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <net/if_arp.h>

#include "idigi_api.h"
#include "platform.h"
#include "idigi_remote.h"
#include "remote_config.h"

extern int app_os_malloc(size_t const size, void ** ptr);
extern void app_os_free(void * const ptr);

#define MAX_INTERFACES      5

#define ETHERNET_NO_MEMORY_HINT            "Memory"

#define ETHERNET_IPV4_ADDR_SIZE 4
#define ETHERNET_IPV4_STRING_LENGTH 16
#define ETHERNET_DNS_FQDN_LENGTH    128

enum {
    ethernet_dhcp_disabled,
    ethernet_dhcp_enabled
};

enum {
    ethernet_duplex_auto,
    ethernet_duplex_half,
    ethernet_duplex_full,
};
typedef struct {
    unsigned char ip_address[ETHERNET_IPV4_ADDR_SIZE];
    unsigned char subnet[ETHERNET_IPV4_ADDR_SIZE];
    unsigned char gateway[ETHERNET_IPV4_ADDR_SIZE];
    unsigned int duplex;
    char dns[ETHERNET_DNS_FQDN_LENGTH];

    unsigned char dhcp_enabled;
} ethernet_config_data_t;

typedef struct {
    char ip_address[ETHERNET_IPV4_STRING_LENGTH];
    char subnet[ETHERNET_IPV4_STRING_LENGTH];
    char gateway[ETHERNET_IPV4_STRING_LENGTH];
    char dns[ETHERNET_DNS_FQDN_LENGTH];
    idigi_group_ethernet_duplex_id_t  duplex;
    idigi_boolean_t dhcp_enabled;
} ethernet_idigi_data_t;

ethernet_config_data_t ethernet_config_data = {{0}, {0}, {0}, ethernet_duplex_auto, "\0", ethernet_dhcp_enabled};


int ethernet_configuration_init(void)
{
    int             fd = -1;
    int            status=-1;
    char            *buf = malloc(MAX_INTERFACES*sizeof(struct ifreq));
    struct ifconf   conf;
    unsigned int    entries = 0;
    unsigned int    i;
    static struct in_addr  ip_addr; /* Used to store the IP address */

    ethernet_config_data_t * const ethernet_ptr = &ethernet_config_data;

    if (buf == NULL)
    {
        APP_DEBUG("get_ip_address malloc failed\n");
        goto error;
    }
    conf.ifc_len = MAX_INTERFACES*sizeof(struct ifreq);
    conf.ifc_buf = buf;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("socket");
        goto error;
    }

    if( ioctl(fd, SIOCGIFCONF , &conf) == -1)
    {
        APP_DEBUG("get_ip_address: Error using ioctl SIOCGIFCONF.\n");
        goto error;
    }

    entries = conf.ifc_len / sizeof(struct ifreq);

    APP_DEBUG("get_ip_address: Looking for current device IP address: found [%d] entries\n", entries);

    if (entries == 0)
    {
        goto error;
    }
    for( i = 0; i < entries; i++)
    {
        struct ifreq * req = &conf.ifc_req[i];
        struct sockaddr_in * sa = (struct sockaddr_in *) &req->ifr_addr;

        APP_DEBUG("get_ip_address: %d: Interface name [%s]\tIP Address [%s]\n", i+1, req->ifr_name, inet_ntoa(sa->sin_addr));
        if (sa->sin_addr.s_addr != htonl(INADDR_LOOPBACK))
        {
            ip_addr = sa->sin_addr;
            break;
        }
    }

    {
        in_addr_t * addr = (in_addr_t *)ethernet_ptr->ip_address;
        *addr = ip_addr.s_addr;
    }
    status = 0;

    {
        struct ifreq * req = &conf.ifc_req[i];
        struct sockaddr_in * sa = (struct sockaddr_in *) &req->ifr_addr;

        if( ioctl(fd, SIOCGIFNETMASK , req) == -1)
        {
            perror("get_ip_address: Error using ioctl SIOCGIFNETMASK");
            status = -1;
            goto error;
        }

        APP_DEBUG("get_ip_address: IP Address [%s]\n", inet_ntoa(sa->sin_addr));
        {
            in_addr_t * addr = (in_addr_t *)ethernet_ptr->subnet;
            *addr = sa->sin_addr.s_addr;
        }

    }

error:
    if (fd != -1)
    {
        close(fd);
    }
    if (buf != NULL)
    {
        free(buf);
    }
    return status;
}

void ethernet_get_ip_address(unsigned char ** addr, size_t * size)
{
    *addr = &ethernet_config_data.ip_address[0];
    *size = sizeof ethernet_config_data.ip_address;
}

idigi_callback_status_t app_ethernet_group_init(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    void * ptr;
    ethernet_idigi_data_t * ethernet_ptr = NULL;
    struct in_addr * addr;

    UNUSED_ARGUMENT(request);

    if (app_os_malloc(sizeof *ethernet_ptr, &ptr) != 0)
    {
        response->error_id = idigi_group_error_load_failed;
        response->element_data.error_hint = ETHERNET_NO_MEMORY_HINT;
        goto done;
    }

    ethernet_ptr = ptr;
    addr = (struct in_addr *)ethernet_config_data.ip_address;
    sprintf(ethernet_ptr->ip_address, "%s", inet_ntoa(*addr));

    addr = (struct in_addr *)ethernet_config_data.subnet;
    sprintf(ethernet_ptr->subnet, "%s", inet_ntoa(*addr));

    addr = (struct in_addr *)ethernet_config_data.gateway;
    sprintf(ethernet_ptr->gateway, "%s", inet_ntoa(*addr));

    strcpy(ethernet_ptr->dns, ethernet_config_data.dns);
    ethernet_ptr->dhcp_enabled = (ethernet_config_data.dhcp_enabled == ethernet_dhcp_enabled) ? idigi_boolean_true : idigi_boolean_false;

    switch (ethernet_config_data.duplex)
    {
    case ethernet_duplex_auto:
        ethernet_ptr->duplex = idigi_group_ethernet_duplex_auto;
        break;
    case ethernet_duplex_half:
        ethernet_ptr->duplex = idigi_group_ethernet_duplex_half;
        break;
    case ethernet_duplex_full:
        ethernet_ptr->duplex = idigi_group_ethernet_duplex_full;
        break;
    }

done:
    response->user_context = ethernet_ptr;
    return idigi_callback_continue;
}

idigi_callback_status_t app_ethernet_group_get(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    ethernet_idigi_data_t * ethernet_ptr;

    ASSERT(reqeust->user_context != NULL);

    ethernet_ptr = response->user_context;

    switch (request->element_id)
    {
    case idigi_group_ethernet_dhcp:
        ASSERT(request->element_type == idigi_element_type_boolean);
        response->element_data.element_value->boolean_value = ethernet_ptr->dhcp_enabled;
        break;
    case idigi_group_ethernet_dns:
        ASSERT(request->element_type == idigi_element_type_string);
        response->element_data.element_value->string_value.buffer = ethernet_ptr->dns;
        response->element_data.element_value->string_value.length_in_bytes = strlen(ethernet_ptr->dns);
        break;
    case idigi_group_ethernet_duplex:
        ASSERT(request->element_type == idigi_element_type_enum);
        response->element_data.element_value->enum_value = ethernet_ptr->duplex;
        break;
    case idigi_group_ethernet_ip:
    case idigi_group_ethernet_subnet:
    case idigi_group_ethernet_gateway:
    {
        char * config_data[] = {ethernet_ptr->ip_address, ethernet_ptr->subnet, ethernet_ptr->gateway};

        ASSERT(request->element_type == idigi_element_type_ipv4);
        response->element_data.element_value->string_value.buffer = config_data[request->element_id];
        response->element_data.element_value->string_value.length_in_bytes = sizeof ethernet_ptr->ip_address;
        break;
    }
    default:
        ASSERT(0);
        response->error_id = idigi_group_error_unknown_value;
        response->element_data.error_hint = NULL;
        break;
    }

    return status;
}

idigi_callback_status_t app_ethernet_group_set(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{
    idigi_callback_status_t status = idigi_callback_continue;

    ethernet_idigi_data_t * ethernet_ptr;

    ASSERT(response->user_context != NULL);
    ASSERT(request->element_value != NULL);

    ethernet_ptr = response->user_context;
    switch (request->element_id)
    {
    case idigi_group_ethernet_dhcp:
        ASSERT(request->element_type == idigi_element_type_boolean);
        ethernet_ptr->dhcp_enabled = request->element_value->boolean_value;
        break;
    case idigi_group_ethernet_dns:
        ASSERT(request->element_type == idigi_element_type_string);
        ASSERT(reqeust->element_value->length_in_bytes <= sizeof ethernet_ptr->dns);
        memcpy(ethernet_ptr->dns, request->element_value->string_value.buffer, request->element_value->string_value.length_in_bytes);
        ethernet_ptr->dns[request->element_value->string_value.length_in_bytes] = '\0';
        break;
    case idigi_group_ethernet_duplex:
        ASSERT(request->element_type == idigi_element_type_enum);
        ethernet_ptr->duplex = request->element_value->enum_value;
        break;
    case idigi_group_ethernet_ip:
    case idigi_group_ethernet_subnet:
    case idigi_group_ethernet_gateway:
    {
        struct {
            char * data;
            size_t max_length;
        } config_data[] = {
                {ethernet_ptr->ip_address, sizeof ethernet_ptr->ip_address},
                {ethernet_ptr->subnet, sizeof ethernet_ptr->subnet},
                {ethernet_ptr->gateway, sizeof ethernet_ptr->gateway}
        };

        ASSERT(request->element_type == idigi_element_type_ipv4);
        ASSERT(request->element_value->string_value.length_in_bytes <= sizeof config_data[request->element_id].max_length);
        memcpy(config_data[request->element_id].data, request->element_value->string_value.buffer, request->element_value->string_value.length_in_bytes);
        config_data[request->element_id].data[request->element_value->string_value.length_in_bytes] = '\0';
        break;
    }
    default:
        ASSERT(0);
        response->error_id = idigi_group_error_unknown_value;
        response->element_data.error_hint = NULL;
        break;
    }

    return status;
}

idigi_callback_status_t app_ethernet_group_end(idigi_remote_group_request_t * request, idigi_remote_group_response_t * response)
{

    ethernet_idigi_data_t * ethernet_ptr;

    /* save the data */

    ASSERT(response->user_context != NULL);

    ethernet_ptr = response->user_context;

    if (request->action == idigi_remote_action_set)
    {
        if (inet_aton(ethernet_ptr->ip_address, (struct in_addr *)ethernet_config_data.ip_address) == 0)
        {
            response->error_id = idigi_group_error_save_failed;
            response->element_data.error_hint = "IP address";
            goto done;
        }

        if (inet_aton(ethernet_ptr->subnet, (struct in_addr *)ethernet_config_data.subnet) == 0)
        {
            response->error_id = idigi_group_error_save_failed;
            response->element_data.error_hint = "Subnet";
            goto done;
        }

        if (inet_aton(ethernet_ptr->gateway, (struct in_addr *)ethernet_config_data.gateway) == 0)
        {
            response->error_id = idigi_group_error_save_failed;
            response->element_data.error_hint = "Gateway";
            goto done;
        }

        /* TODO: need to check valid DNS? */
        strcpy(ethernet_config_data.dns, ethernet_ptr->dns);
        ethernet_config_data.dhcp_enabled = (request->element_value->boolean_value == idigi_boolean_true) ? ethernet_dhcp_enabled : ethernet_dhcp_disabled;

        switch (ethernet_ptr->duplex)
        {
        case idigi_group_ethernet_duplex_auto:
            ethernet_config_data.duplex = ethernet_duplex_auto;
            break;
        case idigi_group_ethernet_duplex_half:
            ethernet_config_data.duplex = ethernet_duplex_half;
            break;
        case idigi_group_ethernet_duplex_full:
            ethernet_config_data.duplex = ethernet_duplex_full;
            break;
        default:
            break;
        }

    }

done:
    if (ethernet_ptr != NULL)
    {
        app_os_free(ethernet_ptr);
    }
    return idigi_callback_continue;
}

void app_ethernet_group_cancel(void * context)
{
    if (context != NULL)
    {
        app_os_free(context);
    }

}

