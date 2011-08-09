/*********************************************************************
 *
 *            IP Route Header File
 *
 *********************************************************************
 * FileName:        Route.h
 * Description: 	Route API for TCP Stack
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
*/

void AddHostRoute( DWORD IPadr, BYTE* pMACadr, BOOL CreatedByRemoteHost );
void InitHostRouteTable();
BYTE* QueryHostRoute( DWORD IPadr );

int Rte_AddReplyRecord( DWORD IPadr );

#define ROUTE_REC_EXISITS 	1
#define ROUTE_REC_ADDED		2
#define ROUTE_NO_MEM		3


