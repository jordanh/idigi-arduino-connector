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
/* #define IDIGI_DEBUG */
#define APP_DEBUG_ENABLED
/* #define IDIGI_FIRMWARE_SERVICE */
/* #define IDIGI_COMPRESSION */
/* #define IDIGI_DATA_SERVICE */
/* #define IDIGI_FILE_SYSTEM */
/* #define IDIGI_RCI_SERVICE */
#define IDIGI_DEFAULT_SERVERHOST    "my.idigi.com"
#define IDIGI_DEFAULT_DEVICETYPE    "Arduino"
#define IDIGI_DEFAULT_LINKSPEED     10000000
#define IDIGI_DEFAULT_PHONENUMBER   ""

#define IDIGI_MAC_LENGTH            6
#define IDIGI_IP_LENGTH             4
#define IDIGI_DEVICEID_LENGTH       16
#define IDIGI_VENDORID_LENGTH       4
#define IDIGI_SERVERHOST_LENGTH     32
#define IDIGI_DEVICETYPE_LENGTH     32
#define IDIGI_PHONENUMBER_LENGTH    32

#define IDIGI_PRINTF_LENGTH         128

#define IDIGI_RCI_MAXIMUM_CONTENT_LENGTH    256
#define IDIGI_FILE_SYSTEM_MAX_PATH_LENGTH   256

#define UNUSED_ARGUMENT(x)     ((void)x)

#endif
