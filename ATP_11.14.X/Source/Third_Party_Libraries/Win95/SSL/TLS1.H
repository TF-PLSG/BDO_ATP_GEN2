/* ssl/tls1.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_TLS1_H 
#define HEADER_TLS1_H 

#ifdef  __cplusplus
extern "C" {
#endif

#define TLS1_VERSION			0x0301
#define TLS1_VERSION_MAJOR		0x03
#define TLS1_VERSION_MINOR		0x01

#define TLS1_AD_DECRYPTION_FAILED	21
#define TLS1_AD_RECORD_OVERFLOW		22
#define TLS1_AD_UNKNOWN_CA		48	/* fatal */
#define TLS1_AD_ACCESS_DENIED		49	/* fatal */
#define TLS1_AD_DECODE_ERROR		50	/* fatal */
#define TLS1_AD_DECRYPT_ERROR		51
#define TLS1_AD_EXPORT_RESTRICION	60	/* fatal */
#define TLS1_AD_PROTOCOL_VERSION	70	/* fatal */
#define TLS1_AD_INSUFFICIENT_SECURITY	71	/* fatal */
#define TLS1_AD_INTERNAL_ERROR		80	/* fatal */
#define TLS1_AD_USER_CANCLED		90
#define TLS1_AD_NO_RENEGOTIATION	100

#define TLS_CT_RSA_SIGN			1
#define TLS_CT_DSS_SIGN			2
#define TLS_CT_RSA_FIXED_DH		3
#define TLS_CT_DSS_FIXED_DH		4
#define TLS_CT_NUMBER			4

#define TLS1_FINISH_MAC_LENGTH		12

#define TLS_MD_MAX_CONST_SIZE			20
#define TLS_MD_CLIENT_FINISH_CONST		"client finished"
#define TLS_MD_CLIENT_FINISH_CONST_SIZE		15
#define TLS_MD_SERVER_FINISH_CONST		"server finished"
#define TLS_MD_SERVER_FINISH_CONST_SIZE		15
#define TLS_MD_SERVER_WRITE_KEY_CONST		"server write key"
#define TLS_MD_SERVER_WRITE_KEY_CONST_SIZE	16
#define TLS_MD_KEY_EXPANSION_CONST		"key expansion"
#define TLS_MD_KEY_EXPANSION_CONST_SIZE		13
#define TLS_MD_CLIENT_WRITE_KEY_CONST		"client write key"
#define TLS_MD_CLIENT_WRITE_KEY_CONST_SIZE	16
#define TLS_MD_SERVER_WRITE_KEY_CONST		"server write key"
#define TLS_MD_SERVER_WRITE_KEY_CONST_SIZE	16
#define TLS_MD_IV_BLOCK_CONST			"IV block"
#define TLS_MD_IV_BLOCK_CONST_SIZE		8
#define TLS_MD_MASTER_SECRET_CONST		"master secret"
#define TLS_MD_MASTER_SECRET_CONST_SIZE		13

#ifdef  __cplusplus
}
#endif
#endif

