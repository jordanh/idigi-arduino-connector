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
#ifndef _EI_SECURITY_H
#define _EI_SECURITY_H


/* Identity verification form codes... */
#define SECURITY_IDENT_FORM_SIMPLE   0x00 /* simple verification */
#define SECURITY_IDENT_FORM_CRYPTO   0x01 /* cryptographic verification */
#define SECURITY_IDENT_FORM_PASSWORD 0x02 /* simple+passwd */


/* The data security coding schemes (a.k.a. encryption types)... */
#define SECURITY_PROTO_NONE          0x00 /* no encryption, no authentication */
#define SECURITY_PROTO_AESMD5        0x01 /* AES encryption, MD5 authentication */
#define SECURITY_PROTO_NONEMD5       0x02 /* no encryption, MD5 authentication */
/* The following coding scheme is no longer supported. (4 Sep 2001) */
#define SECURITY_PROTO_RC4MD5        0x01 /* RC4 encryption, MD5 authentication */

/* The security operations other than encryption... */
#define SECURITY_OPER_MINID          0x80
#define SECURITY_OPER_MAXID          0xff
#define SECURITY_OPER_IDENT_FORM     0x80
#define SECURITY_OPER_DEVICE_ID      0x81
#define SECURITY_OPER_CHALLENGE      0x82
#define SECURITY_OPER_RESPONSE       0x83
#define SECURITY_OPER_FINAL_RESPONSE 0x84
#define SECURITY_OPER_FAILURE        0x85
#define SECURITY_OPER_URL            0x86
#define SECURITY_OPER_URL_VERIFIER   0x87
#define SECURITY_OPER_PASSWD         0x88

/* Maximum packet size for a security message. */
#define SECURITY_MAX_PACKET_SIZE     (E_MSG_MAX_PACKET_SIZE-PKT_PRE_SECURITY)


#endif	/* _EI_SECURITY_H */
