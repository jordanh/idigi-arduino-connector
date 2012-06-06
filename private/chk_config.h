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

#ifndef CHK_CONFIG_H_
#define CHK_CONFIG_H_

#if defined(IDIGI_VENDOR_ID)
#if (IDIGI_VENDOR_ID > 0xFE000000) || (IDIGI_VENDOR_ID == 0)
    #error "IDIGI_VENDOR_ID must be greater than 0 and less than 0xFE000000 in idigi_config.h"
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
