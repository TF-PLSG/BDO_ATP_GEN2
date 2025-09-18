/* crypto/md/md2.org */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

/* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *
 * Always modify md2.org since md2.h is automatically generated from 
 * it during SSLeay configuration.
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 */


#ifndef HEADER_MD2_H
#define HEADER_MD2_H

#ifdef  __cplusplus
extern "C" {
#endif

#define MD2_DIGEST_LENGTH	16
#define MD2_BLOCK       	16

#define MD2_INT unsigned int

typedef struct MD2state_st
	{
	int num;
	unsigned char data[MD2_BLOCK];
	MD2_INT cksm[MD2_BLOCK];
	MD2_INT state[MD2_BLOCK];
	} MD2_CTX;

#ifndef NOPROTO
char *MD2_options(void);
void MD2_Init(MD2_CTX *c);
void MD2_Update(MD2_CTX *c, register unsigned char *data, unsigned long len);
void MD2_Final(unsigned char *md, MD2_CTX *c);
unsigned char *MD2(unsigned char *d, unsigned long n,unsigned char *md);
#else
char *MD2_options();
void MD2_Init();
void MD2_Update();
void MD2_Final();
unsigned char *MD2();
#endif

#ifdef  __cplusplus
}
#endif

#endif
