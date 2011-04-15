/*
 * irl_os.h
 *
 *  Created on: Apr 8, 2011
 *      Author: mlchan
 */

#ifndef IRL_OS_H_
#define IRL_OS_H_

#include <stdio.h>

#include "e_types.h"
#include "irl_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IRL_OS_CONFIG		(0x1 << IRL_CONFIG_OS_IFACE)

enum {
	IRL_GET_SYSTEM_TIME = IRL_OS_CONFIG,
	IRL_WAIT,
	IRL_LOCK,
	IRL_UNLOCK,
	IRL_MALLOC,
	IRL_FREE,
};

struct irl_malloc_t {
		size_t		length;
		void		* ptr;
};


#ifdef __cplusplus
}
#endif

#endif /* IRL_OS_H_ */
