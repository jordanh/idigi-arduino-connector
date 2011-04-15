/*
 * os_intf.c
 *
 *  Created on: Mar 24, 2011
 *      Author: mlchan
 */
#include <malloc.h>
#include "irl_os.h"
#include "os_intf.h"


int irl_wait(struct irl_setting_t * irl_ptr, uint32_t wait_time)
{
	uint32_t	mswait = wait_time;

	int					rc = IRL_CONFIG_ERR;
	IrlStatus_t			status;

	status = irl_get_config(irl_ptr, IRL_WAIT, &mswait);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;


}

int irl_get_system_time(struct irl_setting_t * irl_ptr, uint32_t * mstime)
{
	int					rc = IRL_CONFIG_ERR;
	IrlStatus_t			status;

	status = irl_get_config(irl_ptr, IRL_GET_SYSTEM_TIME, mstime);
	if (status == IRL_STATUS_CONTINUE)
	{
		rc = IRL_SUCCESS;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;
}

struct malloc_stats_t {
	void 	* ptr;
	size_t	length;
	struct malloc_stats_t * next;
};

size_t					gMallocLength = 0;
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
		DEBUG_TRACE("add_malloc_stats: malloc failed\n");
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
		DEBUG_TRACE("del_malloc_stats: free invalid pointer\n");
	}
}

int irl_malloc(struct irl_setting_t * irl_ptr, size_t length, void ** ptr)
{
	int					rc = IRL_CONFIG_ERR;
	IrlStatus_t			status;
	struct irl_malloc_t	data;

	data.length = length;
	data.ptr = NULL;

	status = irl_get_config(irl_ptr, IRL_MALLOC, &data);
	if (status == IRL_STATUS_CONTINUE)
	{
		add_malloc_stats(data.ptr, data.length);
		*ptr = data.ptr;
		rc = IRL_SUCCESS;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;
}

int irl_free(struct irl_setting_t * irl_ptr, void * ptr)
{
	int				rc = IRL_CONFIG_ERR;
	IrlStatus_t		status;

	status = irl_get_config(irl_ptr, IRL_FREE, ptr);
	if (status == IRL_STATUS_CONTINUE)
	{
		del_malloc_stats(ptr);
		rc = IRL_SUCCESS;
	}
	else if (status == IRL_STATUS_BUSY)
	{
		rc = IRL_BUSY;
	}

	return rc;
}

