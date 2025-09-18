/***************************************************************************
 *
 * MODULE:      DUMP_PROTOTYPES.H
 *
 * TITLE:       ATP SQL DUMP Utility Prototypes
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the ATP SQL Dump application.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * Copyright (c) 2007, Hypercom, Inc. All Rights Reserved.
 *
 ***************************************************************************/

/***************************************************************************

   REVISION HISTORY
   ----------------

   $Log$

****************************************************************************/

#ifndef DUMP_CONSTANTS_H
#define DUMP_CONSTANTS_H

/* Error Conditons */
#define  DUMP_APP_SUCCESS            0
#define  DUMP_APP_XIPC_ERROR        -1
#define  DUMP_APP_LOG_CREATE_ERROR  -2
#define  DUMP_APP_LOG_WRITE_ERROR   -3
#define  DUMP_APP_PARM_ERROR        -4
#define  DUMP_APP_SHARED_MEM_ERROR  -5

/* Log Flags */
#define SYSLOG_YES  1
#define SYSLOG_NO   0
#define WINDOW_YES  1
#define WINDOW_NO   0
#define LOG_YES     1
#define LOG_NO      0


#endif
