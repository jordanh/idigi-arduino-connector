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

#if defined(DEBUG)
#include <malloc.h>
#include "idk_def.h"

struct malloc_stats_t {
	void  * ptr;
	size_t length;
	struct malloc_stats_t * next;
};

size_t gMallocLength = 0;
struct malloc_stats_t * gMallocStats = NULL;

static void add_malloc_stats(void * ptr, size_t length)
{
	struct malloc_stats_t	* pMalloc;

	pMalloc = (struct malloc_stats_t *)malloc(sizeof(struct malloc_stats_t));
	if (pMalloc != NULL)
	{
		pMalloc->ptr = ptr;
		pMalloc->length = length;
		pMalloc->next = gMallocStats;

		gMallocStats = pMalloc;

		gMallocLength += length;

	}
	else
	{
		DEBUG_PRINTF("add_malloc_stats: malloc failed\n");
	}
}

static void del_malloc_stats(void * ptr)
{
	struct malloc_stats_t	* pMalloc, * pMalloc1 = NULL;

	for (pMalloc = gMallocStats; pMalloc != NULL; pMalloc = pMalloc->next)
	{
		if (pMalloc->ptr == ptr)
		{
			if (pMalloc1 != NULL)
			{
				pMalloc1->next = pMalloc->next;
			}
			if (pMalloc == gMallocStats)
			{
				gMallocStats = pMalloc->next;
			}

			gMallocLength -= pMalloc->length;
			free(pMalloc);
			break;
		}
		pMalloc1 = pMalloc;
	}

	if (pMalloc == NULL)
	{
		DEBUG_PRINTF("del_malloc_stats: free invalid pointer\n");
	}
}
#else
#define add_malloc_stats(ptr, size)
#define del_malloc_stats(ptr)
#endif

