/*
 * Copyright (c) 2011 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

#ifndef BELE_H_
#define BELE_H_

#include "idigi_types.h"

/*
 *  Endian-independent byte-extraction macros
 */
#define	LOW8(x16)      ((uint8_t) (x16))
#define	HIGH8(x16)      ((uint8_t) (((uint16_t)(x16)) >> 8))

#define	LOW16(x32)      ((uint16_t) (x32))
#define	HIGH16(x32)     ((uint16_t) (((uint32_t)(x32)) >> 16))

#define BYTE32_3(x32)   ((uint8_t) (((uint32_t)(x32)) >> 24))
#define BYTE32_2(x32)   ((uint8_t) (((uint32_t)(x32)) >> 16))
#define BYTE32_1(x32)   ((uint8_t) (((uint32_t)(x32)) >>  8))
#define BYTE32_0(x32)   ((uint8_t)  ((uint32_t)(x32)))

/*
 *	Endian-independent multi-byte-creation macros
 */
#define MAKE16(hi8,lo8)     ((uint16_t) (((uint16_t)(((uint16_t) (hi8) ) << 8 )) | ((uint16_t) (lo8))))
#define MAKE32(hi16,lo16)   ((uint32_t)  (((uint32_t) (((uint32_t)(hi16)) << 16)) | ((uint32_t)(lo16))))
#define MAKE64(hi32,lo32)   ((uint64_t)  (((uint64_t) (((uint32_t)(hi32)) << 32)) | ((uint32_t)(lo32))))
#define MAKE32_4(b3, b2, b1, b0)    MAKE32( MAKE16( b3, b2 ), MAKE16( b1, b0 ))

static  uint16_t LoadBE16(void const * const array)
{
    return MAKE16(((uint8_t *)(array))[0], ((uint8_t *)(array))[1]);
}

#if !(defined IDIGI_VENDOR_ID) || (defined IDIGI_DATA_SERVICE) || (defined IDIGI_FIRMWARE_SERVICE) || (defined IDIGI_FILE_SYSTEM) || (defined IDIGI_RCI_SERVICE)
static  uint32_t LoadBE32(void const * const array)
{
    return MAKE32_4(((uint8_t *)(array))[0], ((uint8_t *)(array))[1], ((uint8_t *)(array))[2], ((uint8_t *)(array))[3]);
}
#endif

static  void StoreBE16(void * const array, uint16_t const val)
{
    ((uint8_t *)(array))[0] = HIGH8(val);
    ((uint8_t *)(array))[1] = LOW8(val);
}

static  void StoreBE32(void * const array, uint32_t const val)
{
    ((uint8_t *)(array))[0] = BYTE32_3(val);
    ((uint8_t *)(array))[1] = BYTE32_2(val);
    ((uint8_t *)(array))[2] = BYTE32_1(val);
    ((uint8_t *)(array))[3] = BYTE32_0(val);
}

/*
 * static  uint32_t bele_SWAP32(uint32_t const val)
 * {
 *     return MAKE32(bele_SWAP16(LOW16(val)), bele_SWAP16(HIGH16(val)));
 * }
 */

#if (defined IDIGI_LITTLE_ENDIAN)

static  uint16_t bele_SWAP16(uint16_t const val)
{
    return MAKE16(LOW8(val), HIGH8(val));
}

#define FROM_LE32(x)    (x)
#define FROM_LE16(x)    (x)

#define TO_BE32(x)      (bele_SWAP32(x))
#define TO_BE16(x)      (bele_SWAP16(x))
#define FROM_BE32(x)    (bele_SWAP32(x))
#define FROM_BE16(x)    (bele_SWAP16(x))

#else

#define FROM_LE32(x)    (bele_SWAP32(x))
#define FROM_LE16(x)    (bele_SWAP16(x))

#define TO_BE32(x)      (x)
#define TO_BE16(x)      (x)
#define FROM_BE32(x)    (x)
#define FROM_BE16(x)    (x)

#endif

#endif /* BELE_H_ */
