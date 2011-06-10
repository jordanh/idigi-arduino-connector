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
#define RCI_COMMAND_REPLY           0x04
#define RCI_NO_REPLY_REQUESTED_FLAG 0x00

static char const * no_query_state_response = "<rci_replay version=\"1.1\"> <query_state/> </rci_reply>";
static idigi_status_t rci_process_function(idigi_data_t * idigi_ptr, idigi_facility_t * fac_ptr, idigi_packet_t * packet)
{
    idigi_status_t rc = idigi_success;
    idigi_packet_t   * send_packet;
    uint8_t             * buf, * data_ptr;
    uint32_t            length;

    UNUSED_PARAMETER(fac_ptr);
    UNUSED_PARAMETER(packet);

    DEBUG_PRINTF("rci_process_function: fake response\n");

    send_packet = get_packet_buffer(idigi_ptr, E_MSG_FAC_RCI_NUM, sizeof(idigi_packet_t), &buf);
    if (send_packet == NULL)
    {
        goto done;
    }
    data_ptr = buf;
    *buf++ = RCI_COMMAND_REPLY;
    *buf++ = RCI_NO_REPLY_REQUESTED_FLAG;
    length = strlen(no_query_state_response);
    StoreBE32(buf, length);
    buf += sizeof length;

    /* now add this target to the target list message */
    memcpy(buf, no_query_state_response, strlen(no_query_state_response));
    buf += strlen(no_query_state_response);
    send_packet->header.length = buf - data_ptr;

    rc = enable_facility_packet(idigi_ptr, send_packet, E_MSG_FAC_RCI_NUM, release_packet_buffer);
done:
    return rc;
}
