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
