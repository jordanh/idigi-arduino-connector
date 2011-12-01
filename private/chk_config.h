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

#ifndef CHK_CONFIG_H_
#define CHK_CONFIG_H_

#if defined(IDIGI_VENDOR_ID)
#if (IDIGI_VENDOR_ID > 0xFFFFFFFF)
    #error "Invalid IDIGI_VENDOR_ID 4byte vendor id in idigi_config.h"
#endif
#endif

#if defined(IDIGI_TX_KEEPALIVE_IN_SECONDS)
#if (IDIGI_TX_KEEPALIVE_IN_SECONDS < MIN_TX_KEEPALIVE_INTERVAL_IN_SECONDS) || (IDIGI_TX_KEEPALIVE_IN_SECONDS > MAX_TX_KEEPALIVE_INTERVAL_IN_SECONDS)
    #error "Invalid IDIGI_TX_KEEPALIVE_IN_SECONDS value in idigi_config.h."
#endif
#endif

#if defined(IDIGI_RX_KEEPALIVE_IN_SECONDS)
#if (IDIGI_RX_KEEPALIVE_IN_SECONDS < MIN_RX_KEEPALIVE_INTERVAL_IN_SECONDS) || (IDIGI_RX_KEEPALIVE_IN_SECONDS > MAX_RX_KEEPALIVE_INTERVAL_IN_SECONDS)
    #error "Invalid IDIGI_RX_KEEPALIVE_IN_SECONDS value in idigi_config.h."
#endif
#endif

#if defined(IDIGI_WAIT_COUNT)
#if (IDIGI_WAIT_COUNT < WAIT_COUNT_MIN) || (IDIGI_WAIT_COUNT > WAIT_COUNT_MAX)
    #error "Invalid IDIGI_RX_KEEPALIVE_IN_SECONDS value in idigi_config.h."
#endif
#endif

#if defined(IDIGI_MSG_MAX_TRANSACTION)
#if (IDIGI_MSG_MAX_TRANSACTION < 0 || IDIGI_MSG_MAX_TRANSACTION > IDIGI_MAX_TRANSACTIONS_LIMIT)
    #error "Invalid IDIGI_MSG_MAX_TRANSACTION value in idigi_config.h"
#endif
#endif

#endif /* CHK_CONFIG_H_ */
