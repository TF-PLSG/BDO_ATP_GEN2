/* crypto/rsa/rsa.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_RSA_H
#define HEADER_RSA_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "bn.h"
#include "crypto.h"

typedef struct rsa_st RSA_dummy;

/* I've added proto-types because of bugs I had in the number of
 * parameters being passed to the function.  There were picked up by
 * Max Masyutin <max@ritlabs.com>.  eay - 26-Jul-98 */
typedef struct rsa_meth_st
	{
	char *name;
#ifdef NOPROTO
	int (*rsa_pub_enc)();
	int (*rsa_pub_dec)();
	int (*rsa_priv_enc)();
	int (*rsa_priv_dec)();
	int (*rsa_mod_exp)();		/* Can be null */
	int (*bn_mod_exp)();		/* Can be null */
	int (*init)(/* RSA * */);	/* called at new */
	int (*finish)(/* RSA * */);	/* called at free */
#else
	int (*rsa_pub_enc)(int, unsigned char *, unsigned char *,
		RSA_dummy *,int);
	int (*rsa_pub_dec)(int, unsigned char *, unsigned char *,
		RSA_dummy *,int);
	int (*rsa_priv_enc)(int, unsigned char *,unsigned char *,
		RSA_dummy *,int);
	int (*rsa_priv_dec)(int, unsigned char *, unsigned char *,
		RSA_dummy *,int);
	int (*rsa_mod_exp)(BIGNUM *, BIGNUM *, struct rsa_st *);
	int (*bn_mod_exp)(BIGNUM *, BIGNUM *, BIGNUM *, BIGNUM *,
		BN_CTX *, char *);	/* Last parameter is variable */
	int (*init)(RSA_dummy *);	/* called at new */
	int (*finish)(RSA_dummy *);	/* called at free */
#endif

	int flags;			/* RSA_METHOD_FLAG_* things */
	char *app_data;			/* may be needed! */
	} RSA_METHOD;

typedef struct rsa_st
	{
	/* The first parameter is used to pickup errors where
	 * this is passed instead of aEVP_PKEY, it is set to 0 */
	int pad;
	int version;
	RSA_METHOD *meth;
	BIGNUM *n;
	BIGNUM *e;
	BIGNUM *d;
	BIGNUM *p;
	BIGNUM *q;
	BIGNUM *dmp1;
	BIGNUM *dmq1;
	BIGNUM *iqmp;
	/* be carefull using this if the RSA structure is shared */
	CRYPTO_EX_DATA ex_data;
	int references;
	int flags;

	/* Normally used to cache montgomery values */
	char *method_mod_n;
	char *method_mod_p;
	char *method_mod_q;

	/* all BIGNUM values are actually in the following data, if it is not
	 * NULL */
	char *bignum_data;
	} RSA;

#define RSA_3	0x3L
#define RSA_F4	0x10001L

#define RSA_METHOD_FLAG_NO_CHECK	0x01 /* don't check pub/private match */

#define RSA_FLAG_CACHE_PUBLIC		0x02
#define RSA_FLAG_CACHE_PRIVATE		0x04
#define RSA_FLAG_THREAD_SAFE		0x10

#define RSA_PKCS1_PADDING	1
#define RSA_SSLV23_PADDING	2
#define RSA_NO_PADDING		3

#define RSA_set_app_data(s,arg)         RSA_set_ex_data(s,0,(char *)arg)
#define RSA_get_app_data(s)             RSA_get_ex_data(s,0)

#ifndef NOPROTO
RSA *	RSA_new(void);
RSA *	RSA_new_method(RSA_METHOD *method);
int	RSA_size(RSA *);
RSA *	RSA_generate_key(int bits, unsigned long e,void
		(*callback)(int,int,char *),char *cb_arg);
	/* next 4 return -1 on error */
int	RSA_public_encrypt(int flen, unsigned char *from,
		unsigned char *to, RSA *rsa,int padding);
int	RSA_private_encrypt(int flen, unsigned char *from,
		unsigned char *to, RSA *rsa,int padding);
int	RSA_public_decrypt(int flen, unsigned char *from, 
		unsigned char *to, RSA *rsa,int padding);
int	RSA_private_decrypt(int flen, unsigned char *from, 
		unsigned char *to, RSA *rsa,int padding);
void	RSA_free (RSA *r);

int	RSA_flags(RSA *r);

void RSA_set_default_method(RSA_METHOD *meth);

/* This function needs the memory locking malloc callbacks to be installed */
int RSA_memory_lock(RSA *r);

/* If you have RSAref compiled in. */
RSA_METHOD *RSA_PKCS1_RSAref(void);

/* these are the actual SSLeay RSA functions */
RSA_METHOD *RSA_PKCS1_SSLeay(void);

void	ERR_load_RSA_strings(void );

RSA *	d2i_RSAPublicKey(RSA **a, unsigned char **pp, long length);
int	i2d_RSAPublicKey(RSA *a, unsigned char **pp);
RSA *	d2i_RSAPrivateKey(RSA **a, unsigned char **pp, long length);
int 	i2d_RSAPrivateKey(RSA *a, unsigned char **pp);
#ifndef NO_FP_API
int	RSA_print_fp(FILE *fp, RSA *r,int offset);
#endif

#ifdef HEADER_BIO_H
int	RSA_print(BIO *bp, RSA *r,int offset);
#endif

int i2d_Netscape_RSA(RSA *a, unsigned char **pp, int (*cb)());
RSA *d2i_Netscape_RSA(RSA **a, unsigned char **pp, long length, int (*cb)());
/* Naughty internal function required elsewhere, to handle a MS structure
 * that is the same as the netscape one :-) */
RSA *d2i_Netscape_RSA_2(RSA **a, unsigned char **pp, long length, int (*cb)());

/* The following 2 functions sign and verify a X509_SIG ASN1 object
 * inside PKCS#1 padded RSA encryption */
int RSA_sign(int type, unsigned char *m, unsigned int m_len,
	unsigned char *sigret, unsigned int *siglen, RSA *rsa);
int RSA_verify(int type, unsigned char *m, unsigned int m_len,
	unsigned char *sigbuf, unsigned int siglen, RSA *rsa);

/* The following 2 function sign and verify a ASN1_OCTET_STRING
 * object inside PKCS#1 padded RSA encryption */
int RSA_sign_ASN1_OCTET_STRING(int type, unsigned char *m, unsigned int m_len,
	unsigned char *sigret, unsigned int *siglen, RSA *rsa);
int RSA_verify_ASN1_OCTET_STRING(int type, unsigned char *m, unsigned int m_len,
	unsigned char *sigbuf, unsigned int siglen, RSA *rsa);

int RSA_padding_add_PKCS1_type_1(unsigned char *to,int tlen,
	unsigned char *f,int fl);
int RSA_padding_check_PKCS1_type_1(unsigned char *to,int tlen,
	unsigned char *f,int fl,int rsa_len);
int RSA_padding_add_PKCS1_type_2(unsigned char *to,int tlen,
	unsigned char *f,int fl);
int RSA_padding_check_PKCS1_type_2(unsigned char *to,int tlen,
	unsigned char *f,int fl,int rsa_len);
int RSA_padding_add_SSLv23(unsigned char *to,int tlen,
	unsigned char *f,int fl);
int RSA_padding_check_SSLv23(unsigned char *to,int tlen,
	unsigned char *f,int fl,int rsa_len);
int RSA_padding_add_none(unsigned char *to,int tlen,
	unsigned char *f,int fl);
int RSA_padding_check_none(unsigned char *to,int tlen,
	unsigned char *f,int fl,int rsa_len);

int RSA_get_ex_new_index(long argl, char *argp, int (*new_func)(),
	int (*dup_func)(), void (*free_func)());
int RSA_set_ex_data(RSA *r,int idx,char *arg);
char *RSA_get_ex_data(RSA *r, int idx);
RSA *RSA_dup(RSA *rsa);

#else

RSA *	RSA_new();
RSA *	RSA_new_method();
int	RSA_size();
RSA *	RSA_generate_key();
int	RSA_public_encrypt();
int	RSA_private_encrypt();
int	RSA_public_decrypt();
int	RSA_private_decrypt();
void	RSA_free ();

int	RSA_flags();

void RSA_set_default_method();
int RSA_memory_lock();

/* RSA_METHOD *RSA_PKCS1_RSAref(); */
RSA_METHOD *RSA_PKCS1_SSLeay();

void	ERR_load_RSA_strings();

RSA *	d2i_RSAPublicKey();
int	i2d_RSAPublicKey();
RSA *	d2i_RSAPrivateKey();
int 	i2d_RSAPrivateKey();
#ifndef NO_FP_API
int	RSA_print_fp();
#endif

int	RSA_print();

int i2d_Netscape_RSA();
RSA *d2i_Netscape_RSA();
RSA *d2i_Netscape_RSA_2();

int RSA_sign();
int RSA_verify();

int RSA_sign_ASN1_OCTET_STRING();
int RSA_verify_ASN1_OCTET_STRING();

int RSA_padding_add_PKCS1_type_1();
int RSA_padding_check_PKCS1_type_1();
int RSA_padding_add_PKCS1_type_2();
int RSA_padding_check_PKCS1_type_2();
int RSA_padding_add_SSLv23();
int RSA_padding_check_SSLv23();
int RSA_padding_add_none();
int RSA_padding_check_none();

int RSA_get_ex_new_index();
int RSA_set_ex_data();
char *RSA_get_ex_data();
RSA *RSA_dup();

#endif

/* BEGIN ERROR CODES */
/* Error codes for the RSA functions. */

/* Function codes. */
#define RSA_F_RSA_DUP					 121
#define RSA_F_RSA_EAY_PRIVATE_DECRYPT			 100
#define RSA_F_RSA_EAY_PRIVATE_ENCRYPT			 101
#define RSA_F_RSA_EAY_PUBLIC_DECRYPT			 102
#define RSA_F_RSA_EAY_PUBLIC_ENCRYPT			 103
#define RSA_F_RSA_GENERATE_KEY				 104
#define RSA_F_RSA_MEMORY_LOCK				 105
#define RSA_F_RSA_NEW_METHOD				 106
#define RSA_F_RSA_PADDING_ADD_NONE			 107
#define RSA_F_RSA_PADDING_ADD_PKCS1_TYPE_1		 108
#define RSA_F_RSA_PADDING_ADD_PKCS1_TYPE_2		 109
#define RSA_F_RSA_PADDING_ADD_SSLV23			 110
#define RSA_F_RSA_PADDING_CHECK_NONE			 111
#define RSA_F_RSA_PADDING_CHECK_PKCS1_TYPE_1		 112
#define RSA_F_RSA_PADDING_CHECK_PKCS1_TYPE_2		 113
#define RSA_F_RSA_PADDING_CHECK_SSLV23			 114
#define RSA_F_RSA_PRINT					 115
#define RSA_F_RSA_PRINT_FP				 116
#define RSA_F_RSA_SIGN					 117
#define RSA_F_RSA_SIGN_ASN1_OCTET_STRING		 118
#define RSA_F_RSA_VERIFY				 119
#define RSA_F_RSA_VERIFY_ASN1_OCTET_STRING		 120

/* Reason codes. */
#define RSA_R_ALGORITHM_MISMATCH			 100
#define RSA_R_BAD_E_VALUE				 101
#define RSA_R_BAD_FIXED_HEADER_DECRYPT			 102
#define RSA_R_BAD_PAD_BYTE_COUNT			 103
#define RSA_R_BAD_SIGNATURE				 104
#define RSA_R_BAD_ZERO_BYTE				 105
#define RSA_R_BLOCK_TYPE_IS_NOT_01			 106
#define RSA_R_BLOCK_TYPE_IS_NOT_02			 107
#define RSA_R_DATA_GREATER_THAN_MOD_LEN			 108
#define RSA_R_DATA_TOO_LARGE				 109
#define RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE		 110
#define RSA_R_DATA_TOO_SMALL				 111
#define RSA_R_DIGEST_TOO_BIG_FOR_RSA_KEY		 112
#define RSA_R_NULL_BEFORE_BLOCK_MISSING			 113
#define RSA_R_PADDING_CHECK_FAILED			 114
#define RSA_R_SSLV3_ROLLBACK_ATTACK			 115
#define RSA_R_THE_ASN1_OBJECT_IDENTIFIER_IS_NOT_KNOWN_FOR_THIS_MD 116
#define RSA_R_UNKNOWN_ALGORITHM_TYPE			 117
#define RSA_R_UNKNOWN_PADDING_TYPE			 118
#define RSA_R_WRONG_SIGNATURE_LENGTH			 119
 
#ifdef  __cplusplus
}
#endif
#endif

