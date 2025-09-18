/* crypto/stack/stack.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_STACK_H
#define HEADER_STACK_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct stack_st
	{
	int num;
	char **data;
	int sorted;

	int num_alloc;
	int (*comp)();
	} STACK;

#define sk_num(sk)		((sk)->num)
#define sk_value(sk,n)		((sk)->data[n])
#define sk_data(sk)		((sk)->data)

#define sk_new_null()	sk_new(NULL)
#ifndef NOPROTO

STACK *sk_new(int (*cmp)());
void sk_free(STACK *);
void sk_pop_free(STACK *st, void (*func)());
int sk_insert(STACK *sk,char *data,int where);
char *sk_delete(STACK *st,int loc);
char *sk_delete_ptr(STACK *st, char *p);
int sk_find(STACK *st,char *data);
int sk_push(STACK *st,char *data);
int sk_unshift(STACK *st,char *data);
char *sk_shift(STACK *st);
char *sk_pop(STACK *st);
void sk_zero(STACK *st);
void sk_set_cmp_func(STACK *sk, int (*c)());
STACK *sk_dup(STACK *st);

#else

STACK *sk_new();
void sk_free();
void sk_pop_free();
int sk_insert();
char *sk_delete();
char *sk_delete_ptr();
int sk_find();
int sk_push();
int sk_unshift();
char *sk_shift();
char *sk_pop();
void sk_zero();
void sk_set_cmp_func();
STACK *sk_dup();

#endif

#ifdef  __cplusplus
}
#endif

#endif
