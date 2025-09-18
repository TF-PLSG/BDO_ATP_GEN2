/* crypto/txt_db/txt_db.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_TXT_DB_H
#define HEADER_TXT_DB_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "stack.h"
#include "lhash.h"

#define DB_ERROR_OK			0
#define DB_ERROR_MALLOC			1
#define DB_ERROR_INDEX_CLASH    	2
#define DB_ERROR_INDEX_OUT_OF_RANGE	3
#define DB_ERROR_NO_INDEX		4
#define DB_ERROR_INSERT_INDEX_CLASH    	5

typedef struct txt_db_st
	{
	int num_fields;
	STACK /* char ** */ *data;
	LHASH **index;
	int (**qual)();
	long error;
	long arg1;
	long arg2;
	char **arg_row;
	} TXT_DB;

#ifndef NOPROTO
#ifdef HEADER_BIO_H
TXT_DB *TXT_DB_read(BIO *in, int num);
long TXT_DB_write(BIO *out, TXT_DB *db);
#else
TXT_DB *TXT_DB_read(char *in, int num);
long TXT_DB_write(char *out, TXT_DB *db);
#endif
int TXT_DB_create_index(TXT_DB *db,int field,int (*qual)(),
	 unsigned long (*hash)(),int (*cmp)());
void TXT_DB_free(TXT_DB *db);
char **TXT_DB_get_by_index(TXT_DB *db, int idx, char **value);
int TXT_DB_insert(TXT_DB *db,char **value);

#else

TXT_DB *TXT_DB_read();
long TXT_DB_write();
int TXT_DB_create_index();
void TXT_DB_free();
char **TXT_DB_get_by_index();
int TXT_DB_insert();

#endif

#ifdef  __cplusplus
}
#endif

#endif
