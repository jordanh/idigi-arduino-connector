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

 /**
  * @file
  *  @brief Types required by the IIK
  *
  */


#ifndef IDIGI_TYPES_H_
#define IDIGI_TYPES_H_

#include <string.h>
#include <limits.h>
#include <assert.h>

#if __STDC_VERSION__ >= 199901L
  #include <stdint.h>
  #include <stdbool.h>
#else

/*! \fn typedef uint8_t
 *  Unsigned 8 bit value. 
 */
  typedef unsigned char uint8_t;

/*! \fn typedef uint16_t
 *  Unsigned 16 bit value. 
 */
  typedef unsigned short uint16_t;

/*! \fn typedef uint32_t
 *  Unsigned 32 bit value. 
 */
  typedef unsigned long uint32_t;


/*! \fn typedef bool
 *  Boolean value which is either true or false. 
 */
  typedef int bool;

/*! \fn typedef true
 *  Expand to 1 for true
 */
  #define true    1

/*! \fn typedef false
 *  Expand to 0 for false. 
 */
  #define false   0
#endif


/*! \fn typedef idigi_network_handle_t
 * Used to reference a network session and is passed into IIK API calls. This 
 * type is defined as an int and is used by the application to keep track of and identify sessions.
 */
typedef int idigi_network_handle_t;

typedef void * idigi_lock_handle_t;

#if defined(IDIGI_DEBUG)
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define USER_DEBUG_VPRINTF  vprintf

/*!
 *  Verify that the condition is true, otherwise halt the program
 */
#define ASSERT(cond)        assert(cond)
#else
#define ASSERT(cond)
#endif

#define UNUSED_PARAMETER(x)     ((void)x)

#endif /* IDIGI_TYPES_H_ */
