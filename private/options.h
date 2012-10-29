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
#ifndef OPTIONS_H_
#define OPTIONS_H_

/*
 * The defines in this file are included to allow compatibility with iDigi Connector v1.0 application makefiles
 */

#if defined(_LITTLE_ENDIAN)
#define IDIGI_LITTLE_ENDIAN
#endif

#if defined(_FIRMWARE_FACILITY)
#define IDIGI_FIRMWARE_SERVICE
#endif

#if defined(DEBUG)
#define IDIGI_DEBUG
#endif

#if (defined IDIGI_COMPRESSION_BUILTIN)
#define IDIGI_COMPRESSION
#endif

#endif /* OPTIONS_H_ */
