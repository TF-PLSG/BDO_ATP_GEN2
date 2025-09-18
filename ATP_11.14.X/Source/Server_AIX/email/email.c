/******************************************************************************
  
        Module: email.c
   
         Title: email
  
   Description: 

   $Log$
   Revision 1.4  Dec-10-2009 14:35:04  TF-Ajay
		Changed code to display msg once succssfully email sent
	    Updated version 1.0.0.2


   Revision 1.3  Jul-06-2009 15:24:04  TF-Ajay
	  Build with new PTE and ATP lib
	  For masking card number- PCI compliance
	  Updated version 1.0.0.1
 * 
 *    Rev 1.2   Jun 08 2006 15:26:06   dirby
 * Made FROM and SUBJECT configurable items from tf.ini.
 * 
 *    Rev 1.1   Jan 24 2002 15:04:36   msaleh
 * modified to send the exact message that is logged
 * 
 *    Rev 1.0   Jan 21 2002 17:11:58   msaleh
 * Initial revision.
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#endif

#include "pte.h"
#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptesystm.h"
#include "ptestats.h"
#include "ntutils.h"
#include "coreinfo.h"
#include "app_info.h"

#define CRLF     "\r\n"

#define State220   220
#define State250   250
#define State354   354
#define State221   221 

/* Extern definition section */
extern int  volatile EndProcessSignalled;
extern int  volatile MainProcessDone;
extern CHAR          ServiceName[12];

PRIVATE INT             connection_type;
PRIVATE LOCAL_DM_CONFIG dm_config;
PRIVATE CHAR            EmailAddress1[128];
PRIVATE CHAR            EmailAddress2[128];
PRIVATE CHAR            EmailAddress3[128];
PRIVATE CHAR            EmailFromAcct[128];
PRIVATE CHAR            EmailSubject [128];

INT     volatile        State;
INT     volatile        GotResponse;

#ifdef WIN32
__declspec(dllimport) XipcMaxThreads;
#else
extern XINT XipcMaxThreads;
#endif

CHAR Version[]="ATP_11.1.0";

CHAR  email_Erro_warning_Filename[256] = {0} ;
CHAR  email_module_error_warning_file_name[256] = {0};
CHAR  email_error_warning_file_path[256] = {0};
/*************************************************************
 *************************************************************/
void SetStartupEnvironment()
{
   DWORD rc;
   CHAR sectionname  [] = "EMAIL";
   CHAR filename     [256];
   CHAR tmpstr       [256];
   
   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   strcpy(dm_config.db_subsystem_name  , "email");
   strcpy(dm_config.db_connection_type , "TCP/IP");
   strcpy(dm_config.db_tcp_socket_type , "CLIENT");
   strcpy(dm_config.db_tpdu_processing , "WITHOUT");
   dm_config.db_tcp_header_length = 0;

   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_TCP_PORT",                     
      "25",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   dm_config.db_tcp_port = atoi(tmpstr);

   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_SERVER",                     
      "",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   strcpy(dm_config.db_host_name , tmpstr);

   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_ADDRESS1",                     
      "",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   memset(EmailAddress1, 0, sizeof(EmailAddress1));
   memcpy(EmailAddress1, tmpstr, sizeof(EmailAddress1));

   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_ADDRESS2",                     
      "",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   memset(EmailAddress2, 0, sizeof(EmailAddress2));
   memcpy(EmailAddress2, tmpstr, sizeof(EmailAddress2));

   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_ADDRESS3",                     
      "",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   memset(EmailAddress3, 0, sizeof(EmailAddress3));
   memcpy(EmailAddress3, tmpstr, sizeof(EmailAddress3));


   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_FROM_ACCOUNT",                     
      "",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   memset( EmailFromAcct, 0x00, sizeof(EmailFromAcct) );
   if ( tmpstr[0] == 0x00 )
      strcpy( EmailFromAcct, dm_config.db_host_name );
   else
      memcpy( EmailFromAcct, tmpstr, sizeof(EmailFromAcct) );


   rc = GetPrivateProfileString(
      sectionname,	                  
      "EMAIL_SUBJECT",                     
      "",     	                 
      tmpstr,                    
      sizeof(tmpstr) - 1,        
      filename                         
   );
   
   memset( EmailSubject, 0x00, sizeof(EmailSubject) );
   if ( tmpstr[0] == 0x00 )
      strcpy( EmailSubject, "Production Issue" );
   else
      memcpy( EmailSubject, tmpstr, sizeof(EmailSubject) );

}   /* SetStartupEnvironment() */

/*************************************************************
 *************************************************************/
void set_connection_status(INT status)
{
  connection_up = status;
}

/*************************************************************
 *************************************************************/
void terminate_connection()
{
  if (port_close) 
     port_close(connection_type);
  
  set_connection_status(FALSE);
}


/*****************************************************************/
/*****************************************************************/
INT dm_connection()
{
  BOOLEAN conn_successful = FALSE;
  CHAR    info_msg[256];

  conn_successful = tcp_convert((BYTE)connection_type, dm_config.db_tcp_port,
                                dm_config.db_tcp_header_length, dm_config.db_host_name);
  sprintf(info_msg, "dm_connection(), TCP/IP, port=%0ld, IP=%s",
                 dm_config.db_tcp_port, (strlen(dm_config.db_host_name) ? dm_config.db_host_name : "NONE"));
  email_log_message( 2, 1, info_msg,"dm_connection", 0 );
  set_connection_status (conn_successful);
  return(conn_successful);
}

/*************************************************************
 *************************************************************/
INT connection_is_up()
{
  return(connection_up);
}

/*************************************************************
 *************************************************************/
INT establish_connection()
{
  if (!dm_connection()) 
  {
    email_log_message( 1, 3, "Connection not established","establish_connection", 0 );
    return(FALSE);
  }

  return(TRUE);
}
/*************************************************************
 *************************************************************/
INT check(pCHAR buffer, WORD len, pCHAR state_desc, pIPC_INFO ipc_info)
{
   CHAR desc[48];

   sprintf(desc, "Function failed: check(), state=%s", state_desc);

   if (!port_write(len, buffer, ipc_info))
   {
	  email_log_message( 1, 3, desc,"check", 0 );
      terminate_connection();
      return (FALSE);
   }

   while (!GotResponse) pteipc_sleep(1000);
   if (GotResponse < 0)
   {
	  email_log_message( 1, 3, desc,"check", 0 );
      terminate_connection();
      return(FALSE);
   }

   return (TRUE);
}

/*************************************************************
 *************************************************************/
void MainProcessor()
{
   pPTE_MSG    p_msg;
   INT         max_threads = 0;
   LONG        ret_code;
   IPC_INFO    ipc_info;
   CHAR        AppName [8];
   CHAR        ErrorMsg[256];
   CHAR        Buffer  [256];

  //GetXipcMaxThread(&max_threads);
  //XipcMaxThreads = max_threads;
  XipcMaxThreads = 1024;

  GetAppName( AppName );
  GetXipcInstanceName( Buffer );
  email_get_error_warning_file_name_path();
  if( !pteipc_init_multiple_instance_app( AppName, ServiceName,  Buffer ) ) 
  {

	  email_log_message( 1, 3, "Failed to create XIPC queues","MainProcessor", 0 );
      EndProcessSignalled = 1;
  }
  else
  {
      SetStartupEnvironment();
      connection_type = DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU;
      set_connection_status(FALSE);  
      memset(&ipc_info, 0, sizeof(IPC_INFO));
      ipc_info.connectiontype      = connection_type;
      ipc_info.messageheaderlength = dm_config.db_tcp_header_length;
      ipc_info.tpduoption          = WITHOUT_TPDU;
  }

  while(!EndProcessSignalled)
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for 5 seconds, the blocking call returns   */
      p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ret_code );

      if( p_msg != NULL_PTR )
      {
         ptestats_accumulate_msg_stats (p_msg);

         switch( ptemsg_get_msg_type( p_msg ) )
         {
          case MT_SYSTEM_REQUEST : 
            ptesystm_system_msg_handler(p_msg);
            break;

          default:
            /* Establish the connection and receive sign on message */
            GotResponse = FALSE;
            State = State220;
            if (!establish_connection())
            {
               terminate_connection();
               continue;
            }
            
            while (!GotResponse) pteipc_sleep(1000);
            if (GotResponse < 0)
            {
               sprintf(ErrorMsg, "MainProcessor(), dm_notify(), state=signon response, error=%0d", GotResponse);
               email_log_message( 1, 3, ErrorMsg,"MainProcessor", 0 );
               terminate_connection();
               continue;
            }

			//printf("Ajay: Sending mail \n");
            /* send HELO message */
            GotResponse = FALSE;
            State = State250;
            sprintf(Buffer, "HELO %s%s", dm_config.db_host_name, CRLF);
            
            if (!check(Buffer, (WORD)strlen(Buffer), "Signon", &ipc_info)) continue;

            /* send MAIL FROM */
            GotResponse = FALSE;
            State = State250;
            sprintf(Buffer, "MAIL FROM:<%s>%s", EmailFromAcct, CRLF);
            if (!check(Buffer, (WORD)strlen(Buffer), "Mail From", &ipc_info)) continue;

            /* send RCPT TO */
            GotResponse = FALSE;
            State = State250;
            if (strlen(EmailAddress1))
            {
               sprintf(Buffer, "RCPT TO:<%s>%s", EmailAddress1, CRLF);
               if (!check(Buffer, (WORD)strlen(Buffer), "Rcpt To Add1", &ipc_info)) continue;
            }

            if (strlen(EmailAddress2))
            {
               GotResponse = FALSE;
               sprintf(Buffer, "RCPT TO:<%s>%s", EmailAddress2, CRLF);
               if (!check(Buffer, (WORD)strlen(Buffer), "Rcpt To Add2", &ipc_info)) continue;
            }

            if (strlen(EmailAddress3))
            {
               GotResponse = FALSE;
               sprintf(Buffer, "RCPT TO:<%s>%s", EmailAddress3, CRLF);
               if (!check(Buffer, (WORD)strlen(Buffer), "Rcpt To Add3", &ipc_info)) continue;
            }

            /* send DATA */
            GotResponse = FALSE;
            State = State354;
            sprintf(Buffer, "DATA%s", CRLF);
            if (!check(Buffer, (WORD)strlen(Buffer), "Data", &ipc_info)) continue;

            /* send message */
            GotResponse = TRUE; /* no response expected until we send the ending */
            State = State250;
            sprintf(Buffer, "Subject: %s%s%s", EmailSubject, CRLF, CRLF);
            if (!check(Buffer, (WORD)strlen(Buffer), "Message", &ipc_info)) continue;

            GotResponse = TRUE;
            memset(Buffer, 0, sizeof(ErrorMsg));
            memcpy(Buffer, ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg)),
                             ptemsg_get_pte_msg_data_data_length(ptemsg_get_pte_msg_data(p_msg)));
            if (!check(Buffer, (WORD)strlen(Buffer), "Message", &ipc_info)) continue;

            GotResponse = FALSE;
            sprintf(Buffer, "%s.%s", CRLF, CRLF);
		    if (!check(Buffer, (WORD)strlen(Buffer), "Message EOT", &ipc_info)) continue;

            /* send QUIT */
            GotResponse = FALSE;
            State = State221;
            sprintf(Buffer, "QUIT%s", CRLF);
            if (!check(Buffer, (WORD)strlen(Buffer), "Quit", &ipc_info)) continue;

            terminate_connection();
            pteipc_sleep(3000);
			/* Display msg once successfully sent mail TF-Ajay 12 Dec,2009*/
			sprintf(Buffer,"Email sent successfuly to %s, %s and %s :Successful",EmailAddress1,EmailAddress2,EmailAddress3);
		    email_log_message( 1, 2, Buffer,"MainProcessor", 0 );
         }
        
         free (p_msg);
      }
      else if( ret_code != QUE_ER_TIMEOUT ) 
      {
        pteipc_get_errormsg( ret_code, ErrorMsg );
        sprintf( Buffer, "MainProcessor(), pteipc_receive(), error=%s", ErrorMsg );
        email_log_message( 1, 3, Buffer,"MainProcessor", 0 );
        if (ret_code == QUE_ER_NOTLOGGEDIN)
           break;
      }
   }
  
  terminate_connection();
  email_log_message( 2, 1, "Process has stopped - EndProcess()","MainProcessor", 0 );
  pteipc_shutdown_multiple_instance_app(); 

  MainProcessDone = 1;
}

/*************************************************************
 *************************************************************/
void EndProcess()
{

}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void email_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};

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
								   email_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(email_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  email_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(email_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   if((strlen(email_error_warning_file_path) > 0) &&
	  (strlen(email_module_error_warning_file_name)> 0))
   {
	   email_create_Error_Filename();
   }
}

INT email_Log_error_warning_to_File(pCHAR Error_Buf,int sev, int detail)
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

	#define NUM_SIZE_WRITES  1

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(email_Erro_warning_Filename);
    path_len = strlen(email_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&email_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	email_create_Error_Filename();
    }
	if((fp = fopen(email_Erro_warning_Filename,"a+b"))==NULL)
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
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}
	fclose(fp);
	return  0;
}

void email_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( email_Erro_warning_Filename, email_error_warning_file_path );
    strcat( email_Erro_warning_Filename, email_module_error_warning_file_name );
	strcat( email_Erro_warning_Filename, ".EW." );
    strncat(email_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(email_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(email_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/******************************************************************************
 *
 *  NAME:         email_log_message
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
void email_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   email_Log_error_warning_to_File(text_message,sev,details);
   if(monitor_flag == 1)
   {
	   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
								 	 eventlog_flag, msg_type, func,
									 severity, log_type,
									 NULL, NULL,NULL );
   }

   return;
}
