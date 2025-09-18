/******************************************************************************
  
        Module: convert.c
  
         Title: dialog manager API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\convert.c  $
   
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

#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
#include "NTUtils.h"
#include "notify.h"


/* Global Variable Section */
PRIVATE char                *newline = "\n\0";

PRIVATE IPC_INFO            ipc_info;

PRIVATE USER_SERIAL_STRUCT  sd;
PRIVATE USER_TCP_STRUCT     td;
PRIVATE USER_DC_STRUCT      dc;



/***********************************************************
 ***********************************************************/
int log_message(pCHAR str)
{
  FILE *fptr;
  CHAR log_dir[100];

  GetPinnacleLogDirectory(log_dir);
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
 *****************************************************************/
void pteipc_send_with_check(pPTE_MSG p_msg, pCHAR que_name, pCHAR loginname)
{
  LONG ret_code;
  CHAR ErrorMsg[100];
  CHAR Buffer[256];

  /* check to see if the destination queue exists */
  ret_code = pteipc_queaccess( que_name );
  if (ret_code < 0)
     return;

  ret_code = pteipc_send( p_msg, que_name );
   if( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "PINNACLE: pteipc_send_with_check, error at sending to que %s: %s", que_name, ErrorMsg );
      LogEvent( Buffer, ERROR_MSG );
   }

  if (ret_code == XIPC_ER_NOTLOGGEDIN)
  {
    
    GetXipcInstanceName( Buffer );

    if (pteipc_xipclogin(Buffer, loginname))
    {
      pteipc_send( p_msg, que_name );
      pteipc_xipclogout();
    }
    else
    {
      LogEvent("PINNACLE: pteipc_send_with_check, login failed" , ERROR_MSG );
    }
  }
}

/*****************************************************************
 *****************************************************************/
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

/**************************
  Do not use orig_comms_info field for sending tpdu to GUI. This field is 
  reserved for applink. Instead, use chained data to send tpdu field
   ptemsg_set_orig_comms_info(p_msg_out, ascii_tpdu);
**************************/

	p_msg_out = ptemsg_chain_data( p_tmp_msg, (pBYTE)ascii_tpdu,
								          strlen( ascii_tpdu ) + 1, 0 );
   if ( p_msg_out == NULL_PTR )
   {
     LogEvent("PINNACLE: send_trace_to_gui, insufficient Memory to build message", ERROR_MSG );
     return;
   }
  
   ptemsg_set_orig_comms_info(p_msg_out, gui_orig_info);
   pteipc_send_with_check( p_msg_out, trace_queue, loginname );
   free( p_msg_out );
}

/***********************************************************
    function write_to_trace_file ()

    This function will write the data buffer to the trace
    file as a hex ascii dump.

 ***********************************************************/
void write_to_dialog_trace_file ( pBYTE bufptr, INT len,INT outgoing )
{
   static   BOOLEAN trace_busy = false;

   INT  numlines; 
   INT  line_count;
   INT  char_count;
   INT  col_index;
   INT  buf_index;
   CHAR tempstr [80];


   while (trace_busy == true)
   {
   }

   trace_busy = true;

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

  if (outgoing) 
    send_trace_to_gui(tmp, MT_OUTGOING, ascii_tpdu, loginname);
  else 
    send_trace_to_gui(tmp, MT_INCOMING, ascii_tpdu, loginname);
}

//***********************************************************
//***********************************************************
BOOLEAN serial_convert(INT connection_type, pCHAR pComPort, 
                    pCHAR pBaud        , pCHAR pByte,  
                    pCHAR pParity      ,  pCHAR pStop, 
                    pCHAR pBuffer)
{
  CHAR tmpbuf[51];

  if(!pComPort || !pBaud || !pByte || !pParity || !pStop)
    return(FALSE);

  // COM port so populate DM_SERIAL_STRUCT
  memset(&sd, 0, sizeof(sd));

  strncpy(sd.cPort, pComPort, 4);
  sscanf(pBaud, "%lu", &sd.dwBaudRate);
  
  if(strncmp(pByte, "7", 1) == 0)
    sd.bByteSize = DATABITS_7;
  else
    if(strncmp(pByte, "8", 1) == 0)
      sd.bByteSize = DATABITS_8;

  if(strncmp(pParity, "none", 4) == 0)
    sd.bParity = NOPARITY;
  else
    if(strncmp(pParity, "odd", 3) == 0)
      sd.bParity = ODDPARITY;
    else
      if(strncmp(pParity, "even", 4) == 0)
        sd.bParity = EVENPARITY;

  if(strncmp(pStop, "1", 3) == 0)
    sd.bStopBits = ONESTOPBIT;
  else
    if(strncmp(pStop, "1.5", 3) == 0)
      sd.bStopBits = ONE5STOPBITS;
    else
      if(strncmp(pStop, "2", 1) == 0)
        sd.bStopBits = TWOSTOPBITS;

  sd.lpBuf      = (pBYTE) pBuffer;
  sd.wBufferLen = MAX_OUTGOING_BUFFER_SIZE;

  switch(connection_type)
  {
  case SERIAL_TO_NAC:
    sd.pfNotify   = dm_notify;
    break;

  case SERIAL_TO_HOST:
    sd.pfNotify   = dtcp_notify;
    break;

  default:
    sd.pfNotify   = dm_notify;
    break;
  }

  GetXipcInstanceName( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  sprintf(ipc_info.instancename, "@%s", tmpbuf);
  ipc_info.connectiontype = DM_PORTTYPE_SERIAL;

  switch(connection_type)
  {
  case SERIAL_TO_NAC:
    sd.pfNotify   = dm_notify;
    strcpy(ipc_info.loginname, "ser2nac");
    break;

  case SERIAL_TO_HOST:
    sd.pfNotify   = dtcp_notify;
    strcpy(ipc_info.loginname, "ser2hst");
    break;

  default:
    sd.pfNotify   = dm_notify;
    strcpy(ipc_info.loginname, "ser2nac");
    break;
  }
  
  if(port_init(DM_PORTTYPE_SERIAL, &sd, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}      



//***********************************************************
//***********************************************************
BOOLEAN tcp_convert(BYTE connection_type, WORD tcp_port, pCHAR pBuffer)
{
  CHAR tmpbuf[51];

  // TCP/IP so populate USER_TCP_STRUCT
  memset(&td, 0, sizeof(td));

  td.usPort     = tcp_port;
  td.lpBuf      = (pBYTE)pBuffer;
  td.wBufferLen = MAX_OUTGOING_BUFFER_SIZE;
  td.pfNotify   = dm_notify;

  GetXipcInstanceName( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  sprintf(ipc_info.instancename, "@%s", tmpbuf);
  strcpy(ipc_info.loginname, "tcp2nac");
  ipc_info.connectiontype = connection_type;

  if(port_init(connection_type, &td, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}

/***********************************************************
 ***********************************************************/
BOOLEAN direct_tcp_convert(WORD host_tcp_port, 
                        pCHAR   host_name, 
                        pCHAR   pBuffer)
{
  CHAR tmpbuf[51];

  // TCP/IP so populate USER_TCP_STRUCT
  memset(&dc, 0, sizeof(dc));

  dc.DirectPort   = host_tcp_port;
  dc.HostName     = host_name;
  dc.lpBuf        = (pBYTE) pBuffer;
  dc.wBufferLen   = MAX_OUTGOING_BUFFER_SIZE;
  dc.pfNotify     = dtcp_notify;

  GetXipcInstanceName( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  sprintf(ipc_info.instancename, "@%s", tmpbuf);
  strcpy(ipc_info.loginname, "tcp2hst");
  ipc_info.connectiontype = DM_PORTTYPE_DIRECT;

  if(port_init(DM_PORTTYPE_DIRECT, &dc, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}

//***********************************************************
//***********************************************************
BOOLEAN raw_tcp_convert(WORD tcp_port, pCHAR pBuffer)
{
  CHAR tmpbuf[51];

  // TCP/IP so populate USER_TCP_STRUCT
  memset(&td, 0, sizeof(td));

  td.usPort     = tcp_port;
  td.lpBuf      = (pBYTE)pBuffer;
  td.wBufferLen = MAX_OUTGOING_BUFFER_SIZE;
  td.pfNotify   = dm_notify;

  GetXipcInstanceName( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  sprintf(ipc_info.instancename, "@%s", tmpbuf);
  strcpy(ipc_info.loginname, "rawtcp");
  ipc_info.connectiontype = DM_PORTTYPE_RAW_TCP;

  if(port_init(DM_PORTTYPE_RAW_TCP, &td, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}

/***********************************************************
 ***********************************************************/
BOOLEAN raw_direct_tcp_convert( WORD host_tcp_port, 
                                pCHAR   host_name, 
                                pCHAR   pBuffer)
{
  CHAR tmpbuf[51];

  // TCP/IP so populate USER_TCP_STRUCT
  memset(&dc, 0, sizeof(dc));

  dc.DirectPort   = host_tcp_port;
  dc.HostName     = host_name;
  dc.lpBuf        = (pBYTE) pBuffer;
  dc.wBufferLen   = MAX_OUTGOING_BUFFER_SIZE;
  dc.pfNotify     = dtcp_notify;

  GetXipcInstanceName( tmpbuf );

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  sprintf(ipc_info.instancename, "@%s", tmpbuf);
  strcpy(ipc_info.loginname, "rawdtcp");
  ipc_info.connectiontype = DM_PORTTYPE_RAW_DIRECT;

  if(port_init(DM_PORTTYPE_RAW_DIRECT, &dc, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}