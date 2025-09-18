/******************************************************************************
  
        Module: pteipc.c
  
         Title: Pinnacle Inter-Process Communication (IPC) Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\pteipc_lib\PTEIPC.C  $
   
      Rev 1.29   Jan 18 2000 11:53:32   MSALEH
   modify sleep time to be MS for
   consistency
   
      Rev 1.28   Jan 17 2000 17:03:00   MSALEH
   add pteipc_receive_r() for multi threaded
   applications on unix to address blocking
   xipc calls
   
      Rev 1.27   Jan 17 2000 08:56:52   svelaga
   Gabriel :
   
   Modified code of pteipc_receive routine for AIX.
   
   
      Rev 1.26   Dec 30 1999 12:00:52   MSALEH
   add getcoreversion() to init..() functions
   
      Rev 1.25   Nov 10 1999 17:53:10   MSALEH
   added pteipc_format_xipc_instance_name()
   
      Rev 1.24   Oct 22 1999 11:35:54   MSALEH
   Priority based message retrieval
   
      Rev 1.23   Oct 15 1999 11:15:02   MSALEH
   Move SSL_SUPPORT to preprocessor definitions
   in the project files
   
      Rev 1.22   Sep 29 1999 14:43:06   MSALEH
   AIX Modifications
   
      Rev 1.21   Sep 28 1999 14:45:16   MSALEH
   AIX Modifications
   
      Rev 1.20   Aug 30 1999 16:56:40   MSALEH
   modified pteipc_checkin() to correct bug
   
      Rev 1.19   Aug 27 1999 16:27:56   MSALEH
   add pteipc_checkin() to send the PID to 
   service manager
   
      Rev 1.18   Aug 26 1999 11:24:14   MSALEH
   add pteipc_quedestroy
   
      Rev 1.17   Aug 06 1998 15:07:44   drodden
   Changed the send-receive function to always free the outgoing
   (send) message, even if there is an error.
   
   
      Rev 1.16   Jul 20 1998 08:58:50   drodden
   Changed the trace routine to maintain a static busy flag to keep multiple 
   threads from writing to the trace file at the same time.  This is not the best 
   sychronization method, but is better than nothing.
   
   
      Rev 1.15   Jun 04 1998 17:01:02   drodden
   Changed the init routines to call pteipc login rather than the xipc 
   login API directly.  Then moved the trace filename init call to the 
   pteipc login function so that it occurrs in a common spot and every 
   application can have tracing.  Programs that do not use the pteipc_init
   routines should still call the pteipc login.
   
   
      Rev 1.14   May 27 1998 13:56:30   drodden
   Changed the init code to look for the IPC_TRACE environment variable.  This is
   a list of service/app names that should start an IPC trace when they load.  If this 
   service's/app's name is in that list, it starts a trace automatically.
   
   
      Rev 1.13   May 18 1998 13:30:04   drodden
   Changed to write the originating system name to the trace file.
   
   
      Rev 1.12   17 Apr 1998 11:29:32   MSALEH
   added :
   pteipc_que_spool(que_name, base_name);
   pteipc_que_spool_off(que_name);
   to enable/disable queue spooling
   
   
      Rev 1.11   16 Apr 1998 16:09:10   MSALEH
   remove pte_instance_name variable, all
   applications should call GetXipcInstanceName()
   and pass the name to any init function.
   
      Rev 1.10   17 Feb 1998 16:19:40   MSALEH
   added set_logger_queue()
   
      Rev 1.9   Feb 13 1998 08:13:28   drodden
   Changed to put the trace log file in the Pinnacle trace directory.
   
   
      Rev 1.8   Jan 16 1998 11:44:12   drodden
   Changed pteipc to write the current and last tranIDs to the trace file.
   
   
      Rev 1.7   Dec 23 1997 08:30:34   drodden
   Changed to not log a timeout error on timed receives.
   
   
      Rev 1.6   18 Dec 1997 11:29:20   rmalathk
   1. modified the pteipc_init_multiple_instance routine to
   create queues based on the service name.
   2. Also modified all three init routines to store the
   trace file name
   
      Rev 1.5   Dec 16 1997 12:19:38   drodden
   Changed the send-receive function to always free the outgoing
   (send) message.
   
   
      Rev 1.4   08 Dec 1997 19:01:26   rmalathk
   1.fixed bug in call to pte_XipcLogin routine
      in the init routines.
   2. Added a complete list of error codes to
       the pteipc_get_errormsg routine.
   
      Rev 1.3   Dec 01 1997 16:46:04   drodden
   Added the pte_instance_name variable for applications
   to use rather than them all using a hard coded value.
   Enhanced the IPC trace to build the trace file name using
   an application instance number so that each instance of
   a multi-instance app will have its own trace file:
      single instance:  appname.ipc
      multi instance  :  appnameNN.ipc (where NN=instance #)
   Fixed the IPC trace.  It was blowing up when trying to format
   the output string for an xipc error code.
   Changed the send and receive routines to always write to the 
   trace file.  They were not always writing errors to the trace.
   
   
      Rev 1.2   Dec 01 1997 09:18:34   drodden
   Added IPC message tracing.
   Changed the init routines to log errors if they fail.
   
   
      Rev 1.1   06 Nov 1997 12:22:22   rmalathk
   made further enhancements to the previous
   revision.
   
      Rev 1.0   Oct 23 1997 11:42:20   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#include <stdio.h>

#ifdef WIN32
#include <io.h>
#endif

#include <fcntl.h>
#include <sys/stat.h>

#include "pteipc.h"
#include "ptetime.h"
#include "ntutils.h"

#define MAX_INSTANCES 5

PRIVATE LONG      application_que_id;
PRIVATE LONG      control_que_id;
PRIVATE LONG      interactive_que_id;

PRIVATE BOOLEAN   logging           = false;
PRIVATE CHAR      logfile_name [128] = {'\0'};
PRIVATE pCHAR     newline           = "\n\0";
PRIVATE pCHAR     srvcmgr_que       = "srvcmgrA";

CHAR  application_que_name [MAX_QUE_NAME_SIZE];
CHAR  control_que_name     [MAX_QUE_NAME_SIZE];
CHAR  interactive_que_name [MAX_QUE_NAME_SIZE];
CHAR  logger_que_name      [MAX_QUE_NAME_SIZE];



/******************************************************************************
******************************************************************************/
void set_logger_que()
{
	GetLoggerQueName( logger_que_name );
}



/******************************************************************************
*  function build_trace_filename
*
*  This function will write the data buffer to the trace
*  file as a hex ascii dump.
*
*  Parameters:
*     comment......optional comment/text to be written on the line after the
*                  timestamp.  Pass NULL_PTR if a comment isn't desired.
*     p_msg........pointer to the message to be sent or just received.
*     outgoing.....pass true to indicate an outgoing message; false for incoming.
*
******************************************************************************/
void build_trace_filename ( pCHAR app_name )
{
   CHAR     trace_list [256];
   CHAR     seps [] = " ,";
   pCHAR    token;


   /* if already built, leave it alone */
   if (logfile_name [0] != '\0')
      return;

   memset (logfile_name, 0, sizeof(logfile_name));
   GetPinnacleTraceDirectory (logfile_name);
   strcat (logfile_name, app_name);
   strcat (logfile_name, ".ipc");

   GetIpcTraceList (trace_list);

   if (trace_list [0] != '\0')
   {
      token = strtok (trace_list, seps);
   
      while (token != NULL_PTR)
      {
#ifdef WIN32
         if (stricmp (app_name, token) == 0)
#else
         if (strcmp (app_name, token) == 0)
#endif
         {
            pteipc_trace_on ();
            break;
         }

         /* Get next token: */
         token = strtok (NULL_PTR, seps);
      }
   }
}



/******************************************************************************
*  function write_to_trace_file
*
*  This function will write the data buffer to the trace
*  file as a hex ascii dump.
*
*  Parameters:
*     comment......optional comment/text to be written on the line after the
*                  timestamp.  Pass NULL_PTR if a comment isn't desired.
*     p_msg........pointer to the message to be sent or just received.
*     outgoing.....pass true to indicate an outgoing message; false for incoming.
*
******************************************************************************/
void write_to_trace_file ( pCHAR    p_comment,
                           pPTE_MSG p_msg,
                           BOOLEAN  outgoing,
                           LONG     rcode     )
{
   static   BOOLEAN  trace_busy = false;

   CHAR  temp_str [80];
   char  err_str  [60]; 
   int   file;
   int   col_index;
   int   char_count;
   LONG  line_count;
   LONG  num_lines; 
   LONG  buf_index;
   LONG  msg_len;
   LONG  data_len;
   pBYTE p_data;
   pPTE_MSG_DATA  p_msg_data;


   if (logging == false)
      return;

   while (trace_busy == true)
   {
   }

   trace_busy = true;

#ifdef WIN32
   file = open (logfile_name, O_APPEND|O_CREAT|O_TEXT|O_RDWR, S_IREAD|S_IWRITE);
#else
   file = open (logfile_name, O_APPEND|O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
#endif

   if (file == -1)
   {
      trace_busy = false;
      return;
   }


   /*
    * This is the basic format of the trace message.
    * notes:
    *    1) the numeric and alpha data following the labels are variable
    *       length and may be zero or more characters in length.
    *    2) the hex dump is of the optional app data following the PTE
    *       message header and may not be present.
    *
    * 1998-00-16-10.55.04.581003
    * outgoing message length: nnnnnn
    * message type: nnn     subtype1: nnn     subtype2: nnn
    * originator queue: aaaaaaaaaaaaaaa     destination queue: aaaaaaaaaaaaaaa
    * this tran ID: aaaaaaaaaaaaaaaaaaaaaaaa
    * last tran ID: aaaaaaaaaaaaaaaaaaaaaaaa
    * result code: nnn
    * number chained messages: nnn     number chained data: nnn
    * xx xx xx xx xx xx xx xx - xx xx xx xx xx xx xx xx        [................]
    *
    */


   /*
    * log the current timestamp.
    */
   write (file, newline, strlen(newline));
   write (file, newline, strlen(newline));

   ptetime_get_timestamp (temp_str);
   write (file, temp_str, strlen(temp_str));
   write (file, newline,  strlen(newline));

   if (p_comment != NULL_PTR)
   {
      write (file, p_comment, strlen(p_comment));
      write (file, newline,   strlen(newline));
   }

   if (rcode < 0)
   {
      pteipc_get_errormsg (rcode, err_str);
      sprintf(temp_str, "xipc error: %ld   %s\n", rcode, err_str);
      write  (file, temp_str, strlen(temp_str));
   }

   /*
    * if there isn't a message, close the trace file and return.
    */
   if (p_msg == NULL_PTR)
   {
      close (file);
      trace_busy = false;
      return;
   }

   /*
    * write the message length and other ptemsg "fixed" fields of interest.
    */
   msg_len = ptemsg_get_msg_length (p_msg);

   if (outgoing)
      sprintf (temp_str, "outgoing message length: %ld\n", msg_len);
   else                                             
      sprintf (temp_str, "incoming message length: %ld\n", msg_len);

   write (file, temp_str, strlen(temp_str));

   
   sprintf (temp_str, "message type: %u     subtype1: %u     subtype2: %u\n",
            ptemsg_get_msg_type     (p_msg),
            ptemsg_get_msg_subtype1 (p_msg),
            ptemsg_get_msg_subtype2 (p_msg));
   write (file, temp_str, strlen(temp_str));


   sprintf (temp_str, "orig system: %s     orig queue: %s     dest queue: %s\n",
            ptemsg_get_msg_orig_system (p_msg),
            ptemsg_get_msg_orig_queue (p_msg),
            ptemsg_get_msg_dest_queue (p_msg));
   write (file, temp_str, strlen(temp_str));


   sprintf (temp_str, "this transaction ID: %s\n",
            ptemsg_get_current_msg_tran_id (p_msg));
   write (file, temp_str, strlen(temp_str));

   
   sprintf (temp_str, "last transaction ID: %s\n",
            ptemsg_get_last_msg_tran_id (p_msg));
   write (file, temp_str, strlen(temp_str));


   sprintf (temp_str, "result code: %u\n",
            ptemsg_get_result_code (p_msg));
   write (file, temp_str, strlen(temp_str));


   sprintf (temp_str, "number chained messages: %d     number chained data: %d\n",
            ptemsg_get_num_chained_messages (p_msg),
            ptemsg_get_num_chained_data     (p_msg));
   write (file, temp_str, strlen(temp_str));


   /*
      format every sixteen bytes of the data into a trace line.
      "xx xx xx xx xx xx xx xx - xx xx xx xx xx xx xx xx       [................]"
      this outer for-loop controls the line count.
   */
   p_msg_data = ptemsg_get_pte_msg_data (p_msg);

   if (ptemsg_get_pte_msg_data_data_type (p_msg_data) == APPLICATION_DATA)
   {
      sprintf (temp_str, "app data type: %u\n", ptemsg_get_pte_msg_data_app_data_type (p_msg_data));
      write (file, temp_str, strlen(temp_str));
   }

   p_data   = ptemsg_get_pte_msg_data_data        (ptemsg_get_pte_msg_data (p_msg));
   data_len = ptemsg_get_pte_msg_data_data_length (ptemsg_get_pte_msg_data (p_msg));

   buf_index = 0;
   num_lines = (data_len / 16);

   if ((data_len % 16) != 0)
      num_lines++;

   if (num_lines > 16)           /* only dump first 128 bytes (or 16 lines) of data */
      num_lines = 16;

   for (line_count=0; line_count < num_lines; line_count++)
   {
      /*
         this while loop will format the next sixteen bytes into
         the hex ascii digits on the left side of the trace line.
      */
      col_index  = 0;
      char_count = 0;

      while ( (char_count < 16) && ((buf_index+char_count) < data_len) )
      {
         sprintf (&temp_str [col_index], "%02x ", p_data[(buf_index+char_count)]);

         char_count++;
         col_index+= 3;

         if (char_count == 8)
         {
            temp_str [col_index + 0] = '-';
            temp_str [col_index + 1] = ' ';
            col_index+= 2;
         }
      }  /* while */


      /*
         this while loop will format the same sixteen bytes into the ascii
         text display on the right end of the trace line.  Non-printable
         characters are represented by a ".".
      */
      memset (&temp_str [col_index], ' ', sizeof(temp_str) - col_index - 1);
      temp_str [57] = '[';
      col_index  = 58;
      char_count = 0;
      
      while (char_count < 16)
      {
         if ((buf_index+char_count) < data_len) 
         {
            if ( (p_data[(buf_index+char_count)] > 31 ) &&
                 (p_data[(buf_index+char_count)] < 127)    )
               temp_str [col_index] = p_data[(buf_index+char_count)];
            else
               temp_str [col_index] = '.';
         }

         col_index++;
         char_count++;
      }  /* while */

      /*
         finish the trace line by adding the closing bracket and a newline,
         then write it to the log.
      */
      temp_str [col_index  ] = ']';
      temp_str [col_index+1] = '\0';
      strcat (temp_str, newline);
      write (file, temp_str, strlen(temp_str));

      buf_index+= 16;
   }  /* for line_count */

   close (file);

   trace_busy = false;
}



/******************************************************************************
*  function pteipc_trace_on
*
*  This function start an IPC trace file that will capture all incoming/outgoing 
*  messages for this application.  The trace file is named: [appname].ipc
*
*  Parameters: none
*
******************************************************************************/
void pteipc_trace_on ()
{
   logging = true;
}



/******************************************************************************
*  function pteipc_trace_off
*
*  This function stop the IPC tracing.
*
*  Parameters: none
*
******************************************************************************/
void pteipc_trace_off ()
{
   logging = false;
}

/******************************************************************************
*  function pteipc_format_xipc_instance_name
*
*  This function format a string using @xxxx:yyyy
*  where xxxx = host name, yyyy = instance name
*
*  Parameters: pointer to string to hold the result
*
******************************************************************************/
void pteipc_format_xipc_instance_name ( pCHAR xipc_instance )
{
   CHAR instance[MAX_INSTANCE_NAME_SIZE];
   CHAR host    [MAX_INSTANCE_NAME_SIZE];

   GetXipcInstanceName(instance);
   GetXipcHostName(host);
   sprintf(xipc_instance, "@%s:%s", host, instance);
}

/******************************************************************************
*
******************************************************************************/
BOOLEAN pteipc_init_gui_app( pCHAR app_name, pCHAR instance_name )
{
   LONG rcode;
   BYTE temp_inst[MAX_INSTANCE_NAME_SIZE];

   set_logger_que();

   memset (&application_que_name, 0, sizeof(application_que_name));
   memset (&control_que_name,     0, sizeof(control_que_name    ));
   memset (&interactive_que_name, 0, sizeof(interactive_que_name));
   application_que_id = -1;
   control_que_id     = -1;
   interactive_que_id = -1;

   /* print the version of the core the current application is using */
   GetCoreVersion( app_name );

   /* 
    *  first try to login to the xipc instance
    *  use the app name as the user name
    */

   pteipc_format_xipc_instance_name( temp_inst );

   if( ( rcode = pteipc_xipclogin( temp_inst, app_name ) ) < 0 )
   {
      LogEvent ("pteipc_xipclogin failed", ERROR_MSG);
      return( FALSE );
   }

   /* 
    *  since this is a GUI app, create only a interactive queue 
    */
   sprintf( interactive_que_name, "%sI", app_name );

   if( ( interactive_que_id = QueCreate( interactive_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
   {
      if( ( interactive_que_id = QueAccess( interactive_que_name ) ) < 0 )
      {
         LogEvent ("Could not access the interactive queue", ERROR_MSG);
         return( FALSE );
      }

      /*
       *  I was able to access the existing queue.
       *  Do I delete and re-create it.....or use the existing queue?????
       */
      QueDestroy( interactive_que_id );

      if ( ( interactive_que_id = QueCreate( interactive_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
      {
         LogEvent ("Could not create the interactive queue", ERROR_MSG);
         return( FALSE );
      }
   }

   rcode = pteipc_checkin(app_name);
   if (rcode < 0)
      LogEvent("pteipc_checkin: Failed", ERROR_MSG);

   return( TRUE );
}



/******************************************************************************
*
******************************************************************************/
BOOLEAN pteipc_init_single_instance_app( pCHAR app_name, pCHAR instance_name )
{
   LONG rcode;
   BYTE temp_inst[MAX_INSTANCE_NAME_SIZE];
   BYTE tempstr[80] = "";

   set_logger_que();

   memset (&application_que_name, 0, sizeof(application_que_name));
   memset (&control_que_name,     0, sizeof(control_que_name    ));
   memset (&interactive_que_name, 0, sizeof(interactive_que_name));
   application_que_id = -1;
   control_que_id     = -1;
   interactive_que_id = -1;

   /* print the version of the core the current application is using */
   GetCoreVersion( app_name );

   /*
    *  single instance application.
    *  first try to login to the xipc instance
    *  use the app name as the user name
    */


   pteipc_format_xipc_instance_name( temp_inst );

   if( ( rcode = pteipc_xipclogin( temp_inst, app_name ) ) < 0 )
   {
      sprintf( tempstr,
              "pteipc_xipclogin failed. rcode = %d",
               rcode );
      LogEvent (tempstr, ERROR_MSG);

      sprintf( tempstr,
              "app_name: %s, instance: %s",
               app_name, temp_inst );
      LogEvent( tempstr, ERROR_MSG );
      return( FALSE );
   }

   /*
    *  Create the Application Queue. Control Queue and an Interactive Queue
    */
   sprintf( application_que_name, "%sA", app_name );

   if( ( application_que_id = QueCreate( application_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
   {
      if( ( application_que_id = QueAccess( application_que_name ) ) < 0 )
      {
         LogEvent ("Could not access the application queue", ERROR_MSG);
         return( FALSE );
      }

      /*
       *  I was able to access the existing queue.
       *  Do I delete and re-create it.....or use the existing queue?????
       */
      QueDestroy( application_que_id );

      if ( ( application_que_id = QueCreate( application_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
      {
         LogEvent ("Could not create the application queue", ERROR_MSG);
         return( FALSE );
      }
   }


   /*
    *  Create the Interactive Queue.
    */
   sprintf( interactive_que_name, "%sI", app_name );

   if( ( interactive_que_id = QueCreate( interactive_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
   {
      if( ( interactive_que_id = QueAccess( interactive_que_name ) ) < 0 )
      {
         LogEvent ("Could not access the interactive queue", ERROR_MSG);
         return( FALSE );
      }

      /*
       *  I was able to access the existing queue.
       *  Do I delete and re-create it.....or use the existing queue?????
       */
      QueDestroy( interactive_que_id );

      if ( ( interactive_que_id = QueCreate( interactive_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
      {
         LogEvent ("Could not create the interactive queue", ERROR_MSG);
         return( FALSE );
      }
   }


   /*
    *  Create the Control Queue.
    */
   sprintf( control_que_name, "%sC", app_name );

   if( ( control_que_id = QueCreate( control_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
   {
      if( ( control_que_id = QueAccess( control_que_name ) ) < 0 )
      {
         LogEvent ("Could not access the control queue", ERROR_MSG);
         return( FALSE );
      }

      /*
       *  I was able to access the existing queue.
       *  Do I delete and re-create it.....or use the existing queue?????
       */
      QueDestroy( control_que_id );

      if ( ( control_que_id = QueCreate( control_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
      {
         LogEvent ("Could not create the control queue", ERROR_MSG);
         return( FALSE );
      }
   }

   rcode = pteipc_checkin(app_name);
   if (rcode < 0)
      LogEvent("pteipc_checkin: Failed", ERROR_MSG);

   return( TRUE );
}



/******************************************************************************
*
******************************************************************************/
BOOLEAN pteipc_init_multiple_instance_app( pCHAR app_name, pCHAR service_name, pCHAR instance_name )
{
   LONG  rcode;
   BYTE  temp_inst[MAX_INSTANCE_NAME_SIZE];

   set_logger_que();

   memset (&application_que_name, 0, sizeof(application_que_name));
   memset (&control_que_name,     0, sizeof(control_que_name    ));
   memset (&interactive_que_name, 0, sizeof(interactive_que_name));
   application_que_id = -1;
   control_que_id     = -1;
   interactive_que_id = -1;

   /* print the version of the core the current application is using */
   GetCoreVersion( service_name );

   /*first try to login to the xipc instance
      use the service name as the user name
   */
   pteipc_format_xipc_instance_name( temp_inst );

   if( ( rcode = pteipc_xipclogin( temp_inst, service_name ) ) < 0 )
   {
      LogEvent ("pteipc_xipclogin failed", ERROR_MSG);
      return( FALSE );
   }

   /* multiple instance application. Create a Control Queue,
      an Application queue and an Interactive Queue
   */

   /* create or access the application queue for this application */
   sprintf( application_que_name, "%sA", app_name );

   if( (  application_que_id = QueAccess( application_que_name ) ) < 0 )
   {
      /* first instance of this process. create shared application queue */
      if( ( application_que_id = QueCreate( application_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
      {
         LogEvent ("Could not create the application queue", ERROR_MSG);
         return( FALSE );
      }
   }

   
   /* create the control queue for this instance */
   sprintf (control_que_name, "%sC", service_name);

   if( ( control_que_id = QueCreate( control_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
   {
      LogEvent ("Could not create the control queue", ERROR_MSG);
      return( FALSE );
   }

   /* create the interactive queue for this instance */
   sprintf (interactive_que_name, "%sI", service_name);

   if( ( interactive_que_id = QueCreate( interactive_que_name, QUE_NOLIMIT, QUE_NOLIMIT ) ) < 0 )
   {
      LogEvent ("Could not create the interactive queue", ERROR_MSG);
      return( FALSE );
   }
 
   rcode = pteipc_checkin(service_name);
   if (rcode < 0)
      LogEvent("pteipc_checkin: Failed", ERROR_MSG);

   return( TRUE );
}



/******************************************************************************
*
******************************************************************************/
void pteipc_shutdown_gui_app()
{
   QueDestroy ( interactive_que_id );
   pteipc_xipclogout ();
}



/******************************************************************************
*
******************************************************************************/
void pteipc_shutdown_single_instance_app()
{
   QueDestroy ( application_que_id );
   QueDestroy ( interactive_que_id );
   QueDestroy ( control_que_id );
   pteipc_xipclogout ();
}



/******************************************************************************
*
******************************************************************************/
void pteipc_shutdown_multiple_instance_app()
{
   /* QueDelete( application_que_id ); */
   QueDestroy( interactive_que_id );
   QueDestroy( control_que_id );
   pteipc_xipclogout ();
}



/******************************************************************************
*
******************************************************************************/
LONG  pteipc_xipclogin ( pCHAR instance_name, pCHAR username )
{
   /* store the trace file name as the app/service name passed as username*/
   build_trace_filename (username);

   return ( XipcLogin ( instance_name, username ) );
}



/******************************************************************************
*
******************************************************************************/
LONG  pteipc_xipclogout ()
{
   return ( XipcLogout () );
}



/******************************************************************************
*
******************************************************************************/
LONG pteipc_quecreate ( pCHAR que_name )
{
   return ( QueCreate ( que_name, QUE_NOLIMIT, QUE_NOLIMIT ) );
}



/******************************************************************************
*
******************************************************************************/
LONG pteipc_quedelete ( LONG que_id )
{
   return ( QueDelete ( que_id ) );
}



/******************************************************************************
*
*
******************************************************************************/
LONG pteipc_queaccess ( pCHAR que_name )
{
   return ( QueAccess ( que_name ) );
}

/******************************************************************************
*
*
******************************************************************************/
LONG pteipc_quespool ( pCHAR que_name, pCHAR base_name )
{
  LONG qid;

  qid = pteipc_queaccess ( que_name );

  if (qid >= 0)
    return ( QueSpool ( qid, base_name ) );
  else
    return ( qid );
}

/******************************************************************************
*
*
******************************************************************************/
LONG pteipc_quespool_off ( pCHAR que_name )
{
  LONG qid;

  qid = pteipc_queaccess ( que_name );

  if (qid >= 0)
    return ( QueSpool( qid, QUE_SPOOL_OFF ) );
  else
    return ( qid );
}

/******************************************************************************
*
*
******************************************************************************/
LONG pteipc_quedestroy ( pCHAR que_name )
{
  LONG qid;

  qid = pteipc_queaccess ( que_name );

  if (qid >= 0)
    return ( QueDestroy( qid ) );
  else
    return ( qid );
}

/******************************************************************************
*
******************************************************************************/
void pteipc_quepurge ( LONG que_id )
{
   QuePurge ( que_id );
}



/******************************************************************************
*
******************************************************************************/
void pteipc_sleep( LONG msecs )
{
#ifdef WIN32
   Sleep(msecs);
#else
if(msecs < 1000)
{
	usleep(msecs * 1000);
}
else
{
   sleep(msecs/1000);
}
#endif

#ifdef notdef
   /* We do not use this function becuase it is not standard on all
      unix platforms 
   */
   XipcSleep ( msecs );
#endif
}



/******************************************************************************
*
******************************************************************************/
LONG pteipc_send ( pPTE_MSG p_msg,
                   pCHAR    que_name )
{  
   LONG     que_id;
   LONG     rcode;
   QIDLIST  send_quelist;


   rcode = que_id = QueAccess (que_name);
   
   if (que_id >= 0)
   {
      QueListBuild (send_quelist, que_id, QUE_EOL);

      rcode = QueSend (QUE_Q_ANY,
                       send_quelist,
                       p_msg,
                       ptemsg_get_msg_length (p_msg),
                       1L,
                       NULL_PTR,
                       QUE_NOWAIT);
   }

   write_to_trace_file (NULL_PTR, p_msg, true, rcode);

   return( rcode );
}

/******************************************************************************
*
******************************************************************************/
LONG pteipc_send_falcon ( pPTE_MSG p_msg,
                   pCHAR    que_name, int pid )
{
   LONG     que_id;
   LONG     rcode;
   QIDLIST  send_quelist;


   rcode = que_id = QueAccess_falcon (que_name, pid);

   if (que_id >= 0)
   {
      QueListBuild (send_quelist, que_id, QUE_EOL);
      rcode = ptemsg_get_msg_length (p_msg);

      rcode = QueSend_falcon(QUE_Q_ANY,
                       send_quelist,
                       p_msg,
                       rcode,
                       1L,
                       NULL_PTR,
                       QUE_NOWAIT,
					   pid);
   }

   write_to_trace_file (NULL_PTR, p_msg, true, rcode);

   return( rcode );
}
/******************************************************************************
*
******************************************************************************/
LONG pteipc_send_hp ( pPTE_MSG p_msg,
                      pCHAR    que_name,
                      LONG     priority)
{  
   LONG     que_id;
   LONG     rcode;
   QIDLIST  send_quelist;


   rcode = que_id = QueAccess (que_name);
   
   if (que_id >= 0)
   {
      QueListBuild (send_quelist, que_id, QUE_EOL);

      rcode = QueSend (QUE_Q_ANY,
                       send_quelist,
                       p_msg,
                       ptemsg_get_msg_length (p_msg),
                       priority,
                       NULL_PTR,
                       QUE_NOWAIT);
   }

   write_to_trace_file (NULL_PTR, p_msg, true, rcode);

   return( rcode );
}

/******************************************************************************
*
******************************************************************************/
PRIVATE pPTE_MSG read_from_queues ( LONG  que_id1,
                                    LONG  que_id2,
                                    SHORT time_out,
                                    pLONG p_ret_code )
{
   pPTE_MSG p_msg;
   MSGHDR   message_hdr;
   QIDLIST  recv_quelist;
   

   p_msg = NULL_PTR;

   QueListBuild ( recv_quelist, QUE_M_EA(que_id1), QUE_M_EA(que_id2), QUE_EOL );
    
   if (time_out > 0)
   {
      *p_ret_code = QueGet ( &message_hdr,
                             QUE_Q_HP,
                             recv_quelist,
                             NULL_PTR,
                             NULL_PTR,
                             QUE_TIMEOUT(time_out) );
   }
   else 
      if (time_out == PTEIPC_NOWAIT)
      {
         *p_ret_code = QueGet ( &message_hdr,
                                QUE_Q_HP,
                                recv_quelist,
                                NULL_PTR,
                                NULL_PTR,
                                QUE_NOWAIT );
      }
      else
      {
         *p_ret_code = QueGet ( &message_hdr,
                                QUE_Q_HP,
                                recv_quelist,
                                NULL_PTR,
                                NULL_PTR,
                                QUE_WAIT );
      }

   if ( (*p_ret_code >= 0) && (message_hdr.Size > 0) )
   {
      p_msg = malloc (message_hdr.Size);

      if (p_msg == NULL_PTR)
         *p_ret_code = PTEIPC_MALLOC_ERROR;
      else
         if ( (*p_ret_code = QueRead (&message_hdr, p_msg, message_hdr.Size)) < 0 )
         {
            free (p_msg);
            p_msg = NULL_PTR;
         }
   }

   /*
    * Don't log timeout errors for timed receives.
    */
   if ( (time_out <= 0) || (*p_ret_code != QUE_ER_TIMEOUT) )
      write_to_trace_file (NULL_PTR, p_msg, false, *p_ret_code);

   return (p_msg);
}



/******************************************************************************
*
******************************************************************************/
pPTE_MSG pteipc_receive ( pCHAR que_name1,
                          pCHAR que_name2,
                          SHORT time_out,
                          pLONG p_ret_code )
{
   LONG  que_id1;
   LONG  que_id2;
   pPTE_MSG p_ret_msg = NULL_PTR;
   

   que_id1 = QUE_EOL;
   que_id2 = QUE_EOL;

   if ( (que_name1 != NULL_PTR) && (que_name1 [0] != '\0') )
   {
      que_id1 = QueAccess (que_name1);

      if ( que_id1 < 0)
      {
         *p_ret_code = que_id1;

         write_to_trace_file (NULL_PTR, NULL_PTR, false, *p_ret_code);
         return (NULL_PTR);      
      }
   }

   
   if ( (que_name2 != NULL_PTR) && (que_name2 [0] != '\0') ) 
   {
      que_id2 = QueAccess (que_name2);

      if (que_id2 < 0)
      {
         *p_ret_code = que_id2;

         write_to_trace_file (NULL_PTR, NULL_PTR, false, *p_ret_code);
         return (NULL_PTR);      
      }
   }

   return (read_from_queues (que_id1, que_id2, time_out, p_ret_code));
}


/******************************************************************************
*
******************************************************************************/
pPTE_MSG pteipc_receive_r ( pCHAR que_name1,
                            pCHAR que_name2,
                            SHORT time_out,
                            pLONG p_ret_code )
{
   LONG  que_id1;
   LONG  que_id2;
   pPTE_MSG p_ret_msg = NULL_PTR;
   

   que_id1 = QUE_EOL;
   que_id2 = QUE_EOL;

   if ( (que_name1 != NULL_PTR) && (que_name1 [0] != '\0') )
   {
      que_id1 = QueAccess (que_name1);

      if ( que_id1 < 0)
      {
         *p_ret_code = que_id1;

         write_to_trace_file (NULL_PTR, NULL_PTR, false, *p_ret_code);
         return (NULL_PTR);      
      }
   }

   
   if ( (que_name2 != NULL_PTR) && (que_name2 [0] != '\0') ) 
   {
      que_id2 = QueAccess (que_name2);

      if (que_id2 < 0)
      {
         *p_ret_code = que_id2;

         write_to_trace_file (NULL_PTR, NULL_PTR, false, *p_ret_code);
         return (NULL_PTR);      
      }
   }

   p_ret_msg = read_from_queues (que_id1, que_id2, PTEIPC_NOWAIT, p_ret_code);

   
  if(p_ret_msg == NULL_PTR)
  { 
	   pteipc_sleep(time_out*1000);
      if( *p_ret_code == QUE_ER_NOWAIT)
		   *p_ret_code = QUE_ER_TIMEOUT;
  }

  return p_ret_msg;

}


/************************************************************************
*
************************************************************************/
pPTE_MSG pteipc_send_receive ( pPTE_MSG p_msg_out,
                               pCHAR    send_queue,
                               pCHAR    recv_queue,
                               pLONG    p_ret_code )
{
   LONG     que_id;
   pPTE_MSG p_msg_in;


   p_msg_in = NULL_PTR;

   que_id = QueAccess (recv_queue);
   
   if (que_id < 0)
      *p_ret_code = que_id;
   else
   {
      QuePurge (que_id);
   
      if ( (*p_ret_code = pteipc_send (p_msg_out, send_queue)) >= 0 )
         p_msg_in = read_from_queues (que_id, QUE_EOL, 5, p_ret_code);
   }

   free (p_msg_out);
   return (p_msg_in);
}  /* pteipc_send_recieve */

/************************************************************************
*
************************************************************************/
pPTE_MSG pteipc_send_receive_hp ( pPTE_MSG p_msg_out,
                                  pCHAR    send_queue,
                                  pCHAR    recv_queue,
                                  pLONG    p_ret_code,
                                  LONG     priority)
{
   LONG     que_id;
   pPTE_MSG p_msg_in;


   p_msg_in = NULL_PTR;

   que_id = QueAccess (recv_queue);
   
   if (que_id < 0)
      *p_ret_code = que_id;
   else
   {
      QuePurge (que_id);
   
      if ( (*p_ret_code = pteipc_send_hp (p_msg_out, send_queue, priority)) >= 0 )
         p_msg_in = read_from_queues (que_id, QUE_EOL, 5, p_ret_code);
   }

   free (p_msg_out);
   return (p_msg_in);
}  /* pteipc_send_recieve_hp */

/************************************************************************
*
************************************************************************/
void pteipc_get_errormsg ( LONG ret_code, pCHAR error_msg )
{

   switch( ret_code )
   {
      case QUE_ER_NOACCESS:         strcpy( error_msg, "QUE_ER_NOACCESS" ); break;
      case QUE_ER_NOTLOGGEDIN:      strcpy( error_msg, "QUE_ER_NOTLOGGEDIN" ); break;
      /* case QUE_ER_CAPACITY:         strcpy( error_msg, "QUE_ER_CAPACITY" ); break; */
      case QUE_ER_NOSUBSYSTEM:      strcpy( error_msg, "QUE_ER_NOSUBSYSTEM" ); break;
      case QUE_ER_NOASYNC:          strcpy( error_msg, "QUE_ER_NOASYNC" ); break;
      case QUE_ER_ISFROZEN:         strcpy( error_msg, "QUE_ER_ISFROZEN" ); break;
      case QUE_ER_NOTFROZEN:        strcpy( error_msg, "QUE_ER_NOTFROZEN" ); break;
      case QUE_ER_ISMASKED:         strcpy( error_msg, "QUE_ER_ISMASKED" ); break;
      case QUE_ER_NOTMASKED:        strcpy( error_msg, "QUE_ER_NOTMASKED" ); break;
      case QUE_ER_BADCONFIGNAME:    strcpy( error_msg, "QUE_ER_BADCONFIGNAME" ); break;
      case QUE_ER_BADCONFIGFILE:    strcpy( error_msg, "QUE_ER_BADCONFIGFILE" ); break;
      case QUE_ER_BADCONFIGINFO:    strcpy( error_msg, "QUE_ER_BADCONFIGINFO" ); break;
      case QUE_ER_BADLISTOFFSET:    strcpy( error_msg, "QUE_ER_BADLISTOFFSET" ); break;
      case QUE_ER_BADUSERNAME:      strcpy( error_msg, "QUE_ER_BADUSERNAME" ); break;
      case QUE_ER_BADFILENAME:      strcpy( error_msg, "QUE_ER_BADFILENAME" ); break;
      case QUE_ER_BADUID:           strcpy( error_msg, "QUE_ER_BADUID" ); break;
      case QUE_ER_BADVAL:           strcpy( error_msg, "QUE_ER_BADVAL" ); break;
      case QUE_ER_BADOPTION:        strcpy( error_msg, "QUE_ER_BADOPTION" ); break;
      case QUE_ER_BADBLOCKOPT:      strcpy( error_msg, "QUE_ER_BADBLOCKOPT" ); break;
      case QUE_ER_DUPLICATE:        strcpy( error_msg, "QUE_ER_DUPLICATE" ); break;
      case QUE_ER_NOTFOUND:         strcpy( error_msg, "QUE_ER_NOTFOUND" ); break;
      case QUE_ER_NOWAIT:           strcpy( error_msg, "QUE_ER_NOWAIT" ); break;
      case QUE_ER_DESTROYED:        strcpy( error_msg, "QUE_ER_DESTROYED" ); break;
      case QUE_ER_NOTLOCAL:         strcpy( error_msg, "QUE_ER_NOTLOCAL" ); break;
      case QUE_ER_BLOCKCANCELED:    strcpy( error_msg, "QUE_ER_BLOCKCANCELED" ); break;
      case QUE_ER_FAILCANWAIT:      strcpy( error_msg, "QUE_ER_FAILCANWAIT" ); break;
      case QUE_ER_FAILRESET:        strcpy( error_msg, "QUE_ER_FAILRESET" ); break;
      case QUE_ER_FAILACQRESP:      strcpy( error_msg, "QUE_ER_FAILACQRESP" ); break;
      case QUE_ER_FAILTIMEOUT:      strcpy( error_msg, "QUE_ER_FAILTIMEOUT" ); break;
      case QUE_ER_FAILWAKEUP:       strcpy( error_msg, "QUE_ER_FAILWAKEUP" ); break;
      case QUE_ER_BADTRIGGERCODE:   strcpy( error_msg, "QUE_ER_BADTRIGGERCODE" ); break;
      case QUE_ER_TRIGGERNOTEXIST:  strcpy( error_msg, "QUE_ER_TRIGGERNOTEXIST" ); break;
      /* case QUE_ER_STARTCFG:         strcpy( error_msg, "QUE_ER_STARTCFG" ); break; */
      case QUE_ER_BADIDENTFILE:     strcpy( error_msg, "QUE_ER_BADIDENTFILE" ); break;
      case QUE_ER_BADTEMPFILE:      strcpy( error_msg, "QUE_ER_BADTEMPFILE" ); break;
      /* case QUE_ER_STOPIDS:          strcpy( error_msg, "QUE_ER_STOPIDS" ); break; */
      case QUE_ER_INSTACTIVE:       strcpy( error_msg, "QUE_ER_INSTACTIVE" ); break;
      case QUE_ER_INSTNOTACTIVE:    strcpy( error_msg, "QUE_ER_INSTNOTACTIVE" ); break;
      case QUE_ER_ASYNC:            strcpy( error_msg, "QUE_ER_ASYNC" ); break;
      case QUE_ER_ASYNCABORT:       strcpy( error_msg, "QUE_ER_ASYNCABORT" ); break;
      case QUE_ER_TIMEOUT:          strcpy( error_msg, "QUE_ER_TIMEOUT" ); break;
      case QUE_ER_INTERRUPT:        strcpy( error_msg, "QUE_ER_INTERRUPT" ); break;
      case QUE_ER_SYSERR:           strcpy( error_msg, "QUE_ER_SYSERR" ); break;
      case QUE_ER_FAILCRITON:       strcpy( error_msg, "QUE_ER_FAILCRITON" ); break;
      case QUE_ER_FAILCRITOFF:      strcpy( error_msg, "QUE_ER_FAILCRITOFF" ); break;
      case QUE_ER_FAILACQTRACESTEP: strcpy( error_msg, "QUE_ER_FAILACQTRACESTEP" ); break;
      case QUE_ER_FAILACQTRACEDONE: strcpy( error_msg, "QUE_ER_FAILACQTRACEDONE" ); break;
      case QUE_ER_FAILRELTRACESTEP: strcpy( error_msg, "QUE_ER_FAILRELTRACESTEP" ); break;
      case QUE_ER_FAILRELTRACEDONE: strcpy( error_msg, "QUE_ER_FAILRELTRACEDONE" ); break;
      case QUE_ER_MEMORY:           strcpy( error_msg, "QUE_ER_MEMORY" ); break;
      case QUE_ER_BADBUFFER:        strcpy( error_msg, "QUE_ER_BADBUFFER" ); break;

      case QUE_ER_BLOCKEDWRITE:     strcpy( error_msg, "QUE_ER_BLOCKEDWRITE" ); break;
      case QUE_ER_BLOCKEDPUT:       strcpy( error_msg, "QUE_ER_BLOCKEDPUT" ); break;
      case QUE_ER_BLOCKEDGET:       strcpy( error_msg, "QUE_ER_BLOCKEDGET" ); break;
      case QUE_ER_BADQUENAME:       strcpy( error_msg, "QUE_ER_BADQUENAME" ); break;
      case QUE_ER_BADQID:           strcpy( error_msg, "QUE_ER_BADQID" ); break;
      case QUE_ER_BADQIDLIST:       strcpy( error_msg, "QUE_ER_BADQIDLIST" ); break;
      case QUE_ER_BADLENGTH:        strcpy( error_msg, "QUE_ER_BADLENGTH" ); break;
      case QUE_ER_BADLIMIT:         strcpy( error_msg, "QUE_ER_BADLIMIT" ); break;
      case QUE_ER_BADPRIORITY:      strcpy( error_msg, "QUE_ER_BADPRIORITY" ); break;
      case QUE_ER_BADTEXT:          strcpy( error_msg, "QUE_ER_BADTEXT" ); break;
      case QUE_ER_BADMSGSELECTCODE: strcpy( error_msg, "QUE_ER_BADMSGSELECTCODE" ); break;
      case QUE_ER_BADQUESELECTCODE: strcpy( error_msg, "QUE_ER_BADQUESELECTCODE" ); break;
      case QUE_ER_BADDIRECTION:     strcpy( error_msg, "QUE_ER_BADDIRECTION" ); break;
      case QUE_ER_PURGED:           strcpy( error_msg, "QUE_ER_PURGED" ); break;
      case QUE_ER_NOTEMPTY:         strcpy( error_msg, "QUE_ER_NOTEMPTY" ); break;
      case QUE_ER_WAITEDON:         strcpy( error_msg, "QUE_ER_WAITEDON" ); break;
      case QUE_ER_MSGHDRREMOVED:    strcpy( error_msg, "QUE_ER_MSGHDRREMOVED" ); break;
      case QUE_ER_ENDOFQUEUE:       strcpy( error_msg, "QUE_ER_ENDOFQUEUE" ); break;
      case QUE_ER_MSGHDRNOTREMOVED: strcpy( error_msg, "QUE_ER_MSGHDRNOTREMOVED" ); break;
      case QUE_ER_TEXTFULL:         strcpy( error_msg, "QUE_ER_TEXTFULL" ); break;
      case QUE_ER_TOOBIG:           strcpy( error_msg, "QUE_ER_TOOBIG" ); break;
      case QUE_ER_UNGETFAILED:      strcpy( error_msg, "QUE_ER_UNGETFAILED" ); break;
      case QUE_ER_MEMPOOL:          strcpy( error_msg, "QUE_ER_MEMPOOL" ); break;

      case QUE_ER_CAPACITY_USER:    strcpy( error_msg, "QUE_ER_CAPACITY_USER" ); break;
      case QUE_ER_CAPACITY_HEADER:  strcpy( error_msg, "QUE_ER_CAPACITY_HEADER" ); break;
      case QUE_ER_CAPACITY_NODE:    strcpy( error_msg, "QUE_ER_CAPACITY_NODE" ); break;
      case QUE_ER_CAPACITY_TABLE:   strcpy( error_msg, "QUE_ER_CAPACITY_TABLE" ); break;
      case QUE_ER_CAPACITY_ASYNC_USER:strcpy( error_msg, "QUE_ER_CAPACITY_ASYNC_USER" ); break;

      case QUE_ER_FAILSTART:        strcpy( error_msg, "QUE_ER_FAILSTART" ); break;
      case QUE_ER_FAILSTOP:         strcpy( error_msg, "QUE_ER_FAILSTOP" ); break;
      case QUE_ER_GHOSTSTART:       strcpy( error_msg, "QUE_ER_GHOSTSTART" ); break;
      case QUE_ER_GHOSTSTOP:        strcpy( error_msg, "QUE_ER_GHOSTSTOP" ); break;
      case QUE_ER_NOSECCFG:         strcpy( error_msg, "QUE_ER_NOSECCFG" ); break;
      case QUE_ER_NOSECIDS:         strcpy( error_msg, "QUE_ER_NOSECIDS" ); break;

      case QUE_ER_RECEIVEBURSTSYNC: strcpy( error_msg, "QUE_ER_RECEIVEBURSTSYNC" ); break;
      case QUE_ER_RECEIVEBURSTSTOP: strcpy( error_msg, "QUE_ER_RECEIVEBURSTSTOP" ); break;
      case QUE_ER_NOTINSENDBURST:   strcpy( error_msg, "QUE_ER_NOTINSENDBURST" ); break;
      case QUE_ER_NOTINRECEIVEBURST:strcpy( error_msg, "QUE_ER_NOTINRECEIVEBURST" ); break;
      case QUE_ER_INSENDBURST:      strcpy( error_msg, "QUE_ER_INSENDBURST" ); break;
      case QUE_ER_INRECEIVEBURST:   strcpy( error_msg, "QUE_ER_INRECEIVEBURST" ); break;
      case QUE_ER_BADERROROPT:      strcpy( error_msg, "QUE_ER_BADERROROPT" ); break;
      case QUE_ER_BADQCL:           strcpy( error_msg, "QUE_ER_BADQCL" ); break;
      case QUE_ER_BADCREDITVALUE:   strcpy( error_msg, "QUE_ER_BADCREDITVALUE" ); break;
      case QUE_ER_BADCALLBACKOPT:   strcpy( error_msg, "QUE_ER_BADCALLBACKOPT" ); break;
      case QUE_ER_BADREADAHEAD:     strcpy( error_msg, "QUE_ER_BADREADAHEAD" ); break;
      case QUE_ER_BADFLOWCONTROL:   strcpy( error_msg, "QUE_ER_BADFLOWCONTROL" ); break;
      case QUE_ER_FLOWCONTROL:      strcpy( error_msg, "QUE_ER_FLOWCONTROL" ); break;
      case QUE_ER_BADSYNCMODE:      strcpy( error_msg, "QUE_ER_BADSYNCMODE" ); break;
      case QUE_ER_MSGHDRDUPLICATED: strcpy( error_msg, "QUE_ER_MSGHDRDUPLICATED" ); break;

      case PTEIPC_MALLOC_ERROR:     strcpy( error_msg, "PTEIPC_MALLOC_ERROR" ); break;
      default:                      strcpy( error_msg, "PTE_IPC_UNKNOWN_ERROR" );
   }

}

/************************************************************************
*
************************************************************************/
LONG pteipc_checkin(pCHAR service_name)
{
   LONG     ipc_rcode;
   pPTE_MSG p_msg_out;
   CHAR     pid_str[MAX_ORIG_INFO_SIZE];

   if ( (ipc_rcode = pteipc_queaccess(srvcmgr_que)) >= 0)
   {
   	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REPLY,
   			               ST1_SYS_APP_CHECKIN,
   			               0,
   			               NULL_PTR,
   			               NULL_PTR,
   			               NULL_PTR,
   			               0,
   			               0 );
   
   	if( p_msg_out != NULL_PTR )
   	{
#ifdef WIN32
       sprintf(pid_str, "%s,%0ld", service_name, GetCurrentProcessId());
#else
       sprintf(pid_str, "%s,%0ld", service_name, getpid());
#endif

       ptemsg_set_orig_comms_info( p_msg_out, pid_str );
   
   	 ipc_rcode = pteipc_send( p_msg_out, srvcmgr_que );
       free( p_msg_out );
   	}
   }

   return(ipc_rcode);
}
