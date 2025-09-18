/* crypto/rc5/rc5.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_RC5_H
#define HEADER_RC5_H

#ifdef  __cplusplus
extern "C" {
#endif

#define RC5_ENCRYPT	1
#define RC5_DECRYPT	0

/* 32 bit.  For Alpha, things may get weird */
#define RC5_32_INT unsigned long

#define RC5_32_BLOCK		8
#define RC5_32_KEY_LENGTH	16 /* This is a default, max is 255 */

/* This are the only values supported.  Tweak the code if you want more
 * The most supported modes will be
 * RC5-32/12/16
 * RC5-32/16/8
 */
#define RC5_8_ROUNDS	8
#define RC5_12_ROUNDS	12
#define RC5_16_ROUNDS	16

typedef struct rc5_key_st
	{
	/* Number of rounds */
	int rounds;
	RC5_32_INT data[2*(RC5_16_ROUNDS+1)];
	} RC5_32_KEY;

#ifndef NOPROTO
 
void RC5_32_set_key(RC5_32_KEY *key, int len, unsigned char *data,
	int rounds);
void RC5_32_ecb_encrypt(unsigned char *in,unsigned char *out,RC5_32_KEY *key,
	int enc);
void RC5_32_encrypt(unsigned long *data,RC5_32_KEY *key);
void RC5_32_decrypt(unsigned long *data,RC5_32_KEY *key);
void RC5_32_cbc_encrypt(unsigned char *in, unsigned char *out, long length,
	RC5_32_KEY *ks, unsigned char *iv, int enc);
void RC5_32_cfb64_encrypt(unsigned char *in, unsigned char *out, long length,
	RC5_32_KEY *schedule, unsigned char *ivec, int *num, int enc);
void RC5_32_ofb64_encrypt(unsigned char *in, unsigned char *out, long length,
	RC5_32_KEY *schedule, unsigned char *ivec, int *num);

#else

void RC5_32_set_key();
void RC5_32_ecb_encrypt();
void RC5_32_encrypt();
void RC5_32_decrypt();
void RC5_32_cbc_encrypt();
void RC5_32_cfb64_encrypt();
void RC5_32_ofb64_encrypt();

#endif

#ifdef  __cplusplus
}
#endif

#endif
