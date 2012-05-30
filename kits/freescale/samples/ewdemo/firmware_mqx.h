
/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
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

#ifndef _FAPP_TFTP_H_

#define _FAPP_TFTP_H_

#define FNET_OK         (0) /*< No error. */
#define FNET_ERR        -1
#define FNET_MK         (1)
#define FNET_TRUE       (1)
#define FNET_FALSE      (0)

#define PFLASH_SIZE               0x00080000
#define PFLASH_BLOCK0_BASE        0x0000C000 /* Skip the 1st 48K which is the bootloader */
#define PFLASH_BLOCK1_BASE        0x00040000
#define PFLASH_BLOCK1_IMAGE_BASE  0x00040004
#define PFLASH_BLOCK0_SIZE        PFLASH_BLOCK1_BASE - 0x0000C000
#define PFLASH_BLOCK1_SIZE        PFLASH_SIZE - PFLASH_BLOCK1_BASE
#define PFLASH_BLOCK1_IMAGE_SIZE  PFLASH_SIZE - PFLASH_BLOCK1_IMAGE_BASE

/* Command definitions for FNET_FTFL_FCCOB0 */
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_READ_BLOCK             (0x00)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_READ_SECTION           (0x01)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_PROGRAM_CHECK          (0x02)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_READ_RESOURCE          (0x03)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_PROGRAM_LONGWORD       (0x06) 
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_ERASE_BLOCK            (0x08)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_ERASE_SECTOR           (0x09)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_PROGRAM_SECTION        (0x0B)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_READ_BLOCKS            (0x40)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_READ_ONCE              (0x41)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_PROGRAM_ONCE           (0x43)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_ERASE_BLOCKS           (0x44)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_VERIFY_KEY             (0x45)
#define FNET_MK_FNET_FTFL_FCCOB0_CMD_PROGRAM_PARTITION      (0x80)

/*/ Smallest logical block which can be erased independently.*/
#define FNET_CFG_CPU_FLASH_PAGE_SIZE        (2*1024)        /* 2KB sector.*/

/* On-chip Flash size.*/
#define FNET_CFG_CPU_FLASH_SIZE             (1024 * 512)    /* 512 KB */

/* To use _Pragma(), enable C99 Extensions. */

#pragma c99 on

/* For CW10.1 ARM compiler. After CW10.2 Release is obsolete. */
#if FNET_MK
	#define FNET_COMP_PACKED __attribute__ ((aligned (1)))
#endif

#if 1 /* Newest CWs pragma */
	#define FNET_COMP_PACKED_BEGIN  _Pragma("pack(1)")
	#define FNET_COMP_PACKED_END    _Pragma("pack()")
#else /* Old CWs pragma */
	#define FNET_COMP_PACKED_BEGIN  _Pragma("options align = packed")
	#define FNET_COMP_PACKED_END    _Pragma("options align = reset")
#endif
/* SREC Tx Handler states */ 
typedef enum 
{
    FAPP_TFTP_RX_HANDLER_SREC_STATE_INIT = 0,
    FAPP_TFTP_RX_HANDLER_SREC_STATE_GETSTART,
    FAPP_TFTP_RX_HANDLER_SREC_STATE_GETTYPE,
    FAPP_TFTP_RX_HANDLER_SREC_STATE_GETCOUNT,
    FAPP_TFTP_RX_HANDLER_SREC_STATE_GETDATA
}
fapp_tftp_rx_handler_srec_state_t;

/* SREC Rx Handler states */ 
typedef enum 
{
    FAPP_TFTP_TX_HANDLER_SREC_STATE_INIT = 0,
    FAPP_TFTP_TX_HANDLER_SREC_STATE_DATA,
    FAPP_TFTP_TX_HANDLER_SREC_STATE_EOB,
    FAPP_TFTP_TX_HANDLER_SREC_STATE_END
}
fapp_tftp_tx_handler_srec_state_t;


/*
+-------------------//------------------//-----------------------+
| type | count | address  |            data           | checksum |
+-------------------//------------------//-----------------------+
*/

FNET_COMP_PACKED_BEGIN
/* TFTP SREC handler control structure. */
struct fapp_tftp_rx_handler_srec
{
    fapp_tftp_rx_handler_srec_state_t state FNET_COMP_PACKED;
    unsigned char type 						FNET_COMP_PACKED;	/* Record type */
    int record_hex_index 					FNET_COMP_PACKED;
    union 
    {
        struct
        {
            unsigned char count 	FNET_COMP_PACKED;	/* Byte count */
            unsigned char data[255] FNET_COMP_PACKED; 	/* Address + Data */
        } record	FNET_COMP_PACKED;
        unsigned char record_bytes[255+1]	FNET_COMP_PACKED;
    }	FNET_COMP_PACKED;
};
/* TFTP SREC TX handler control structure. */
#define FAPP_TFTP_SREC_DATA_MAX     (28)
struct fapp_tftp_tx_handler_srec
{
    fapp_tftp_tx_handler_srec_state_t state	FNET_COMP_PACKED;
    struct
    {
        char S 			FNET_COMP_PACKED;
        char type 		FNET_COMP_PACKED;
        char count[2] 	FNET_COMP_PACKED;
        char address[8] FNET_COMP_PACKED;
        short data[FAPP_TFTP_SREC_DATA_MAX + 1/*checksum*/ +1/*\r\n*/+ 1/*eol*/] FNET_COMP_PACKED;
    } srec_line 		FNET_COMP_PACKED;
    int srec_line_size 	FNET_COMP_PACKED;
    char *srec_line_cur FNET_COMP_PACKED;
    unsigned char *data_start 	FNET_COMP_PACKED;
    unsigned char *data_end 	FNET_COMP_PACKED;
};
FNET_COMP_PACKED_END

/************************************************************************
*     Function Prototypes
*************************************************************************/
int idigi_rx_handler_srec (unsigned char* data, unsigned long data_size);

#endif
