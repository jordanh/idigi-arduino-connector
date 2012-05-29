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

#ifndef IDIGI_CONFIG_H_
#define IDIGI_CONFIG_H_

#define IDIGI_LITTLE_ENDIAN

#ifdef USE_THE_MAKEFILE_INSTEAD

#define IDIGI_DEBUG
#define IDIGI_FIRMWARE_SERVICE
#define IDIGI_COMPRESSION
#define IDIGI_DATA_SERVICE
#endif

#ifdef USE_THE_DEFINES
#define IDIGI_DEVICE_TYPE                          "iDigi Connector Linux Sample"
#define IDIGI_CLOUD_URL                            "developer.idigi.com" 
#define IDIGI_TX_KEEPALIVE_IN_SECONDS              75 
#define IDIGI_RX_KEEPALIVE_IN_SECONDS              75 
#define IDIGI_WAIT_COUNT                           5 
#define IDIGI_VENDOR_ID                            0x00000000 
#define IDIGI_MSG_MAX_TRANSACTION                  1 
#define IDIGI_CONNECTION_TYPE                      idigi_lan_connection_type 
#define IDIGI_WAN_LINK_SPEED_IN_BITS_PER_SECOND    0 
#define IDIGI_WAN_PHONE_NUMBER_DIALED              "012345678"
#define IDIGI_FIRMWARE_SUPPORT
#define IDIGI_DATA_SERVICE_SUPPORT
#endif

#endif
