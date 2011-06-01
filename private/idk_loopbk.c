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
static idk_callback_status_t loopback_process(idk_data_t * idk_ptr, idk_facility_t * fac_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_facility_packet_t * out_packet;
    idk_facility_packet_t * in_packet;
    uint8_t * src, * dst;

    /* get the current message and copy it into send packet */
    in_packet = GET_FACILITY_PACKET(fac_ptr);
    if (in_packet == NULL)
    {
        DEBUG_PRINTF("loopback_process: No Packet\n");
        goto done;
    }
    src = GET_PACKET_DATA_POINTER(in_packet, sizeof(idk_facility_packet_t));

    out_packet =(idk_facility_packet_t *) get_packet_buffer(idk_ptr, sizeof(idk_facility_packet_t), &dst);
    if (out_packet == NULL)
    {
        status = idk_callback_busy;
        goto done;
    }

    *out_packet = *in_packet;
    memcpy(dst, src, in_packet->length);

    status = enable_facility_packet(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM, SECURITY_PROTO_NONE);

    if (status != idk_callback_busy)
    {
        DONE_FACILITY_PACKET(fac_ptr);
    }
done:
    return status;
}

static idk_status_t loopback_delete_facility(idk_data_t * idk_ptr)
{
    return del_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM);
}

static idk_callback_status_t loopback_init_facility(idk_data_t *idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_facility_t * fac_ptr;


    fac_ptr = get_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM);
    if (fac_ptr == NULL)
    {
        void * ptr;
        status = add_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM, &ptr,
                                   0, NULL, loopback_process);

        if (status != idk_callback_continue || ptr == NULL)
        {
            goto done;
        }
   }
done:
    return status;
}

