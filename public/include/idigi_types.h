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

/**
 * @file
 *  @brief Types required by the iDigi connector
 *
 */

#ifndef IDIGI_TYPES_H_
#define IDIGI_TYPES_H_

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>

#if __STDC_VERSION__ >= 199901L
  /* If the compiler is C99 complaint, then we have stdint.h. */
  #define IDIGI_HAVE_STDINT_HEADER
#endif

#if (defined __MWERKS__) && (defined __MQX__)
  #if (!defined IDIGI_HAVE_STDINT_HEADER)
    #define IDIGI_HAVE_STDINT_HEADER
  #endif
#else
  #include <stdio.h>
#endif

#if defined IDIGI_HAVE_STDINT_HEADER
  #include <stdint.h>
  #include <inttypes.h>
#else

#if defined __SAM3X8E__
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned long int uint32_t;
  typedef long int int32_t;

#else
  typedef unsigned char uint8_t;
  typedef unsigned int uint16_t;
  typedef unsigned long uint32_t;
  typedef long int32_t;
#endif /* __SAM3X8E__ */
#ifndef UINT16_MAX
/**
*  Unsigned 16 bit maximum value.
*/
#define UINT16_MAX  0xFFFF
#endif

#ifndef INT32_MIN
/**
*  Signed 32 bit minimum value.
*/
#define INT32_MIN (-2147483647 -1)
#endif

#ifndef INT32_MAX
/**
*  Signed 32 bit maximum value.
*/
#define INT32_MAX 2147483647
#endif

#ifndef UINT32_MAX
/**
*  Unsigned 32 bit maximum value.
*/
#define UINT32_MAX 4294967295U
#endif

#ifndef SIZE_MAX
/**
*  size_t maximum value.
*/
#define SIZE_MAX  UINT32_MAX
#endif

#ifndef SCNd32
/**
*  Scan format specifier for signed 32 bit value.
*/
#define SCNd32 "ld"
#endif

#ifndef SCNu32
/**
*  Scan format specifier for unsigned 32 bit value.
*/
#define SCNu32 "lu"
#endif

#ifndef SCNx32
/**
*  Scan format specifier for 32 bit hex value.
*/
#define SCNx32 "lx"
#endif

#ifndef PRId32
/**
*  Print format specifier for signed 32 bit value.
*/
#define PRId32 "ld"
#endif

#ifndef PRIu32
/**
*  Print format specifier for unsigned 32 bit value.
*/
#define PRIu32 "lu"
#endif

#ifndef PRIx32
/**
*  Print format specifier for 32 bit hex value.
*/
#define PRIx32 "lx"
#endif

/**
* @}
*/


#endif

/**
* @defgroup idigi_network_handle_t Network Handle
* @{
*/
/**
 * Used to reference a network session and is passed into iDigi connector API calls. This
 * type is defined as an int and is used by the application to keep track of and identify sessions.
 * Application may define this to different type for its own network reference in idigi_type.h
 */
typedef int idigi_network_handle_t;
/**
* @}
*/


#endif /* IDIGI_TYPES_H_ */
