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


#if (defined IDIGI_DEBUG)
void idigi_debug_hexvalue(char * label, uint8_t * buff, int length)
{
    int i;

    idigi_debug_printf("%s = ", label);
    for (i=0; i<length; i++)
    {
        idigi_debug_printf(" %02X", buff[i]);
    }
    idigi_debug_printf("\n");
}


#else
#define idigi_debug_hexvalue(label, start, length)

static void idigi_debug_printf(char const * const format, ...)
{
    (void) format;
}

#endif

