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
    idk_facility_packet_t * p;
    uint8_t * src, * dst;

    /* get the current message and copy it into send packet */
    if (fac_ptr->packet == NULL)
    {
        DEBUG_PRINTF("loopback_process: No Packet\n");
        goto _ret;
    }
    src = IDK_PACKET_DATA_POINTER(fac_ptr->packet, sizeof(idk_facility_packet_t));

    p =(idk_facility_packet_t *) net_get_send_packet(idk_ptr, sizeof(idk_facility_packet_t), &dst);
    if (p == NULL)
    {
        status = idk_callback_busy;
        goto _ret;
    }

    *p = *fac_ptr->packet;
    memcpy(dst, src, fac_ptr->packet->length);

    status = net_enable_facility_packet(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM, SECURITY_PROTO_NONE);

    if (status != idk_callback_busy)
    {
        fac_ptr->packet = NULL;
    }
_ret:
    return status;
}

static idk_status_t loopback_delete_facility(idk_data_t * idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_facility_t * fac_ptr;

    fac_ptr = (idk_facility_t *)get_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM);
    if (fac_ptr != NULL)
    {
        status = del_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM);
    }
    return status;
}

static idk_callback_status_t loopback_init_facility(idk_data_t *idk_ptr)
{
    idk_callback_status_t status = idk_callback_continue;
    idk_facility_t * fac_ptr;


    fac_ptr = (idk_facility_t *)get_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM);
    if (fac_ptr == NULL)
    {
        status = add_facility_data(idk_ptr, E_MSG_FAC_DEV_LOOP_NUM, (idk_facility_t **) &fac_ptr,
                                                        sizeof(idk_facility_t), NULL, loopback_process);

        if (status != idk_callback_continue || fac_ptr == NULL)
        {
            goto _ret;
        }
   }
_ret:
    return status;
}

