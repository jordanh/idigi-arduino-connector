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
//#include <string.h>

//#include "idk_def.h"
//#include "ei_msg.h"
//#include "ei_security.h"
//#include "bele.c"


char * no_query_state_response = "<rci_replay version=\"1.1\"> <query_state/> </rci_reply>";
static idk_status_t rci_process_function(idk_data_t * idk_ptr, idk_facility_t * fac_ptr, idk_facility_packet_t * p)
{
    idk_status_t rc = idk_success;
    idk_facility_packet_t   * pkt;
    uint8_t             * buf, * data_ptr;
    uint32_t            length;

    (void)fac_ptr;
    (void)p;

    DEBUG_PRINTF("rci_process_function: fake response\n");

    pkt =(idk_facility_packet_t *) net_get_send_packet(idk_ptr, sizeof(idk_facility_packet_t), &buf);
    if (pkt == NULL)
    {
        goto _ret;
    }
    data_ptr = buf;
    *buf++ = 0x04;
    *buf++ = 0x00;
    length = TO_BE32(strlen(no_query_state_response));
    memcpy(buf, &length, sizeof length);
    buf += sizeof length;

    /* now add this target to the target list message */
    memcpy(buf, no_query_state_response, strlen(no_query_state_response));
    buf += strlen(no_query_state_response);
    pkt->length = buf - data_ptr;

    rc = net_enable_facility_packet(idk_ptr, E_MSG_FAC_RCI_NUM, SECURITY_PROTO_NONE);
_ret:
    return rc;
}
