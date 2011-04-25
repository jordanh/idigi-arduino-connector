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
#ifndef IRL_FW_H_
#define IRL_FW_H_

#include "e_types.h"
#include "irl_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define	IRL_FIRMWARE_FACILTIY_VERSION		0x00000001
#define IRL_FIRMWARE_ACCESS_FACILITY		(0x1 << IRL_CONFIG_FIRMWARE_FACILITY)

enum {
	IRL_FA_TARGET_COUNT  = IRL_FIRMWARE_ACCESS_FACILITY,
	IRL_FA_VERSION,
	IRL_FA_CODE_SIZE,
	IRL_FA_DESCRIPTION,
	IRL_FA_FILE_NAME_SPEC,
	IRL_FA_DOWNLOAD_REQ,
   	IRL_FA_DOWNLOAD_DATA,
	IRL_FA_DOWNLOAD_DONE,
	IRL_FA_DOWNLOAD_ABORT,
	IRL_FA_DOWNLOAD_RESET
};

#define IRL_FA_ID_STRING_LENGTH		128  /* bytes */


#define IRL_FA_SUCCESS													0x00
#define IRL_FA_DOWNLOAD_DENIED_ERR						0x01
#define IRL_FA_DOWNLOAD_INVALID_SIZE_ERR				0x02
#define IRL_FA_DOWNLOAD_INVALID_VERSION_ERR		0x03
#define IRL_FA_DOWNLOAD_UNAUTHENTICATED_ERR	0x04
#define IRL_FA_DOWNLOAD_NOT_ALLOWED_ERR			0x05
#define IRL_FA_DOWNLOAD_REJECT_ERR							0x06
#define IRL_FA_FIRMWARE_ERR	 										0x07

#define IRL_FA_USER_ABORT_ERR				0x08
#define IRL_FA_DEVICE_ERR						0x09
#define IRL_FA_INVALID_OFFSET_ERR		0x0a
#define IRL_FA_INVALID_DATA_ERR			0x0b
#define IRL_FA_HARWARE_ERR					0x0c

/* download complete status */
#define IRL_FA_DOWNLOAD_STATUS_SUCCESS			0x0
#define IRL_FA_DOWNLOAD_CHECKSUM_MISMATCH	0x1
#define IRL_FA_DOWNLOAD_NOT_COMPLETE				0x2

typedef union {
	struct {
		uint8_t		target;
		uint32_t		version;
		uint32_t		code_size;
		char			* desc_string;
		char			* file_name_spec;
		char			* filename;
	} download_request;

	struct {
		uint8_t		target;
		uint32_t		code_size;
		uint32_t		checksum;
	} download_complete;

	struct {
		uint8_t		target;
		uint32_t		offset;
		uint8_t		* data;
		size_t			length;
	} image_data;

	struct {
		uint8_t	target;
		uint8_t	status;
	} abort;

	uint8_t	target;

} IrlFirmwareReq_t;

typedef union {
		uint8_t		target_count;
		uint32_t		target_version;
		uint32_t		avail_size;
		char 			*desc_string;
		char			* file_name_spec;

		struct {
			uint32_t	version;
			uint32_t	calculated_checksum;
			uint8_t	status;
		} download_complete;

		uint8_t		error_status;
} IrlFirmwareRsp_t;


typedef IrlStatus_t (* IrlFimwareFacilityCb_t )(unsigned command, IrlFirmwareReq_t * request, IrlFirmwareRsp_t * response, unsigned timeout);


typedef struct irl_firmware_facility_t {
	uint32_t							version;
	IrlFimwareFacilityCb_t	callback;
} IrlFirmwareFacility_t;

int irlEnable_FirmwareFacility(unsigned long irl_handle, void * firmware_data);


#ifdef __cplusplus
}
#endif

#endif /* IRL_FW_H_ */
