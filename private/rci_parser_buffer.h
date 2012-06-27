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

static void rci_set_buffer(rci_buffer_t * const dst, rci_service_buffer_t const * const src)
{
    char * const start = src->data;

    dst->start = start;
    dst->end = start + src->bytes;
    dst->current = start;
}

static size_t rci_buffer_remaining(rci_buffer_t const * const buffer)
{
    return (buffer->end - buffer->current);
}

static size_t rci_buffer_used(rci_buffer_t const * const buffer)
{
    return (buffer->current - buffer->start);
}

static char * rci_buffer_position(rci_buffer_t const * const buffer)
{
    return buffer->current;
}

static void rci_buffer_advance(rci_buffer_t * const buffer, size_t const amount)
{
    ASSERT((buffer->current + amount) <= buffer->end);
    buffer->current += amount;
}

static int rci_buffer_read(rci_buffer_t const * const buffer)
{
    ASSERT(rci_buffer_remaining(buffer) != 0);

    return *(buffer->current);
}

static void rci_buffer_write(rci_buffer_t const * const buffer, int const value)
{
    ASSERT(rci_buffer_remaining(buffer) != 0);
    ASSERT(value <= UCHAR_MAX);

    *(buffer->current) = value;
}

static idigi_bool_t ptr_in_buffer(rci_buffer_t const * const buffer, char const * const pointer)
{
    return idigi_bool((pointer >= buffer->start) && (pointer < buffer->end));
}

