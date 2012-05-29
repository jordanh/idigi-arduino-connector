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
#include <Windows.h>
#include "idigi_api.h"
#include "platform.h"

int main (void)
{
	WSADATA wsaData;

	/* Initialize Winsock */
	if (WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		APP_DEBUG("WSAStartup failed: %d\n", WSAGetLastError());
		return 1;
	}

    (void)idigi_init((idigi_callback_t)0);

	WSACleanup();

    return 0;
}
