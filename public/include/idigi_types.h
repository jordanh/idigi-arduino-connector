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

#if __STDC_VERSION__ >= 199901L /* Test used for C89/C99 Compiler */
  /* 
   * If the compilier is C99/C89 compliant, then stdint.h will have the defines
   * we need.
   */
  #include <stdint.h>
#else
/**
* @defgroup user_types User Defined C types
* If your compiler is not C89/C99 compliant these defines are used: you will need 
* to define them approprialtly for your system.  If your compiler is C89/C99 compliant 
* then the types from stdint.h are used.
* @{
*/
/** 
 *  Unsigned 8 bit value. 
 */
  typedef unsigned char uint8_t;

/**
 *  Unsigned 16 bit value. 
 */
  typedef unsigned short uint16_t;

/**
 *  Unsigned 32 bit value. 
 */
  typedef unsigned long uint32_t;
/**
* @}
*/
#define UINT16_MAX  0xFFFF

#endif


/**
* @defgroup idigi_network_handle_t Network Handle
* @{
*/
/**
 * Used to reference a network session and is passed into IIK API calls. This 
 * type is defined as an int and is used by the application to keep track of and identify sessions.
 * Application may define this to different type for its own network reference in idigi_type.h
 */
typedef int idigi_network_handle_t;
/**
* @}
*/

#if defined(IDIGI_DEBUG)
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
* @defgroup DEBUG_MACROS User Defined Debug Macros
* @{
*/
/**
 * Debug output from the IIK, Writes a formatted string to stdout, expanding the format 
 * tags with the value of the argument list arg.  This function behaves exactly as 
 * printf except that the variable argument list is passed as a va_list instead of a 
 * succession of arguments.
 *
 * In the C library the prototype for vprintf is defined as vprintf(const char *format, va_list ap);
 *
 */
#define USER_DEBUG_VPRINTF  vprintf

/**
 *  Verify that the condition is true, otherwise halt the program.
 */
#define ASSERT(cond)        assert(cond)
#else
#define ASSERT(cond)
#endif
/**
* @}
*/

#endif /* IDIGI_TYPES_H_ */
