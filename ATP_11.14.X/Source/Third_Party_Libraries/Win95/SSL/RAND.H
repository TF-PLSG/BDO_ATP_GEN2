/* crypto/rand/rand.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_RAND_H
#define HEADER_RAND_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct rand_meth_st
	{
#ifndef NOPROTO
	void (*seed)(unsigned char *buf, int num);
	void (*bytes)(unsigned char *buf, int num);
	void (*cleanup)(void);
#else
	void (*seed)();
	void (*bytes)();
	void (*cleanup)();
#endif
	} RAND_METHOD;

#ifndef NOPROTO
void RAND_set_rand_method(RAND_METHOD *meth);
RAND_METHOD *RAND_get_rand_method(void );
RAND_METHOD *RAND_SSLeay(void);
void RAND_cleanup(void );
void RAND_bytes( unsigned char *buf,int num);
void RAND_seed( unsigned char *buf,int num);
int  RAND_load_file(char *file,long max_bytes);
int  RAND_write_file(char *file);
char *RAND_file_name(char *file,int num);
#ifdef WINDOWS
void RAND_screen(void);
#endif
#else
void RAND_set_rand_method();
RAND_METHOD *RAND_get_rand_method();
RAND_METHOD *RAND_SSLeay();
void RAND_cleanup();
void RAND_bytes();
void RAND_seed();
int  RAND_load_file();
int  RAND_write_file();
char *RAND_file_name();
#ifdef WINDOWS
void RAND_screen();
#endif
#endif

#ifdef  __cplusplus
}
#endif

#endif
