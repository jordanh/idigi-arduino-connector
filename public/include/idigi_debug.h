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
#ifndef IDIGI_DEBUG_H_
#define IDIGI_DEBUG_H_

#include "idigi_config.h"

#if defined(IDIGI_DEBUG)
/**
* @defgroup DEBUG_MACROS User Defined Debug Macros
* @{
*/
/**
 * Debug output from the iDigi connector, Writes a formatted string to stdout, expanding the format
 * tags with the value of the argument list arg.  This function behaves exactly as
 * printf except that the variable argument list is passed as a va_list instead of a
 * succession of arguments.
 *
 * In the C library the prototype for vprintf is defined as vprintf(const char *format, va_list ap);
 *
 */
void idigi_debug_printf(char const * const format, ...);

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

/**
 * Compile time assertion of functional state (limits, range checking, etc.)
 *
 *   Failure will emit a compiler-specific error
 *           gcc: 'duplicate case value'
 *   Example:
 *           CONFIRM(sizeof (int) == 4);
 *           CONFIRM(CHAR_BIT == 8);
 *           CONFIRM(ElementCount(array) == array_item_count);
 */
#define CONFIRM(cond)           do { switch(0) {case 0: case (cond):;} } while (0)


#endif /* IDIGI_DEBUG_H_ */
