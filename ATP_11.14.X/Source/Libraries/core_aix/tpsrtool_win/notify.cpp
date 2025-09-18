/******************************************************************************

 Module: notify.c
 
  Title: dialog manager API Routines
  
   Description: 
   
	$Log:   N:\PVCS\PTE\CORE\ptecom_lib\NOTIFY.C  $
	

******************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#endif

#include "tpsrtool.h"
#include "convert.h"
#include "isolib.h"
extern "C" {
	
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
	
#include "ntutils.h"
}
#ifdef SSL_SUPPORT
#include "ssldefs.h"
#endif

extern HWND hWnd;

/* changes made for the activity timer */
extern INT      volatile connection_up;
extern BYTE     volatile send_keepalive;
extern long volatile responsesReceived;
double last_message_time = 0.0;
LONG   timeout_sec = 0;

int FindField(PBYTE in_msg, int len, int field_no, PBYTE out_ptr, int no_of_tpdu_bytes);
// This is where responses are received and parsed or dumped to logfile.

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info )
{
	CHAR      ascii_tpdu[60] = {"^"};
	
	/* Increment the counter for number of responses received. */
	responsesReceived++;

	PostMessageToParent(WM_REPONSERECEIVED,0,0);
	
	/* re-initialize the last_mesage_timestamp whenever we get a message
	into the port. */
	last_message_time = ptetime_get_time();
	BYTE out_ptr[10] = {0};
	FindField(tmpbuf, len, 39, out_ptr, 5);
    while (trace_lock_on);
    trace_lock_on = TRUE;
	{
		switch(ipc_info->tpduoption)
		{
		case WITH_TPDU:
			PrintBuffer(tmpbuf, (int)len, false, ascii_tpdu + 1, "temp");
			break;
			
		case WITHOUT_TPDU:
			PrintBuffer(tmpbuf, (int)len, false, "NO TPDU", "temp");
			break;
			
		case IGNORE_TPDU:
			PrintBuffer(tmpbuf, (int)len, false, "TPDU IGNORED", "temp");
			break;
		}
	}
	
    trace_lock_on = FALSE;
}

int FindField(PBYTE in_msg, int len, int field_no, PBYTE out_ptr, int no_of_tpdu_bytes)
{
	BYTE iso_msg_cpy[200] = {0};
	memcpy(iso_msg_cpy, in_msg + no_of_tpdu_bytes, len - no_of_tpdu_bytes);
	//iso_msg_cpy = iso_msg_cpy + 2;	//Msg type
	BOOLEAN result = isoutil_parse_terminal_request_or_response_message(iso_msg_cpy, len, field_no, out_ptr);

	return result;


}

/************************************************************************************
************************************************************************************/
void activity_timer_thread( void *dummy )
{
	CHAR  alert_msg[100];
	INT   connection_up_earlier = 1;
	double current_time;
	
	/* initialize timestamp and error msg */
	last_message_time = ptetime_get_time();
	sprintf( alert_msg, "ASCENDENT: activity_timer_thread, No activity for the last %d minutes.", (timeout_sec/60) );
	
	while( 1 )
	{
		//pteipc_sleep(60000);
        {
#ifdef WIN32
			Sleep(60000);
#else
			struct timespec intv;
			intv.tv_sec = 60;
			intv.tv_nsec = 0;
			//pteipc_sleep(100);
			nanosleep (&intv, NULL);
#endif
        }
		
		/* if server is not online, then don't do anything */
		if( !connection_up )
		{
			connection_up_earlier = 0;
			continue;
		}
		
		/* if server just went online, initialize last message timestamp */
		if( !connection_up_earlier )
		{
			connection_up_earlier = 1;
			last_message_time = ptetime_get_time();
			continue;
		}
		
		/* check if the timer has expired */
		current_time = ptetime_get_time();
		if( (current_time - last_message_time) > (double)timeout_sec )
		{
			/* timer has expired. initialize timestamp and Log Event */
			last_message_time = ptetime_get_time();
			LogEvent( alert_msg, WARN_MSG );
		}
	}
}


/************************************************************************************
************************************************************************************/
void initialize_activity_timer( pCHAR service_name )
{
	CHAR     ini_file[256];
	CHAR     timer_flag[10] = {0};
	CHAR     timeout_str[20] = {0};
	INT      rc;
#ifdef WIN32
	HANDLE   lhandle;
	DWORD    lthread_id;
#else
	static   pthread_t tcp_timer_thread;
#endif
	
	
	/* read the tf.ini and get the activity_timer config
	for this service. If unsuccessful, do not enable
	activity timer
	*/
/*	if( !GetAscendentConfigDirectory( ini_file ) )
	{
		LogEvent( "ASCENDENT: Unable to determine Ascendent Home directory. Activity timer not initialized",
			WARN_MSG );
		return;
	}
*/	
	strcat( ini_file, "tpsr.ini" );
	rc = GetPrivateProfileString(
        service_name, 	            /* points to section name */
        "SEND_KEEPALIVE",           /* points to key name */
        "0",	                     /* points to default string */ 
        timer_flag,                 /* points to destination buffer */ 
        sizeof(timer_flag) - 1,     /* size of destination buffer */
        ini_file );                 /* points to initialization filename */ 
	
	send_keepalive = atoi(timer_flag);
	
	strcat( ini_file, "tpsr.ini" );
	rc = GetPrivateProfileString(
        service_name, 	           /* points to section name */
        "ENABLE_ACTIVITY_TIMER",   /* points to key name */
        "0",	                   /* points to default string */ 
        timer_flag,                /* points to destination buffer */ 
        sizeof(timer_flag) - 1,    /* size of destination buffer */
        ini_file );                /* points to initialization filename */ 
	
	if( atol(timer_flag) == 0 )
		return;
	
	rc = GetPrivateProfileString(
        service_name, 	           /* points to section name */
        "NOACTIVITY_TIMEOUT",	   /* points to key name */
        "60",	                   /* points to default string */ 
        timeout_str,               /* points to destination buffer */ 
        sizeof(timeout_str) - 1,   /* size of destination buffer */
        ini_file );                /* points to initialization filename */ 
	
	timeout_sec = atol( timeout_str ) * 60;
	if( timeout_sec <= 0 )
	{
		LogEvent( "ASCENDENT: initialize_activity_timer, Invalid value for NOACTIVITY_TIMEOUT. Activity timer not initialized", WARN_MSG );
		return;
	}
	
	/* configuration from file processed. Initialize timestamp values
	and start the timer thread.
	*/
#ifdef WIN32
	lhandle = CreateThread(  NULL,
		0,
		(LPTHREAD_START_ROUTINE)activity_timer_thread, 
		NULL,
		0,
		&lthread_id );
	if( lhandle == NULL )
	{
		LogEvent( "ASCENDENT: initialize_activity_timer, Unable to create activity timer thread", ERROR_MSG );
		return;
	}
#else
	rc = pthread_create (&tcp_timer_thread, NULL, (void*(*)(void*))activity_timer_thread, (void*)NULL);
	if (rc != 0)
	{
		LogEvent( "ASCENDENT: initialize_activity_timer, Unable to create activity timer thread", ERROR_MSG );
		return;
	}
#endif
}
