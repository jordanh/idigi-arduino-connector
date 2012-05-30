/*
 * Copyright (c) 2011, 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
#ifndef __IDIGI_CONFIG_H_
#define __IDIGI_CONFIG_H_

#define IDIGI_LITTLE_ENDIAN
#define IDIGI_DEBUG
#define IDIGI_FIRMWARE_SERVICE
/* #define IDIGI_COMPRESSION */
#define IDIGI_DATA_SERVICE
#define IDIGI_FILE_SYSTEM
/* #define IDIGI_RCI_SERVICE */
#define IDIGI_RCI_MAXIMUM_CONTENT_LENGTH    256

#if ENABLE_COMPILE_TIME_DATA_PASSING
#define IDIGI_DEVICE_TYPE                          "iDigi connector Linux Sample"
#define IDIGI_CLOUD_URL                            "developer.idigi.com"
#define IDIGI_TX_KEEPALIVE_IN_SECONDS              5
#define IDIGI_RX_KEEPALIVE_IN_SECONDS              75
#define IDIGI_WAIT_COUNT                           5
#define IDIGI_VENDOR_ID                            0x00000000
#define IDIGI_MSG_MAX_TRANSACTION                  1
#define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type
#define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0
#define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678"
#define IDIGI_FIRMWARE_SUPPORT
#define IDIGI_DATA_SERVICE_SUPPORT
#define IDIGI_FILE_SYSTEM_SUPPORT
#define IDIGI_REMOTE_CONFIGURATION_SUPPORT

#define IDIGI_DEVICE_ID_METHOD                      idigi_auto_device_id_method
#endif

#endif
