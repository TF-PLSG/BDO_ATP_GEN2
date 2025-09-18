/******************************************************************************

 Module: convert.c
 
  Title: dialog manager API Routines
  
   Description: 
   
	$Log:   N:\PVCS\PTE\CORE\ptecom_lib\convert.c  $
	

								
******************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
// For nanosleep
#include <time.h>
#endif

#include "tpsrtool.h"
#include "convert.h"
extern "C" {
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
#include "ntutils.h"
}
#include "notify.h"


#ifdef SSL_SUPPORT
#include "ssldefs.h"
#endif

extern char log_folder_name[100];
extern HWND hWnd;
/* Global Variable Section */
PRIVATE char                *newline = "\n\0";


PRIVATE USER_SERIAL_STRUCT  Serial;
PRIVATE USER_TCP_STRUCT     TcpServer, TcpClient;

#ifdef SSL_SUPPORT
PRIVATE USER_SSL_STRUCT     SSLServer, SSLClient;
#endif

/***********************************************************
***********************************************************/
int log_message(pCHAR str, INT outgoing)
{
	FILE *fptr;
	char szThreadId[100] = {0};
	char log_dir[1024];
	
	strcpy(log_dir,log_folder_name);
	strcat(log_dir, trace_file);
	DWORD dwThreadId = GetCurrentThreadId();
	if(outgoing)
	{
		sprintf(szThreadId,".outgoing.%X",dwThreadId);
	}
	else
	{
		sprintf(szThreadId,".incoming.%X",dwThreadId);
	}
	strcat(log_dir,szThreadId);
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
#ifdef WIN32
		  Sleep(10);
#else
		  struct timespec intv;
		  intv.tv_sec = 0;
		  intv.tv_nsec = 10000000;
		  //pteipc_sleep(100);
		  nanosleep (&intv, NULL);
#endif
	  }
	  
	  trace_busy = TRUE;
	  
	  /*
      log the timestamp and message length.
	  */
	  ptetime_get_timestamp (tempstr);
	  strcat (tempstr, newline);
	  log_message(tempstr, outgoing);
	  
	  if (outgoing)
		  sprintf (tempstr, "outgoing message length = %d\n", len);
	  else
		  sprintf (tempstr, "incoming message length = %d\n", len);
	  
	  log_message(tempstr, outgoing);
	  
	  /*
      format every sixteen bytes of the data into a trace line.
      "xx xx xx xx xx xx xx xx - xx xx xx xx xx xx xx xx       [................]"
      this outer for-loop controls the line count.
	  */
	  buf_index = 0;
	  numlines  = (len / 16);
	  
	  if ((len % 16) != 0)
		  numlines++;
	  		  int byteCount = 0;

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
			  byteCount++;
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
		  log_message(tempstr, outgoing);
		  buf_index = buf_index + 16;
      }  /* for line_count */
	  
	  log_message(newline, outgoing);
	  
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

#ifdef SERIAL_WIN32
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
	IPC_INFO            ipc_info;

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
		TcpClient.HostName    = (unsigned char *)host_name;
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
