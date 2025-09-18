/***************************************************************************
 * 
 * MODULE:      DUMPMAIN.C
 *  
 * TITLE:       ATP SQL DUMP Utility
 *  
 * DESCRIPTION: This module reads the records in XIPC shared memory
 *              table SQL01. They are all logged into the ATP/log
 *              directory in a file called SQLDump_<date/time>.log.
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

/***************************************************************************

   Logic Flow
   ----------
   1.  Input Parameters
       1.1  None = Display Version and Proceed
       1.2  ? = Display usage and exit
       1.3  Any other input, give warning, recommend '?' for help, exit.

   2.  Create Log File
       2.1  Create file name = "SQLDump_YYYYMMDDhhmmss.log"
       2.2  Create the file
            2.2.1  No Error = Proceed
            2.2.2  Error - Log message to syslog; display in window;
                           wait for user to hit <enter>; exit.
       2.3  Display log file name in window.

   3.  Check into Xipc
       3.1  No Error = Proceed
       3.2  Error - Log to log file and syslog and display in window;
                    wait for user to hit <enter>; exit.

   4.  Read SQL Records from Shared Memory
       4.1  Call ATP Library Function "ReadSQL01_MultipleRecs_Table" to
            read all records from SQL01 into an SQL01 List structure.
       4.2  Once the read is complete, log each record into the Dump log file.
       4.3  If error on the read, log to syslog, dump log, and window; wait
            for user to hit <enter>; call cleanup (step 5); exit.

   5.  Cleanup
       5.1  Flush and close dump log file if it is opened.
       5.2  Check out of Xipc if checkin was successful.
       5.3  Exit

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "basictyp.h"
#include "pteipc.h"
#include "ptetime.h"
#include "app_info.h"
#include "equitdb.h"
#include "memmnger.h"
#include "ntutils.h"
#include "dump_constants.h"
#include "dump_prototypes.h"

/*+--------------------+
  | Global Definitions |
  +--------------------+*/

CHAR   AppName[80];
CHAR   Version[] = "ATP_11.1.0";

/* Boolean Flags */
INT    XIPC_Login_Successful;
INT    DumpLogFileSuccess;

/* Log File Info */
FILE   *fpDumpFilePtr;
CHAR   DumpFileName   [MAX_APP_NAME_SIZE];
CHAR   DumpFileDirPath[MAX_APP_NAME_SIZE];

/*****************************************************************************
 *
 * Function:     main
 *
 * Description:  This function is the entry point for the application.
 *               It drives the logic flow as indicated in the above
 *               comments.
 *
 *               There should not be any input parameters except for
 *               a question mark '?' requesting usage.
 *
 * Input:        argc   - number of input parameters
 *               argv[] - list of input parameters including application name
 *
 * Output:       None
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -1 Failure related to Xipc
 *              -2 Failure to create dump log file
 *
 * 
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT main( INT argc, pCHAR argv[] )
{
   INT  retval = DUMP_APP_PARM_ERROR;
   INT  i,j;
   INT  len;

   /* ------------------------- */
   /* Initialization of Globals */
   /* ------------------------- */
   DumpLogFileSuccess    = false;
   XIPC_Login_Successful = false;

   memset( AppName,         0x00, sizeof(AppName        ) );
   memset( DumpFileName,    0x00, sizeof(DumpFileName   ) );
   memset( DumpFileDirPath, 0x00, sizeof(DumpFileDirPath) );

   /* ---------------- */
   /* Input Parameters */
   /* ---------------- */
   len = strlen( argv[0] ) - 1;
   for( i=len,j=len; i>=0; i-- )
   {
      if ( (argv[0][i] == '/') || (argv[0][i] == '\\') )
         break;
      else if ( argv[0][i] == '.' )  /* Check for a file extension */
         j = i-1;
   }

   if ( j == len )
   {
      /* No file extension */
      strcpy( AppName, &argv[0][i+1] );
   }
   else
   {
      /* This strips the file extension. */
      memcpy( AppName, &argv[0][i+1], j-i );
   }

   if ( argc > 2 )
   {
      printf("\n\nInput Error - too many parameters. Try '?' for usage.\n" );
   }
   else if ( (argc == 2) && (0 != strcmp("?",argv[1])) )
   {
      printf("\n\nInput Error - invalid parameter. Try '?' for usage.\n" );
   }
   else if ( argc == 2 )
   {
      usage();
      retval = DUMP_APP_SUCCESS;
   }
   else
   {
      /* ------------------------ */
      /* Create SQL Dump Log File */
      /* ------------------------ */
      retval = createLogFile();
      if ( retval == DUMP_APP_SUCCESS )
      {
         /* --------------- */
         /* Check into XIPC */
         /* --------------- */
         retval = XipcCheckin();
         if ( retval == DUMP_APP_SUCCESS )
         {
            /* ---------------------------- */
            /* Read and Log the SQL Records */
            /* ---------------------------- */
            retval = getSqlRecords();
         }
      }
   }

   /* ------------------------------ */
   /* Perform Cleanup before Exiting */
   /* ------------------------------ */
   cleanup();

   return( retval );
}


/*****************************************************************************
 *
 * Function:     usage
 *
 * Description:  This function gives the usage of the application. That
 *               is, input parameters and return values.
 *
 * Input:        None
 *
 * Output:       None
 *
 * Return Value: None
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
void usage()
{
   INT  i;
   INT  len;

   len = strlen( AppName );

   printf( "\n\n" );
   printf( "%s\n", AppName );
   for( i=0; i<len; i++ )
      printf("-");
   printf( "\n\n" );

   printf( "Version: %s\n\n", Version );

   printf( "Description: This application reads the Xipc Shared Memory  \n" );
   printf( "table 'SQL01Table'. This table contains a list of the latest\n" );
   printf( "SQLs performed by each of the data servers. This application\n" );
   printf( "reads the SQLs from this table and logs them into a file in \n" );
   printf( "the Ascendent/log directory called SQLDump_<date/time>.log  \n" );
   printf( "where date/time are formatted as YYYYMMDDhhmmss.          \n\n" );

   printf( "Usage: %s [?]\n\n", AppName );
   printf( "       ? = Display usage of the application            \n" );
   printf( "       Any other paramter(s) result in an invalid input\n" );
   printf( "       Use no parameters to run the application.     \n\n" );

   printf( "Return Values:\n" );
   printf( "   0 = Successfully read SQL01 and logged the records          \n");
   printf( "  -1 = Error condition relating to Xipc                        \n");
   printf( "  -2 = Error condition relating to dump log file creation      \n");
   printf( "  -3 = Error condition relating to writing to the dump log file\n");
   printf( "  -4 = Error condition relating to input parameters            \n");
   printf( "  -5 = Error condition relating to shared memory             \n\n");

   return;
}

/*****************************************************************************
 *
 * Function:     createLogFile
 *
 * Description:  This function creates and opens the ATP Dump Log file.
 *               It puts it in the Ascendent/log directory.  Its name
 *               is SQLDump_YYYYMMDDhhmmss.log
 *
 * Input:        None
 *
 * Output:       DumpLogFileSuccess - (Global) Set 'true' on success
 *
 * Return Value: DUMP_APP_SUCCESS
 *               DUMP_APP_LOG_CREATE_ERROR = Unable to create log file
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT createLogFile()
{
   INT   retval = DUMP_APP_SUCCESS;
   CHAR  date_str [ 9] = "";
   CHAR  time_str [ 7] = "";
   CHAR  time_date[25] = "";
   CHAR  errbuf[200];

   /* Create Time Portion of File Name */
   ptetime_get_timestamp( time_date ); // YYYY-0M-0D-0H.0I.0S.JJJ
   getDate( time_date, date_str );     // YYYYMMDD
   getTime( time_date, time_str );     // hhmmss

   /* Get path to the log file. */
   GetPinnacleLogDirectory( DumpFileDirPath );

   /* Create full filename with directory path */
   sprintf(DumpFileName, "%sSQLDump_%s%s.log", DumpFileDirPath, date_str, time_str);

   /* Create the file */
   if ( NULL == (fpDumpFilePtr = fopen( DumpFileName, "w" )) )
   {
      retval = DUMP_APP_LOG_CREATE_ERROR;
      memset( errbuf, 0x00, sizeof(errbuf) );
      sprintf( errbuf, "Err: unable to create Dump Log File: %s", DumpFileName);
      (void)logMessage( SYSLOG_YES, WINDOW_YES, LOG_NO, errbuf, FATAL_ERROR );
   }
   else
   {
      DumpLogFileSuccess = true;
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     XipcCheckin
 *
 * Description:  This function logs the application into XIPC. This needs
 *               to be done in order to access Xipc's shared memory.
 *
 * Input:        None
 *
 * Output:       XIPC_Login_Successful - (Global) Set 'true' on success
 *
 * Return Value: DUMP_APP_SUCCESS    = Successfully logged into Xipc
 *               DUMP_APP_XIPC_ERROR = Unable to log into Xipc
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT XipcCheckin()
{
   INT    retval = DUMP_APP_SUCCESS;
   INT    log_rc;
   CHAR   errbuf[100];
   CHAR   xipc_instance[30];

   GetXipcInstanceName( xipc_instance );

   if( !pteipc_init_single_instance_app(AppName, xipc_instance) )
   {
      retval = DUMP_APP_XIPC_ERROR;
      memset( errbuf, 0x00, sizeof(errbuf) );
      strcpy( errbuf, "Err: unable to log into Xipc" );
      log_rc = logMessage(SYSLOG_YES, WINDOW_YES, LOG_YES, errbuf, FATAL_ERROR);
      if ( log_rc != DUMP_APP_SUCCESS )
      {
         strcpy( errbuf, "Err: unable to write to log file" );
         (void)logMessage(SYSLOG_YES, WINDOW_YES, LOG_NO, errbuf, FATAL_ERROR);
      }
   }
   else
   {
      XIPC_Login_Successful = true;
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     getSqlRecords
 *
 * Description:  This function reads all of the records from SQL01 in
 *               Xipc Shared Memory and puts them into an SQL01 list
 *               structure. Then each record is written to the log file
 *               one at a time.
 *
 * Input:        None
 *
 * Output:       None
 *
 * Return Value: DUMP_APP_SUCCESS          = Success all around
 *               DUMP_APP_LOG_WRITE_ERROR  = Unable to write to log file
 *               DUMP_APP_SHARED_MEM_ERROR = Unable to read shared memory
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT getSqlRecords()
{
   INT         retval = DUMP_APP_SUCCESS;
   INT         rc;
   INT         log_rc;
   INT         i;
   INT         num_recs;
   INT         applen;
   INT         maxlen = 0;
   CHAR        tmpstr[100];
   CHAR        errbuf[100] = "";
   SQL01_LIST  sqlList;

   memset( &sqlList, 0x00, sizeof(SQL01_LIST) );
   rc = ReadSQL01_MultipleRecs_Table( "", &sqlList );
   if ( rc < 0 )
   {
      /* Unable to read shared memory. */
      retval = DUMP_APP_SHARED_MEM_ERROR;
      sprintf( errbuf,
              "Err: Problem encountered reading shared memory - %d",
               rc );

      log_rc = logMessage(SYSLOG_YES, WINDOW_YES, LOG_YES, errbuf, FATAL_ERROR);
      if ( log_rc != DUMP_APP_SUCCESS )
      {
         strcpy( errbuf, "Err: unable to write to log file" );
         (void)logMessage(SYSLOG_YES, WINDOW_YES, LOG_NO, errbuf, FATAL_ERROR);
      }
   }
   else
   {
      /* The records have been read from shared memory. */
      num_recs = atoi( sqlList.num_returned );

      /* Make sure the number of records read is valid. */
      if ( num_recs > (INT)((sizeof(SQL01_LIST)-5) / sizeof(SQL01)) )
      {
         /* Invalid number of records. */
         retval = DUMP_APP_SHARED_MEM_ERROR;
         sprintf( errbuf,
                 "Err: Invalid number of records read from shared memory - %d",
                  num_recs );

         log_rc = logMessage(SYSLOG_YES,WINDOW_YES,LOG_YES,errbuf,FATAL_ERROR);
         if ( log_rc != DUMP_APP_SUCCESS )
         {
            strcpy( errbuf, "Err: unable to write to log file" );
            (void)logMessage(SYSLOG_YES, WINDOW_YES, LOG_NO, errbuf, FATAL_ERROR);
         }
      }
      else
      {
         /* Log each record into the log file
          * ---------------------------------
          * But before we do, left justify, blank fill the AppNames in the
          * list so they are all the same length (the length of the longest
          * AppName). This is to make the log file more readable.
          */
         for( i=0; i<num_recs; i++ )
         {
            applen = strlen( sqlList.sql01_list[i].appname );
            if ( applen > maxlen )
               maxlen = applen;
         }

         /* Left justify, blank fill each appname. */
         for( i=0; i<num_recs; i++ )
         {
            memset( tmpstr, 0x00, sizeof(tmpstr) );
            strcpy( tmpstr, sqlList.sql01_list[i].appname );
            sprintf( sqlList.sql01_list[i].appname, "%-*s", maxlen, tmpstr );
         }

         /* Now log the SQLs to the log file. */
         for( i=0; i<num_recs; i++ )
         {
            log_rc = logMessage( SYSLOG_NO,
                                 WINDOW_NO,
                                 LOG_YES,
                                 (pBYTE)(&sqlList.sql01_list[i]),
                                 INFO_ERROR);
            if ( log_rc != DUMP_APP_SUCCESS )
            {
               sprintf( errbuf,
                       "Err: unable to write to log file. Record# %d out of %d",
                        i+1, num_recs );
               (void)logMessage(SYSLOG_YES,WINDOW_YES,LOG_NO,errbuf,FATAL_ERROR);
            }
         }
      }
   }

   return( retval );
}


/*****************************************************************************
 *
 * Function:     cleanup
 *
 * Description:  This function closes the log file and logs out of Xipc.
 *
 * Input:        XIPC_Login_Successful - (Global) true if logged into Xipc
 *               DumpLogFileSuccess    - (Global) true if log file opened
 *
 * Output:       None
 *
 * Return Value: None
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
void cleanup()
{
   if ( DumpLogFileSuccess == true )
   {
      fflush( fpDumpFilePtr );
      fclose( fpDumpFilePtr );
   }

   if ( XIPC_Login_Successful == true )
   {
      pteipc_shutdown_single_instance_app();
   }

   return;
}
