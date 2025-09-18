/* crypto/sslc.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_SSLC_H
#define HEADER_SSLC_H

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* SSLC is defined to aid in migration of code from SSLeay-based
 * applications into the API contained in RSA BSAFE SSL-C
 */
#define SSLC 				1

/* the current version of the library */
#define SSLC_VERSION_NUMBER		0x1004

/* there are a number of items which control the definition of
 * prototypes for functions that are not in the standard library
 * build and as such we need to make sure that we are not
 * including definitions for items that are not present
 */

#ifndef NO_CAST
# define NO_CAST
#endif
#ifndef NO_IDEA
# define NO_IDEA
#endif
#ifndef NO_MDC2
# define NO_MDC2
#endif
#ifndef NO_RMD160
# define NO_RMD160
#endif
#ifndef NO_RC5
# define NO_RC5
#endif

#include <stdio.h>

#ifdef SSLC_COMPAT_INCLUDE

/* if we are building with a full-set of header files then we can
 * pull in all the definitions; however this will include APIs that
 * are not "supported" and as such is not recommended
 */
#include "buffer.h"
#include "stack.h"
#include "bio.h"
#include "lhash.h"
#include "err.h"
#include "rand.h"
#include "evp.h"
#include "asn1.h"
#include "objects.h"
#include "x509.h"
#include "pem.h"
#include "crypto.h"
#include "ssl.h"

#else /* !SSLC_COMPAT_INCLUDE */

/* pull in the required library defines */
#include "crypto.h"

/*---------------------------------------------------------------------------*/
/* bio.h */
/*---------------------------------------------------------------------------*/
#ifndef HEADER_BIO_H
#define HEADER_BIO_H

typedef struct bio_method_st BIO_METHOD;
typedef struct bio_st BIO;

#define BIO_NOCLOSE		0x00
#define BIO_CLOSE		0x01
#define BIO_CTRL_RESET		1  /* opt - rewind/zero etc */
#define BIO_CTRL_EOF		2  /* opt - are we at the eof */
#define BIO_CTRL_INFO		3  /* opt - extra tit-bits */
#define BIO_CTRL_SET		4  /* man - set the 'IO' type */
#define BIO_CTRL_GET		5  /* man - get the 'IO' type */
#define BIO_CTRL_PUSH		6  /* opt - internal, used to signify change */
#define BIO_CTRL_POP		7  /* opt - internal, used to signify change */
#define BIO_CTRL_GET_CLOSE	8  /* man - set the 'close' on free */
#define BIO_CTRL_SET_CLOSE	9  /* man - set the 'close' on free */
#define BIO_CTRL_PENDING	10  /* opt - is their more data buffered */
#define BIO_CTRL_FLUSH		11  /* opt - 'flush' buffered output */
#define BIO_CTRL_DUP		12  /* man - extra stuff for 'duped' BIO */
#define BIO_CTRL_WPENDING	13  /* opt - number of bytes still to write */
#define BIO_CTRL_SET_CALLBACK	14  /* opt - set callback function */
#define BIO_CTRL_GET_CALLBACK	15  /* opt - set callback function */
#define BIO_CTRL_SET_FILENAME	30	/* BIO_s_file special */

#define BIO_C_SET_CONNECT			100
#define BIO_C_DO_STATE_MACHINE			101
#define BIO_C_SET_NBIO				102
#define BIO_C_SET_PROXY_PARAM			103
#define BIO_C_SET_FD				104
#define BIO_C_GET_FD				105
#define BIO_C_SET_FILE_PTR			106
#define BIO_C_GET_FILE_PTR			107
#define BIO_C_SET_FILENAME			108
#define BIO_C_SET_SSL				109
#define BIO_C_GET_SSL				110
#define BIO_C_SET_MD				111
#define BIO_C_GET_MD				112
#define BIO_C_GET_CIPHER_STATUS			113
#define BIO_C_SET_BUF_MEM			114
#define BIO_C_GET_BUF_MEM_PTR			115
#define BIO_C_GET_BUFF_NUM_LINES		116
#define BIO_C_SET_BUFF_SIZE			117
#define BIO_C_SET_ACCEPT			118
#define BIO_C_SSL_MODE				119
#define BIO_C_GET_MD_CTX			120
#define BIO_C_GET_PROXY_PARAM			121
#define BIO_C_SET_BUFF_READ_DATA		122 /* data to read first */
#define BIO_C_GET_CONNECT			123
#define BIO_C_GET_ACCEPT			124
#define BIO_C_SET_SSL_RENEGOTIATE_BYTES		125
#define BIO_C_GET_SSL_NUM_RENEGOTIATES		126
#define BIO_C_SET_SSL_RENEGOTIATE_TIMEOUT	127
#define BIO_C_FILE_SEEK				128
#define BIO_C_GET_CIPHER_CTX			129
#define BIO_C_SET_BUF_MEM_EOF_RETURN		130/*return end of input value*/
#define BIO_C_SET_BIND_MODE			131
#define BIO_C_GET_BIND_MODE			132
#define BIO_C_FILE_TELL				133
#define BIO_C_GET_SOCKS				134
#define BIO_C_SET_SOCKS				135

/* modifiers */
#define BIO_FP_READ		0x02
#define BIO_FP_WRITE		0x04
#define BIO_FP_APPEND		0x08
#define BIO_FP_TEXT		0x10

#define BIO_FLAGS_READ		0x01
#define BIO_FLAGS_WRITE		0x02
#define BIO_FLAGS_IO_SPECIAL	0x04
#define BIO_FLAGS_RWS (BIO_FLAGS_READ|BIO_FLAGS_WRITE|BIO_FLAGS_IO_SPECIAL)
#define BIO_FLAGS_SHOULD_RETRY	0x08
#define BIO_FLAGS_BASE64_NO_NL	0x100

#define BIO_set_nbio(b,n)	BIO_ctrl(b,BIO_C_SET_NBIO,(n),NULL)

#define BIO_set_nbio_accept(b,n) BIO_ctrl(b,BIO_C_SET_ACCEPT,1,(n)?"a":NULL)
#define BIO_set_accept_bios(b,bio) BIO_ctrl(b,BIO_C_SET_ACCEPT,2,(char *)bio)
#define BIO_set_conn_hostname(b,name) BIO_ctrl(b,BIO_C_SET_CONNECT,0,(char *)name)
#define BIO_set_accept_port(b,name) BIO_ctrl(b,BIO_C_SET_ACCEPT,0,(char *)name)

#define BIO_BIND_NORMAL			0
#define BIO_BIND_REUSEADDR_IF_UNUSED	1
#define BIO_BIND_REUSEADDR		2

#define BIO_set_bind_mode(b,mode) BIO_ctrl(b,BIO_C_SET_BIND_MODE,mode,NULL)
#define BIO_get_bind_mode(b,mode) BIO_ctrl(b,BIO_C_GET_BIND_MODE,0,NULL)

#define BIO_do_connect(b)	BIO_do_handshake(b)
#define BIO_do_accept(b)	BIO_do_handshake(b)
#define BIO_do_handshake(b)	BIO_ctrl(b,BIO_C_DO_STATE_MACHINE,0,NULL)

#define BIO_set_fd(b,fd,c)	BIO_int_ctrl(b,BIO_C_SET_FD,c,fd)
#define BIO_get_fd(b,c)		BIO_ctrl(b,BIO_C_GET_FD,0,(char *)c)

#define BIO_set_fp(b,fp,c)	BIO_ctrl(b,BIO_C_SET_FILE_PTR,c,(char *)fp)
#define BIO_get_fp(b,fpp)	BIO_ctrl(b,BIO_C_GET_FILE_PTR,0,(char *)fpp)

#define BIO_seek(b,ofs)	(int)BIO_ctrl(b,BIO_C_FILE_SEEK,ofs,NULL)
#define BIO_tell(b)	(int)BIO_ctrl(b,BIO_C_FILE_TELL,0,NULL)

#define BIO_read_filename(b,name) BIO_ctrl(b,BIO_C_SET_FILENAME, \
		BIO_CLOSE|BIO_FP_READ,name)
#define BIO_write_filename(b,name) BIO_ctrl(b,BIO_C_SET_FILENAME, \
		BIO_CLOSE|BIO_FP_WRITE,name)
#define BIO_append_filename(b,name) BIO_ctrl(b,BIO_C_SET_FILENAME, \
		BIO_CLOSE|BIO_FP_APPEND,name)
#define BIO_rw_filename(b,name) BIO_ctrl(b,BIO_C_SET_FILENAME, \
		BIO_CLOSE|BIO_FP_READ|BIO_FP_WRITE,name)

#define BIO_reset(b)		(int)BIO_ctrl(b,BIO_CTRL_RESET,0,NULL)
#define BIO_eof(b)		(int)BIO_ctrl(b,BIO_CTRL_EOF,0,NULL)
#define BIO_set_close(b,c)	(int)BIO_ctrl(b,BIO_CTRL_SET_CLOSE,(c),NULL)
#define BIO_get_close(b)	(int)BIO_ctrl(b,BIO_CTRL_GET_CLOSE,0,NULL)
#define BIO_pending(b)		(int)BIO_ctrl(b,BIO_CTRL_PENDING,0,NULL)
#define BIO_wpending(b)		(int)BIO_ctrl(b,BIO_CTRL_WPENDING,0,NULL)
#define BIO_flush(b)		(int)BIO_ctrl(b,BIO_CTRL_FLUSH,0,NULL)

/* These are passed by the BIO callback */
#define BIO_CB_FREE	0x01
#define BIO_CB_READ	0x02
#define BIO_CB_WRITE	0x03
#define BIO_CB_PUTS	0x04
#define BIO_CB_GETS	0x05
#define BIO_CB_CTRL	0x06
#define BIO_CB_RETURN	0x80

#ifdef NO_STDIO
#define NO_FP_API
#endif

#ifndef NOPROTO
#  if defined(WIN16) && defined(_WINDLL)
BIO_METHOD *BIO_s_file_internal(void);
BIO *BIO_new_file_internal(char *filename, char *mode);
BIO *BIO_new_fp_internal(FILE *stream, int close_flag);
#    define BIO_s_file	BIO_s_file_internal
#    define BIO_new_file	BIO_new_file_internal
#    define BIO_new_fp	BIO_new_fp_internal
#  else /* FP_API */
BIO_METHOD *BIO_s_file(void );
BIO *BIO_new_file(char *filename, char *mode);
BIO *BIO_new_fp(FILE *stream, int close_flag);
#    define BIO_s_file_internal		BIO_s_file
#    define BIO_new_file_internal	BIO_new_file
#    define BIO_new_fp_internal		BIO_s_file
#  endif /* FP_API */
#else
#  if defined(WIN16) && defined(_WINDLL)
BIO_METHOD *BIO_s_file_internal();
BIO *BIO_new_file_internal();
BIO *BIO_new_fp_internal();
#    define BIO_s_file	BIO_s_file_internal
#    define BIO_new_file	BIO_new_file_internal
#    define BIO_new_fp	BIO_new_fp_internal
#  else /* FP_API */
BIO_METHOD *BIO_s_file();
BIO *BIO_new_file();
BIO *BIO_new_fp();
#    define BIO_s_file_internal		BIO_s_file
#    define BIO_new_file_internal	BIO_new_file
#    define BIO_new_fp_internal		BIO_s_file
#  endif /* FP_API */
#endif

#ifndef NOPROTO
#define VAR_ANSI	/* select ANSI version by default */
#endif /* !NOPROTO */

#ifdef VAR_ANSI
/* ANSI version of a "portable" macro set for variable length args */
#ifndef __STDARG_H__ /**/
#include <stdarg.h>
#endif /**/

#define VAR_PLIST(arg1type,arg1)    arg1type arg1, ...
#define VAR_PLIST2(arg1type,arg1,arg2type,arg2) arg1type arg1,arg2type arg2,...
#define VAR_ALIST
#define VAR_BDEFN(args,arg1type,arg1)   va_list args
#define VAR_BDEFN2(args,arg1type,arg1,arg2type,arg2)    va_list args
#define VAR_INIT(args,arg1type,arg1)    va_start(args,arg1);
#define VAR_INIT2(args,arg1type,arg1,arg2type,arg2) va_start(args,arg2);
#define VAR_ARG(args,type,arg)	arg=va_arg(args,type)
#define VAR_END(args)		va_end(args);

#else

/* K&R version of a "portable" macro set for variable length args */
#ifndef __VARARGS_H__
#include <varargs.h>
#endif

#define VAR_PLIST(arg1type,arg1)	va_alist
#define VAR_PLIST2(arg1type,arg1,arg2type,arg2) va_alist
#define VAR_ALIST		va_dcl
#define VAR_BDEFN(args,arg1type,arg1)	va_list args; arg1type arg1
#define VAR_BDEFN2(args,arg1type,arg1,arg2type,arg2)    va_list args; \
	arg1type arg1; arg2type arg2
#define VAR_INIT(args,arg1type,arg1)	va_start(args); \
	arg1=va_arg(args,arg1type);
#define VAR_INIT2(args,arg1type,arg1,arg2type,arg2) va_start(args); \
	arg1=va_arg(args,arg1type);	arg2=va_arg(args,arg2type);
#define VAR_ARG(args,type,arg)		arg=va_arg(args,type)
#define VAR_END(args)			va_end(args);

#endif

#ifndef NOPROTO
int BIO_printf( VAR_PLIST( BIO *, bio ) );
int BIO_sock_should_retry(int i);
int BIO_sock_non_fatal_error(int error);
int BIO_fd_should_retry(int i);
int BIO_fd_non_fatal_error(int error);
int BIO_dump(BIO *b,char *bytes,int len);

BIO_METHOD *BIO_s_mem(void);
BIO *BIO_new_mem(void);
BIO *BIO_new_socket(int sock, int close_flag);
BIO *BIO_new_fd(int fd, int close_flag);
BIO *BIO_new_connect(char *host_port);
BIO *BIO_new_accept(char *host_port);

BIO *	BIO_new(BIO_METHOD *type);
int	BIO_set(BIO *a,BIO_METHOD *type);
int	BIO_free(BIO *a);
int	BIO_read(BIO *b, char *data, int len);
int	BIO_gets(BIO *bp,char *buf, int size);
int	BIO_write(BIO *b, char *data, int len);
int	BIO_puts(BIO *bp,char *buf);
long	BIO_ctrl(BIO *bp,int cmd,long larg,char *parg);
char *	BIO_ptr_ctrl(BIO *bp,int cmd,long larg);
long	BIO_int_ctrl(BIO *bp,int cmd,long larg,int iarg);
BIO *	BIO_push(BIO *b,BIO *append);
BIO *	BIO_pop(BIO *b);
void	BIO_free_all(BIO *a);
BIO *	BIO_find_type(BIO *b,int bio_type);
BIO *	BIO_get_retry_BIO(BIO *bio, int *reason);
int	BIO_get_retry_reason(BIO *bio);
BIO *	BIO_dup_chain(BIO *in);

void ERR_load_BIO_strings(void );

void BIO_set_flags(BIO *b,int f);
int BIO_get_flags(BIO *b);
void BIO_set_retry_special(BIO *b);
void BIO_set_retry_read(BIO *b);
void BIO_set_retry_write(BIO *b);
void BIO_clear_flags(BIO *b,int f);
void BIO_clear_retry_flags(BIO *b);
int BIO_get_retry_flags(BIO *b);
int BIO_should_read(BIO *b);
int BIO_should_write(BIO *b);
int BIO_should_io_special(BIO *b);
int BIO_retry_type(BIO *b);
int BIO_should_retry(BIO *b);
void BIO_set_callback(BIO *b,long (*cb)());
void BIO_set_callback_arg(BIO *b,char *arg);
char *BIO_get_callback_arg(BIO *b);
long (*BIO_get_callback(BIO *b))();
char *BIO_method_name(BIO *b);
int BIO_method_type(BIO *b);
#else /* !NOPROTO */
int BIO_printf();
int BIO_sock_should_retry();
int BIO_sock_non_fatal_error();
int BIO_fd_should_retry();
int BIO_fd_non_fatal_error();
int BIO_dump();

BIO_METHOD *BIO_s_mem();
BIO *BIO_new_mem();
BIO *BIO_new_socket();
BIO *BIO_new_fd();
BIO *BIO_new_connect();
BIO *BIO_new_accept();

BIO *	BIO_new();
int	BIO_set();
int	BIO_free();
int	BIO_read();
int	BIO_gets();
int	BIO_write();
int	BIO_puts();
long	BIO_ctrl();
char *	BIO_ptr_ctrl();
long	BIO_int_ctrl();
BIO *	BIO_push();
BIO *	BIO_pop();
void	BIO_free_all();
BIO *	BIO_find_type();
BIO *	BIO_get_retry_BIO();
int	BIO_get_retry_reason();
BIO *	BIO_dup_chain();

void ERR_load_BIO_strings();

void BIO_set_flags();
int BIO_get_flags();
void BIO_set_retry_special();
void BIO_set_retry_read();
void BIO_set_retry_write();
void BIO_clear_flags();
void BIO_clear_retry_flags();
int BIO_get_retry_flags();
int BIO_should_read();
int BIO_should_write();
int BIO_should_io_special();
int BIO_retry_type();
int BIO_should_retry();
void BIO_set_callback();
void BIO_set_callback_arg();
char *BIO_get_callback_arg();
long (*BIO_get_callback())();
char *BIO_method_name();
int BIO_method_type();
#endif /* NOPROTO */

#endif /* HEADER_BIO_H */


/*---------------------------------------------------------------------------*/

#include "err.h"
#include "rand.h"

/* x509.h - only the required items listed here */
#ifndef HEADER_X509_H
#define X509_FILETYPE_PEM	1
#define X509_FILETYPE_ASN1	2
#define X509_FILETYPE_DEFAULT	3
#endif /* !HEADER_X509_H */

/* evp.h - only the required items listed here */
#ifndef HEADER_EVP_H
#define EVP_RC2_KEY_SIZE		16
#define EVP_RC4_KEY_SIZE		16
#define EVP_BLOWFISH_KEY_SIZE		16
#define EVP_MAX_MD_SIZE			20
#define EVP_MAX_KEY_LENGTH		24
#define EVP_MAX_IV_LENGTH		8
#endif /* !HEADER_EVP_H */

/* ssl.h (which includes ssl2.h, ssl23.h, ssl3.h and tls1.h) */
#include "ssl.h"

#ifndef HEADER_X509_H

#define HEADER_X509_H 

typedef struct x509_extension_st X509_EXTENSION;
typedef struct x509_extension_st X509_NAME_ENTRY;
typedef struct x509_store_state_st X509_STORE_CTX;

/* some defines are not included unless we have pulled in the 
 * real x509.h header file ... but it is safe to define them 
 * as we have the required typedef in place above
 */
X509 *	SSL_get_peer_certificate(SSL *s);
void X509_reference_inc(X509 *x);

/* additionally, we have documented the following apis so we 
 * include their details here 
 */
#ifndef NOPROTO
char *		X509_get_serialNumber_buf(X509 *x,char *buf,int size);
int 		X509_set_issuer_name(X509 *x, X509_NAME *name);
X509_NAME *	X509_get_issuer_name(X509 *a);
int 		X509_set_subject_name(X509 *x, X509_NAME *name);
X509_NAME *	X509_get_subject_name(X509 *a);
long 		X509_get_version(X509 *x);
int  		X509_get_pubkey_bits(X509 *x);
char *		X509_get_notBefore_str(X509 *x);
char *		X509_get_notBefore_buf(X509 *x,char *buf,int size);
char *		X509_get_notAfter_str(X509 *x);
char *		X509_get_notAfter_buf(X509 *x,char *buf,int size);
char *		X509_NAME_oneline(X509_NAME *a,char *buf,int size);

int 		X509_NAME_entry_count(X509_NAME *name);
X509_NAME_ENTRY *X509_NAME_get_entry(X509_NAME *name,int loc);

/* new */
char *		X509_NAME_ENTRY_get_data_ptr(X509_NAME_ENTRY *ne);
int		X509_NAME_ENTRY_get_data_len(X509_NAME_ENTRY *ne);
char *		X509_NAME_ENTRY_get_data_buf(X509_NAME_ENTRY *ne,char *buf,int size);
char *		X509_NAME_ENTRY_get_oid_buf(X509_NAME_ENTRY *ne,char *buf,int size);

int		X509_get_ext_count(X509 *x);
X509_EXTENSION *X509_get_ext(X509 *x, int loc);

/* new */
char *		X509_EXTENSION_get_oid_buf(X509_EXTENSION *xe,char *buf,int size);
char *		X509_EXTENSION_get_data_ptr(X509_EXTENSION *xe);
int		X509_EXTENSION_get_data_len(X509_EXTENSION *xe);
int		X509_EXTENSION_get_data_critical(X509_EXTENSION *xe);

int	X509_STORE_CTX_set_ex_data(X509_STORE_CTX *ctx,int idx,char *data);
char *	X509_STORE_CTX_get_ex_data(X509_STORE_CTX *ctx,int idx);
int	X509_STORE_CTX_get_error(X509_STORE_CTX *ctx);
void	X509_STORE_CTX_set_error(X509_STORE_CTX *ctx,int s);
int	X509_STORE_CTX_get_error_depth(X509_STORE_CTX *ctx);
X509 *	X509_STORE_CTX_get_current_cert(X509_STORE_CTX *ctx);
/*
STACK *	X509_STORE_CTX_get_chain(X509_STORE_CTX *ctx);
*/
void	X509_STORE_CTX_set_cert(X509_STORE_CTX *c,X509 *x);
void	X509_STORE_CTX_set_chain(X509_STORE_CTX *c,STACK /* X509 */ *sk);

char *X509_verify_cert_error_string(long n);

/* free apis */
void		X509_free(X509 *a);
void		X509_NAME_free(X509_NAME *a);
void		X509_NAME_ENTRY_free(X509_NAME_ENTRY *a);
void		X509_EXTENSION_free(X509_EXTENSION *a);

#else /* !NOPROTO */

X509 *	SSL_get_peer_certificate();
void X509_reference_inc();

char *		X509_get_serialNumber_buf();
int 		X509_set_issuer_name();
X509_NAME *	X509_get_issuer_name();
int 		X509_set_subject_name();
X509_NAME *	X509_get_subject_name();
long 		X509_get_version();
int  		X509_get_pubkey_bits();
char *		X509_get_notBefore_buf();
char *		X509_get_notBefore_time();
char *		X509_get_notAfter_buf();
char *		X509_get_notAfter_time();
char *		X509_NAME_oneline();

int 		X509_NAME_entry_count();
int		X509_NAME_get_entry_len();
char *		X509_NAME_get_entry_buf();

int		X509_get_ext_count();
X509_EXTENSION *X509_get_ext();

char *		X509_EXTENSION_get_oid_buf();
char *		X509_EXTENSION_get_data_ptr();
int		X509_EXTENSION_get_data_len();
int		X509_EXTENSION_get_data_critical();

/* free apis */
void		X509_free();
void		X509_NAME_free();
void		X509_NAME_ENTRY_free();
void		X509_EXTENSION_free();

#endif /* NOPROTO */

#endif /* !HEADER_X509_H */


/* pem.h */
#ifndef HEADER_PEM_H

#ifndef NOPROTO
char *	PEM_ASN1_read_bio(char *(*d2i)(),char *name,BIO *bp,char **x,
		int (*cb)());
X509 *          d2i_X509(X509 **a,unsigned char **pp,long length);
EVP_PKEY *      d2i_PrivateKey(int type,EVP_PKEY **a, unsigned char **pp,
                        long length);
#else /* NOPROTO */
char *	PEM_ASN1_read_bio();
X509 *          d2i_X509();
EVP_PKEY *      d2i_PrivateKey();
#endif /* NOPROTO */

/* now for the macros that are exposed for certificate and private 
 * key loading in "PEM" format
 */
#define PEM_STRING_X509		"CERTIFICATE"
#define PEM_STRING_EVP_PKEY	"PRIVATE KEY"
#define	PEM_read_bio_X509(bp,x,cb) (X509 *)PEM_ASN1_read_bio( \
	(char *(*)())d2i_X509,PEM_STRING_X509,bp,(char **)x,cb)
#define	PEM_read_bio_PrivateKey(bp,x,cb) (EVP_PKEY *)PEM_ASN1_read_bio( \
	(char *(*)())d2i_PrivateKey,PEM_STRING_EVP_PKEY,bp,(char **)x,cb)

#define HEADER_PEM_H
#endif /* HEADER_PEM_H */

#ifndef HEADER_EVP_H

#ifndef NOPROTO
void		EVP_PKEY_free(EVP_PKEY *pkey);
void		EVP_PKEY_reference_inc(EVP_PKEY *pkey);
#else /* NOPROTO */
void		EVP_PKEY_free();
void		EVP_PKEY_reference_inc();
#endif /* NOPROTO */

#endif /* !HEADER_EVP_H */


#endif /* SSLC_COMPAT_INCLUDE */

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* HEADER_SSLC_H */


