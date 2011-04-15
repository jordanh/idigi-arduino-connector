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
#ifndef FW_DEF_H_
#define FW_DEF_H_

#include "irl_fw.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Firmware Upgrade Facility Opcodes
 * @doc These are the valid opcodes for the Firmware Upgrade Facility
 * All other values are invalid and reserved.
 */
/// Target List Message
#define E_FAC_FU_OP_LIST_TARGETS 0x00
/// Firmware Info Request
#define E_FAC_FU_OP_INFO_REQ 0x01
/// Firmware Info Response
#define E_FAC_FU_OP_INFO_RESP 0x02
/// Firmware Download Request
#define E_FAC_FU_OP_DNLD_REQ 0x03
/// Firmware Download Response
#define E_FAC_FU_OP_DNLD_RESP 0x04
/// Firmware Code Block
#define E_FAC_FU_OP_CODE_BLK 0x05
/// Firmware Binary Block Acknowledge
#define E_FAC_FU_OP_CODE_BLK_ACK 0x06
/// Firmware Download Abort
#define E_FAC_FU_OP_DNLD_ABORT 0x07
/// Firmware Download Complete
#define E_FAC_FU_OP_DNLD_CMPLT 0x08
/// Firmware Download Complete Response
#define E_FAC_FU_OP_DNLD_CMPLT_RESP 0x09
/// Request Target Reset
#define E_FAC_FU_OP_TARGET_RESET 0x0a
/// Firmware Download Status
#define E_FAC_FU_OP_DNLD_STATUS 0x0b
/// Error, Target Invalid
#define E_FAC_FU_OP_TARGET_INVALID 0x0c


#ifdef __cplusplus
}
#endif


#endif /* FW_DEF_H_ */
