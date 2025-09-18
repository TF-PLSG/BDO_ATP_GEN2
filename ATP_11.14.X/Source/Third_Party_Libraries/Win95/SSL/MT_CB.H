/* mt/cb/mt_cb.h */

/* Copyright (C) 1998, RSA Data Security Pty Ltd.
 *
 * This file shall only be used to demonstrate how to interface to an
 * RSA Data Security Pty Ltd licensed development product.
 *
 * You have a royalty-free right to use, reproduce and distribute this 
 * demonstration file, provided that you agree that RSA Data Security Pty Ltd
 * has no warranty, implied or otherwise, or liability for this 
 * demonstration file.
 *
 */

#ifndef HEADER_MT_CB_MT_CB_H
#define HEADER_MT_CB_MT_CB_H

/* 16k stack for each thread should be sufficient */
#ifndef THREAD_STACK_SIZE
#define THREAD_STACK_SIZE       (16*1024)
#endif

#ifndef MAX_THREAD_NUMBER
#define MAX_THREAD_NUMBER	256
#endif

/* remove the following define if you want the lock trace code
 * compiled out - which removes all fprintf(stderr,...) calls
 */
#define APP_LOCK_TRACE
	
#ifdef SOLARIS
#include "../cb/mt_sol.h"
#endif
#ifdef WIN32
#include "mt_win32.h"
#endif
#if defined(PTHREAD) || defined(LINUX)
#include "../cb/mt_pth.h"
#endif
#ifdef VXWORKS
#include "../cb/mt_vxw.h"
#endif

int APP_locking_callbacks_add(void);
int APP_locking_callbacks_delete(void);

int APP_thread_initialize(int thread_number);
int APP_thread_create(void (*func)(void *arg),void *func_arg,
                                APP_THREAD_ID *tid);
int APP_thread_finalize(int thread_number);

/* fwd decl of the callbacks registered */
void APPCB_locking_callback(int mode, int lockid, char *file, int line);
unsigned long APPCB_thread_id();

/* code for trace, stats display */
int APP_thread_trace(int val);
int APP_thread_show_stats(void);

#endif /* HEADER_MT_CB_MT_CB_H */

