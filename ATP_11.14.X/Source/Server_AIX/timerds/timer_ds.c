/******************************************************************************

   Module: timerds.c

   Title: Timer Dataserver Routines

   Description: 

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\timerds\timer_ds.c  $
   
      Rev 1.17   Apr 05 2005 16:00:00   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.16   Jul 08 2004 17:18:58   dirby
   Updated to 4.4.0.1
   
      Rev 1.15   May 27 2004 17:22:52   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.14   Feb 17 2003 10:06:34   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.13   Aug 16 2002 18:00:12   dirby
   Added functionality to display version when pinged.
   SCR 255
   
      Rev 1.12   Aug 16 2002 17:42:28   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.11   Nov 03 2001 12:25:14   dirby
   Updated version to 4.0.0.1     SCR 403, 361, 256, 251
   
      Rev 1.10   Jan 09 2001 14:16:48   dirby
   Updated version number.  This marks the release where the production
   stability issue has been resolved.  Some debug code has been removed and
   added #ifndef around some Unix signal code so the data server will compile
   on NT.
   
      Rev 1.9   Oct 16 2000 12:27:32   dirby
   1.  Updated version number to 3.2.2.2 - TEST.
   2.  Added code to support XipcSystemErrReport in the event of XIPC problems.
   3.  Removed catching of OS signals CHILD and HangUP so the daemon process can catch them.
   
      Rev 1.8   Oct 05 2000 15:03:22   dirby
   Updated the version number to '3.2.2.1 - TEST'.
   This version is compiled with more diagnostic stuff
   in daemon.c.
   
      Rev 1.7   Sep 01 2000 16:19:40   dirby
   Added TrapFunction to catch Unix signals.  This will not compile under NT.
   
      Rev 1.6   Aug 31 2000 17:30:20   dirby
   Appended "TEST" to the version number to account for some
   daemon testing getting linked in.
   
      Rev 1.5   Jul 30 2000 15:37:08   dirby
   Modified to display version number at startup and shutdown.
   
      Rev 1.4   Jan 22 2000 10:28:10   ddabberu
   reverted back to the old timerds version
   now built as  a single threaded application.
   
      Rev 1.2   Jan 21 2000 12:37:14   dirby
   Removed UNIX function that was used for multi threading.
   
      Rev 1.1   Jan 18 2000 13:06:00   svelaga
   calling pteipc_receive_r() function for 
   unix timerds application.
   
      Rev 1.0   Aug 19 1999 14:41:36   svelaga
   Initial release

   AUTHOR : Beth Gable
   
   
   $Log:    $

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <signal.h>

#include "basictyp.h"
#include "app_info.h"
#include "pte.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "logger.h"

#include "dbcommon.h"
#include "nc_dbstruct.h"
#include "nc_database.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"


/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;

UINT Max_File_Size_Defined = 0 ;

void TrapFunction( INT signum );

PRIVATE union
{
  NCF01   ncf01;
  NCF02   ncf02;
  NCF20   ncf20;
  NCF21   ncf21;
  NCF30   ncf30;
  NEG01   neg01;
  ATM01   atm01;
  SAF01   saf01;
} db_recs;

PRIVATE union
{
   NCF01_GUI_LIST      Ncf01_Gui_List;
   NCF01_LIST          Ncf01_List;
   NCF02_GUI_LIST      Ncf02_Gui_List;
   NCF30_BATCH_LIST    Ncf30_List;
}db_lists;

PRIVATE BOOLEAN      Ncf01InMem = false;
PRIVATE BOOLEAN      TranDetailInMem = false;
PRIVATE LONG         MemSize;
PRIVATE LONG         MemAvailable;

PRIVATE BYTE         MsgSubtype2;
PRIVATE BYTE         MsgSubtype1;
PRIVATE BYTE         result_code;
PRIVATE BOOLEAN      updates_to_db_made = false;
PRIVATE CHAR         ErrorMsg  [100];
PRIVATE CHAR         AppName   [MAX_APP_NAME_SIZE];
PRIVATE CHAR         last_trid [MAX_TRAN_ID_SIZE];
PRIVATE pCHAR        reply_que;
PRIVATE CHAR         orig_comm_inf[MAX_ORIG_INFO_SIZE];
PRIVATE CHECKPOINT   checkpoint_rec;
PRIVATE CHAR         AtpBuffer[30000];
PRIVATE BYTE         app_data_type;
PRIVATE CHAR         applnk_que_name[8]="applnkA";

BOOLEAN   in_refeed_mode    = false;
CHAR      Version[] = "ATP_11.1.0";

CHAR  timerds_Erro_warning_Filename[256] = {0} ;
CHAR  timerds_module_error_warning_file_name[256] = {0};
CHAR  timerds_error_warning_file_path[256] = {0};

NCF21   ncf21;

#ifdef _DEBUG
   #define PRINT( X ) printf( X )
#else
   #define PRINT( X )
#endif


/*
 * These defines control whether the dataserver is using logger-reply
 * and whether it should be doing checkpointing.
 *
 * Possible combinations:
 *    a) using logger-reply with checkpoints
 *    b) using logger-reply without checkpoints
 *    c) using neither.
 *
 * note: cannot use checkpoints without logger-reply!!!!!
 *
 */ 
// 12-07-98 gabriel
//#define USING_LOGGER_REPLY
//#define USING_CHECKPOINTS

#ifndef USING_LOGGER_REPLY
   #undef USING_CHECKPOINTS
#endif



void timerds_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( timerds_Erro_warning_Filename, timerds_error_warning_file_path );
    strcat( timerds_Erro_warning_Filename, timerds_module_error_warning_file_name );
	strcat( timerds_Erro_warning_Filename, ".EW." );
    strncat(timerds_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(timerds_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(timerds_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void timerds_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  timerdss_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   GetAppName (appname) ;

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   timerds_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(timerds_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  timerds_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(timerds_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  timerdss_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(timerdss_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(timerdss_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(timerds_error_warning_file_path) > 0) &&
	  (strlen(timerds_module_error_warning_file_name)> 0))
   {
	   timerds_create_Error_Filename();
   }
}

INT timerds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(timerds_Erro_warning_Filename);
    path_len = strlen(timerds_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&timerds_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	timerds_create_Error_Filename();
    }
	if((fp = fopen(timerds_Erro_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

/******************************************************************************
 *
 *  NAME:         timerds_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void timerds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
   INT  Appname_len = 0;
   CHAR  appname[512] = {0};

   GetAppName (appname) ;
   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",appname);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   timerds_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
								 	 eventlog_flag, msg_type, func,
									 severity, log_type,
									 NULL, NULL,NULL );
   }

   return;
}

/******************************************************************************
 *
 *  NAME:         process_echo_msg
 *
 *  DESCRIPTION:  This function echos a message back to the originator.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   A reply message to be sent to the originating queue.
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_echo_msg ( pPTE_MSG p_msg_in )
{
   BYTE           app_data_type;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;


   p_msg_data     = ptemsg_get_pte_msg_data              (p_msg_in);
   p_data         = ptemsg_get_pte_msg_data_data         (p_msg_data);
   data_len       = ptemsg_get_pte_msg_data_data_length  (p_msg_data);
   app_data_type  = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);


   p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                 ptemsg_get_msg_subtype1 (p_msg_in),
                                 ptemsg_get_msg_subtype2 (p_msg_in),
                                 reply_que,
                                 application_que_name,
                                 p_data,
                                 data_len,
                                 app_data_type);

   if (p_msg_out == NULL_PTR)
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf( ErrorMsg, "Insufficient Memory to build Echo response to %s",
               reply_que);
   }
   return( p_msg_out );
}


/******************************************************************************
 *
 *  NAME:         process_db_request
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                application.  It will switch a message type of database
 *                request, by subtype 1.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
pPTE_MSG process_db_request ( pPTE_MSG p_msg_in )
{
   pPTE_MSG       p_msg_out;
  
   MsgSubtype2 = ptemsg_get_msg_subtype2( p_msg_in );
   MsgSubtype1 = ptemsg_get_msg_subtype1( p_msg_in );

   memset(AtpBuffer, 0, sizeof(AtpBuffer));
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {

      case ST1_DB_ECHO:
               p_msg_out = process_echo_msg( p_msg_in );
               break;

      default:
               p_msg_out   = NULL_PTR;
               result_code = PTEMSG_INVALID_SUBTYPE1;
               sprintf(ErrorMsg,"Unknown subtype1 for timer database request from %s",
                       reply_que);
               break;
   }
   return( p_msg_out );
}

/*************************************************************************************/
/*************************************************************************************/
void process_monitor_request(pPTE_MSG p_msg_in )
{
   CHAR           monitor_flag[100]="";
   pCHAR          orig_comm;
   CHAR           MonAdd[100]="";
   //pPTE_MSG       p_msg;
   //LONG           ipc_rcode;
   
   memset(AtpBuffer, 0, sizeof(AtpBuffer));
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
       case ST1_MONITOR_UP   : 
         orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
         WriteMemMonitorTable( (BYTE *)orig_comm );  
         /*
         if(ReadMemMonitorTable() != NULL)
         {
            strcpy(MonAdd, ReadMemMonitorTable());
            p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_OPRMSG, 0,applnk_que_name, application_que_name, (pBYTE) "Dataserver ready", 16,0);
            ptemsg_set_orig_comms_info(p_msg, MonAdd);
            ipc_rcode = pteipc_send(p_msg, applnk_que_name);
            free(p_msg);
       
         }
         else
            printf("Null value in MonAdd\n");*/
         break;
      case ST1_MONITOR_DOWN  :
         WriteMemMonitorTable((BYTE *)monitor_flag); 
         break;
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
         break;
   }
}


/******************************************************************************
 *
 *  NAME:         process_logger_reply
 *
 *  DESCRIPTION:  This function is used to communicate with Logger / Reply.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void process_logger_reply( pPTE_MSG p_msg_in )
{
   pCHAR       p_msg_trid;


   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_LOG_REFEED_COMPLETE : 
         in_refeed_mode = false;
         p_msg_trid     = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy (checkpoint_rec.checkpoint_trid, p_msg_trid,
                     sizeof(checkpoint_rec.checkpoint_trid));
            strncpy (last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint(&checkpoint_rec, ErrorMsg);

            if (result_code == PTEMSG_OK)
            {
               dbcommon_commit ();
            }
            else
            {
               dbcommon_rollback ();

               /*The following function writes the message both to the Sys.Monitor and EventLog */
               timerds_log_message( 1, 3, ErrorMsg, "process_logger_reply", 0 );

               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
                  /*The following function writes the message both to the Sys.Monitor and EventLog */
                  timerds_log_message( 1, 3, ErrorMsg, "process_logger_reply", 0 );

               }
            }
         }
                                     
         break;
      
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for logger reply from %s", reply_que);

         /*The following function writes the message both to the Sys.Monitor and EventLog */
         timerds_log_message( 1, 3, ErrorMsg, "process_logger_reply", 0 );
         break;
   }
}



/******************************************************************************
 *
 *  NAME:         perform_refeed
 *
 *  DESCRIPTION:  This function will re-feed a command to Logger / Reply.
 *
 *  INPUTS:       trid - Transaction Id
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True is successful, else False
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN perform_refeed ( pCHAR trid )
{
   CHAR     Buffer[256];


   if (trid [0] != '\0')
   {
      /* send refeed command to TFLogger */
      if (SendRefeedCmd (trid, ErrorMsg) == false)
      {
         sprintf  (Buffer, "Error sending refeed command to TFLogger: %s", ErrorMsg);
         /*The following function writes the message both to the Sys.Monitor and EventLog */
         timerds_log_message( 1, 3, ErrorMsg, "perform_refeed", 0 );
         return (false);
      }

      in_refeed_mode = true;

   }

   return (true);
}



/******************************************************************************
 *
 *  NAME:         validate_last_trid
 *
 *  DESCRIPTION:  
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Code indicating disposition of the validation
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE validate_last_trid ( pPTE_MSG p_msg_in )
{
   int         cmp_result;
   LONG        ipc_rcode;
   CHAR        temp_str [80];
   pCHAR       p_msg_last_trid;
   pPTE_MSG    p_msg_out;


   /* 
    * get a pointer to the msg last_trid.  if the pointer is null, error 
    */
   p_msg_last_trid = ptemsg_get_last_msg_tran_id (p_msg_in);

   /*
    * If the msg last_trid is empty, OK.  This is a request that came 
    * directly from an application (select, etc).  If it is not empty,
    * need to compare it with my last_trid.
    */
   if (p_msg_last_trid [0] != '\0')
   {
      if (last_trid [0] == '\0')
      {
         /*
          * My last_trid is empty.  Update it from this message.
          */
         strncpy (last_trid, p_msg_last_trid, sizeof(last_trid));
      }
      else
      {
         /*
          * My last_trid is not empty.  If it doesn't match the msg last_trid,
          * we have an out of sequence error.  Need to initiate a refeed from
          * my last_trid.
          */
         cmp_result = strncmp (last_trid, p_msg_last_trid, sizeof(last_trid));

         if (cmp_result != 0)
         {
            perform_refeed (last_trid);

            result_code = PTEMSG_INVALID_TRID;
            sprintf  (ErrorMsg, "invalid last-trid received: %s.   expected: %s",
                      p_msg_last_trid, last_trid); 
         }
      }
   }
   else
   {
      if (in_refeed_mode == true)
      {
         result_code = PTEMSG_IN_REFEED_MODE;
         strcpy (ErrorMsg, 
                 "In refeed mode; cannot accept application requests until complete");
      }
   }

   if (result_code != PTEMSG_OK)
   {
	   timerds_log_message( 1, 3, ErrorMsg, "validate_last_trid", 0 );
       /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).
       */
      reply_que = ptemsg_get_msg_orig_queue (p_msg_in);

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) ErrorMsg,
                                    strlen(ErrorMsg) + 1,
                                    0);

      if (p_msg_out == NULL_PTR)
      {
         sprintf  (ErrorMsg, "Insufficient Memory to build error response to %s", reply_que);
         timerds_log_message( 1, 2, ErrorMsg, "validate_last_trid", 0 );
         return (result_code);
      }

      ptemsg_set_result_code (p_msg_out, result_code);
      ptemsg_set_orig_comms_info( p_msg_out, orig_comm_inf );
      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_msg_out, reply_que);

      free (p_msg_out);

      if (ipc_rcode < 0)
      {
         timerds_log_message( 1, 2, ErrorMsg, "validate_last_trid", 0 );
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s", reply_que, temp_str);
      }
   }

   return (result_code);
}



/******************************************************************************
 *
 *  NAME:         rollback_the_update
 *
 *  DESCRIPTION:  Roll back the database changes.  Do not commit them.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void rollback_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* First, rollback the database update(s) */
      dbcommon_rollback ();

      /* Then rollback any timer requests (makes sure memory table matches db)*/
      dstimer_rollback ();

      updates_to_db_made = false;
   }
}  /* rollback_the_update */


/******************************************************************************
 *
 *  NAME:         commit_the_update
 *
 *  DESCRIPTION:  Commit any database changes.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void commit_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* commit the database update(s) */
      dbcommon_commit ();

      /* let the timer know a commit has occurred */
      dstimer_commit ();

      updates_to_db_made = false;
   }
}  /* commit_the_update */



/******************************************************************************
 *
 *  NAME:         PinnacleMsgHandler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                application.  It will transfer control to the appropriate
 *                module, based on message type.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE PinnacleMsgHandler ( pPTE_MSG p_msg_in )
{
   LONG           i;
   LONG           num_messages;
   LONG           ipc_rcode;
   CHAR           temp_str [80];
   pPTE_MSG       p_cur_msg;
   pPTE_MSG       p_tmp_msg;
   pPTE_MSG       p_chn_out;
   pPTE_MSG       p_new_chn;
   pPTE_MSG_DATA  p_msg_data;
   pBYTE          p_data;
   int            refresh_flag = 0;  /* If an error in update_mem_record or
                                      * delete_mem_record, delete all recs. in
                                      * mem and recreate all tables over again 
                                      * from the database.
                                      */
   

   #ifdef USING_CHECKPOINTS
      pCHAR       p_msg_trid;
   #endif


   ptestats_accumulate_msg_stats (p_msg_in);

   result_code = PTEMSG_OK;

   #ifdef USING_CHECKPOINTS
      if ( (result_code = validate_last_trid (p_msg_in)) != PTEMSG_OK )
         return (result_code);
   #endif

   p_cur_msg    = p_msg_in;
   p_tmp_msg    = NULL_PTR;
   p_chn_out    = NULL_PTR;
   i            = 1;
   num_messages = ptemsg_get_num_chained_messages (p_msg_in);
   /* Added for UpdateMemRecord stuff */
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data +  sizeof(AUTH_TX);


   while ( (i <= num_messages) && (p_cur_msg != NULL_PTR) )
   {
      reply_que = ptemsg_get_msg_orig_queue (p_cur_msg);
      strcpy(orig_comm_inf, ptemsg_get_msg_orig_comms_info( p_cur_msg ));
      app_data_type = 0;
  
      switch( ptemsg_get_msg_type( p_cur_msg ) )
      {
         case MT_DB_REQUEST     : p_tmp_msg = process_db_request (p_cur_msg);  
                                  break;                                       
         case MT_TIMER_REQUEST  : p_tmp_msg =
                                      dstimer_process_request(p_cur_msg,
                                                        &updates_to_db_made);
                                  break;
         case MT_SYSTEM_REQUEST : ptesystm_system_msg_handler (p_cur_msg);
                                  if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
                                  {
                                     /* When pinged, display application version on Monitor. */
                                     sprintf( temp_str,
                                             "Pinged -> Data Server: %s, version %s",
                                              ServiceName, Version );
                                     timerds_log_message( 1,1, temp_str, "PinnacleMsgHandler", 0 );
                                  }
                                  break;
         case MT_LOGGER_REPLY   : process_logger_reply (p_cur_msg);
                                  break;
         case MT_SYS_MONITOR    : process_monitor_request (p_cur_msg);
                                  break;
         default :
            /*
             * Received an unknown msg type.  If the request was destined for
             * this dataserver, log an error because I can't process it.
             * Otherwise, try to forward it on to the actual destination.
             */
            if ( (0 == strncmp (ptemsg_get_msg_dest_queue( p_cur_msg ),
                                              application_que_name,
                                              strlen(application_que_name)) ) ||
                 (0 == strncmp (ptemsg_get_msg_dest_queue (p_cur_msg),
                                              control_que_name,
                                              strlen(control_que_name)) )   )
            {
               result_code = PTEMSG_INVALID_MSGTYPE;
               sprintf (ErrorMsg,
                       "Unknown msgtype for request from %s", reply_que);
            }
            else
            {
               /*
                * Forward the request to its destination (if not in refeed mode)
                */
               if (in_refeed_mode == false)
               {
                  ptemsg_set_orig_comms_info( p_cur_msg, orig_comm_inf );
                  ipc_rcode = pteipc_send (p_cur_msg,
                                           ptemsg_get_msg_dest_queue(p_cur_msg));
                  if (ipc_rcode < 0)
                  {
                     result_code = PTEMSG_COULD_NOT_FORWARD;
                     pteipc_get_errormsg (ipc_rcode, temp_str);
                     sprintf  (ErrorMsg, "error forwarding to queue %s: %s", 
                               ptemsg_get_msg_dest_queue (p_cur_msg), temp_str);
                  }
               }
            }

            break;
      }  /* switch msg_type */


      if (result_code != PTEMSG_OK)
         break;

      if (p_tmp_msg != NULL_PTR)
      {
         result_code = ptemsg_get_result_code (p_tmp_msg);

         if (result_code != PTEMSG_OK /*&& result_code != PTEMSG_NOT_FOUND*/)
         {

            free (p_chn_out);
            p_chn_out = p_tmp_msg;
            p_tmp_msg = NULL_PTR;

            if(result_code == PTEMSG_NOT_FOUND )
            {
            	p_cur_msg = ptemsg_get_chained_pte_msg (p_msg_in, ++i);
                reply_que = ptemsg_get_msg_orig_queue (p_cur_msg);
                strcpy(orig_comm_inf, ptemsg_get_msg_orig_comms_info( p_cur_msg ));
                app_data_type = 0;
            	p_tmp_msg = process_echo_msg( p_cur_msg );
            	p_new_chn = ptemsg_chain_message (p_chn_out, p_tmp_msg);


            	if (p_new_chn == NULL_PTR)
            	{
            		  result_code = PTEMSG_INSUFFICIENT_MEMORY;
            		  sprintf( ErrorMsg, "Insufficient Memory to chain response to %s",
            		                        reply_que );
            break;
         }
            	else
            	{
            		 p_chn_out = p_new_chn;
            		 p_tmp_msg = NULL_PTR;
            	}
         
            	free (p_tmp_msg);
            	p_tmp_msg = NULL_PTR;
            }
            break;
         }
         if (p_chn_out == NULL_PTR)
         {
            p_chn_out = p_tmp_msg;
            p_tmp_msg = NULL_PTR;
         }
         else
         {
            p_new_chn = ptemsg_chain_message (p_chn_out, p_tmp_msg);

            if (p_new_chn == NULL_PTR)
            {
               result_code = PTEMSG_INSUFFICIENT_MEMORY;
               sprintf( ErrorMsg, "Insufficient Memory to chain response to %s",
                        reply_que );
               break;
            }
            else
            {
               p_chn_out = p_new_chn;
               p_tmp_msg = NULL_PTR;
            }
         }
      }

      if ((++i) <= num_messages)
         p_cur_msg = ptemsg_get_chained_pte_msg (p_msg_in, i);
   }  /* while */


   /*
    * Be sure to free up the temp message.
    */
   if (p_tmp_msg != NULL_PTR)
   {
      free (p_tmp_msg);
      p_tmp_msg = NULL_PTR;
   }

   #ifdef USING_CHECKPOINTS
      /*
       * Update the checkpoint.
       */
      if (result_code == PTEMSG_OK)
      {
         p_msg_trid = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy( checkpoint_rec.checkpoint_trid, p_msg_trid,
                     sizeof(checkpoint_rec.checkpoint_trid));
            strncpy( last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint (&checkpoint_rec, ErrorMsg);
         }
      }
   #endif

   /*
    * If there was an error, back out the changes.
    * Otherwise commit the changes.
    */
   reply_que = ptemsg_get_msg_orig_queue (p_msg_in);

   if (result_code != PTEMSG_OK)
   {
      rollback_the_update ();
      if(result_code != PTEMSG_NOT_FOUND)
      {
      if (p_chn_out != NULL_PTR)
      {
         free (p_chn_out);
         p_chn_out = NULL_PTR;
      }
      }
      /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).
       */

      // test
      if(result_code == PTEMSG_NOT_FOUND)
      {
    	  p_chn_out->msg_type     = MT_DB_REPLY;
    	  p_chn_out->msg_subtype1 = ST1_DB_UPDATE;

	      ptemsg_set_result_code (p_chn_out, result_code);
	      ptemsg_set_orig_comms_info( p_chn_out, orig_comm_inf );

	      /*
	       * Send the response.
	       */
	      ipc_rcode = pteipc_send (p_chn_out, reply_que);

	      free (p_chn_out);

	      if (ipc_rcode < 0)
	      {
	         /* Send message to Monitor and EventLog. */
	         TxUtils_Send_Msg_To_Operator(1, ErrorMsg, 1, WARN_MSG,
	                                     "PinnacleMsgHandler", 2, WARNING_ERROR,
	                                      NULL_PTR, NULL_PTR, NULL_PTR);
	         result_code = PTEMSG_IPC_ERROR;
	         pteipc_get_errormsg (ipc_rcode, temp_str);
	         sprintf(ErrorMsg,"error sending response to %s: %s",reply_que,temp_str);
	      }
	      return (result_code);
	      //test
      }
      else
      {
        memcpy(AtpBuffer + sizeof(AUTH_TX), ErrorMsg, strlen(ErrorMsg) + 1);
      }
      p_chn_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_cur_msg),
                                    ptemsg_get_msg_subtype2 (p_cur_msg),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) AtpBuffer,
                                    strlen(ErrorMsg) + 1 + sizeof(AUTH_TX),
                                    app_data_type);

      if (p_chn_out == NULL_PTR)
      {
         /* Send message to Monitor and EventLog. */
         timerds_log_message( 1,2, ErrorMsg, "PinnacleMsgHandler", 0 );
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf( ErrorMsg, "Insufficient Memory to build error response to %s",
                  reply_que );
      }
   }
   else
   {
      commit_the_update ();
   }

   if (p_chn_out != NULL_PTR)
   {
      /*
       * If in refeed mode, don't send the response.
       */
      if (in_refeed_mode == true)
         return (PTEMSG_OK);

      /*
       * If the result is OK, don't send the response if they don't want it.
       */
      if((PTEMSG_OK                == result_code  ) &&
        (PTEMSG_REPLY_ONLY_IF_FAILS==ptemsg_get_type_of_reply_needed(p_msg_in)))
      {
         free (p_chn_out);
         return (PTEMSG_OK);
      }

      ptemsg_set_result_code (p_chn_out, result_code);
      ptemsg_set_orig_comms_info( p_chn_out, orig_comm_inf );

      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_chn_out, reply_que);

      free (p_chn_out);

      if (ipc_rcode < 0)
      {
         /* Send message to Monitor and EventLog. */
         timerds_log_message( 1,2, ErrorMsg, "PinnacleMsgHandler", 0 );
         result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf(ErrorMsg,"error sending response to %s: %s",reply_que,temp_str);
      }
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
      timerds_log_message( 2, 1, ErrorMsg, "PinnacleMsgHandler",0 );
   }*/

   return (result_code);
}



/******************************************************************************
 *
 *  NAME:         perform_startup
 *
 *  DESCRIPTION:  This function performs basic startup functions such as
 *                logging into the database and initializing XIPC.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN perform_startup ()
{
   CHAR     XipcInstanceName[12];


   memset (last_trid, 0, sizeof(last_trid));

   /* Try to connect to the database */
   if ( PTEMSG_OK != 
        dbcommon_connect("equitable","equitable","equitable","ORCL",ErrorMsg) )
   {
      /* Send message to Monitor and EventLog. */
      timerds_log_message( 1,3, ErrorMsg, "perform_startup", 0 );
      return( false );
   }

   /* Try to initialise the xipc stuff */
   GetAppName          (AppName);
   GetXipcInstanceName (XipcInstanceName);

#ifndef WIN32

   /* Diagnostic code to trap the following signals. */
   sigset( SIGALRM, TrapFunction );
   sigset( SIGBUS,  TrapFunction );
   sigset( SIGEMT,  TrapFunction );
   sigset( SIGFPE,  TrapFunction );
   sigset( SIGILL,  TrapFunction );
   sigset( SIGIOT,  TrapFunction );
   sigset( SIGKILL, TrapFunction );
   sigset( SIGPIPE, TrapFunction );
   sigset( SIGPOLL, TrapFunction );
   sigset( SIGPWR,  TrapFunction );
   sigset( SIGQUIT, TrapFunction );
   sigset( SIGSEGV, TrapFunction );
   sigset( SIGSYS,  TrapFunction );
   sigset( SIGTRAP, TrapFunction );
   sigset( SIGUSR1, TrapFunction );
   sigset( SIGUSR2, TrapFunction );

#endif

   //12-07-98 gabriel multiple instance

   if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)
   //if( !pteipc_init_multiple_instance_app(AppName,ServiceName,XipcInstanceName))
   {
      /* Send message to Monitor and EventLog. */
      timerds_log_message( 1,3, "Failed to create XIPC queues", "perform_startup", 0 );
      pteipc_shutdown_single_instance_app();
      //pteipc_shutdown_multiple_instance_app();
      dbcommon_disconnect (ErrorMsg);
      return (false);
   }
   

   #ifdef USING_LOGGER_REPLY
      /* send restart command to TFLogger */
      if (SendRestartCmd (ErrorMsg) == false)
      {
         /* Send message to Monitor and EventLog. */
         timerds_log_message( 1,3, "Error sending restart command to TFLogger",
        		 	 	 	 	 	 "perform_startup", 0 );
         dstimer_shutdown ();
         pteipc_shutdown_single_instance_app();
         //pteipc_shutdown_multiple_instance_app();
         dbcommon_disconnect (ErrorMsg);
         return (false);
      }

      #ifdef USING_CHECKPOINTS
         /* get the checkpoint record from the database */
         if (PTEMSG_OK != 
             dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg))
         {
            /* Send message to Monitor and EventLog. */
            timerds_log_message( 1,3, ErrorMsg,"perform_startup", 0 );
            dstimer_shutdown ();
            pteipc_shutdown_single_instance_app();
            //pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }

         /* 
          * refeed from my last known trid.
          */
         strncpy (last_trid, checkpoint_rec.checkpoint_trid, sizeof(last_trid));

         if (perform_refeed (checkpoint_rec.checkpoint_trid) == false)
         {
            dstimer_shutdown ();
            pteipc_shutdown_single_instance_app();
            //pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }
      #endif
   #endif

   /* Try to initialise the timer stuff */
   dstimer_startup (0);

   return (true);
}

#ifndef WIN32

void TrapFunction( INT signum )
{
   CHAR Buffer[100];

   XIPC_TRAP_FUNCTION_TEST( TrapFunction, signum );

   memset( Buffer, 0x00, sizeof(Buffer) );
   sprintf( Buffer, "%s:  Caught signal %d", AppName, signum );
   timerds_log_message( 2, 3, Buffer, "TrapFunction",0 );
}

#endif

/******************************************************************************
 *
 *  NAME:         MainProcessor
 *
 *  DESCRIPTION:  This is the driver function for the Network Control
 *                Data Server.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void MainProcessor()
{
   LONG     ipc_rcode, xipc_code;
   pPTE_MSG p_msg;
   CHAR     Buffer[256];
// CHAR     XipcErrBuffer[1000];
   CHAR     MonAdd[100]="";

   #ifdef _DEBUG
      pteipc_trace_on();
   #endif

   if (perform_startup () == false)
   {
      MainProcessDone = 1;
      return;
   }
   timerds_get_error_warning_file_name_path( );
   sprintf( Buffer,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   timerds_log_message( 2, 1, Buffer, "MainProcessor",0 );

   PRINT( "DataServer is ready. press <ENTER> to exit\n" );

   while ( !EndProcessSignalled )
   {
      /* You are blocked here waiting for a message on either app queue or
       * control queue.  If there is no message on either que for the specified
       * time, the blocking call returns.
       */
      
      p_msg = pteipc_receive( application_que_name, control_que_name, TIME_BETWEEN_CHECKS, &ipc_rcode );

      if( p_msg != NULL_PTR )
      {
         PinnacleMsgHandler( p_msg );
         free (p_msg);
      }
      else 
         if( ipc_rcode != QUE_ER_TIMEOUT ) 
         {
            pteipc_get_errormsg ( ipc_rcode, ErrorMsg );
            sprintf ( Buffer, "Error on pteipc_receive %s", ErrorMsg );

            /* Send message to Monitor and EventLog. */
            timerds_log_message( 1,2, Buffer,"MainProcessor", 0 );
            /* Comment the following out after the production stability
             * problem is resolved.
             */
            /* This line is to gather XIPC info *
            memset( XipcErrBuffer, 0x00, sizeof(XipcErrBuffer) );
            XipcSystemErrorReport( XipcErrBuffer, sizeof(XipcErrBuffer)-1 );
            timerds_log_message( 2, 3, XipcErrBuffer, "MainProcessor",0 );
            exit(1);
            */
         }
         else
         {
            in_refeed_mode = false;
         }

      dstimer_check_for_timeouts ();
   }

   dstimer_shutdown ();

   if( Ncf01InMem )
   {
      xipc_code = DestroyXipcTable("Ncf01Table");
      //PRINT("Bin01 destroy %d\n", xipc_code);
   }

   /* Shut down ipc */
   pteipc_shutdown_single_instance_app(); 
   //pteipc_shutdown_multiple_instance_app(); 

   printf ("pteipc shutdown successful\n");

   /* Disconnect from database */
   if (dbcommon_disconnect (ErrorMsg) != PTEMSG_OK)
      printf ( "%s\n", ErrorMsg );
   else
      printf ( "Disconnected from ORACLE\n" );

   /* Signal that we are ready for shutdown */
   MainProcessDone = 1;
}


/******************************************************************************
 *
 *  NAME:         EndProcess
 *
 *  DESCRIPTION:  This is the function used to clean up when the app is
 *                being exited.  All clean up code goes in here.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void EndProcess()
{
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   timerds_log_message( 2, 1, Buffer, "EndProcess",0 );
   strcat( Buffer, "\n" );
   PRINT( Buffer );
   pteipc_sleep (3000);
}



/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/

