/******************************************************************************
  
        Module: convert.c
  
         Title: dialog manager API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\convert.c  $
   
      Rev 1.21   Nov 10 1999 17:48:56   MSALEH
   * Modify SSL connection flow
   * Use a function to build a complete XIPC
   login string
   
      Rev 1.20   Oct 18 1999 14:47:52   MSALEH
   correct sprintf and login name for ssl connections
   
      Rev 1.19   Oct 14 1999 10:41:46   MSALEH
   Integrate SSL modifications
   
      Rev 1.18   Oct 06 1999 09:38:24   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.17   Sep 28 1999 14:47:40   MSALEH
   AIX Modifications
   
      Rev 1.15   Aug 25 1999 15:54:30   MSALEH
   reworked the socket handling to use linked lists
   so that there's no maximum on the number of 
   connections
   
      Rev 1.14   Apr 14 1999 15:23:10   MSALEH
   Mods to handle new IGNORE option
   
      Rev 1.13   Apr 01 1999 12:22:00   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.12   Mar 01 1999 17:23:34   MSALEH
   TCP: added functionality to handle inclusive
   and exclusive length bytes, if the tcp_header_length >= 100
   then inclusive length is assumed.
   Serial: For length 0, don't send an LRC, this
   is a racal thing only
   
      Rev 1.11   Feb 03 1999 16:01:38   MSALEH
   corrected bug in Serial Connection, we were not 
   passing the processwithtpdu flag
   
      Rev 1.10   Oct 08 1998 16:42:48   MSALEH
   1) Change "PINNACLE" to "ASCENDENT"
   2) Add new fields to dm_config
       db_tcp_socket_type
       db_tcp_headet_length
       db_tpdu_processing
   3) delete the field db_host_tcp_port from dm_config
   4) Restructure ptecom.lib/coreds/pindlg/dialog
   to be more configurable
   5) Update GUI to reflect new fields
   6) Rework the functionality of the connection
   spy to by included within the main loop
   7) Client connect tries only once then return
   and tries again after a timeout value, this will
   prevent the Client Connection from hanging during connect
   8) Streamline functions/variables
   9) All character comparisons are case insensitive 
   
   
      Rev 1.9   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.8   19 Aug 1998 14:58:44   rmalathk
   1. modified send_trace_to_gui function to
       send trace messages from a service to
       GUI via applink.
   2. The tpdu field is no longer sent in the 
       orig_comms_info field to GUI. It is sent
       as a second piece of data chained to the
       trace.
   
   
      Rev 1.7   Jul 20 1998 08:59:48   drodden
   Changed the trace routine to maintain a static busy flag to keep multiple 
   threads from writing to the trace file at the same time.  This is not the best 
   sychronization method, but is better than nothing.
   
   
      Rev 1.6   01 May 1998 14:29:48   MSALEH
   Changed MT_DIALOG_INCOMING to 
   ST1_TRACE to conform to new message
   type definitions
   
      Rev 1.5   Apr 27 1998 09:48:06   drodden
   Moved all of the common system wide defines and typedefs out of the application
   headers (app_info) into the core headers (pte, coreinfo).
   This is part of the creation of the Core Dataserver.
   
   
      Rev 1.4   Apr 17 1998 10:15:46   drodden
   Changed to put a newline after the timestamp in the trace.
   
   
      Rev 1.3   Apr 07 1998 08:27:28   drodden
   Removed the dialogg header.  The defines were move to pte_h and it is no longer used.
   
   
      Rev 1.2   27 Mar 1998 16:07:40   MSALEH
   1) extract notify routines from convert.c
   into notify.c and notify.h
   2) Applications can either use the default notification
   routines or override them with their own
   local routines
   3) Add routines to handle "RAW" tcp read and write
   that DO NOT append or parse two byte length
   from an outgoing or incoming message
   4) Cannot handle concatenation and segmentation
   in that case, the calling application will have
   to handle that
   
      Rev 1.1   Mar 26 1998 08:45:04   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types wherever possible.
   
   
      Rev 1.0   17 Mar 1998 14:01:14   MSALEH
   Common file for all PTE Comm Apps
   Initial Release
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
// For nanosleep
#include <time.h>
#endif

#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
#include "ntutils.h"
#include "notify.h"

#ifdef SSL_SUPPORT
#include "ssldefs.h"
#endif

/* Global Variable Section */
PRIVATE char                *newline = "\n\0";

PRIVATE IPC_INFO            ipc_info;

PRIVATE USER_SERIAL_STRUCT  Serial;
PRIVATE USER_TCP_STRUCT     TcpServer, TcpClient;

#ifdef SSL_SUPPORT
PRIVATE USER_SSL_STRUCT     SSLServer, SSLClient;
#endif

/***********************************************************
 ***********************************************************/
int log_message(pCHAR str)
{
  FILE *fptr;
  CHAR log_dir[100];

  GetAscendentLogDirectory(log_dir);
  strcat(log_dir, trace_file);
  if ((fptr = fopen(log_dir,"a+")) != NULL)
  {
    fputs(str, fptr);
    fclose(fptr);
    return(TRUE);
  }
  else
    return(FALSE);
}



/*****************************************************************
 ****************************************************************
void pteipc_send_with_check(pPTE_MSG p_msg, pCHAR que_name, pCHAR loginname)
{
  LONG ret_code;
  CHAR ErrorMsg[100];
  CHAR Buffer[256];

  // check to see if the destination queue exists
  ret_code = pteipc_queaccess( que_name );
  if (ret_code < 0)
  {
      return;
  }

  ret_code = pteipc_send( p_msg, que_name );
  if( ret_code < 0 )
  {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "ASCENDENT: pteipc_send_with_check, error at sending to que %s: %s", que_name, ErrorMsg );
      LogEvent( Buffer, ERROR_MSG );
  }

  if (ret_code == XIPC_ER_NOTLOGGEDIN)
  {
    
    pteipc_format_xipc_instance_name( Buffer );

    if (pteipc_xipclogin(Buffer, loginname))
    {
      pteipc_send( p_msg, que_name );
      pteipc_xipclogout();
    }
    else
    {
      LogEvent("ASCENDENT: pteipc_send_with_check, login failed" , ERROR_MSG );
    }
  }
}
*/

/*****************************************************************
 ****************************************************************
void send_trace_to_gui(pBYTE tempbuf,
                       BYTE msg_type,
                       pCHAR ascii_tpdu,
                       pCHAR loginname)
{
  pPTE_MSG p_msg_out, p_tmp_msg;
  
  p_tmp_msg = ptemsg_build_msg (msg_type,
                                ST1_TRACE,
                                0,
                                trace_queue,
                                application_que_name,
                                tempbuf,
                                strlen(tempbuf),
                                0);

//
//  Do not use orig_comms_info field for sending tpdu to GUI. This field is 
//  reserved for applink. Instead, use chained data to send tpdu field
//   ptemsg_set_orig_comms_info(p_msg_out, ascii_tpdu);
//

	p_msg_out = ptemsg_chain_data( p_tmp_msg, (pBYTE)ascii_tpdu,
								          strlen( ascii_tpdu ) + 1, 0 );
   if ( p_msg_out == NULL_PTR )
   {
     LogEvent("ASCENDENT: send_trace_to_gui, insufficient Memory to build message", ERROR_MSG );
     return;
   }
  
   ptemsg_set_orig_comms_info(p_msg_out, gui_orig_info);
   //pteipc_send_with_check( p_msg_out, trace_queue, loginname );
   free( p_msg_out );
}
*/

/***********************************************************
    function write_to_trace_file ()

    This function will write the data buffer to the trace
    file as a hex ascii dump.

 ***********************************************************/
void write_to_dialog_trace_file ( pBYTE bufptr, INT len,INT outgoing )
{
   static   BOOLEAN trace_busy = FALSE;

   INT  numlines; 
   INT  line_count;
   INT  char_count;
   INT  col_index;
   INT  buf_index;
   CHAR tempstr [80];


   while (trace_busy)
    {
	struct timespec intv;
	intv.tv_sec = 0;
	intv.tv_nsec = 10000000;
	//pteipc_sleep(100);
	nanosleep (&intv, NULL);
    }

   trace_busy = TRUE;

   /*
      log the timestamp and message length.
   */
   ptetime_get_timestamp (tempstr);
   strcat (tempstr, newline);
   log_message(tempstr);

   if (outgoing)
      sprintf (tempstr, "outgoing message length = %d\n", len);
   else
      sprintf (tempstr, "incoming message length = %d\n", len);

   log_message(tempstr);

   /*
      format every sixteen bytes of the data into a trace line.
      "xx xx xx xx xx xx xx xx - xx xx xx xx xx xx xx xx       [................]"
      this outer for-loop controls the line count.
   */
   buf_index = 0;
   numlines  = (len / 16);

   if ((len % 16) != 0)
      numlines++;

   for (line_count=0; line_count < numlines; line_count++)
      {
      /*
         this while loop will format the next sixteen bytes into
         the hex ascii digits on the left side of the trace line.
      */
      col_index  = 0;
      char_count = 0;

      while ( (char_count             < 16 ) &&
              ((buf_index+char_count) < len)    )
         {
         sprintf (&tempstr [col_index], "%02x ", bufptr[(buf_index+char_count)]);

         char_count++;
         col_index = col_index + 3;

         if (char_count == 8)
            {
            tempstr [col_index]     = '-';
            tempstr [col_index + 1] = ' ';
            col_index = col_index + 2;
            }
         }  /* while */


      /*
         this while loop will format the same sixteen bytes into the ascii
         text display on the right end of the trace line.  Non-printable
         characters are represented by a ".".
      */
      memset (&tempstr [col_index], ' ', sizeof(tempstr) - col_index - 1);
      tempstr [57] = '[';
      col_index  = 58;
      char_count = 0;
      
      while (char_count < 16)
         {
         if ((buf_index+char_count) < len) 
            {
            if ( (bufptr[(buf_index+char_count)] > 31 ) &&
                 (bufptr[(buf_index+char_count)] < 127)    )
               tempstr [col_index] = bufptr[(buf_index+char_count)];
            else
               tempstr [col_index] = '.';
            }

         col_index++;
         char_count++;
         }  /* while */

      /*
         finish the trace line by adding the closing bracket and a newline,
         then write it to the log.
      */
      tempstr [col_index  ] = ']';
      tempstr [col_index+1] = '\0';
      strcat (tempstr, newline);
      log_message(tempstr);
      buf_index = buf_index + 16;
      }  /* for line_count */

   log_message(newline);

   trace_busy = false;
}


/***********************************************************
 ***********************************************************/
void PrintBuffer (pBYTE tmpbuf,
                  INT len,
                  INT outgoing,
                  pCHAR ascii_tpdu,
                  pCHAR loginname)
{
  char   tmp[MAX_CHARS_TO_DISPLAY * 3];
  int    x, msg_len;

  memset(tmp, 0, sizeof(tmp));
  write_to_dialog_trace_file (tmpbuf, len, outgoing);
  msg_len = ((int)len < MAX_CHARS_TO_DISPLAY) ? (int)len : MAX_CHARS_TO_DISPLAY;

  if ((msg_len % 2) == 0) 
    msg_len -= 2;
  else 
    msg_len -= 1;

  for(x = 0; x < msg_len; x++) 
    sprintf (tmp + (x * 3), "%02x ", tmpbuf[x]);

  #ifdef _DEBUG
  printf("\n----formatted trace message in ascii format");
  printf("\nMessage type - %s ", outgoing ? "outgoing" : "incoming");
  printf("\nMessage data body - %s", tmp);
  printf("\n----end of trace message");
  #endif
}

#ifdef WIN32
/***********************************************************
***********************************************************/
BOOLEAN serial_convert( BYTE connection_type, pCHAR pComPort, 
                        pCHAR pBaud        , pCHAR pByte,  
                        pCHAR pParity      , pCHAR pStop, 
                        INT headerlength)
{
  CHAR tmpbuf[MAX_INSTANCE_NAME_SIZE] = "";

  if(!pComPort || !pBaud || !pByte || !pParity || !pStop)
    return(FALSE);

  /* COM port so populate DM_SERIAL_STRUCT */
  memset(&Serial, 0, sizeof(Serial));

  strncpy(Serial.cPort, pComPort, 4);
  sscanf(pBaud, "%lu", &Serial.dwBaudRate);
  
  if(strncmp(pByte, "7", 1) == 0)
    Serial.bByteSize = SEVENBITS;
  else
    if(strncmp(pByte, "8", 1) == 0)
      Serial.bByteSize = EIGHTBITS;

  if(strncmp(_strupr(pParity), "NONE", 4) == 0)
    Serial.bParity = NOPARITY;
  else
    if(strncmp(_strupr(pParity), "ODD", 3) == 0)
      Serial.bParity = ODDPARITY;
    else
      if(strncmp(_strupr(pParity), "EVEN", 4) == 0)
        Serial.bParity = EVENPARITY;

  if(strncmp(pStop, "1", 3) == 0)
    Serial.bStopBits = ONESTOPBIT;
  else
    if(strncmp(pStop, "1.5", 3) == 0)
      Serial.bStopBits = ONE5STOPBITS;
    else
      if(strncmp(pStop, "2", 1) == 0)
        Serial.bStopBits = TWOSTOPBITS;

  Serial.pfNotify   = dm_notify;

  //pteipc_format_xipc_instance_name( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  strcpy(ipc_info.instancename, tmpbuf);
  ipc_info.connectiontype        = connection_type;
  ipc_info.messageheaderlength   = headerlength;

  switch(connection_type)
  {
      case DM_PORTTYPE_SERIAL_WITHOUT_TPDU:
         strcpy(ipc_info.loginname, "swotpdu");
         ipc_info.tpduoption = WITHOUT_TPDU;
         break;

      case DM_PORTTYPE_SERIAL_WITH_TPDU:
         strcpy(ipc_info.loginname, "swtpdu");
         ipc_info.tpduoption = WITH_TPDU;
         break;

      case DM_PORTTYPE_SERIAL_IGNORE_TPDU:
         strcpy(ipc_info.loginname, "sigtpdu");
         ipc_info.tpduoption = IGNORE_TPDU;
         break;
  }
  
  if(tpsr_port_init(connection_type, &Serial, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}
#endif

/***********************************************************
***********************************************************/
BOOLEAN tpsr_tcp_convert(BYTE connection_type  , WORD tcp_port,
                    INT header_length     , pCHAR host_name , int tlevel)
{
  memset(&ipc_info, 0, sizeof(IPC_INFO));
  strcpy(ipc_info.instancename, "pteipc");
  ipc_info.connectiontype = connection_type;
  ipc_info.messageheaderlength = header_length;

  switch (connection_type)
  {
      case DM_PORTTYPE_TCP_SERVER_WITH_TPDU:
      case DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU:
      case DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU:
         memset(&TcpServer, 0, sizeof(TcpServer));
         TcpServer.usPort     = tcp_port;
         TcpServer.pfNotify   = dm_notify;

         if (connection_type == DM_PORTTYPE_TCP_SERVER_WITH_TPDU)
         {
            strcpy(ipc_info.loginname, "tswtpdu");
            ipc_info.tpduoption = WITH_TPDU;
         }
         else if (connection_type == DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU)
         {
            strcpy(ipc_info.loginname, "tswotpdu");
            ipc_info.tpduoption = WITHOUT_TPDU;
         }
         else if (connection_type == DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU)
         {
            strcpy(ipc_info.loginname, "tsigtpdu");
            ipc_info.tpduoption = IGNORE_TPDU;
         }

         if(tpsr_port_init(connection_type, &TcpServer, &ipc_info, tlevel) == FALSE)
         {
            return(FALSE);
         }

         return(TRUE);

      case DM_PORTTYPE_TCP_CLIENT_WITH_TPDU:
      case DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU:
      case DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU:
         memset(&TcpClient, 0, sizeof(TcpClient));
         TcpClient.usPort      = tcp_port;
         TcpClient.HostName    = host_name;
         TcpClient.pfNotify    = dm_notify;

         if (connection_type == DM_PORTTYPE_TCP_CLIENT_WITH_TPDU)
         {
            strcpy(ipc_info.loginname, "tcwtpdu");
            ipc_info.tpduoption = WITH_TPDU;
         }
         else if (connection_type == DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU)
         {
            strcpy(ipc_info.loginname, "tcwotpdu");
            ipc_info.tpduoption = WITHOUT_TPDU;
         }
         else if (connection_type == DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU)
         {
            strcpy(ipc_info.loginname, "tcigtpdu");
            ipc_info.tpduoption = IGNORE_TPDU;
         }


         if(tpsr_port_init(connection_type, &TcpClient, &ipc_info, tlevel) == FALSE)
         {
            return(FALSE);
         }

         return(TRUE);
  }

  return (FALSE);

}

#ifdef SSL_SUPPORT
/**************************************************************************/
/**************************************************************************/
BOOLEAN ssl_convert(BYTE connection_type          , WORD tcp_port, 
                    LOCAL_CM_CONFIG cm_config     , pCHAR host_name )
{
  CHAR		tmpbuf[MAX_INSTANCE_NAME_SIZE] = "";

  //pteipc_format_xipc_instance_name( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  strcpy(ipc_info.instancename, tmpbuf);
  ipc_info.connectiontype = connection_type;
  ipc_info.messageheaderlength = cm_config.db_tcp_header_length;
 
  memset(&TcpServer, 0, sizeof(TcpServer));
  TcpServer.usPort     = tcp_port;
  TcpServer.pfNotify   = cm_notify;

  //strcpy(ipc_info.loginname, "tswtpdu");
  ipc_info.tpduoption = WITH_TPDU;

  /* set up the port */
  sprintf(ipc_info.ssl_port, "%s:%0ld", cm_config.db_host_name, cm_config.db_tcp_port);
  strcpy(ipc_info.loginname, "sslwtpdu");

  if(server_ssl_init(&TcpServer, &ipc_info, cm_config) == FALSE)
    return(FALSE);
  else
    return(TRUE);
}

#endif
