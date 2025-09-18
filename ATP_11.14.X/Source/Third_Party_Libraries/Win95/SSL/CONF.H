/* crypto/conf/conf.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef  HEADER_CONF_H
#define HEADER_CONF_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "stack.h"
#include "lhash.h"

typedef struct
	{
	char *section;
	char *name;
	char *value;
	} CONF_VALUE;

#ifndef NOPROTO

LHASH *CONF_load(LHASH *conf,char *file,long *eline);
STACK *CONF_get_section(LHASH *conf,char *section);
char *CONF_get_string(LHASH *conf,char *group,char *name);
long CONF_get_number(LHASH *conf,char *group,char *name);
void CONF_free(LHASH *conf);
void ERR_load_CONF_strings(void );

#else

LHASH *CONF_load();
STACK *CONF_get_section();
char *CONF_get_string();
long CONF_get_number();
void CONF_free();
void ERR_load_CONF_strings();

#endif

/* BEGIN ERROR CODES */
/* Error codes for the CONF functions. */

/* Function codes. */
#define CONF_F_CONF_LOAD				 100
#define CONF_F_STR_COPY					 101

/* Reason codes. */
#define CONF_R_MISSING_CLOSE_SQUARE_BRACKET		 100
#define CONF_R_MISSING_EQUAL_SIGN			 101
#define CONF_R_NO_CLOSE_BRACE				 102
#define CONF_R_UNABLE_TO_CREATE_NEW_SECTION		 103
#define CONF_R_VARIABLE_HAS_NO_VALUE			 104
 
#ifdef  __cplusplus
}
#endif
#endif

