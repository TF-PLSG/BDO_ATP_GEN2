/* crypto/dsa/dsa.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_DSA_H
#define HEADER_DSA_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "bn.h"

#define DSA_FLAG_CACHE_MONT_P	0x01

typedef struct dsa_st
	{
        /* This first variable is used to pick up errors where
         * a DSA is passed instead of of a EVP_PKEY */
        int pad;
        int version;
        int write_params;
        BIGNUM *p;
        BIGNUM *q;      /* == 20 */
        BIGNUM *g;
       
        BIGNUM *pub_key;  /* y public key */
        BIGNUM *priv_key; /* x private key */
       
        BIGNUM *kinv;   /* Signing pre-calc */
        BIGNUM *r;      /* Signing pre-calc */
       
        int flags;
        /* Normally used to cache montgomery values */
        char *method_mont_p;
	int references;
	} DSA;

#define DSAparams_dup(x) (DSA *)ASN1_dup((int (*)())i2d_DSAparams, \
		(char *(*)())d2i_DSAparams,(char *)(x))
#define d2i_DSAparams_fp(fp,x) (DSA *)ASN1_d2i_fp((char *(*)())DSA_new, \
		(char *(*)())d2i_DSAparams,(fp),(unsigned char **)(x))
#define i2d_DSAparams_fp(fp,x) ASN1_i2d_fp(i2d_DSAparams,(fp), \
		(unsigned char *)(x))
#define d2i_DSAparams_bio(bp,x) (DSA *)ASN1_d2i_bio((char *(*)())DSA_new, \
		(char *(*)())d2i_DSAparams,(bp),(unsigned char **)(x))
#define i2d_DSAparams_bio(bp,x) ASN1_i2d_bio(i2d_DSAparams,(bp), \
		(unsigned char *)(x))

#ifndef NOPROTO

DSA *	DSA_new(void);
int	DSA_size(DSA *);
	/* next 4 return -1 on error */
int	DSA_sign_setup( DSA *dsa,BN_CTX *ctx_in,BIGNUM **kinvp,BIGNUM **rp);
int	DSA_sign(int type,unsigned char *dgst,int dlen,
		unsigned char *sig, unsigned int *siglen, DSA *dsa);
int	DSA_verify(int type,unsigned char *dgst,int dgst_len,
		unsigned char *sigbuf, int siglen, DSA *dsa);
void	DSA_free (DSA *r);

void	ERR_load_DSA_strings(void );

DSA *	d2i_DSAPublicKey(DSA **a, unsigned char **pp, long length);
DSA *	d2i_DSAPrivateKey(DSA **a, unsigned char **pp, long length);
DSA * 	d2i_DSAparams(DSA **a, unsigned char **pp, long length);
DSA *	DSA_generate_parameters(int bits, unsigned char *seed,int seed_len,
		int *counter_ret, unsigned long *h_ret,void
		(*callback)(),char *cb_arg);
int	DSA_generate_key(DSA *a);
int	i2d_DSAPublicKey(DSA *a, unsigned char **pp);
int 	i2d_DSAPrivateKey(DSA *a, unsigned char **pp);
int	i2d_DSAparams(DSA *a,unsigned char **pp);

#ifdef HEADER_BIO_H
int	DSAparams_print(BIO *bp, DSA *x);
int	DSA_print(BIO *bp, DSA *x, int off);
#endif
#ifndef NO_FP_API
int	DSAparams_print_fp(FILE *fp, DSA *x);
int	DSA_print_fp(FILE *bp, DSA *x, int off);
#endif

int DSA_is_prime(BIGNUM *q,void (*callback)(),char *cb_arg);

#else

DSA *	DSA_new();
int	DSA_size();
int	DSA_sign_setup();
int	DSA_sign();
int	DSA_verify();
void	DSA_free ();

void	ERR_load_DSA_strings();

DSA *	d2i_DSAPublicKey();
DSA *	d2i_DSAPrivateKey();
DSA * 	d2i_DSAparams();
DSA *	DSA_generate_parameters();
int	DSA_generate_key();
int	i2d_DSAPublicKey();
int 	i2d_DSAPrivateKey();
int	i2d_DSAparams();

int	DSA_is_prime();

int	DSAparams_print();
int	DSA_print();

#ifndef NO_FP_API
int	DSAparams_print_fp();
int	DSA_print_fp();
#endif

#endif

/* BEGIN ERROR CODES */
/* Error codes for the DSA functions. */

/* Function codes. */
#define DSA_F_DSAPARAMS_PRINT				 100
#define DSA_F_DSAPARAMS_PRINT_FP			 101
#define DSA_F_DSA_IS_PRIME				 102
#define DSA_F_DSA_PRINT					 103
#define DSA_F_DSA_PRINT_FP				 104
#define DSA_F_DSA_SIGN					 105
#define DSA_F_DSA_SIGN_SETUP				 106
#define DSA_F_DSA_VERIFY				 107

/* Reason codes. */
#define DSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE		 100
 
#ifdef  __cplusplus
}
#endif
#endif

