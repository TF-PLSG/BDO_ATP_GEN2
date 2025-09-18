/******************************************************************************

 Module: tpsrtool.c
 
  Title: tpsrtool manager
  
   Description: 
   
	$Log:   N:\PVCS\PTE\CORE\DIALOG\COMMS\DIALOG.C  $
	

									   
******************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#else
#include <time.h>
#include <signal.h>
#include <errno.h>
#endif

#include "tpsrtool.h"
#include <direct.h>
#include "common.h"
#include "convert.h"
extern "C" {
#include "pte.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptesystm.h"
#include "ptestats.h"
#include "ntutils.h"
}
#include "coreinfo.h"
#include "cdsapi.h"

HWND hWnd;

/* Extern definition section */
int  volatile EndProcessSignalled;
int  volatile MainProcessDone;
extern char          ServiceName[12];
extern long volatile responsesReceived;
extern long volatile transactionsSent;
extern int errno;

int	volatile	startSending = 0;
char        ServiceName[12];
char		ExeName[MAX_APP_NAME_SIZE];
char		log_folder_name[1024];

/*
Global variables to hold tool settings read from tf.ini.
*/
int			ReqTpsRate;
int			SimultaneousPosConnections;
int			autoMode;
char			flowListFileName[256];
char			CardNumberListFileName[256];
char			deviceMerchantListFileName[256];

PRIVATE int          cur_tpdu = 0;
PRIVATE int volatile received_disconnect_from_gui;
PRIVATE int          tpdu_tran_len;
PRIVATE int          tpdu_dst_len;
PRIVATE int          tpdu_src_len;
PRIVATE int          tpdu_length;
PRIVATE int          connection_type;
PRIVATE int          length_of_tpdu_scroll;
PRIVATE int          length_of_router_scroll;

PRIVATE WORD         primary_tcp_port_number, secondary_tcp_port_number;
PRIVATE char         primary_tcp_host_name[21], secondary_tcp_host_name[21];
BOOLEAN volatile     use_primary;

PRIVATE pTPDU_TABLE_LIST    db_tpdu_list;
PRIVATE pROUTER_TABLE_LIST  db_router_list;
PRIVATE pDM_CONFIG          db_dm_config;

PRIVATE LOCAL_DM_CONFIG     dm_config;
PRIVATE LOCAL_TPDU          tpdu_scroll   [MAX_CONFIG_ENTRY];
PRIVATE LOCAL_ROUTER_TABLE  router_scroll [MAX_CONFIG_ENTRY];

PRIVATE BYTE outgoing_buf[MAX_OUTGOING_BUFFER_SIZE];


/* Functions */
void send_confirm(BYTE msg_subtype, char * dest_queue, char *orig_info);
extern void initialize_activity_timer( char * service_name );

INT GetPrivateProfileString_TPSR( pCHAR section_name,
                             pCHAR key_name,
                             pCHAR default_str,
                             pCHAR dest_buffer,
                             UINT  buffer_size,
                             pCHAR file_name );

/*************************************************************
*************************************************************/
void set_connection_status(int status)
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
int tpsr_connection(int tlevel)
{
	BOOLEAN conn_successful = FALSE;
	char    info_msg[256];
	
	switch (connection_type)
	{
#ifdef WIN32
	case DM_PORTTYPE_SERIAL_WITH_TPDU:
	case DM_PORTTYPE_SERIAL_WITHOUT_TPDU:
	case DM_PORTTYPE_SERIAL_IGNORE_TPDU:
#ifdef SERIAL_WIN32
		conn_successful = serial_convert((BYTE)connection_type      , dm_config.db_com_port, 
			dm_config.db_baud_rate     , dm_config.db_byte_size, 
			dm_config.db_parity        , dm_config.db_stop_bits, 
			dm_config.db_tcp_header_length);
#endif
		LogEvent("ASCENDENT: dm_connection, Serial Connection", INFO_MSG);
		break;
#endif
		
	case DM_PORTTYPE_TCP_SERVER_WITH_TPDU:
	case DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU:
	case DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU:
	case DM_PORTTYPE_TCP_CLIENT_WITH_TPDU:
	case DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU:
	case DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU:
		if ((primary_tcp_port_number <= 1024) || (secondary_tcp_port_number <= 1024))
		{
			printf("ASCENDENT: dm_connection, invalid socket port number, must be > 1024");
		}
		else
		{
			if (use_primary)
			{
				conn_successful = tpsr_tcp_convert((BYTE)connection_type, primary_tcp_port_number,
					dm_config.db_tcp_header_length, primary_tcp_host_name, tlevel);
				
				sprintf(info_msg, "ASCENDENT: dm_connection, TCP/IP Connection using port number %0ld, IP Address %s",
					primary_tcp_port_number, 
					(strlen(primary_tcp_host_name) ? primary_tcp_host_name : "NONE"));
				
			}
			else
			{
				conn_successful = tpsr_tcp_convert((BYTE)connection_type, secondary_tcp_port_number,
					dm_config.db_tcp_header_length, secondary_tcp_host_name, tlevel);
				
				sprintf(info_msg, "ASCENDENT: dm_connection, TCP/IP Connection using port number %0ld, IP Address %s",
					secondary_tcp_port_number, 
					(strlen(secondary_tcp_host_name) ? secondary_tcp_host_name : "NONE"));
				
			}
			
			printf("%s\n",info_msg);
			
		}
		break;
		
	default: 
		printf("ASCENDENT: dm_connection, illegal value for connection type");
		break;
	}
	
	set_connection_status (conn_successful);
	
	return(conn_successful);
}

/*************************************************************
*************************************************************/
int connection_is_up()
{
	return(connection_up);
}

/*************************************************************
*************************************************************/
int tpsr_establish_connection(int tlevel)
{
	if (!tpsr_connection(tlevel))
	{
		printf("tpsr_connection, nac connection not established.\n");
		return(FALSE);
	}
	
	return(TRUE);
}

/*************************************************************
*************************************************************/
int exists_in_tpdu_scroll(char *inStr)
{
	char tmp_buf[100];
	int i;
	
	for (i = 0; i < length_of_tpdu_scroll; i++) 
	{
		if (strcmp(inStr, tpdu_scroll[i].tpdu_id) == 0)
		{
			cur_tpdu = i;
			return(TRUE);
		}
	}
	
	sprintf(tmp_buf, "Ascendent: exists_in_tpdu_scroll, cannot find tpdu_id<%s> ", inStr);
	LogEvent(tmp_buf, ERROR_MSG);
	return (FALSE);
}

/************************************************************* Mark for delete
*************************************************************/
int get_tpdu_length()
{
	return(tpdu_length);
}

/*************************************************************
*************************************************************/
void refresh_dialog()
{
	if (strcmp(dm_config.db_connection_type, "SERIAL") == 0) 
	{
		if (strcmp(dm_config.db_tpdu_processing, "WITH") == 0)
			connection_type = DM_PORTTYPE_SERIAL_WITH_TPDU;
		else if (strcmp(dm_config.db_tpdu_processing, "WITHOUT") == 0)
			connection_type = DM_PORTTYPE_SERIAL_WITHOUT_TPDU;
		else if (strcmp(dm_config.db_tpdu_processing, "IGNORE") == 0)
			connection_type = DM_PORTTYPE_SERIAL_IGNORE_TPDU;
	}
	else 
	{
		if (strcmp(dm_config.db_tcp_socket_type, "SERVER") == 0)
		{
			if (strcmp(dm_config.db_tpdu_processing, "WITH") == 0)
				connection_type = DM_PORTTYPE_TCP_SERVER_WITH_TPDU;
			else if (strcmp(dm_config.db_tpdu_processing, "WITHOUT") == 0)
				connection_type = DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU;
			else if (strcmp(dm_config.db_tpdu_processing, "IGNORE") == 0)
				connection_type = DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU;
		}
		else
		{
			if (strcmp(dm_config.db_tpdu_processing, "WITH") == 0)
				connection_type = DM_PORTTYPE_TCP_CLIENT_WITH_TPDU;
			else if (strcmp(dm_config.db_tpdu_processing, "WITHOUT") == 0)
				connection_type = DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU;
			else if (strcmp(dm_config.db_tpdu_processing, "IGNORE") == 0)
				connection_type = DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU;
		}
	}
	
	tpdu_tran_len = 1;
	tpdu_dst_len  = 2;
	tpdu_src_len  = 2;
	tpdu_length   = 5;
	
}

/*************************************************************
*************************************************************/
int update_dm_config()
{
	char  *ptr;
	//    char   err_msg[100];
	char   temp[256];
	char   hostNameOrIP[16];
	char   dialogPort[10];
	char   filename[256], filename2[256], filename3[256], filename4[256];
	char   tpsRate[10], autoModeStr[5], no_of_pos[10]= {0};
	
	//GetAscendentConfigDirectory( filename );
	
	strcpy( filename, "tpsr.ini" );

	FILE *fptr = fopen(filename,"r");
	if(fptr != NULL)
	{
		fclose (fptr);
	}
	
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"SERVER",
		"",
		hostNameOrIP,
		sizeof( hostNameOrIP ),
		filename );
	printf("\nSERVER = %s\n",hostNameOrIP);
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"PORT",
		"1400",
		dialogPort,
		sizeof( dialogPort ),
		filename );
	printf("DialogPort = %s\n",dialogPort);
	
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"TPS",
		"10",
		tpsRate,
		sizeof( tpsRate ),
		filename );
	ReqTpsRate = atoi (tpsRate);
	printf("TPS = %s\n",tpsRate);
	
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"POS_CONNECTIONS",
		"10",
		no_of_pos,
		sizeof( no_of_pos),
		filename );
	SimultaneousPosConnections = atoi (no_of_pos);
	printf("POS_CONNECTIONS = %s\n",no_of_pos);

	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"AUTOMODE",
		"",
		autoModeStr,
		sizeof( autoModeStr ),
		filename );
	autoMode = atoi (autoModeStr);
	
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"FLOW_FILE_NAME",
		"",
		filename2,
		sizeof( filename2 ),
		filename );
	
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"CARD_FILE_NAME",
		"",
		filename3,
		sizeof( filename3 ),
		filename );
	
	GetPrivateProfileString_TPSR( "TPSRTOOL",
		"DEVICE_FILE_NAME",
		"",
		filename4,
		sizeof( filename4 ),
		filename );
	
/*	GetAscendentRootDirectory( flowListFileName );
	strcpy ( CardNumberListFileName, flowListFileName);	// Copy it for later usage
	strcpy ( deviceMerchantListFileName, flowListFileName);	// Copy it for later usage
	strcat ( flowListFileName, "flowfile\\");
*/
	strcpy ( flowListFileName, filename2 );
	
	
/*	strcat ( CardNumberListFileName, "flowfile\\");
	strcat ( CardNumberListFileName, filename3 );	// Built the file name, including path, for list of valid card numbers
	strcat ( deviceMerchantListFileName, "flowfile\\");

*/	
	strcpy ( CardNumberListFileName, filename3);
	strcpy ( deviceMerchantListFileName, filename4 );	// Built the file name, including path, for list of valid device-merchant combination
	//strcpy ( deviceMerchantListFileName, flowListFileName);

	strcpy((pCHAR) db_dm_config->db_subsystem_name, ServiceName);
	
	/* copy the returned struct to locally defined struct */
	strcpy((pCHAR) dm_config.db_subsystem_name  , (const char *) db_dm_config->db_subsystem_name);
	strcpy((pCHAR) dm_config.db_connection_type , "TCP/IP");
	strcpy((pCHAR) dm_config.db_com_port        , "");
	strcpy((pCHAR) dm_config.db_baud_rate       , "");
	strcpy((pCHAR) dm_config.db_byte_size       , "");
	strcpy((pCHAR) dm_config.db_parity          , "");
	strcpy((pCHAR) dm_config.db_stop_bits       , "");
	
	strcpy((pCHAR) db_dm_config->db_host_name       , (const char *)hostNameOrIP);
	strcpy((pCHAR) db_dm_config->db_tcp_port        , (const char *)dialogPort);
	
	/*
	we will use Primary_tcp_port_number and secondary_tcp_port_number
	to support primary/secondary tcp connections
	*/
	
	strcpy(temp, (const char *)db_dm_config->db_tcp_port);
	ptr = (char*)strchr(temp, ORIG_INFO_SEPARATOR);
	if (ptr)
	{
		*ptr = 0x00;
		primary_tcp_port_number   = atoi(temp);
		ptr++;
		secondary_tcp_port_number = atoi(ptr);
	}
	else
	{
		primary_tcp_port_number   = 
			secondary_tcp_port_number = atoi(temp);
	}
	
	strcpy(temp, (const char *)db_dm_config->db_host_name);
	ptr = (char*)strchr(temp, ORIG_INFO_SEPARATOR);
	if (ptr)
	{
		*ptr = 0x00;
		strcpy(primary_tcp_host_name, temp);
		ptr++;
		strcpy(secondary_tcp_host_name, ptr);
	}
	else
	{
		strcpy(primary_tcp_host_name, temp); 
		strcpy(secondary_tcp_host_name, temp); 
	}
	
	use_primary = TRUE;
	
	/* dm_config.db_tcp_port = atoi(db_dm_config->db_tcp_port); */
	/* strcpy(dm_config.db_host_name       , db_dm_config->db_host_name); */
	
	dm_config.db_tcp_header_length = atoi("2");
	
	strcpy(dm_config.db_tcp_socket_type , "CLIENT");
	strcpy(dm_config.db_tpdu_processing , "WITHOUT");
	
	refresh_dialog();
	return(TRUE);
}

  /*************************************************************
  *************************************************************/
  /** Mark for delete **/
  int decode_tpdu(pPTE_MSG p_cur_msg, pBYTE tmpbuf, char *dest_que_name, SOCKET *client_socket)      
  {
	  int   total_tpdu_len, len1, len2, len3;
	  char *ptr;
	  char  tpdu[60], tmp_tpdu[60];
	  
	  strcpy(tmp_tpdu, ptemsg_get_msg_orig_comms_info(p_cur_msg));
	  
	  /* The format of orig_comms_info set in notify() is 
	  ^6000010000,1234 or *dialogA,3232 the port number is varying depending
	  on the socket that received the transaction, that same socket will be used
	  to send the response back
	  OR a tpdu id
	  */
	  ptr = (char*)strchr(tmp_tpdu, ORIG_INFO_SEPARATOR);
	  if (ptr)
	  {
		  *ptr = 0x00;
		  ptr++;
		  *client_socket = (SOCKET)(atoi(ptr));
	  }
	  else 
		  *client_socket = 0;
	  
		  /* IMPORTANT:
		  Org Info Starting with ORIGINAL_REQUEST means a response to a terminal request
		  
		   Org Info Starting with FORWARD_MESSAGE means data forward to Direct Host, 
		   then use the rest of the entry as the destination queue name
		   
			Org Info Starting with MSG_WITHOUT_TPDU means that this is message that is outgoing 
			and was forwarded to this application from dialog manager
	  */
	  switch(tmp_tpdu[0])
	  {
	  case ORIGINAL_REQUEST:
		  ascii_to_bin((pBYTE)tmp_tpdu + 1, (pBYTE)tpdu, tpdu_length);
		  /* this is a response to a terminal request 
		  Will Reverse TPDU only if it is a response going out to the terminal 
		  Copy TPDU tran type, then reverse the src/dst 
		  */
		  memcpy(tmpbuf, tpdu, tpdu_tran_len);
		  memcpy(tmpbuf + tpdu_tran_len, tpdu + tpdu_tran_len + tpdu_dst_len, tpdu_src_len);
		  memcpy(tmpbuf + tpdu_tran_len + tpdu_dst_len, tpdu + tpdu_tran_len, tpdu_dst_len);
		  return(tpdu_length);
		  
	  case MSG_WITHOUT_TPDU:
	  /*  
	  This is an outgoing message without a TPDU, send it as is
	  Unless it is an IGNORE situation, in that case, add on a dummy
	  5 byte TPDU starting with 60, we'll take the string associated with
	  TPDU_SIZE in the TPDU table so that this string is configurable
		  */
		  if (strcmp(dm_config.db_tpdu_processing, "IGNORE"))
		  {
			  /* Not equal to IGNORE, process normally */
			  return(SEND_MSG);
		  }
		  else
		  {
			  if (exists_in_tpdu_scroll("TPDU_SIZE"))
			  {
				  /* if it is with TPDU */
				  /* Regular transaction through NAC */
				  len1 = tpdu_scroll[cur_tpdu].tran_len;
				  len2 = tpdu_scroll[cur_tpdu].dst_len;
				  len3 = tpdu_scroll[cur_tpdu].src_len;
				  
				  /* Test if any of them is zero */
				  if ((len1 * len2 * len3) != 0)  
				  {
					  total_tpdu_len = len1 + len2 + len3;
					  ascii_to_bin ((pBYTE)tpdu_scroll[cur_tpdu].tpdu_string, tmpbuf, total_tpdu_len);
					  return(total_tpdu_len);
				  }
				  
				  /* bad tpdu setup */
				  LogEvent("Ascendent: decode_tpdu, tpdu length error" , ERROR_MSG);
				  return(TPDU_DECODE_ERROR);
			  }
			  else
			  {
				  LogEvent("Ascendent, decode_tpdu, TPDU_SIZE missing from TPDU Table" , ERROR_MSG);
				  return(TPDU_DECODE_ERROR);
			  }
		  }
		  break;
		  
	  default:
		  /* this is a PTE_MSG foreward to an external host auth */
		  if (exists_in_tpdu_scroll(tmp_tpdu) == false)
		  {
			  LogEvent("Ascendent, decode_tpdu, tpdu id error" , ERROR_MSG);
			  return(TPDU_DECODE_ERROR);
		  }
		  
		  if (tpdu_scroll[cur_tpdu].tpdu_string[0] != FORWARD_MESSAGE)
		  {
			  /* if it is with TPDU */
			  /* Regular transaction through NAC */
			  len1 = tpdu_scroll[cur_tpdu].tran_len;
			  len2 = tpdu_scroll[cur_tpdu].dst_len;
			  len3 = tpdu_scroll[cur_tpdu].src_len;
			  
			  /* Test if any of them is zero */
			  if ((len1 * len2 * len3) != 0)  
			  {
				  total_tpdu_len = len1 + len2 + len3;
				  ascii_to_bin ((pBYTE)tpdu_scroll[cur_tpdu].tpdu_string, tmpbuf, total_tpdu_len);
				  return(total_tpdu_len);
			  }
			  
			  /* bad tpdu setup */
			  LogEvent("Ascendent: decode_tpdu, tpdu length error" , ERROR_MSG);
			  return(TPDU_DECODE_ERROR);
		  }
		  else
		  {
		  /* Direct connection to Host, extract the queue name to be whatever
		  after the FORWARD_MESSAGE, and send to that queue
			  */
			  strcpy(dest_que_name, &(tpdu_scroll[cur_tpdu].tpdu_string[1]));
			  return(REROUTE_MESSAGE);
		  }
	  }   
}

/************************************************************* Mark for delete
*************************************************************/
int get_queue_name (char *str, char *dest_que)
{
	long  router_len;
	long  router_off;
	int   index;
	int   comp_int;
	int   compare_ok = false;
	
	for (index = 0; index < length_of_router_scroll; index++) 
	{
		router_len = router_scroll [index].router_length;
		router_off = router_scroll [index].router_offset;
		
		if (router_scroll[index].router_datatype[0] == 'c')      //"char"
		{
			if ( (memcmp (&str[router_off], router_scroll[index].router_lo_end, router_len) >= 0) &&
				(memcmp (&str[router_off], router_scroll[index].router_hi_end, router_len) <= 0) )
				compare_ok= true;
		}
		else if (router_scroll[index].router_datatype[0] == 'i')   //"int"
		{
			comp_int = 0;
			memcpy (&comp_int, &str[router_off], router_len);
			
			if ( (comp_int >= atoi(router_scroll[index].router_lo_end)) && 
				(comp_int <= atoi(router_scroll[index].router_hi_end)))               
				compare_ok= true;
		}
		else 
			return (false);
		
		if (compare_ok)
		{
			strcpy (dest_que, router_scroll[index].router_queue);
			return (true);
		}
	}
	
	return (FALSE);
}

/*************************************************************
*************************************************************/
int config_dialog()
{
	connection_type = -1;
	
	db_dm_config    = NULL;
	db_tpdu_list    = NULL;
	db_router_list  = NULL;
	db_dm_config    = (pDM_CONFIG)malloc(sizeof(DM_CONFIG));
	
	if (db_dm_config == NULL) 
		return (FALSE);
	
	if ( ! update_dm_config() )
		return (FALSE);
	
	trace_number = 0;
	trace_on = TRUE;
	sprintf(trace_file, "%s.%03d", ServiceName, trace_number);
	received_disconnect_from_gui = FALSE;
	return (TRUE);
}

extern "C"{
	void MainProcessor()
	{
		// Dummy to keep libpte/startup.o silent.
	}
}

void tpsr_signal_handler( int signal )
{
	EndProcessSignalled = 1;
#if WIN32
	Sleep(2000);
#else
	sleep(2);
#endif
}

extern void tpsrSendHandler(int *tlevel);

/*************************************************************
*************************************************************/
int main(int argc, char *argv[])
{
	char       AppName [8];
	int ptlevel = 0;
	char	lpszHwnd[16] = {0};
#ifdef WIN32
	PRIVATE HANDLE			 thandle;
	DWORD	  lthread_id;
#else
	PRIVATE pthread_t tThread;
#endif
	CHAR log_dir[1024] = {0};
	HWND hConsoleHandle = NULL;
	//GetAscendentLogDirectory(log_dir);
	strcpy(log_dir,"log\\");
	
	if(argc > 1)
	{
		char szConsoleTitle[256];
		GetConsoleTitle(szConsoleTitle, 256);
		hConsoleHandle = FindWindow(NULL,szConsoleTitle);
		ShowWindow(hConsoleHandle, SW_HIDE);
	}
	// Code
	strcpy( ExeName, argv[0] );
	GetAppName( AppName );	// let it know the name of the tool - tpsrtool.
	GetAppName( ServiceName );
	SYSTEMTIME timeStamp;
	GetLocalTime(&timeStamp);
	sprintf(log_folder_name,"%sA%04d%02d%02d%02d%02d%02d\\",log_dir,
		timeStamp.wYear,timeStamp.wMonth, timeStamp.wDay, 
		timeStamp.wHour, timeStamp.wMinute, timeStamp.wSecond);
	if(_mkdir(log_folder_name) == -1)
	{
		//Not able to create folder for this transaction session, so use the log directory
		strcpy(log_folder_name, log_dir);
	}

	if(argc > 2)
	{
		strcpy(lpszHwnd, argv[1]);
		hWnd = (HWND)atoi(lpszHwnd);
		printf("\n\n%s\n\n",lpszHwnd);
		if(IsWindow(hWnd))
		{
			char lpStr[20] = {0};
			GetWindowText(hWnd,lpStr,20);
			printf("\nWindow Title %s\n",lpStr);
			printf("Console Handle  %d\n",hConsoleHandle);
			PostMessageToParent(WM_CONSOLE_HANDLE,(UINT)hConsoleHandle, 0);
		}
	}
	
#ifndef WIN32
	if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
		signal( SIGTERM, tpsr_signal_handler );
#endif
	
	// This logic is NOT updated for multiple threads. So there is a
	// single global variable to store connection status of all threads.
	// Any thread gets disconnected, it is assumed that all threads are
	// disconnected. All are connected to the same IP/Port, so it is
	// a safe assumption unless we create so many threads here, that
	// cannot be handled by the server simultaneously.
	set_connection_status(FALSE);
	if (!config_dialog())
	{
		return -1;
	}
	
	// create a sending thread
#ifdef WIN32
	/* create thread to listen on the socket */
	thandle = CreateThread(	NULL,
		0,
		(LPTHREAD_START_ROUTINE)tpsrSendHandler,
		&ptlevel,
		0,
		&lthread_id );
	if( thandle == NULL )
	{
		LogEvent( "XCOM: dtest: send thread, Unable to create thread", ERROR_MSG );
		//pteipc_shutdown_single_instance_app(); 
		MainProcessDone = 1;
		return -1;
	}
#else
	ret = pthread_create( &tThread, NULL, (void*(*)(void*))tpsrSendHandler, (void*)&ptlevel );
	if( ret != 0 )
	{
		sprintf(szBuff,
			"XCOM: dtest: send thread, Unable to create thread, %d %s\n", errno, sys_errlist[errno] );
		LogEvent(szBuff, ERROR_MSG);
		//pteipc_shutdown_single_instance_app(); 
		MainProcessDone = 1;
		return;
	}
#endif
	
	// Send signal to all threads to start sending transactions.
	startSending = 1;
	while(!EndProcessSignalled)
	{
		// Display status.
		// Delete the previous line
		printf("\r                                                                              ");
		// Update the new status
		printf("\rSent = %ld, Received = %ld  ", transactionsSent, responsesReceived);
		fflush(stdout);
		
		// This reconnection logic cannot work for all the
		// threads created above, so comment it out.
#if 0
		if ( !connection_up && !EndProcessSignalled )
		{
			terminate_connection();
			if (use_primary)
				use_primary = FALSE;
			else
				use_primary = TRUE;
			
			establish_connection();
			
			/* give it enough time to restart properly */
			//pteipc_sleep(5000);
			{
				struct timespec intv;
				intv.tv_sec = 5;
				intv.tv_nsec = 0;
				nanosleep (&intv, NULL);
			}
		}
#endif
		// Let this main thread sleep and not flood the CPU time.
		{
#if WIN32
			Sleep(1000);
#else
			sleep(1);
#endif
		}
	}
	
	MainProcessDone = 1;
	return 0;
}

/*************************************************************
*************************************************************/
extern "C" {
	void EndProcess()
	{
		received_disconnect_from_gui = TRUE;
		terminate_connection();
		
		if (db_dm_config) free(db_dm_config);
		if (db_tpdu_list) free(db_tpdu_list);
		if (db_router_list) free(db_router_list);
		
		//pteipc_quespool_off( application_que_name );
		
		printf("Ascendent: dialog manager service is shutting down");
	}
}

void pteipc_sleep(long tim)
{
#ifdef WIN32
	Sleep(tim);
#else
	struct timespec intv;
	intv.tv_sec = tim/1000;
	intv.tv_nsec = (1000000 * (tim%1000));
	//pteipc_sleep(100);
	nanosleep (&intv, NULL);
#endif
}

INT GetPrivateProfileString_TPSR( pCHAR section_name,
                             pCHAR key_name,
                             pCHAR default_str,
                             pCHAR dest_buffer,
                             UINT  buffer_size,
                             pCHAR file_name )
{
    FILE *src_file;
    CHAR buffer[256], full_section_name[256];
    INT  found_sect, found_eq;
    UINT temp;

    /* copy the default string to the destination buffer */
    strcpy( dest_buffer, default_str );
    sprintf( full_section_name, "[%s]", section_name);

    /* try to open the source file */
    src_file = fopen( file_name, "r+" );
    if( src_file == NULL )
    {
        return -1;
    }

    /* file is open, go through each line of the file..... */
    found_sect = 0;
    while( !feof( src_file ) )
    {
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            fclose( src_file );
            return -2;
        }

        /* ....and look for the specified section */
        if( strstr( buffer, full_section_name ) != NULL )
        {
            /* found the section, break out */
            found_sect = 1;
            break;
        }
    }
    
    /* if end of file was reached and section was not found, return */
    if( !found_sect )
    {
        fclose( src_file );
        return -3;
    }

    /* section has been located, now look for requested key_name */
    while( !feof( src_file ) )
    {
        /* get each line in the section.... */
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            fclose( src_file );
            return -4;
        }

        /* ....and check if it contains the required key_name */
        if( strncmp( key_name, buffer, strlen( key_name ) ) == 0 )
        {
            /* found the key_name, retrieve the value */
            found_eq = 0;
            for( temp = strlen( key_name ); temp < strlen( buffer ); temp++ )
            {
                if( buffer[temp] == '=' )
                {
                    found_eq = 1;
                    break;
                }
            }

            if( !found_eq )
            {
                fclose( src_file );
                return -5;
            }

            while( ++temp < strlen( buffer ) && buffer[temp] == ' ' );
            if( strlen( &buffer[temp] ) > buffer_size )
            {
                fclose( src_file );
                return -6;
            }

            strcpy( dest_buffer, &buffer[temp] );
            fclose( src_file );
            return 1;
        }
        else if( buffer[0] == '[' )
        {
            /* reached end of section, key_name was not found */
            fclose( src_file );
            return -7;
        }
    }

    /* reached end of file, key_name was not found */
    fclose( src_file );
    return -8;
}

int PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(hWnd != NULL)
	{
		return PostMessage(hWnd, message, wParam, lParam);

	}
	return -1;
}
