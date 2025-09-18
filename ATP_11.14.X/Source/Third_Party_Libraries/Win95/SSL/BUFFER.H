/* crypto/buffer/buffer.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_BUFFER_H
#define HEADER_BUFFER_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct buf_mem_st
	{
	int length;	/* current number of bytes */
	char *data;
	int max;	/* size of buffer */
	} BUF_MEM;

#ifndef NOPROTO
BUF_MEM *BUF_MEM_new(void);
void	BUF_MEM_free(BUF_MEM *a);
int	BUF_MEM_grow(BUF_MEM *str, int len);
char *	BUF_strdup(char *str);

void ERR_load_BUF_strings(void );

#else

BUF_MEM *BUF_MEM_new();
void	BUF_MEM_free();
int	BUF_MEM_grow();
char *	BUF_strdup();

void ERR_load_BUF_strings();

#endif

/* BEGIN ERROR CODES */
/* Error codes for the BUF functions. */

/* Function codes. */
#define BUF_F_BUF_MEM_GROW				 100
#define BUF_F_BUF_MEM_NEW				 101
#define BUF_F_BUF_STRDUP				 102

/* Reason codes. */
 
#ifdef  __cplusplus
}
#endif
#endif

