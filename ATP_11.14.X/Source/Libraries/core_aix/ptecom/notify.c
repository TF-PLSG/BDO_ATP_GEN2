/******************************************************************************
  
        Module: notify.c
  
         Title: dialog manager API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\NOTIFY.C  $
   
      Rev 1.19   Jan 21 2000 15:55:10   MSALEH
   use last_tran_id field of the pte_msg to save
   the source ip address, add same support
   in cm_notify()
   
      Rev 1.18   Jan 14 2000 15:07:42   MSALEH
   * Use new buffer field of IPC_INFO to 
   pass IP address to PTE_MSG in
   current_tran_id field
   * Correct keepalive for Client sockets
   * implement getpeername() for server/client
   sockets
   
      Rev 1.17   Nov 05 1999 16:40:18   MSALEH
   * Correct bug with sleep between accepts
   * Add Keep alive support to tcp.c
   
      Rev 1.16   Oct 14 1999 10:41:48   MSALEH
   Integrate SSL modifications
   
      Rev 1.15   Oct 06 1999 09:38:26   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.14   Sep 30 1999 15:38:28   MSALEH
   AIX Modifications
   
      Rev 1.13   Sep 28 1999 14:47:40   MSALEH
   AIX Modifications
   
      Rev 1.12   Aug 25 1999 15:54:30   MSALEH
   reworked the socket handling to use linked lists
   so that there's no maximum on the number of 
   connections
   
      Rev 1.11   Jun 18 1999 11:10:28   rmalathk
   changes made for adding 'Activity Timer' functionality
   
      Rev 1.10   Apr 19 1999 17:11:24   MSALEH
   display message even if it does not meet 
   requirements
   
      Rev 1.9   Apr 14 1999 15:23:10   MSALEH
   Mods to handle new IGNORE option
   
      Rev 1.8   Apr 06 1999 16:23:00   MSALEH
   TCP:in tcp_write, if the connection is client
   socket then use the main socket id and don't
   depend on the value passed by other apps, this
   has the following limitation:Cannot have a server 
   listen for a connection from the network.
   Serial: Use CreateThread instead of _beginthread
   
      Rev 1.7   Apr 01 1999 12:22:00   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.6   Mar 01 1999 17:23:34   MSALEH
   TCP: added functionality to handle inclusive
   and exclusive length bytes, if the tcp_header_length >= 100
   then inclusive length is assumed.
   Serial: For length 0, don't send an LRC, this
   is a racal thing only
   
      Rev 1.5   Oct 08 1998 16:42:50   MSALEH
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
   
   
      Rev 1.4   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.3   Apr 27 1998 09:48:08   drodden
   Moved all of the common system wide defines and typedefs out of the application
   headers (app_info) into the core headers (pte, coreinfo).
   This is part of the creation of the Core Dataserver.
   
   
      Rev 1.2   08 Apr 1998 14:40:52   MSALEH
   dtcp_notify(), switch on the application_que_name
   variable to find where to route the incoming host 
   response. so the router table will have to have an 
   entry with the lo/hi parameters set to the host connection
   queue name
   
      Rev 1.1   Apr 07 1998 08:27:28   drodden
   Removed the dialogg header.  The defines were move to pte_h and it is no longer used.
   
   
      Rev 1.0   27 Mar 1998 16:07:48   MSALEH
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
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#endif

#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
#include "ntutils.h"

#ifdef SSL_SUPPORT
#include "ssldefs.h"
#endif

extern INT get_queue_name  (pCHAR str, pCHAR dest_que);
extern INT get_tpdu_length ();

/* changes made for the activity timer */
extern INT      volatile connection_up;
extern BYTE     volatile send_keepalive;

double last_message_time = 0.0;
LONG   timeout_sec = 0;

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info )
{    
  WORD      tpdu_len;
  INT       found_destination;
  LONG      ret_code;
  CHAR      ascii_tpdu[60] = {"^"};
  CHAR      dest_que[17];
  CHAR      Buffer[256], ErrorMsg[100];
  pPTE_MSG  p_msg_out;
  
  /* re-initialize the last_mesage_timestamp whenever we get a message
     into the port. */
  last_message_time = ptetime_get_time();

  /*  
    Put in indicator for a terminal transaction, on the way back, 
    if the indicator is found, then no TPDU search is done, 
    the length specified by tpdu_size() entry in the TPDU table

    IMPORTANT: 
    if the message is without a TPDU, then there
    is no TPDU in front of the incoming message, if that
    is the case, the org_info field will be set using the queue nama,
    and that is what is going to be returned to us on an outgoing message.
  */

  switch(ipc_info->tpduoption)
  {
  case WITH_TPDU:
    tpdu_len = get_tpdu_length();
    bin_to_ascii((pBYTE)tmpbuf, (pBYTE)(ascii_tpdu + 1), tpdu_len);
    found_destination = get_queue_name(ascii_tpdu, dest_que);
    break;

  case WITHOUT_TPDU:
    sprintf(ascii_tpdu, "*%s", application_que_name);
    found_destination = get_queue_name(application_que_name, dest_que);
    tpdu_len = 0;
    break;

  case IGNORE_TPDU:
    tpdu_len = get_tpdu_length();
    bin_to_ascii((pBYTE)tmpbuf, (pBYTE)(ascii_tpdu + 1), tpdu_len);
    found_destination = get_queue_name(application_que_name, dest_que);
    break;
  }

  if (found_destination)
  {
    p_msg_out = ptemsg_build_msg( MT_INCOMING,
                                  0,
                                  trace_number,
                                  dest_que,
                                  application_que_name,
                                  tmpbuf + tpdu_len,
                                  (int)len - tpdu_len,
                                  0);

    if (p_msg_out != NULL_PTR)
    {
      sprintf(Buffer, "%s,%0ld", ascii_tpdu, ipc_info->socket_id);
      ptemsg_set_orig_comms_info(p_msg_out, Buffer);

      /* If the buffer contains an IP address then include it */
      if (strlen(ipc_info->buffer))
         ptemsg_set_last_msg_tran_id ( p_msg_out, ipc_info->buffer );

#ifdef LOW_PRIORITY
	// Using lowest priority of 1. 2 is for all internal ATP messages.
	// 3 is used for all system/control messages sent by service
	// manager. System priority is defined in tf.ini
      ret_code = pteipc_send_hp( p_msg_out, dest_que, 1L );
#else
	// This call will use normal priority 2. required by host controllers.
	// compile ptecom normally to build dialog for host controllers.
	// Compile ptecom with "make lowprt" to build dialog manager.
      ret_code = pteipc_send( p_msg_out, dest_que );
#endif

      free(p_msg_out);

      if( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer, "ASCENDENT: pteipc_send, error at sending to que %s: %s", dest_que, ErrorMsg );
         LogEvent( Buffer, ERROR_MSG );
      }
    }
    else 
    {
      LogEvent("ASCENDENT: dm_notify, out of memory when creating new message", ERROR_MSG);
    }
  } 
  else
  {
    LogEvent("ASCENDENT: dm_notify, invalid destination queue", ERROR_MSG);
  }

  if (trace_on)
  {
    while (trace_lock_on);
    trace_lock_on = TRUE;

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

    trace_lock_on = FALSE;
  }
}

#ifdef SSL_SUPPORT
/************************************************************************************
*************************************************************************************/
void cm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info )
{    
  WORD			tpdu_len;
  INT				found_destination;
  LONG			ret_code;
  CHAR			ascii_tpdu[60] = {"^"};
  CHAR			dest_que[17];
  CHAR			Buffer[256],ErrorMsg[100];
  pPTE_MSG		p_msg_out;

  /* 
  SSL_BUF		ssl_rec;
  BYTE			buffer[sizeof(SSL_DATA)];
  */


  /* re-initialize the last_mesage_timestamp whenever we get a message
     into the port.
  */
  last_message_time = ptetime_get_time();

  /*  
    Put in indicator for a terminal transaction, on the way back, 
    if the indicator is found, then no TPDU search is done, 
    the length specified by tpdu_size() entry in the TPDU table

    IMPORTANT: 
    if the message is without a TPDU, then there
    is no TPDU in front of the incoming message, if that
    is the case, the org_info field will be set using the queue nama,
    and that is what is going to be returned to us on an outgoing message.
  */

  switch(ipc_info->tpduoption)
  {
  case WITH_TPDU:
    tpdu_len = get_tpdu_length();
    bin_to_ascii((pBYTE)tmpbuf, (pBYTE)(ascii_tpdu + 1), tpdu_len);
    found_destination = get_queue_name(ascii_tpdu, dest_que);
    break;

  case WITHOUT_TPDU:
    sprintf(ascii_tpdu, "*%s", application_que_name);
    found_destination = get_queue_name(application_que_name, dest_que);
    tpdu_len = 0;
    break;

  case IGNORE_TPDU:
    tpdu_len = get_tpdu_length();
    bin_to_ascii((pBYTE)tmpbuf, (pBYTE)(ascii_tpdu + 1), tpdu_len);
    found_destination = get_queue_name(application_que_name, dest_que);
    break;
  }

  if (trace_on)
  {
    while (trace_lock_on);
    trace_lock_on = TRUE;

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

    trace_lock_on = FALSE;
  }


  /* move the ssl record into the buffer to get the sub messate types
    memset(&ssl_rec, 0, sizeof(ssl_rec));
    memcpy(&ssl_rec, tmpbuf, len);
  */

  /* set up the message buffer
    memcpy(&buffer, ssl_rec.sslbuf, len);
  */

  /*
    len = len + sizeof(ssl_rec.ssl);
    memcpy(buffer, &ssl_rec, len);
  */
  
  /* if no matching queue name was found report the error and return */
  if (!found_destination)
  {
	LogEvent("ASCENDENT: cm_notify, invalid destination queue", ERROR_MSG);
	return;
  }

  /* build the outbound message */

  /* message subtype1 & subtype2 unused */
  p_msg_out = ptemsg_build_msg( MT_INCOMING,
                                0,
								        0,
								        dest_que,
                                application_que_name,
								        tmpbuf + tpdu_len,
                                (int)len - tpdu_len,
                                0);

  /* if the message is not built correctly, report the error and return */
  if (p_msg_out == NULL_PTR)
  {
   LogEvent("ASCENDENT: cm_notify, out of memory when creating new message", ERROR_MSG);
   return;
  }	


  /* use the ssl_id in ipc_info to populate the current_tran_id field */
  /* to identify the correct element in the linked list for ssl_write routine */
  memcpy(p_msg_out->current_tran_id, ipc_info->ssl_id, sizeof(ipc_info->ssl_id));

  /* memcpy(&Buffer, ipc_info->ssl_id, sizeof(ipc_info->ssl_id));
     ptemsg_set_orig_comms_info(p_msg_out, Buffer);
  */
   
  /* save the tpdu info in the orig_comms_info field */
  memset(&Buffer, 0, sizeof(Buffer));
  memcpy(&Buffer, ascii_tpdu, (tpdu_len * 2 + 1));
  ptemsg_set_orig_comms_info(p_msg_out, Buffer);

  /* set the msg_ssl field to save the ssl handle */
  /* ptemsg_set_ssl_info(p_msg_out, ipc_info->ssl);
     memcpy(&p_msg_out->msg_ssl, &ipc_info->ssl, sizeof(SSL));
  */

  /* If the buffer contains an IP address then include it */
  if (strlen(ipc_info->buffer))
   ptemsg_set_last_msg_tran_id ( p_msg_out, ipc_info->buffer );

  /* send the message to the outbound destination */
  ret_code = pteipc_send( p_msg_out, dest_que );

  /* free p_msg_out, if valid */
  if (p_msg_out != NULL)
	 free(p_msg_out);

  /* check the return code from pteipc_send */
  if( ret_code < 0 )
  {
     pteipc_get_errormsg( ret_code, ErrorMsg );
     sprintf( Buffer, "ASCENDENT: pteipc_send, error at sending to que %s: %s", dest_que, ErrorMsg );
     LogEvent( Buffer, ERROR_MSG );
  }
   
  /* unlock the linked list */
  /* 
     SSLLinkedListLocked = FALSE;
  */
}

#endif

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
      pteipc_sleep(60000);

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
   if( !GetAscendentConfigDirectory( ini_file ) )
   {
      LogEvent( "ASCENDENT: Unable to determine Ascendent Home directory. Activity timer not initialized",
                WARN_MSG );
      return;
   }

   strcat( ini_file, "tf.ini" );
   rc = GetPrivateProfileString(
        service_name, 	            /* points to section name */
        "SEND_KEEPALIVE",           /* points to key name */
        "0",	                     /* points to default string */ 
        timer_flag,                 /* points to destination buffer */ 
        sizeof(timer_flag) - 1,     /* size of destination buffer */
        ini_file );                 /* points to initialization filename */ 

   send_keepalive = atoi(timer_flag);

   strcat( ini_file, "tf.ini" );
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
