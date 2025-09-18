/***************************************************************************
 *
 * MODULE:      DUMP_PROTOTYPES.H
 *
 * TITLE:       ATP SQL DUMP Utility Prototypes
 *  
 * DESCRIPTION: This header file contains function prototypes that
 *              are used by the ATP SQL Dump application.
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

#ifndef DUMP_PROTOTYPES_H
#define DUMP_PROTOTYPES_H

void usage( void );
INT  createLogFile( void );
INT  XipcCheckin( void );
INT  getSqlRecords( void );
void cleanup( void );



void getDate( pCHAR time_date, pCHAR date_str );
void getTime( pCHAR time_date, pCHAR time_str );
INT  logMessage(INT syslog, INT window, INT logfile, pCHAR message, pCHAR type);

#endif
