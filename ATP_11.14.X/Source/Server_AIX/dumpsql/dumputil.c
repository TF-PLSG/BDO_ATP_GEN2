/***************************************************************************
 * 
 * MODULE:      DUMPUTIL.C
 *  
 * TITLE:       ATP SQL DUMP Utility Support Functions
 *  
 * DESCRIPTION: This module contains utility functions used to support
 *              the main functions of this application.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pteipc.h"
#include "ptetime.h"
#include "ntutils.h"
#include "app_info.h"
#include "equitdb.h"
#include "dump_constants.h"
#include "dump_prototypes.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR   AppName[];

/* Boolean Flags */
extern INT    XIPC_Login_Successful;
extern INT    DumpLogFileSuccess;

/* Log File Info */
extern FILE   *fpDumpFilePtr;



/******************************************************************************
 *
 * Function:     getDate
 *
 * Description:  This procedure takes an input string that contains a date and
 *               time.  It copies the date portion into an output string,
 *               formatted slightly different.
 *
 * Input:        time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 * Output:       date_str  : Format = "YYYY0M0D"
 *
 * Return Value: None
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 ******************************************************************************/
void getDate( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
   return;
}
   

/******************************************************************************
 *
 * Function:     getTime
 *
 * Description:  This procedure takes an input string that contains a date and
 *               time.  It copies the time portion into an output string,
 *               formatted slightly different.
 *
 * Input:        time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 * Output:       time_str  : Format = "0H0I0S"
 *
 * Return Value: None
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 ******************************************************************************/
void getTime( pCHAR time_date, pCHAR time_str )
{
   memset ( time_str,  0x00,          7 );
   strncpy( time_str, &time_date[11], 2 );
   strncat( time_str, &time_date[14], 2 );
   strncat( time_str, &time_date[17], 2 );
   return;
}


/******************************************************************************
 *
 * Function:     logMessage
 *
 * Description:  This function will log a message to any combination of
 *               syslog, the console window, and the dump log file. Input
 *               flags indiate which of the 3 the message is intended for.
 *
 * Input:        syslog  - SYSLOG_YES or SYSLOG_NO
 *               window  - WINDOW_YES or WINDOW_NO
 *               logfile - LOG_YES    or LOG_NO
 *               message - Message to be logged
 *               type    - Category of message
 *
 * Output:       None
 *
 * Return Value: DUMP_APP_SUCCESS
 *               DUMP_APP_LOG_WRITE_ERROR
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 ******************************************************************************/
INT logMessage(INT syslog, INT window, INT logfile, pCHAR message, pCHAR type )
{
   INT    retval = DUMP_APP_SUCCESS;
   INT    file_rcode;
   CHAR   outbuf[500] = "";
   pSQL01 p_sql01 = NULL;

   /* Syslog */
   if ( syslog == SYSLOG_YES )
   {
      if ( 0 == strcmp(type, INFO_ERROR) )
         LogEvent( message, INFO_MSG );

      else if ( 0 == strcmp(type, WARNING_ERROR) )
         LogEvent( message, WARN_MSG );

      else
         LogEvent( message, ERROR_MSG );
   }

   /* Aix Window */
   if ( window == WINDOW_YES )
   {
      printf( "\n%s: %s\n", type, message );
   }

   /* Dump Application Log File */
   if ( logfile == LOG_YES )
   {
      if ( 0 == strcmp(type,INFO_ERROR) )
      {
         /* We are logging an SQL. */
         p_sql01 = (pSQL01)message;
         sprintf( outbuf, "%s  %s  %s",
                  p_sql01->appname, p_sql01->sysdate, p_sql01->sqltext );

         file_rcode = fputs( outbuf, fpDumpFilePtr );
      }
      else
      {
         /* This is an error condition. Log the message. */
         file_rcode = fputs( message, fpDumpFilePtr );
      }

      if ( file_rcode == EOF )
         retval = DUMP_APP_LOG_WRITE_ERROR;

      else if ( fputc( '\n', fpDumpFilePtr ) != EOF )
      {
         retval = DUMP_APP_SUCCESS;
         fflush( fpDumpFilePtr );
      }
      else
         retval = DUMP_APP_LOG_WRITE_ERROR;
   }
   return( retval );
}

