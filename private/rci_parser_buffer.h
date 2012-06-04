/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
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
    
    *(buffer->current) = value;
}

static idigi_bool_t ptr_in_buffer(rci_buffer_t const * const buffer, char const * const pointer)
{
    return ((pointer >= buffer->start) && (pointer <= buffer->end));
}

