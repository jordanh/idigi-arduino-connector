/*
 * fw_def.h
 *
 *  Created on: Mar 29, 2011
 *      Author: mlchan
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
