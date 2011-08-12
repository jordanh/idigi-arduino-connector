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

#ifndef BELE_H_
#define BELE_H_

/*
 *  Byte-extraction/creation macros
 *
 *      These are not endian-dependent.
 */

#define LOW8(x16)       ((unsigned char) (x16))
#define HIGH8(x16)      ((unsigned char) (((unsigned short)(x16)) >> 8))

#define LOW16(x32)      ((unsigned short) (x32))
#define HIGH16(x32)     ((unsigned short) (((unsigned int)(x32)) >> 16))

#define MAKE16(hi8,lo8)     ((unsigned short) (((unsigned short)(((unsigned char) (hi8) ) << 8 )) | ((unsigned char) (lo8))))
#define MAKE32(hi16,lo16)   ((unsigned int)  (((unsigned int) (((unsigned short)(hi16)) << 16)) | ((unsigned short)(lo16))))
#define MAKE64(hi32,lo32)   ((unsigned long long)  (((unsigned long long) (((unsigned int)(hi32)) << 32)) | ((unsigned int)(lo32))))

#define MAKE32_4(b3, b2, b1, b0)        MAKE32( MAKE16( b3, b2 ), MAKE16( b1, b0 ))


    /* These macros handle little-endian vs big-endian
     * byte-swapping for 16- and 32-bit quantities.
     *
     * FROM_xx      Translates FROM xx format to native CPU format
     * TO_xx        Translates from native CPU format TO xx format
     */


#ifdef _LITTLE_ENDIAN

/* These are the definitions for LITTLE_ENDIAN */

#define TO_LE32(x)	(x)
#define TO_LE16(x)  	(x)
#define FROM_LE32(x)	(x)
#define FROM_LE16(x)	(x)

#define TO_BE32(x)    ((((((x) << 16) | ((x) >> 16)) >> 8) & 0x00ff00ff) | (((((x) << 16) | ((x) >> 16)) << 8) & 0xff00ff00))
#define TO_BE16(x)	  ((unsigned short)(((x) >> 8 | (x) << 8) & 0xFFFF))
#define FROM_BE32(x)	((((((x) << 16) | ((x) >> 16)) >> 8) & 0x00ff00ff) | (((((x) << 16) | ((x) >> 16)) << 8) & 0xff00ff00))
#define FROM_BE16(x)	((unsigned short)(((x) >> 8 | (x) << 8) & 0xFFFF))

#define LoadNative16(array)    LoadLE16(array)
#define LoadNative32(array)    LoadLE32(array)
#define StoreNative16(array,w)    StoreLE16(array,w)
#define StoreNative32(array,dw)    StoreLE32(array,dw)

/* Little-endian 16-bit wide interface. */
/* 8-bit strings need a byte swap.
 * Aligned 16-bit values arrive properly.
 * Unaligned 16-bit values are split between words.
 * 32-bit values need a 32-bit organized 16-bit swap.
 */
#define TO_WE32(x)  (x)
#define TO_WE16(x)  (x)
#define FROM_WE32(x)    (x)
#define FROM_WE16(x)    (x)

#define WE16cpy(dst, src, n)    (memcpy((dst), (src), (n)))

#else /* !_LITTLE_ENDIAN */

/* These are the definitions for BIG_ENDIAN */

#define TO_LE32(x)	(swap32(x))
#define TO_LE16(x)  	((unsigned short)(((x) >> 8 | (x) << 8) & 0xFFFF))
#define FROM_LE32(x)	(swap32(x))
#define FROM_LE16(x)	((unsigned short)(((x) >> 8 | (x) << 8) & 0xFFFF))

#define TO_BE32(x)	(x)
#define TO_BE16(x)	(x)
#define FROM_BE32(x)	(x)
#define FROM_BE16(x)	(x)

#define LoadNative16(array)    LoadBE16(array)
#define LoadNative32(array)    LoadBE32(array)
#define StoreNative16(array,w)    StoreBE16(array,w)
#define StoreNative32(array,dw)    StoreBE32(array,dw)

/* Little-endian 16-bit wide interface. */
/* 8-bit strings need a byte swap.
 * Aligned 16-bit values arrive properly.
 * Unaligned 16-bit values are split between words.
 * 32-bit values need a 32-bit organized 16-bit swap.
 */
#define TO_WE32(x)  ((unsigned long)(x)>>16 | (unsigned long)(x) << 16)
#define TO_WE16(x)  (x)
#define FROM_WE32(x)    ((unsigned long)(x)>>16 | (unsigned long)(x) << 16)
#define FROM_WE16(x)    (x)

/* Copy n bytes from src to dst converting the string as you go. */
/* NOTE: If n is odd, this will write one byte past the end! */
extern void WE16cpy(void *dst, void *src, int n);

#endif /* _LITTLE_ENDIAN */

/* NOTE: the do{}while constructs in the macros below are a nice way to combine
   multiple C statements into a single statement.  This allows these macros to
   be used in if statements without compiler complaints:
   if (a)
    StoreBE16(a,b);
   else
        StoreBE16(d,b);
   Otherwise, these would require explicit braces {} to compile.
*/

/* This set of macros is used to read values from misaligned data areas.
 * Typically this is a buffer received from some device (ethernet?) at
 * arbitrary address.  They have been modified so that pointers to larger
 * than character will work.
 */
/* void StoreBE16(char array[2], Word w);
 *  Stores 16-bit value as BigEndian 16-bit value in char array.
 */

#define StoreBE16(addr, w) \
    do { ((unsigned char *)(addr))[0] = (w) >> 8; \
         ((unsigned char *)(addr))[1] = (w) & 0xff; } while (0)


/* Word LoadBE16(char array[2]);
 *  Loads BigEndian 16-bit value from char array and returns it as 16-bit value.
 */
#define LoadBE16(addr) \
    (((unsigned char *)(addr))[0] << 8 | ((unsigned char *)(addr))[1])


/* void StoreBE32(char array[4], DWord dw);
 *  Stores 32-bit value as BigEndian 32-bit value in char array.
 */
#define StoreBE32(array, dw)    \
    do {((unsigned char *)(array))[0] = (dw) >> 24; \
        ((unsigned char *)(array))[1] = (dw) >> 16; \
    ((unsigned char *)(array))[2] = (dw) >> 8; \
        ((unsigned char *)(array))[3] = (dw); } while (0)

/* DWord LoadBE32(char array[4]);
 *  Loads BigEndian 32-bit value from char array and returns it as 32-bit value.
 */
#define LoadBE32(array)    \
    (((unsigned char *)(array))[0] << 24 | \
     ((unsigned char *)(array))[1] << 16 | \
     ((unsigned char *)(array))[2] << 8 | \
     ((unsigned char *)(array))[3])


/* void StoreLE16(char array[2], Word w);
 *  Stores 16-bit value as LittleEndian 16-bit value in char array.
 */
#define StoreLE16(addr, w) \
    do { ((unsigned char *)(addr))[0] = (w) & 0xff; \
         ((unsigned char *)(addr))[1] = (w) >> 8; } while (0)


/* Word LoadLE16(char array[2]);
 *  Loads LittleEndian 16-bit value from char array and returns it as 16-bit value.
 */
#define LoadLE16(addr) \
    (((unsigned char *)(addr))[0] | ((unsigned char *)(addr))[1] << 8)

/* void StoreLE32(char array[4], DWord dw);
 *  Stores 32-bit value as LittleEndian 32-bit value in char array.
 */
#define StoreLE32(array, dw)    \
    do {((unsigned char *)(array))[0] = (dw); \
        ((unsigned char *)(array))[1] = (dw) >> 8; \
    ((unsigned char *)(array))[2] = (dw) >> 16; \
        ((unsigned char *)(array))[3] = (dw) >> 24; } while (0)

/* DWord LoadLE32(char array[4]);
 *  Loads LittleEndian 32-bit value from char array and returns it as 32-bit value.
 */
#define LoadLE32(array)    \
    (((unsigned char *)(array))[0] | \
     ((unsigned char *)(array))[1] << 8 | \
     ((unsigned char *)(array))[2] << 16 | \
     ((unsigned char *)(array))[3] << 24)

/* These functions are for 16-bit wide little-endian interfaces. */

/* void StoreWE16(char array[2], Word w);
 *  Stores native 16-bit value as 16-bitEndian 16-bit value in char array.
 */
#define StoreWE16(array, w) StoreNative16(array, w)

/* Word LoadWE16(char array[2]);
 *  Loads 16-bitEndian 16-bit value from char array and returns it as 16-bit value.
 */
#define LoadWE16(array)    LoadNative16(array)

/* void StoreWE32(char array[4], DWord dw);
 *  Stores 32-bit value as 16-bitEndian 32-bit value in char array.
 */
#define StoreWE32(array, dw)    \
    do {StoreNative16((array), (unsigned short)((dw)&0xFFFF)); \
    StoreNative16((array)+2, (unsigned short)((dw) >> 16)); } while (0)

/* DWord LoadWE32(char array[4]);
 *  Loads 16-bitEndian 32-bit value from char array and returns it as 32-bit value.
 */
#define LoadWE32(array)    \
    (LoadNative16((array)) | LoadNative16((array)+2) << 16)

#endif /* BELE_H_ */
