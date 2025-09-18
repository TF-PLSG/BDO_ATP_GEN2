/* crypto/dh/dh.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_DH_H
#define HEADER_DH_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef HEADER_BN_H
#define BIGNUM 		char
#endif

#define DH_FLAG_CACHE_MONT_P	0x01

typedef struct dh_st
	{
	/* This first argument is used to pick up errors when
	 * a DH is passed instead of a EVP_PKEY */
	int pad;
	int version;
	BIGNUM *p;
	BIGNUM *g;
	int length; /* optional */
	BIGNUM *pub_key;	/* y */
	BIGNUM *priv_key;	/* x */

	int flags;
	char *method_mont_p;
	} DH;

#define DH_GENERATOR_2		2
/* #define DH_GENERATOR_3	3 */
#define DH_GENERATOR_5		5

/* DH_check error codes */
#define DH_CHECK_P_NOT_PRIME		0x01
#define DH_CHECK_P_NOT_STRONG_PRIME	0x02
#define DH_UNABLE_TO_CHECK_GENERATOR	0x04
#define DH_NOT_SUITABLE_GENERATOR	0x08

#define DHparams_dup(x) (DH *)ASN1_dup((int (*)())i2d_DHparams, \
		(char *(*)())d2i_DHparams,(char *)(x))
#define d2i_DHparams_fp(fp,x) (DH *)ASN1_d2i_fp((char *(*)())DH_new, \
		(char *(*)())d2i_DHparams,(fp),(unsigned char **)(x))
#define i2d_DHparams_fp(fp,x) ASN1_i2d_fp(i2d_DHparams,(fp), \
		(unsigned char *)(x))
#define d2i_DHparams_bio(bp,x) (DH *)ASN1_d2i_bio((char *(*)())DH_new, \
		(char *(*)())d2i_DHparams,(bp),(unsigned char **)(x))
#define i2d_DHparams_bio(bp,x) ASN1_i2d_bio(i2d_DHparams,(bp), \
		(unsigned char *)(x))

#ifndef NOPROTO
DH *	DH_new(void);
void	DH_free(DH *dh);
int	DH_size(DH *dh);
DH *	DH_generate_parameters(int prime_len,int generator,
		void (*callback)(int,int,char *),char *cb_arg);
int	DH_check(DH *dh,int *codes);
int	DH_generate_key(DH *dh);
int	DH_compute_key(unsigned char *key,BIGNUM *pub_key,DH *dh);
DH *	d2i_DHparams(DH **a,unsigned char **pp, long length);
int	i2d_DHparams(DH *a,unsigned char **pp);
#ifndef NO_FP_API
int	DHparams_print_fp(FILE *fp, DH *x);
#endif
#ifdef HEADER_BIO_H
int	DHparams_print(BIO *bp, DH *x);
#else
int	DHparams_print(char *bp, DH *x);
#endif
void	ERR_load_DH_strings(void );

#else

DH *	DH_new();
void	DH_free();
int	DH_size();
DH *	DH_generate_parameters();
int	DH_check();
int	DH_generate_key();
int	DH_compute_key();
DH *	d2i_DHparams();
int	i2d_DHparams();
#ifndef NO_FP_API
int	DHparams_print_fp();
#endif
int	DHparams_print();
void	ERR_load_DH_strings();

#endif

/* BEGIN ERROR CODES */
/* Error codes for the DH functions. */

/* Function codes. */
#define DH_F_DHPARAMS_PRINT				 100
#define DH_F_DHPARAMS_PRINT_FP				 101
#define DH_F_DH_COMPUTE_KEY				 102
#define DH_F_DH_GENERATE_KEY				 103
#define DH_F_DH_GENERATE_PARAMETERS			 104
#define DH_F_DH_NEW					 105

/* Reason codes. */
#define DH_R_NO_PRIVATE_VALUE				 100
 
#ifdef  __cplusplus
}
#endif
#endif

