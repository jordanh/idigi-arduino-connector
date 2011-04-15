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
#ifndef _E_TYPES_H
#define _E_TYPES_H

//#include <sys/types.h>
#include <stdint.h>
/*
 * e_types.h   Copyright 2000-2002 Embrace Networks Inc.
 */
#define	TRUE		1
#define	FALSE		0

#if 0
/// signed 8-bit char
typedef signed char		int8_t;

/// signed 16-bit short integer
typedef signed short	int16_t;

/// signed 32-bit long integer
//typedef signed long		int32_t;

/// unsigned 8-bit char
typedef unsigned char	uint8_t;

/// unsigned 16-bit short integer
typedef unsigned short	uint16_t;

/// unsigned 32-bit long integer
typedef unsigned long	uint32_t;
//#else
typedef unsigned long u32, ULONG, uint32;
typedef int i32;
typedef unsigned short u16, uint16;
typedef unsigned char u8, uint8, BYTE;
typedef unsigned char uchar;

#endif

/// boolean value
typedef char				e_boolean_t;


#endif /* _E_TYPES_H */
