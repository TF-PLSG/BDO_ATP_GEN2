/* crypto/tmdiff.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_TMDIFF_H
#define HEADER_TMDIFF_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef NOPROTO
char *ms_time_new(void );
void ms_time_free(char *a);
void ms_time_get(char *a);
double ms_time_diff(char *start,char *end);
int ms_time_cmp(char *ap,char *bp);

#else

char *ms_time_new();
void ms_time_free();
void ms_time_get();
double ms_time_diff();
int ms_time_cmp();
#endif

#ifdef  __cplusplus
}
#endif

#endif

