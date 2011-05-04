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

/*
 * ei_security.h   Copyright 2000-2003 Embrace Networks Inc.
 */

#include "idk_types.h"
//#include "ei_packet.h"

/**
 * @name ei_security
 * @memo Implements the security layer of the Embrace Device Protocol.
 */

/*@{*/

/**
 * To build with full security features enabled, the defined value of
 * SECURITY_ENABLED must be 1. To build the code with minimal security,
 * change the defined value of SECURITY_ENABLED to 0.
 */
#if EN_OSTYPE == 4 // for WinCE right now...
#define SECURITY_ENABLED 0
#else

#ifdef EDP_CRYPTO_SECURITY_ENABLED
#define SECURITY_ENABLED 1
#else /* EDP_CRYPTO_SECURITY_ENABLED */
#define SECURITY_ENABLED 0
#endif /* EDP_CRYPTO_SECURITY_ENABLED */

#endif

/* Identity verification form codes... */
#define SECURITY_IDENT_FORM_SIMPLE   0x00 /* simple verification */
#define SECURITY_IDENT_FORM_CRYPTO   0x01 /* cryptographic verification */
#define SECURITY_IDENT_FORM_PASSWORD 0x02 /* simple+passwd */

/**
 * The firmware is built assuming one of the above identification verification
 * forms. Define SECURITY_IDENT_FORM (below) to the selected default.
 *
 * Note that if cryptographic identity verification is selected, both
 * cryptographic and simple identity verification forms are supported via a
 * selection in the device configuration information. But if the code is
 * built for simple identity verification, only the simple form is possible.
 */
#if SECURITY_ENABLED
#define SECURITY_IDENT_FORM          SECURITY_IDENT_FORM_CRYPTO
#else /* ! SECURITY_ENABLED */
/* #define SECURITY_IDENT_FORM          SECURITY_IDENT_FORM_SIMPLE */
/* #define SECURITY_IDENT_FORM          SECURITY_IDENT_FORM_PASSWORD */
#define SECURITY_IDENT_FORM          SECURITY_IDENT_FORM_PASSWORD
#endif /* ? SECURITY_ENABLED */

/**
 * The enabled security coding scheme (encryption type) defines...
 *
 * A defined value of 0 (zero) disables the corresponding security protocol
 * coding scheme. A defined value 1 (one) enables that coding scheme.
 * Basic (null) encryption cannot be disabled, because the security layer
 * is a mandatory layer of the Embrace Device Protocol.
 *
 * It may be desirable to disable unused security protocol coding schemes
 * to reduce code size and needed data space.
 */
#if SECURITY_ENABLED
#if SECURITY_IDENT_FORM == SECURITY_IDENT_FORM_CRYPTO
#define SECURITY_ENABLED_AESMD5      1
#define SECURITY_ENABLED_NONEMD5     1
#else /* SECURITY_IDENT_FORM != SECURITY_IDENT_FORM_CRYPTO */
#define SECURITY_ENABLED_AESMD5      0
#define SECURITY_ENABLED_NONEMD5     0
#endif /* SECURITY_IDENT_FORM == SECURITY_IDENT_FORM_CRYPTO */
#define SECURITY_ENABLED_RC4MD5      0 /* RC4/MD5 is no longer supported */
#else /* ! SECURITY_ENABLED */
/* Security is disabled in general: disable all coding schemes except null. */
#define SECURITY_ENABLED_AESMD5      0
#define SECURITY_ENABLED_NONEMD5     0
#define SECURITY_ENABLED_RC4MD5      0 /* RC4/MD5 is no longer supported */
#endif /* ? SECURITY_ENABLED */

/* The data security coding schemes (a.k.a. encryption types)... */
#define SECURITY_PROTO_NONE          0x00 /* no encryption, no authentication */
#define SECURITY_PROTO_AESMD5        0x01 /* AES encryption, MD5 authentication */
#define SECURITY_PROTO_NONEMD5       0x02 /* no encryption, MD5 authentication */
/* The following coding scheme is no longer supported. (4 Sep 2001) */
#define SECURITY_PROTO_RC4MD5        0x01 /* RC4 encryption, MD5 authentication */

#if SECURITY_ENABLED
#if SECURITY_ENABLED_AESMD5
#define SECURITY_PROTO_ENCRYPTED     SECURITY_PROTO_AESMD5
#elif SECURITY_ENABLED_NONEMD5
#define SECURITY_PROTO_ENCRYPTED     SECURITY_PROTO_NONEMD5
#elif SECURITY_ENABLED_RC4MD5 /* RC4/MD5 is no longer supported */
#define SECURITY_PROTO_ENCRYPTED     SECURITY_PROTO_RC4MD5
#else
#define SECURITY_PROTO_ENCRYPTED     SECURITY_PROTO_NONE
#endif
#else /* ! SECURITY_ENABLED */
#define SECURITY_PROTO_ENCRYPTED     SECURITY_PROTO_NONE
#endif /* ? SECURITY_ENABLED */

#define SECURITY_PROTO_DEFAULT       SECURITY_PROTO_ENCRYPTED
#define SECURITY_PROTO_USE_DEFAULT   0xff
#define SECURITY_PROTO_MINID         0x00
#define SECURITY_PROTO_MAXID         0x6f

/* The security status message codes... */
#define SECURITY_STATUS_CORRUPT_MSG  0x70 /* corrupted message */
#define SECURITY_STATUS_UNSUPPORTED  0x71 /* unsupported coding scheme */
#define SECURITY_STATUS_WRONG_CS     0x72 /* inappropriate coding scheme */
#define SECURITY_STATUS_MINID        0x70
#define SECURITY_STATUS_MAXID        0x7f

/* The default encryptions for the upper layers and facilities... */
#define SECURITY_PROTO_DISCOVERY     SECURITY_PROTO_ENCRYPTED /* Discovery layer */
#define SECURITY_PROTO_FAC_PIO       SECURITY_PROTO_ENCRYPTED /* PIO */
#define SECURITY_PROTO_FAC_MP        SECURITY_PROTO_NONE      /* Message Passing */
#define SECURITY_PROTO_FAC_TS        SECURITY_PROTO_ENCRYPTED /* Transparent Serial */
#define SECURITY_PROTO_FAC_FS        SECURITY_PROTO_ENCRYPTED /* File System */
#define SECURITY_PROTO_FAC_LOG       SECURITY_PROTO_ENCRYPTED /* Logging */
#define SECURITY_PROTO_FAC_FU        SECURITY_PROTO_ENCRYPTED /* Firmware Update */
#define SECURITY_PROTO_FAC_MB        SECURITY_PROTO_ENCRYPTED /* Mini-Browser */
#define SECURITY_PROTO_FAC_MON       SECURITY_PROTO_ENCRYPTED /* Monitoring */
#define SECURITY_PROTO_FAC_MSG       SECURITY_PROTO_ENCRYPTED /* Messaging */
#define SECURITY_PROTO_FAC_RCI       SECURITY_PROTO_ENCRYPTED /* Remote Command Interface */
#define SECURITY_PROTO_FAC_LOOP      SECURITY_PROTO_ENCRYPTED /* Loopback */
#define SECURITY_PROTO_FAC_CC        SECURITY_PROTO_ENCRYPTED /* Connection Control */

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

/* Identity verification state defines... */
#define SECURITY_IDVER_STATE_NONE        0x00
#define SECURITY_IDVER_STATE_WAIT_DEVID  0x10 /* server-initiated connections */
#define SECURITY_IDVER_STATE_WAIT_URL    0x20 /* server-initiated connections */
#define SECURITY_IDVER_STATE_CHAL_SENT   0x30
#define SECURITY_IDVER_STATE_RESP_RCVD   0x40
#define SECURITY_IDVER_STATE_FINAL_SENT  0x50
#define SECURITY_IDVER_STATE_WAIT_URLVER 0x60 /* server-initiated connections */
#define SECURITY_IDVER_STATE_SUCCESS     0x80
#define SECURITY_IDVER_STATE_FAIL        0xff

/*
 * Security configuration file access defines...
 *
 * Security-related configuration files are:
 *
 *   sec-ver - security version (unused, reserved for future use).
 *
 *   sec-opts - security options: contains these fields in this order:
 *      IVF    Identity Verification Form (1 octet).
 *             Acceptable forms are:
 *                SECURITY_IDENT_FORM_SIMPLE  simple verification
 *                SECURITY_IDENT_FORM_CRYPTO  cryptographic verification
 *                SECURITY_IDENT_FORM_PASSWORD  simple verification with password
 *      DFCS   Discovery/Facility Coding Scheme (1 octet).
 *             Acceptable coding schemes are:
 *                SECURITY_PROTO_NONE     no encryption, no authentication
 *                SECURITY_PROTO_AESMD5   AES encryption, MD5 authentication
 *                SECURITY_PROTO_NONEMD5  no encryption, MD5 authentication
 *      MPCS   Message Passing Coding Scheme (1 octet).
 *             Acceptable coding schemes are the same as for DFCS.
 *
 *   sec-keys - contains these fields in this order:
 *      KEK    Key Encrypting Key (32 octets)
 *      KGK    Key Generating Key (32 octets)
 *      seed   Initial PRNG Seed (16 octets)
 */

/*
 * "sec-opts" file items: length (octets), offset and total size...
 */
#define SECURITY_SECOPTS_IVF_LEN     1 /* length of IVF in "sec-opts" */
#define SECURITY_SECOPTS_DFCS_LEN    1 /* length of DFCS in "sec-opts" */
#define SECURITY_SECOPTS_MPCS_LEN    1 /* length of MPCS in "sec-opts" */
#define SECURITY_SECOPTS_IVF_OFF     0 /* "sec-opts" item offsets follow... */
#define SECURITY_SECOPTS_DFCS_OFF   (SECURITY_SECOPTS_IVF_OFF +  \
                                     SECURITY_SECOPTS_IVF_LEN)
#define SECURITY_SECOPTS_MPCS_OFF   (SECURITY_SECOPTS_DFCS_OFF + \
                                     SECURITY_SECOPTS_DFCS_LEN)
#define SECURITY_SECOPTS_SIZE       (SECURITY_SECOPTS_IVF_LEN +  \
                                     SECURITY_SECOPTS_DFCS_LEN + \
                                     SECURITY_SECOPTS_MPCS_LEN)
/* Used internally by security's "get coding scheme" functions... */
#define SECURITY_SECOPTS_TYPE_DFCS   0 /* access the DFCS item */
#define SECURITY_SECOPTS_TYPE_MPCS   1 /* access the MPCS item */
/* Default values for secopts fields if missing or invalid... */
/* XXX - perhaps these should be dependent upon whether security is enabled. */
#define SECURITY_SECOPTS_IVF_DFLT    SECURITY_IDENT_FORM_CRYPTO
#define SECURITY_SECOPTS_DFCS_DFLT   SECURITY_PROTO_AESMD5
#define SECURITY_SECOPTS_MPCS_DFLT   SECURITY_PROTO_AESMD5

/*
 * "sec-keys" file items: length (octets), offset and total size...
 */
#define SECURITY_KEYS_KEK_LEN       32 /* length of KEK in "sec-keys" */
#define SECURITY_KEYS_KGK_LEN       32 /* length of KGK in "sec-keys" */
#define SECURITY_KEYS_SEED_LEN      16 /* length of PRNG seed in "sec-keys" */
#define SECURITY_KEYS_KEK_OFF        0 /* "sec-keys" item offsets follow... */
#define SECURITY_KEYS_KGK_OFF       (SECURITY_KEYS_KEK_OFF + \
                                     SECURITY_KEYS_KEK_LEN)
#define SECURITY_KEYS_SEED_OFF      (SECURITY_KEYS_KGK_OFF + \
                                     SECURITY_KEYS_KGK_LEN)
#define SECURITY_KEYS_SIZE          (SECURITY_KEYS_KEK_LEN + \
                                     SECURITY_KEYS_KGK_LEN + \
                                     SECURITY_KEYS_SEED_LEN)

#if 0

/**
 * Protocol-specific structure definitions.
 */
struct e_security_none_cxn {
	/* Nothing for now */
	/*char foo;*/
	struct e_security_cxn *sec_cxn; /* pointer to security connection */
};

#if SECURITY_IDENT_FORM == SECURITY_IDENT_FORM_CRYPTO
#include "ei_rijn_api.h"
#endif /* SECURITY_IDENT_FORM == SECURITY_IDENT_FORM_CRYPTO */

#if SECURITY_ENABLED && SECURITY_ENABLED_AESMD5
#include "ei_rijn_api.h"

struct e_security_aesmd5_cxn {
	struct e_security_cxn *sec_cxn; /* pointer to security connection */
};
#endif /* SECURITY_ENABLED && SECURITY_ENABLED_AESMD5 */

#if SECURITY_ENABLED && SECURITY_ENABLED_NONEMD5
#include "ei_rijn_api.h"

struct e_security_nonemd5_cxn {
	struct e_security_cxn *sec_cxn; /* pointer to security connection */
};
#endif /* SECURITY_ENABLED && SECURITY_ENABLED_NONEMD5 */

#if SECURITY_ENABLED && SECURITY_ENABLED_RC4MD5 /* RC4/MD5 is no longer supported */
#include "ei_rc4.h"

struct e_security_rc4md5_cxn {
	rc4_key recv_key;
	rc4_key send_key;
};
#endif /* SECURITY_ENABLED && SECURITY_ENABLED_RC4MD5 */

struct e_security_cxn_proto {
	union {
		struct e_security_none_cxn none;
#if SECURITY_ENABLED
#if SECURITY_ENABLED_AESMD5
		struct e_security_aesmd5_cxn aesmd5;
#endif /* ? SECURITY_ENABLED_AESMD5 */
#if SECURITY_ENABLED_NONEMD5
		struct e_security_nonemd5_cxn nonemd5;
#endif /* ? SECURITY_ENABLED_NONEMD5 */
#if SECURITY_ENABLED_RC4MD5 /* RC4/MD5 is no longer supported */
		struct e_security_rc4md5_cxn rc4md5;
#endif /* ? SECURITY_ENABLED_RC4MD5 */
#endif /* ? SECURITY_ENABLED */
	} u;
};

/**
 * Protocol-general structure definitions.
 */
struct e_security_proto_ops {
	uint8_t sec_coding;
	int (*init)(struct e_security_cxn_proto *, struct e_security_cxn *);
	int (*encrypt)(struct e_security_cxn_proto *, struct e_packet *,
			struct e_packet *);
	int (*decrypt)(struct e_security_cxn_proto *, struct e_packet *,
			struct e_packet *);
	int (*close)(struct e_security_cxn_proto *);
};

struct e_security_proto_ent {
	struct e_security_proto_ops *ops;
	struct e_security_cxn_proto *proto;
	struct e_security_proto_ent *next;
	uint8_t sec_coding;
	uint8_t initialized;
};

struct e_security_cxn {
	struct e_security_proto_ent *pe;
	struct e_packet_callback *layer_down;
	struct e_packet_callback *layer_up;
	struct e_security_proto_ent pe_none;
	struct e_security_none_cxn cxn_none;
	uint8_t default_sec_coding;
	uint8_t idver_state;                 /* state of identity verification process */
	uint8_t is_cic;                      /* is this a client-initiated connection? */
	uint8_t disable_security;            /* use minimal security (DCT communication) */
#if SECURITY_IDENT_FORM == SECURITY_IDENT_FORM_CRYPTO
	/*
	 * We need these data objects only if we're using cryptographic identity
	 * verification (challenge-response).
	 * They must be initialized early in the general security initialization.
	 */
	uint8_t key_KEK[EN_AES_KEY_LEN];     /* key-encrypting key for challenge-response */
	uint8_t key_session[EN_AES_KEY_LEN]; /* negotiated session key */
	cipherInstance encr_cipher;          /* encryption cipher for challenge-response */
	keyInstance encr_key;                /* encryption key for challenge-response */
	uint8_t RNa[EN_AES_KEY_LEN];         /* RNa to produce session key */
	uint8_t key_size;                    /* key size in octets: 32, 24 or 16 */
	/*
	 * A set of common cipher and key instances are implemented for AES data
	 * encryption and decryption. These are used for all security coding
	 * schemes that involve AES in CBC mode. These are also used for the URL
	 * Verifier validation that occurs as part of the cryptographic identity
	 * verification process at the start of an Embrace Device Protocol session.
	 * The URL Verifier message was added in EDP version 0x101.
	 */
	cipherInstance aes_encr_cipher; /* common AES data encryption cipher instance */
	cipherInstance aes_decr_cipher; /* common AES data decryption cipher instance */
	keyInstance aes_encr_key;       /* common AES data encryption key instance */
	keyInstance aes_decr_key;       /* common AES data decryption key instance */
	uint8_t aes_encr_key_scheduled; /* has the common AES encryption key been scheduled? */
	uint8_t aes_decr_key_scheduled; /* has the common AES decryption key been scheduled? */
#endif /* SECURITY_IDENT_FORM == SECURITY_IDENT_FORM_CRYPTO */
};

#if SECURITY_PROTOTYPE_STATICS

/**
 * Initialize the null security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param sc pointer to the security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_none_init(struct e_security_cxn_proto *scp,
			struct e_security_cxn *sc);

/**
 * Encrypt a packet using the null security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext packet to be encrypted
 * @param crypt resulting ciphertext packet
 * @return non-negative for success, negative for failure.
 */
static int e_security_none_encrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Decrypt a packet using the null security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext resulting plaintext
 * @param crypt ciphertext to be decrypted
 * @return non-negative for success, negative for failure.
 */
static int e_security_none_decrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Close a null security layer connection.
 * @param scp pointer to the protocol-specific part of a security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_none_close(struct e_security_cxn_proto *scp);

#if SECURITY_ENABLED && SECURITY_ENABLED_AESMD5

/**
 * Initialize the AES-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param sc pointer to the security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_aesmd5_init(struct e_security_cxn_proto *scp,
			struct e_security_cxn *sc);

/**
 * Encrypt a packet using the AES-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext packet to be encrypted
 * @param crypt resulting ciphertext packet
 * @return non-negative for success, negative for failure.
 */
static int e_security_aesmd5_encrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Decrypt a packet using the AES-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext resulting plaintext
 * @param crypt ciphertext to be decrypted
 * @return non-negative for success, negative for failure.
 */
static int e_security_aesmd5_decrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Close a AES-MD5 security layer connection.
 * @param scp pointer to the protocol-specific part of a security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_aesmd5_close(struct e_security_cxn_proto *scp);

#endif /* SECURITY_ENABLED && SECURITY_ENABLED_AESMD5 */

#if SECURITY_ENABLED && SECURITY_ENABLED_NONEMD5

/**
 * Initialize the none-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param sc pointer to the security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_nonemd5_init(struct e_security_cxn_proto *scp,
			struct e_security_cxn *sc);

/**
 * Encrypt a packet using the none-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext packet to be encrypted
 * @param crypt resulting ciphertext packet
 * @return non-negative for success, negative for failure.
 */
static int e_security_nonemd5_encrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Decrypt a packet using the none-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext resulting plaintext
 * @param crypt ciphertext to be decrypted
 * @return non-negative for success, negative for failure.
 */
static int e_security_nonemd5_decrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Close a none-MD5 security layer connection.
 * @param scp pointer to the protocol-specific part of a security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_nonemd5_close(struct e_security_cxn_proto *scp);

#endif /* SECURITY_ENABLED && SECURITY_ENABLED_NONEMD5 */

#if SECURITY_ENABLED && SECURITY_ENABLED_RC4MD5 /* RC4/MD5 is no longer supported */

/**
 * Initialize the RC4-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param sc pointer to the security connection
 * @return non-negative for success, negative for failure.
 */
int e_security_rc4md5_init(struct e_security_cxn_proto *scp,
			struct e_security_cxn *sc);

/**
 * Encrypt a packet using the RC4-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext packet to be encrypted
 * @param crypt resulting ciphertext packet
 * @return non-negative for success, negative for failure.
 */
int e_security_rc4md5_encrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Decrypt a packet using the RC4-MD5 security layer.
 * @param scp pointer to the protocol-specific part of a security connection
 * @param plaintext resulting plaintext
 * @param crypt ciphertext to be decrypted
 * @return non-negative for success, negative for failure.
 */
int e_security_rc4md5_decrypt(struct e_security_cxn_proto *scp,
			struct e_packet *plaintext,
			struct e_packet *crypt);

/**
 * Close a RC4-MD5 security layer connection.
 * @param scp pointer to the protocol-specific part of a security connection
 * @return non-negative for success, negative for failure.
 */
int e_security_rc4md5_close(struct e_security_cxn_proto *scp);

#endif /* SECURITY_ENABLED && SECURITY_ENABLED_RC4MD5 */

#endif /* SECURITY_PROTOTYPE_STATICS */

#if MLC_NOT_USED
/**
 * Initialize a security layer.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @param layer_down packet callback for the underlying layer
 * @return non-negative for success, negative for failure.
 */
int e_security_init(struct e_security_cxn *sc, struct e_packet_callback *layer_down);

/**
 * Perform identity verification.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @return positive (1) for complete, 0 for pending, negative for failure.
 *
 * Simple identity verification is complete when the client device sends its
 * security messages. Cryptographic identity verification is initiated by
 * this function but not completed, as it involves a challenge-response
 * interaction with a server. For this reason, the "pending" return code
 * is provided, to indicate that verification has been started but is not
 * yet complete.
 */
int e_security_identity_verify(struct e_security_cxn *sc);

/**
 * Initialize a security layer protocol entry.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @param proto security protocol to be used for this entry
 * @param spe pointer to a security protocol entry structure which is to be
 *           used for this protocol and this connection
 * @param scp pointer to the protocol-specific part of a security connection
 * @return non-negative for success, negative for failure.
 */
int e_security_init_proto(struct e_security_cxn *sc,
		int proto,
		struct e_security_proto_ent *spe,
		struct e_security_cxn_proto *scp);

/**
 * Initialize a security layer receive packet_callback structure.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @param pc pointer to a packet callback to be initialized for receive (from
 *           a lower layer) for this connection
 * @return non-negative for success, negative for failure.
 */
void e_security_init_receive(struct e_security_cxn *sc, struct e_packet_callback *pc);

/**
 * Initialize a security layer send packet_callback structure.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @param pc pointer to a packet callback to be initialized for send (from
 *           a higher layer) for this connection
 * @return non-negative for success, negative for failure.
 */
void e_security_init_send(struct e_security_cxn *sc, struct e_packet_callback *pc);

/**
 * Set the security layer default protocol for this connection.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @param proto security protocol to be used as the default
 * @return non-negative for success, negative for failure.
 *
 * For this call to succeed, e_security_init() must have been called and the
 * specified proto must have been initialized via e_security_init_proto().
 */
int e_security_set_default_proto(struct e_security_cxn *sc, int proto);

/**
 * Set the packet callback for the layer above us.
 * @param sc pointer to the security connection
 * @param layer_up packet callback for the layer above
 * @return non-negative for success, negative for failure.
 */
int e_security_set_callback(struct e_security_cxn *sc,
		struct e_packet_callback *layer_up);
#endif

#if SECURITY_PROTOTYPE_STATICS

/**
 * Send a packet, packet callback style.
 * @param p packet to send
 * @param parm pointer to security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_send(struct e_packet *p, void *parm);

/**
 * Receive a packet, packet callback style.
 * @param p packet to receive
 * @param parm pointer to security connection
 * @return non-negative for success, negative for failure.
 */
static int e_security_receive(struct e_packet *p, void *parm);

#endif /* SECURITY_PROTOTYPE_STATICS */

#if MLC_NOT_USED
/**
 * Close a security connection.
 * @param sc pointer to the security connection
 * @return non-negative for success, negative for failure.
 */
int e_security_close(struct e_security_cxn *sc);

/**
 * Close a security layer protocol entry.
 * @param sc pointer to a security connection structure which is to be used
 *           for this connection
 * @param proto security protocol for this entry to be closed
 * @return non-negative for success, negative for failure.
 */
int e_security_close_proto(struct e_security_cxn *sc, int proto);

/**
 * Obtain the configured security identification verification form value.
 * @return one of the SECURITY_IDENT_FORM_xxx values defined in this file.
 */
int e_security_get_ident_ver_form(void);

/**
 * Obtain the current security identification verification state value.
 * @return one of the SECURITY_IDVER_STATE_xxx values defined in this file.
 */
int e_security_get_ident_ver_state(struct e_security_cxn *sc);

/**
 * Obtain the discovery layer security coding scheme.
 * @return one of the SECURITY_PROTO_xxx values defined in this file.
 */
uint8_t e_security_get_discovery_coding_scheme(void);

/**
 * Obtain the security coding scheme for the specified facility.
 * @return one of the SECURITY_PROTO_xxx values defined in this file.
 */
uint8_t e_security_get_facility_coding_scheme(uint16_t facility);

/**
 * Obtain the application (message passing facility) security coding scheme.
 * @return one of the SECURITY_PROTO_xxx values defined in this file.
 * This is the same as calling e_security_get_facility_coding_scheme(E_MUX_FAC_MP_NUM).
 */
uint8_t e_security_get_application_coding_scheme(void);

/**
 * Check whether the coding scheme of a received message should be accepted by
 * the receiving higher layer or facility. This API is called by higher layers
 * and facilities.
 * @param pkt pointer to the packet to be verified
 * @param expected_coding_scheme the expected coding scheme value
 * @return TRUE if the error should be ignored, FALSE otherwise
 */
e_boolean_t e_security_coding_scheme_is_ok(struct e_packet *pkt,
		uint8_t expected_coding_scheme);

/**
 * Report an inappropriate security coding scheme. This API is called by
 * higher layers and facilities to report a problem with the coding scheme
 * of a received message.
 * @param errpkt pointer to the packet that is being rejected
 * @param layer identifier for the rejecting layer or facility
 * @param expected_coding_scheme the expected coding scheme value
 */
void e_security_error_coding_scheme(struct e_packet *errpkt, uint32_t layer,
		uint8_t expected_coding_scheme);

/**
 * Read the security options: identity verification and the
 * security coding schemes.
 * @param opts pointer to a structure of (at least) size SECURITY_SECOPTS_SIZE
 *             to receive the security options data.
 * @return negative is an error of some sort
 *         0 indicates the options were read from the configuration filesystem
 *         1 indicates the options were read from the user override data
 */
int e_security_read_options(uint8_t *opts);

/**
 * Refresh (reread) the security options: identity verification and the
 * security coding schemes. These values may have been cached when they were
 * first obtained. In the event they are changed, this API may be called to
 * force the values to be reread without having to reboot the device.
 */
void e_security_refresh_options(void);

/**
 * Set the destination URL that was used for this connection.
 * This must be called after the connection succeeds but before the
 * security layer is initialized.
 * @param dest pointer to the destination URL
 */
void e_security_set_destination(struct e_destination *dest);

/**
 * Get the destination URL that was used for this connection.
 * This must be called after the security layer is initialized.
 * @param dest pointer to the caller's destination URL buffer
 */
void e_security_get_destination(struct e_destination *dest);

/**
 * Get the password to be used for this connection.
 * This must be called after the security layer is initialized.
 * @param passwd pointer to the caller's password buffer
 */
void e_security_get_password(struct e_password *passwd);

/**
 * Set the password to be used for this connection.
 * This must be called after the connection succeeds but before the
 * security layer is initialized.
 * @param passwd pointer to the password
 */
void e_security_set_password(char *passwd);

/**
 * Create an ephemeral AES key from a 32-octet (256-bit) source key.
 *
 * @param src_key pointer to the source AES key (32 octets)
 * @param dest_key pointer to a buffer to receive the ephemeral key
 * @param dest_key_size size of the key to create, in octets
 *
 * @return the ephemeral key size for success, 0 for failure
 *
 * @doc The dest_key_size value must be one of 16, 24 or 32. The buffer
 * pointed to by dest_key must be large enough to contain the created key.
 * If the value of dest_key_size is greater than the maximum AES encryption
 * key size that the client supports, or is other than one of the valid key
 * sizes (16, 24 or 32), return a failure indication.
 */
int ei_security_create_ephemeral_aes_key(uint8_t *src_key, uint8_t *dest_key,
		int dest_key_size);

/**
 * Is the current EDP session a DCT connection?.
 *
 * @return TRUE if this is a DCT connection, FALSE otherwise
 *
 * @doc This API returns TRUE only for DCT over IP connections, not for
 * DCT over serial connections.
 */
e_boolean_t ei_security_is_dct_connection(void);
#endif

#endif /* not used */
/*@}*/

#endif	/* _EI_SECURITY_H */
