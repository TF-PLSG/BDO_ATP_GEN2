/* crypto/rc2/rc2.org */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

/* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
 *
 * Always modify rc2.org since rc2.h is automatically generated from
 * it during SSLeay configuration.
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 */

#ifndef HEADER_RC2_H
#define HEADER_RC2_H

#ifdef  __cplusplus
extern "C" {
#endif

#define RC2_ENCRYPT	1
#define RC2_DECRYPT	0

/* I need to put in a mod for the alpha - eay */
#define RC2_INT unsigned int

typedef struct rc2_key_st
        {
        /* We need to load bytes into the ints, little-endian */
        unsigned int data[64];
        } RC2_KEY;

#define RC2_BLOCK	8
#define RC2_KEY_LENGTH	16

#ifndef NOPROTO
 
void RC2_set_key(RC2_KEY *key,int len,unsigned char *data,int bits);
void RC2_encrypt(unsigned long *d,RC2_KEY *key);
void RC2_decrypt(unsigned long *d,RC2_KEY *key);
void RC2_ecb_encrypt(unsigned char *in,unsigned char *out,RC2_KEY *key,
	int enc);
void RC2_cbc_encrypt(unsigned char *in, unsigned char *out, long length,
	RC2_KEY *ks, unsigned char *iv, int enc);
void RC2_cfb64_encrypt(unsigned char *in, unsigned char *out, long length,
	RC2_KEY *schedule, unsigned char *ivec, int *num, int enc);
void RC2_ofb64_encrypt(unsigned char *in, unsigned char *out, long length,
	RC2_KEY *schedule, unsigned char *ivec, int *num);

#else

void RC2_set_key();
void RC2_encrypt();
void RC2_decrypt();
void RC2_ecb_encrypt();
void RC2_cbc_encrypt();
void RC2_cfb64_encrypt();
void RC2_ofb64_encrypt();

#endif

#ifdef  __cplusplus
}
#endif

#endif
