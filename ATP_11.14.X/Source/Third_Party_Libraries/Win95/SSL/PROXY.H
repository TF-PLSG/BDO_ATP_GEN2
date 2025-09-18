/* crypto/proxy/proxy.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_PROXY_H
#define HEADER_PROXY_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "buffer.h"
#include "bio.h"
#include "stack.h"

#define DEFAULT_PROXY_PORT	80
#define DEFAULT_SOCKS_PORT	1080
#define DEFAULT_CONNECT_PORT	443

#define PROXY_PROTOCOL_SOCKS		"socks"
#define PROXY_PROTOCOL_HTTPS		"https"
#define PROXY_PROTOCOL_HTTP		"http"

#define PROXY_TYPE_UNKNOWN		0
#define PROXY_TYPE_NORMAL		1
#define PROXY_TYPE_PROXY		2
#define PROXY_TYPE_CONNECT		3
#define PROXY_TYPE_NORMAL_CONNECT	4

typedef struct proxy_entry_st
	{
	char *protocol;		/* http, https, ftp etc etc */
	char *host;		/* The proxy:port name */

	char *ip_port;		/* The ip and numeric port */

	STACK *noproxy;		/* no-proxy list expanded */
	} PROXY_ENTRY;

/* You will probably want to use just PROXY_CON_BIO_DIRECT, but
 * there are situations for (PROXY_CON_BIO_DIRECT|PROXY_CON_BIO_PROXY)
 */
#define PROXY_CON_BIO_NEVER		0x00
#define PROXY_CON_BIO_DIRECT		0x01
#define PROXY_CON_BIO_PROXY		0x02

typedef struct bio_proxy_ctx_st
	{
	STACK *noproxy;	/* no-proxy list expanded */
	STACK *proxies; /* stack of PROXY_ENTRYs */

	/* If set to 0, it means we always use BIO_s_connect().
	 * If 1, use the connect_bio field (if present) for direct
	 * connections.  
	 * If 2, use the connect_bio field when using a proxy service.
	 * If 3, always use the connect_bio field.
	 */
	int connect_bio_mode;

	/* If not NULL, this BIO stack is duplicated and then has
	 * BIO_set_conn_port(),
	 * BIO_set_conn_hostname(),
	 * BIO_set_nbio()
	 * called against it where it makes sense.
	 */
	BIO *connect_bio;

	/* Should really have reference counts */
	} PROXY;

#ifndef NOPROTO

void ERR_load_PROXY_strings(void );

PROXY_ENTRY *PROXY_ENTRY_new(void );
void PROXY_ENTRY_free(PROXY_ENTRY *a);
int PROXY_ENTRY_set_server(PROXY_ENTRY *ctx,char *protocol,char *host);

int PROXY_ENTRY_add_noproxy(PROXY_ENTRY *ctx, char *list);
STACK *PROXY_ENTRY_get_noproxy(PROXY_ENTRY *ctx);
void PROXY_ENTRY_clear_noproxy(PROXY_ENTRY *ctx);

PROXY *PROXY_new(void );
void PROXY_free(PROXY *a);
STACK *PROXY_get_proxies(PROXY *ctx);

int PROXY_add_noproxy(PROXY *ctx, char *list);
STACK *PROXY_get_noproxy(PROXY *ctx);
void PROXY_clear_noproxy(PROXY *ctx);

PROXY_ENTRY *PROXY_get_proxy_entry(PROXY *ctx, char *protocol);
int PROXY_add_server(PROXY *ctx,char *protocol,char *host);

PROXY_ENTRY *PROXY_check_by_host(PROXY *ctx,char *protocol,char *host);
PROXY_ENTRY *PROXY_check_url(PROXY *ctxl,char *url);

#ifdef HEADER_LHASH_H
void PROXY_load_conf(PROXY *ctx, LHASH *conf, char *section);
#endif

void PROXY_print(BIO *bio, PROXY *ctx);
int PROXY_set_connect_bio(PROXY *pxy, BIO *bio,int mode);

#else

void ERR_load_PROXY_strings();

PROXY_ENTRY *PROXY_ENTRY_new();
void PROXY_ENTRY_free();
int PROXY_ENTRY_set_server();

int PROXY_ENTRY_add_noproxy();
STACK *PROXY_ENTRY_get_noproxy();
void PROXY_ENTRY_clear_noproxy();

PROXY *PROXY_new();
void PROXY_free();
STACK *PROXY_get_proxies();

int PROXY_add_noproxy();
STACK *PROXY_get_noproxy();
void PROXY_clear_noproxy();

PROXY_ENTRY *PROXY_get_proxy_entry();
int PROXY_add_server();

PROXY_ENTRY *PROXY_check_by_host();
PROXY_ENTRY *PROXY_check_url();

#ifdef HEADER_LHASH_H
void PROXY_load_conf();
#endif

void PROXY_print();

#endif

/* BEGIN ERROR CODES */
/* Error codes for the PROXY functions. */

/* Function codes. */
#define PROXY_F_ADD_NOPROXY				 100
#define PROXY_F_PROXY_ENTRY_NEW				 101
#define PROXY_F_PROXY_ENTRY_SET_SERVER			 102

/* Reason codes. */
 
#ifdef  __cplusplus
}
#endif
#endif

