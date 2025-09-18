/* crypto/hmac/hmac.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_HMAC_H 
#define HEADER_HMAC_H 

#ifdef  __cplusplus
extern "C" {
#endif

#include "evp.h"

#define HMAC_MAX_MD_CBLOCK	64

typedef struct hmac_ctx_st
	{
	EVP_MD *md;
	EVP_MD_CTX md_ctx;
	EVP_MD_CTX i_ctx;
	EVP_MD_CTX o_ctx;
	unsigned int key_length;
	unsigned char key[HMAC_MAX_MD_CBLOCK];
	} HMAC_CTX;

#define HMAC_size(e)	(EVP_MD_size((e)->md))

#ifndef NOPROTO

void HMAC_Init(HMAC_CTX *ctx, unsigned char *key, int len,
	EVP_MD *md);
void HMAC_Update(HMAC_CTX *ctx,unsigned char *key, int len);
void HMAC_Final(HMAC_CTX *ctx, unsigned char *md, unsigned int *len);
void HMAC_cleanup(HMAC_CTX *ctx);
unsigned char *HMAC(EVP_MD *evp_md, unsigned char *key, int key_len,
	unsigned char *d, int n, unsigned char *md, unsigned int *md_len);


#else

void HMAC_Init();
void HMAC_Update();
void HMAC_Final();
void HMAC_cleanup();
unsigned char *HMAC();

#endif

#ifdef  __cplusplus
}
#endif

#endif

