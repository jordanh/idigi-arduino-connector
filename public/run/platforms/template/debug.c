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
 /**
  * @file
  *  @brief Debug routine for the iDigi Connector.
  *
  */

#include "idigi_config.h"

#if defined(IDIGI_DEBUG)
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/**
 * @brief iDigi connector debug
 *
 * Debug output from the iDigi connector, Writes a formatted string to stdout, expanding the format
 * tags with the value of the argument list arg.
 * iDigi connector uses this routine to display debug information when @ref IDIGI_DEBUG is defined.
 *
 * @param [in] format Tells how to format the various arguments
 * @param [out] ...   A variable argument list of expressions whose values should be printed according
 *                    to the placeholders in the "format" string. If there are more placeholders than
 *                    supplied arguments, the result is undefined. If there are more arguments than
 *                    placeholders, the excess arguments are simply ignored.
 *
 * @see @ref IDIGI_DEBUG
 */
void idigi_debug_printf(char const * const format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}


#else

typedef int dummy;

#endif

