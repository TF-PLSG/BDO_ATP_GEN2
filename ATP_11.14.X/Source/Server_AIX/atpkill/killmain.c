/***************************************************************************
 * 
 * MODULE:      KILLMAIN.C
 *  
 * TITLE:       ATP Kill Service Utility
 *  
 * DESCRIPTION: This utility takes an ATP service name as input. Then
 *              it logs into Xipc and destroys the queues and removes
 *              the Xipc user.  Note: this is for AIX usage.
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

      Rev 1.2   Jun 26 2007  DIrby
   1. Changed the order of process to kill the process first, with retries if
   necessary.  Then destroy the queues and abort the user.

   2. Added the -w input parameter to allow user to configure how many seconds
   to wait before an automatic retry of the kill process command is executed
   again.
   SCR 25003

      Rev 1.1   Apr 03 2007  DIrby
   Initial revision

****************************************************************************/

/***************************************************************************

   Logic Flow
   ----------
   1.  Input Parameters
       1.1  None = Display usage and exit
       1.2  ? = Display usage and exit
       1.3  Accept one or more service names to be stopped or killed (max 10)
       1.4  Allow for -i flag to suppress the confirmation message
       1.5  Note: service names as input are really Xipc user Ids

   2.  Confirm
       2.1  Log into Xipc
       2.2  Display them for confirmation (if -i is not used)
       2.3  Get confirmation input

   3.  Kill and clean the service(s)
       3.1  Get the Aix Process ID (Pid)
       3.2  Kill the Pid with -9
       3.2.1  If unable to kill process
       3.2.2    Display message (do not log it)
       3.2.3    Wait specified number of seconds (default = 2 secs)
       3.2.4    Go to step 3.2
       3.3  Get the Queue ID (Qid) using "QueAccess <service_name + C>"
       3.4  Use "QueInfoQue 'Qid' " to get the User ID (Uid)
       3.5  Destroy the C Qid
       3.6  XipcAbort the Uid
       3.7  Get the Queue ID (Qid) using "QueAccess <service_name + I>"
       3.8  Destroy the I Qid
       3.9  Note: Do not destroy the A queue because of multi instances
                  unless user requests it be destroyed. Repeat step 3.7
                  and 3.8 for the A Qid if desired.
       3.10 Repeat for other services

   4.  Cleanup
       4.1  Check out of Xipc if checkin was successful.
       4.2  Exit

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
#include "kill_constants.h"
#include "kill_prototypes.h"

/*+--------------------+
  | Global Definitions |
  +--------------------+*/

INT    destroyAQueue;
INT    suppressConfirmMsg;
INT    displayPidKillCommand;
INT    killRetryWaitPeriod = DEFAULT_KILL_RETRY_WAIT_TIME;
INT    numServices;
INT    localUid;
CHAR   AppName[80];
CHAR   serviceList[10][80];
CHAR   Version[] = "ATP_11.1.0";

/* XIPC */
INT    XIPC_Login_Successful;
CHAR   xipc_instance[30];


/*****************************************************************************
 *
 * Function:     main
 *
 * Description:  This function is the entry point for the application.
 *               It drives the logic flow as indicated in the above
 *               comments.
 *
 *               Inputs are either a question mark, or a list of one or
 *               service names to be cleanly shutdown.
 *
 * Input:        argc   - number of input parameters
 *               argv[] - list of input parameters including application name
 *
 * Output:       None
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -1 Failure related to Xipc
 *              -2 Failed to destroy 1 or more queues or Xipc users
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
   INT  retval = KILL_APP_SUCCESS;
   INT  i,j;
   INT  len;
   INT  maxInputs;
   INT  invalidInput = false;
   CHAR invalidFlag[256] = "";

   /* ------------------------- */
   /* Initialization of Globals */
   /* ------------------------- */
   XIPC_Login_Successful = false;
   suppressConfirmMsg    = false;
   destroyAQueue         = false;
   displayPidKillCommand = false;
   numServices = 0;

   memset( AppName,     0x00, sizeof(AppName)     );
   memset( serviceList, 0x00, sizeof(serviceList) );

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

   if ( argc == 1 )
   {
      Usage();
   }
   else if ( 0 == strcmp("?", argv[1]) )
   {
      Usage();
   }
   else
   {
      /* ------------------------------------------------------ */
      /* Get the list of services to shutdown and input options */
      /* ------------------------------------------------------ */
      maxInputs = 11;  /* 10 app names + the program name */
      for( i=1,j=0; i<argc; i++ )
      {
         if ( 0 == strcmp("-i", argv[i]) )
         {
            /* Look for -i flag to ignore confirmation message. */
            suppressConfirmMsg = true;
         }
         else if ( 0 == strcmp("-a", argv[i]) )
         {
            /* Look for -a flag to destroy the A queue. */
            destroyAQueue = true;
         }
         else if ( 0 == strcmp("-d", argv[i]) )
         {
            /* Look for -d flag to display system command that kills the pid. */
            displayPidKillCommand = true;
         }
         else if ( 0 == strcmp("-w", argv[i]) )
         {
            /* Look for -w flag to get # of secs to wait between kill retries */
            if ( i+1 >= argc )
               invalidInput = true;
            else
               killRetryWaitPeriod = atoi( argv[++i] );

            if ( killRetryWaitPeriod < 0 )
               killRetryWaitPeriod = DEFAULT_KILL_RETRY_WAIT_TIME;

            else if ( killRetryWaitPeriod > 500 )
               killRetryWaitPeriod = DEFAULT_KILL_RETRY_WAIT_TIME;
         }
         else if ( argv[i][0] != '-' )
         {
            /* Copy service into our list of services to shutdown. */
            strcpy( serviceList[j++], argv[i] );
            numServices++;
         }
         else
         {
            /* Invalid input flag. Expecting only -i, -a, and -d. */
            invalidInput = true;
            strcpy( invalidFlag, argv[i] );
         }
      }

      if ( numServices > maxInputs )
      {
         printf( "\n\nExceeded maximum of 10 services to shutdown\n\n" );
      }
      else if ( invalidInput )
      {
         printf( "\n\nInvalid input: %s\nTry '?' for usage.\n\n", invalidFlag );
      }
      else if ( numServices == 0 )
      {
         printf( "\n\nYou didn't list any services to shutdown."
                 "\nTry ? for usage.\n\n");
      }
      else
      {
         /* --------------- */
         /* Check into XIPC */
         /* --------------- */
         retval = XipcCheckin();
         if ( retval == KILL_APP_SUCCESS )
         {
            /* ------------------------ */
            /* Confirmation of input(s) */
            /* ------------------------ */
            if ( KILL_APP_CONFIRM_YES == ConfirmInputs() )
            {
               /* Get Uid of our application (atpkill). */
               localUid = GetLocalUid();

               /* ------------------- */
               /* Shutdown Service(s) */
               /* ------------------- */
               for( i=0; i<numServices; i++ )
               {
                  printf( "\n" ); /* For readability of the output. */
                  (void)ShutdownService( serviceList[i] );
               }
            }
         }
      }
   }

   /* ------------------------------ */
   /* Perform Cleanup before Exiting */
   /* ------------------------------ */
   Cleanup();

   return( retval );
}


/*****************************************************************************
 *
 * Function:     Usage
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
void Usage()
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

   printf( "Description: This application will shutdown one or more ATP  \n" );
   printf( "applications running in AIX. It will do so in a clean manner.\n" );
   printf( "This means not only does it shut down the application, but it\n" );
   printf( "cleans up the Xipc queues and user IDs associated with the   \n" );
   printf( "application. Only the 'C' and 'I' queues are destroyed.      \n" );
   printf( "The 'A' queue is not destroyed so that multiple instances of \n" );
   printf( "an application can continue to run. Use the -a option to     \n" );
   printf( "force the removal of the 'A' queue along with the other      \n" );
   printf( "queues.                                                    \n\n" );
   printf( "This application can accept up to 10 application names to be \n" );
   printf( "shutdown; they must be space delimited.                    \n\n" );
   printf( "A confirmation request will be issued to the user to ensure  \n" );
   printf( "the inputs. This feature can be overridden with the -i option\n" );
   printf( "                                                             \n" );

   printf( "Usage: %s [?] [-i][-a][-w secs][app1][app2]...[app10]\n\n", AppName );
   printf( "         ? = Display usage of the application           \n" );
   printf( "        -i = ignore/suppress confirmation request       \n" );
   printf( "        -a = destroy 'A' queue with the other queues    \n" );
   printf( "        -d = display system command used to kill Pid    \n" );
   printf( "        -w = if unable to kill pid, wait 'secs' seconds \n" );
   printf( "             and try again. 0 = no retries.             \n" );
   printf( "             Retry up to 100 times.                     \n" );
   printf( "       app = Name of an ATP application such as devds01 \n" );

   printf( "\nReturn Values:\n" );
   printf( "   0 = Success          \n");
   printf( "  -1 = Error condition relating to logging into Xipc    \n" );
   printf( "  -2 = Unable to destroy 1 or more queues or Xipc users \n" );

   return;
}


/*****************************************************************************
 *
 * Function:     XipcCheckin
 *
 * Description:  This function logs the application into XIPC so we can
 *               access the queues and users.
 *
 * Input:        None
 *
 * Output:       XIPC_Login_Successful - (Global) Set 'true' on success
 *
 * Return Value: KILL_APP_SUCCESS    = Successfully logged into Xipc
 *               KILL_APP_XIPC_ERROR = Unable to log into Xipc
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
   INT    retval = KILL_APP_SUCCESS;
   CHAR   xipc_instance[30];

   GetXipcInstanceName( xipc_instance );

   if( !pteipc_init_single_instance_app(AppName, xipc_instance) )
   {
      retval = KILL_APP_XIPC_ERROR;
      printf( "\n\nError: unable to log %s into Xipc\n\n", AppName );
   }
   else
   {
      XIPC_Login_Successful = true;
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     ConfirmInputs
 *
 * Description:  This function will display the names of the applications
 *               to be shut down and prompt the user for confirmation.
 *
 * Input:        serviceList        - (Global) List of applications
 *               suppressConfirmMsg - (Global) prompt (or not) for confirmation
 *
 * Output:       None
 *
 * Return Value: KILL_APP_CONFIRM_YES = Continue to shutdown applications
 *               KILL_APP_CONFIRM_NO  = Exit the program
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT ConfirmInputs()
{
   INT   retval = KILL_APP_CONFIRM_YES;
   INT   i;
   INT   ctr = 0;
   CHAR  instr[255] = "";

   if ( !suppressConfirmMsg )
   {
      printf( "\n\n" );
      if ( numServices == 1 )
      {
         do
         {
            printf( "Shutdown %s (Y or N): ", serviceList[0] );
            scanf( "%s", instr );
            instr[0] = toupper( instr[0] );
            if ( ++ctr > 5 )
            {
               if ( 0 != strcmp("Y",instr) )
                  instr[0] = 'N'; /* Exit, too many failed attempts. */
            }

         } while( (strlen(instr) != 1) || (instr[0] != 'Y' && instr[0] != 'N'));
      }
      else
      {
         do
         {
            printf( "Confirm shutdown of these applications:\n" );
            for( i=0; i<numServices; i++ )
               printf( "\t%s\n", serviceList[i] );

            printf( "\nShutdown all of the above (Y or N): " );
            scanf( "%s", instr );
            instr[0] = toupper( instr[0] );
            printf( "\n\n" );
            if ( ++ctr > 5 )
            {
               if ( 0 != strcmp("Y",instr) )
                  instr[0] = 'N'; /* Exit, too many failed attempts. */
            }

         } while( (strlen(instr) != 1) || (instr[0] != 'Y' && instr[0] != 'N'));
      }

      if ( instr[0] == 'N' )
      {
         retval = KILL_APP_CONFIRM_NO;
         if ( numServices == 1 )
            printf( "\nApplication was not shutdown.\n\n" );
         else
            printf( "\nApplications were not shutdown.\n\n" );
      }
   }

   return( retval );
}


/*****************************************************************************
 *
 * Function:     ShutdownService
 *
 * Description:  This function shuts down a single service/application.
 *               It gets the PID, kills the app with a -9, then destroys
 *               the C and I queues.  If the -a option has been entered
 *               by the user, the A queue is also destroyed.  Lastly, the
 *               Xipc user id (UID) is removed (aborted).
 *
 * Input:        service       - Name of service or application to be shutdown
 *               destroyAQueue - (Global) True or False
 *
 * Output:       None
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -2 Failed to destroy 1 or more queues or Xipc users
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *  1.2     06/26/07 DI   Changed process order to kill the process first,
 *                        with retries if desired. Then get rid of the queues
 *                        and user only after the process is killed.
 *                        SCR 25003
 *
 *****************************************************************************/
INT ShutdownService( pCHAR service )
{
   INT         i;
   INT         retval        = KILL_APP_SUCCESS;
   INT         failure       = false;
   INT         userKilled    = false;
   INT         processKilled = false;
   XINT        Qid = -1;
   XINT        Uid = -1;
   XINT        Pid = -1;
   CHAR        sysCommand[200] = "";
   CHAR        errbuf[100] = "";
   CHAR        queName[MAX_QUE_NAME_SIZE];

   /*+------------------+
     | Kill the Process |
     +------------------+*/

   /* Kill the process (with retry if necessary and if desired). */
   for( i=0; i<MAX_KILL_ATTEMPTS && processKilled == false; i++ )
   {
      /* Kill the Process using an AIX Script system command:
       *
       * kill -9 `ps -ef | awk '{gsub(/\//,"x");print}' | fgrep -w %s | 
       * fgrep -v grep | fgrep -v %s | 
       * awk '{ while( sub(/^[ ]/,"") );while (gsub(/[ ]{2,}/," ",$0));
       * ;wordcount=split($0,a,/[ ]/); print a[2] }'` 
       *  2>/dev/null
       */
      sprintf( sysCommand,
               "kill -9 `ps -ef | grep $ASCENDENTROOT | awk '{gsub(/\\//,\"x\");print}' | fgrep -w %s | "
               "fgrep -v grep | fgrep -v %s | "
               "awk '{ while( sub(/^[ ]/,\"\") );while (gsub(/[ ]{2,}/,\" \",$0));"
               ";wordcount=split($0,a,/[ ]/); print a[2] }'` "
               " 2>/dev/null",  /* Do not display errors. */

               service, AppName );

      if ( displayPidKillCommand == true )
      {
         /* Display the OS system command used to kill the process. */
         printf( "\n" );
         printf( "-----------------------------------------------------\n" );
         printf( "  OS System Command used to kill the process         \n" );
         printf( "-----------------------------------------------------\n" );
         printf( "%s\n", sysCommand );
         printf( "-----------------------------------------------------\n" );
         printf( "\n" );
      }

      #ifndef WIN32
         /* This is AIX Specific. */
         retval = system( sysCommand );
         if ( (retval == 0) || (retval == 256) ) /* 256=no Pid to kill */
         {
            processKilled = true;
         }
         else
         {
            failure = true;
            printf( "Error %d killing Process ID for %s, waiting %d seconds...\n",
                     retval, service, killRetryWaitPeriod );

            /* This function accepts number of microseconds to sleep/wait. */
            usleep( killRetryWaitPeriod * 1000000 );
         }
      #endif

      /* ----------------------------------------------------
       * This block of code uses the Xipc User Id to
       * get the process Id and then kill the process.
       * Keep this code for future use in case we expand
       * this utility to other operating systems. However,
       * it must be put into the FOR loop below so we have
       * the Uid and queName.
       *
      if ( (Uid > 0) && (Uid != localUid) )
      {
         * Get PID for the User; use UID. *
         retval = GetProcessId( Uid, queName, &Pid );
         if ( retval == KILL_APP_SUCCESS )
         {
            * Shutdown the application. *
            sprintf( sysCommand, "kill -9 %d", Pid );
            #ifndef WIN32
               retval = system( sysCommand );
               printf( "System kill -9 returns %d\n", retval );
            #endif
         }
      }
      ------------------------------------------------------- */
   }

   /*+-------------------------------+
     | Kill the Xipc Queues and User |
     +-------------------------------+*/

   if ( processKilled == true )
   {
      /* Loop for each queue (C and I and maybe A) */
      for( i=0; i<3; i++ )
      {
         /* Get the Queue ID of the service's queues. */
         memset( queName, 0x00, sizeof(queName) );
         memcpy( queName, service, sizeof(queName)-2 );
         if ( i == 0 )
            strcat( queName, "C" );
         else if ( i == 1 )
            strcat( queName, "I" );
         else if ( (i == 2) && (destroyAQueue == true) )
            strcat( queName, "A" );
         else
            break;

         retval = GetQueueId( queName, &Qid );
         if ( retval == KILL_APP_SUCCESS )
         {
            /* Get UID for the queue. */
            if ( userKilled == false )
            {
               retval = GetUserId( Qid, queName, &Uid );
               if ( (Uid == localUid) || (retval == QUE_ER_BADUID) )
               {
                  printf( "Xipc User Id %d for queue %s already removed.\n",
                           Uid, queName );
               }
            }

            /* Destroy the queue. */
            retval = DestroyQueue( Qid, queName );
            if ( retval != KILL_APP_SUCCESS )
               failure = true;

            if ( (userKilled == false) && (Uid > 0) && (Uid != localUid) )
            {
               /* Abort the user (Remove the user) */
               retval = RemoveUser( Uid, queName );
               if ( retval != KILL_APP_SUCCESS )
                  failure = true;
               else
                  userKilled = true;
            }
         }
      }
   }
   else
   {
      failure = true;
      printf( "\n\nERROR: Unable to shutdown %s\n\n", service );
   }

   if ( failure )
      retval = KILL_APP_DESTROY_ERROR;
   else
      printf( ">>> %s successfully shutdown\n", service );

   return( retval );
}


/*****************************************************************************
 *
 * Function:     GetQueueId
 *
 * Description:  This function uses the queue name to determine the
 *               queue's Id.
 *
 * Input:        queName - Name of Xipc queue
 *
 * Output:       qid - Id of the queue
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -2 Failed to get queue Id
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT GetQueueId( pCHAR queName, pINT qid )
{
   INT   retval = KILL_APP_SUCCESS;
   XINT  queueId;
   CHAR  errbuf[100] = "";

   queueId = QueAccess( queName );
   if ( queueId < 0 )
   {
      retval = KILL_APP_DESTROY_ERROR;
      pteipc_get_errormsg( (LONG)queueId, errbuf );
      if ( queueId == QUE_ER_NOTFOUND )
      {
         printf( "Queue %s already destroyed\n", queName );
      }
      else
      {
         printf( "\nError: Unable to access queue %s, Err Code %d = %s\n",
                  queName, queueId, errbuf );
      }
   }
   else
   {
      *qid = queueId;
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     DestroyQueue
 *
 * Description:  This function uses the Xipc quedestroy command to remove
 *               an Xipc queue.
 *
 * Input:        qid     - Xipc Id of queue to be destroyed
 *               queName - Name of queue being destroyed
 *
 * Output:       None
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -2 Failed to destroy queue
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT DestroyQueue( XINT qid, pCHAR queName )
{
   INT   retval = KILL_APP_SUCCESS;
   CHAR  errbuf[100] = "";

   retval = QueDestroy( qid );
   if ( retval < 0 )
   {
      pteipc_get_errormsg( (LONG)retval, errbuf );
      printf( "\nError: Unable to destroy queue %s, qid %d, Err Code %d = %s\n",
               queName, qid, retval, errbuf );
      retval = KILL_APP_DESTROY_ERROR;
   }
   else
   {
      printf( "Destroyed Xipc queue %d for %s\n", qid, queName );
   }

   return( retval );
}


/*****************************************************************************
 *
 * Function:     RemoveUser
 *
 * Description:  This function removes an Xipc user.
 *
 * Input:        uid     - Xipc User Id
 *               queName - Name of Xipc queue
 *
 * Output:       None
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -2 Failed to remove Xipc user
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT RemoveUser( XINT uid, pCHAR queName )
{
   INT   retval = KILL_APP_SUCCESS;
   CHAR  errbuf[100] = "";

   if ( uid != localUid )
   {
      retval = XipcAbort( uid );
      if ( retval < 0 )
      {
         pteipc_get_errormsg( (LONG)retval, errbuf );
         if ( retval == QUE_ER_BADUID )
         {
            printf( "Xipc user %d already removed.\n", uid );
         }
         else
         {
            printf( "\nError: Unable to remove Xipc user %s,"
                    " uid %d Err Code %d = %s\n",
                     queName, uid, retval, errbuf );
         }
         retval = KILL_APP_DESTROY_ERROR;
      }
      else
      {
         printf( "Removed Xipc user %d for %s\n", uid, queName );
      }
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     GetUserId
 *
 * Description:  This function uses the queue Id to get the Xipc User Id.
 *
 * Input:        qid     - Id of the queue
 *               queName - Name of Xipc queue 
 *
 * Output:       uid - Id of the user
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -2 Failed to get user Id
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT GetUserId( XINT qid, pCHAR queName, pINT uid )
{
   INT         retval = KILL_APP_SUCCESS;
   CHAR        errbuf[100] = "";
   QUEINFOQUE  queInfo;

   memset( &queInfo, 0x00, sizeof(QUEINFOQUE) );
   retval = QueInfoQue( qid, &queInfo );
   if ( retval < 0 )
   {
      *uid = retval;
      pteipc_get_errormsg( (LONG)retval, errbuf );
      printf( "\nError: Unable to get Xipc User Id for %s, Qid %d, Err Code %d = %s\n",
               queName, qid, retval, errbuf );
      retval = KILL_APP_DESTROY_ERROR;
   }
   else
   {
      *uid = queInfo.CreateUid;
   }

   return( retval );
}


/*****************************************************************************
 *
 * Function:     GetProcessId
 *
 * Description:  This function uses the queue Id to get the Xipc User Id.
 *
 * Input:        uid     - Id of Xipc User
 *               queName - Name of Xipc queue
 *
 * Output:       pid - Operating System Process Id of the application
 *
 * Return Value: 0 for success = no errors occurred during processing
 *              -2 Failed to get process Id
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT GetProcessId( XINT uid, pCHAR queName, pINT pid )
{
   INT         retval = KILL_APP_SUCCESS;
   CHAR        errbuf[100] = "";
   QUEINFOUSER usrInfo;

   memset( &usrInfo, 0x00, sizeof(QUEINFOUSER) );
   retval = QueInfoUser( uid, &usrInfo );
   if ( retval < 0 )
   {
      if ( retval != QUE_ER_BADUID )
      {
         *pid = retval;
         pteipc_get_errormsg( (LONG)retval, errbuf );
         printf( "\n\nError: Unable to get process Id (PID) for %s, Usr %d, Err Code %d = %s\n\n",
                  queName, uid, retval, errbuf );
         retval = KILL_APP_DESTROY_ERROR;
      }
   }
   else
   {
      *pid = usrInfo.Pid;
   }

   return( retval );
}


/*****************************************************************************
 *
 * Function:     GetLocalUid
 *
 * Description:  This function returns the Xipc User ID of this application.
 *               This is necessary because when this application checks into
 *               Xipc, it will take the first available Uid. There might by
 *               hanging queues that still refer to this Uid. When a C or I
 *               queue is destroyed, the Xipc user is removed as well. Then
 *               later, the customer might decide to get rid of the A queue.
 *               The Xipc user Id being referred to by the A queue might be
 *               the one in use by this application when it checked into Xipc.
 *
 * Input:        AppName - (Global) Name of this application
 *
 * Output:       None
 *
 * Return Value: Xipc User ID of this application
 *
 *
 *   File
 * Revision   Date   By   Description
 * -------- -------- ---  -----------
 *
 *
 *****************************************************************************/
INT GetLocalUid()
{
   INT         localUid;
   INT         Qid;
   CHAR        queName[MAX_QUE_NAME_SIZE];

   /* Get the Queue ID of the application's A queue. */
   memset( queName, 0x00, sizeof(queName) );
   memcpy( queName, AppName, sizeof(queName)-2 );
   strcat( queName, "A" );

   (void)GetQueueId( queName, &Qid );

   /* Get UID for the queue. */
   (void)GetUserId( Qid, queName, &localUid );

   return( localUid );
}


/*****************************************************************************
 *
 * Function:     Cleanup
 *
 * Description:  This function logs out of Xipc.
 *
 * Input:        XIPC_Login_Successful - (Global) true if logged into Xipc
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
void Cleanup()
{
   if ( XIPC_Login_Successful == true )
   {
      pteipc_shutdown_single_instance_app();
   }

   return;
}
