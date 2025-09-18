/* crypto/rc4/rc4.org */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */
/* RC4 - Copyright (c) 1998, Dean Povey (povey@dstc.edu.au). All
 * rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This implementation of RC4 is based on a description given in "Applied
 * Cryptography (2nd Ed.)" by Bruce Schneier and published by John Wiley 
 * & Sons Inc.
 */

/* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *
 * Always modify rc4.org since rc4.h is automatically generated from
 * it during SSLeay configuration.
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
 */

#ifndef HEADER_RC4_H
#define HEADER_RC4_H

#ifdef  __cplusplus
extern "C" {
#endif

/* using int types make the structure larger but make the code faster
 * on most boxes I have tested - up to %20 faster. */
#define RC4_INT unsigned int

/* RC4 uses an 8 x 8 S-Box which is initialised using a permutation of 
 * a variable length key.  rc4_key holds this 256 byte s-box which is 
 * filled in using RC4_init_sbox()
 */
typedef struct _rc4_key {
  RC4_INT s[256];
  RC4_INT i;
  RC4_INT j;
} RC4_KEY;

#ifndef NOPROTO
 
char *RC4_options(void);
void RC4_set_key(RC4_KEY *key, int keylength, unsigned char *keydata);
void RC4(RC4_KEY *key, unsigned long datalen, unsigned char *datain,
		unsigned char *dataout);

#else

char *RC4_options();
void RC4_set_key();
void RC4();

#endif

#ifdef  __cplusplus
}
#endif

#endif

