/* crypto/lhash/lhash.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_LHASH_H
#define HEADER_LHASH_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct lhash_node_st
	{
	char *data;
	struct lhash_node_st *next;
#ifndef NO_HASH_COMP
	unsigned long hash;
#endif
	} LHASH_NODE;

typedef struct lhash_st
	{
	LHASH_NODE **b;
	int (*comp)();
	unsigned long (*hash)();
	unsigned int num_nodes;
	unsigned int num_alloc_nodes;
	unsigned int p;
	unsigned int pmax;
	unsigned long up_load; /* load times 256 */
	unsigned long down_load; /* load times 256 */
	unsigned long num_items;

	unsigned long num_expands;
	unsigned long num_expand_reallocs;
	unsigned long num_contracts;
	unsigned long num_contract_reallocs;
	unsigned long num_hash_calls;
	unsigned long num_comp_calls;
	unsigned long num_insert;
	unsigned long num_replace;
	unsigned long num_delete;
	unsigned long num_no_delete;
	unsigned long num_retrieve;
	unsigned long num_retrieve_miss;
	unsigned long num_hash_comps;

	int error;
	} LHASH;

#define LH_LOAD_MULT	256

/* Indicates a malloc() error in the last call, this is only bad
 * in lh_insert(). */
#define lh_error(lh)	((lh)->error)

#ifndef NOPROTO
LHASH *lh_new(unsigned long (*h)(), int (*c)());
void lh_free(LHASH *lh);
char *lh_insert(LHASH *lh, char *data);
char *lh_delete(LHASH *lh, char *data);
char *lh_retrieve(LHASH *lh, char *data);
void lh_doall(LHASH *lh, void (*func)(/* char *b */));
void lh_doall_arg(LHASH *lh, void (*func)(/*char *a,char *b*/),char *arg);
unsigned long lh_strhash(char *c);

#ifndef NO_FP_API
void lh_stats(LHASH *lh, FILE *out);
void lh_node_stats(LHASH *lh, FILE *out);
void lh_node_usage_stats(LHASH *lh, FILE *out);
#endif

#ifdef HEADER_BIO_H
void lh_stats_bio(LHASH *lh, BIO *out);
void lh_node_stats_bio(LHASH *lh, BIO *out);
void lh_node_usage_stats_bio(LHASH *lh, BIO *out);
#endif
#else
LHASH *lh_new();
void lh_free();
char *lh_insert();
char *lh_delete();
char *lh_retrieve();
void lh_doall();
void lh_doall_arg();
unsigned long lh_strhash();

#ifndef NO_FP_API
void lh_stats();
void lh_node_stats();
void lh_node_usage_stats();
#endif
void lh_stats_bio();
void lh_node_stats_bio();
void lh_node_usage_stats_bio();
#endif

#ifdef  __cplusplus
}
#endif

#endif

