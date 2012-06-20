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

#if 0

typedef struct malloc_stats{
    void const * ptr;
    size_t length;
    struct malloc_stats * prev;
    struct malloc_stats * next;
} malloc_stats_t;

/* These are used for debugging and tracking memory
 * usage and high water mark.
 */
static size_t high_malloc_mark = 0;
static size_t total_malloc_length = 0;
static malloc_stats_t * malloc_list = NULL;

/* Added allocated pointer and its size to
 * keep track number of allocated pointers and
 * allocated memory size.
 *
 * This is for debugging only.
 */
static void add_malloc_stats(void const * const ptr, size_t const length)
{
    malloc_stats_t   * pMalloc;

    pMalloc = malloc(sizeof *pMalloc);
    ASSERT(pMalloc != NULL);

    if (pMalloc != NULL)
    {
        pMalloc->ptr = ptr;
        pMalloc->length = length;

        add_node(&malloc_list, pMalloc);

        total_malloc_length += length;
        if (total_malloc_length > high_malloc_mark)
        {
            high_malloc_mark = total_malloc_length;
        }
    }
}

/* free and remove allocated memory */
static void del_malloc_stats(void const * const ptr)
{
    malloc_stats_t   * pMalloc;

    for (pMalloc = malloc_list; pMalloc != NULL; pMalloc = pMalloc->next)
    {
        if (pMalloc->ptr == ptr)
        {
            remove_node(&malloc_list, pMalloc);

            total_malloc_length -= pMalloc->length;
            free(pMalloc);
            break;
        }
    }

    ASSERT(pMalloc != NULL);
}
#endif

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

#define add_malloc_stats(ptr, size)
#define del_malloc_stats(ptr)
