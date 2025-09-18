/******************************************************************************
  
        Module: ptetcp.c
  
         Title: Ascendent TCP/IP library routines
  
   Description: This module contains the routines for communicating with the
                Ascendent Applink Server.

   $Log:   N:\PVCS\PTE\CORE\ptetcp_lib\ptetcp.c  $
   
      Rev 1.8   Jun 12 2002 11:34:32   jgrguric
   Added ptetcp_sleep() function that parallels pteipc_sleep(). 
   Also added WIN32 logic assocoated with this change. Then 
   called this function in ptetcp_reconnect() to sleep 3 seconds
   between the disconnect and the connect when an error occurs.
   This may prevent socket problems on a new connection.
   
      Rev 1.7   Feb 19 2002 15:59:30   jgrguric
   Added new function ptetcp_reconnect to ptetcp.c and called it
   from all send/recv paths to intercept connection errors and
   force an automatic disconnect/reconnect to the same host
   address.
   
      Rev 1.6   Aug 30 1999 17:47:32   MSALEH
   correct ptetcp_receive bug
   
      Rev 1.5   Aug 27 1999 16:31:20   MSALEH
   reworked ptetcp_receive() to handle messeages
   more efficiently
   
      Rev 1.4   Aug 27 1999 10:31:24   MSALEH
   Correct length problem
   
      Rev 1.3   Apr 13 1999 14:06:52   MSALEH
   Modify ptetcp_send/receive to calculate
   the length of a message using the same standard as
   core
   
      Rev 1.2   02 Oct 1998 16:31:20   rmalathk
   1. Increased TCP buffer size to 64k
   2. The send function will now check if the outgoing
       message size exceeds the TCP buffer size.
   
      Rev 1.1   Sep 02 1998 13:40:14   drodden
   Changed the receive routine to pass a null pointer to the select() API
   for a blocking/wait forever call, rather than a null filled time structure
   which gave a call with a timeout of zero seconds.
   
   
      Rev 1.0   10 Aug 1998 14:23:46   rmalathk
   Initial Revision
   1. This revision is not thread safe.
   2.  need to handle fragmentation in ptetcp_send.
   

******************************************************************************/   
#include <stdio.h>
#include <windows.h>
#include <winsock.h>

// JMG - Added 3 lines on 6/12/02 for ptetcp_sleep
#ifdef WIN32
#include <io.h>
#endif

#include "ptemsg.h"
#include "ntutils.h"

#define MAX_MSG_LENGTH 1024


#define      WS_VERSION_REQD    0x0101
#define      WS_VERSION_MAJOR   HIBYTE(WS_VERSION_REQD)
#define      WS_VERSION_MINOR   LOBYTE(WS_VERSION_REQD)
#define      MIN_SOCKETS_REQD   16

#define      TCP_HEADER_SIZE    2
//****************************************************************
//****************** GLOBAL VARIABLES ****************************
SOCKET   cli_sock, cli_sock2, cli_sock3, cli_sock4;
CHAR     host_name[50];
CHAR     outgoing_msg[64 * MAX_MSG_LENGTH];
CHAR     incoming_msg[64 * MAX_MSG_LENGTH];

CHAR     outgoing_msg2[64 * MAX_MSG_LENGTH];
CHAR     incoming_msg2[64 * MAX_MSG_LENGTH];

CHAR     outgoing_msg3[64 * MAX_MSG_LENGTH];
CHAR     incoming_msg3[64 * MAX_MSG_LENGTH];

CHAR     outgoing_msg4[64 * MAX_MSG_LENGTH];
CHAR     incoming_msg4[64 * MAX_MSG_LENGTH];

//JMG - 1/7/02 - Added current host address of successful connection for reconnection logic
CHAR     current_host_address[50];
CHAR     current_host_address2[50];
CHAR     current_host_address3[50];
CHAR     current_host_address4[50];

CHAR     interactive_que_name[MAX_QUE_NAME_SIZE];

//Krishna@TF, 10.6.11, Contains GUI App Product Version. Every GUI should send Version number through this Variable
extern CHAR  appProductVersion[MAX_APPVERSION_SIZE];
CHAR str_port[10];
CHAR str_port2[10];
CHAR str_port3[10];
CHAR str_port4[10];
//*********************************************************************************
//JMG - 1/7/02 - Local function declaration

static void ptetcp_reconnect( int error_code, pCHAR error_msg );
static void ptetcp_reconnect2( int error_code, pCHAR error_msg );
static void ptetcp_reconnect3( int error_code, pCHAR error_msg );
static void ptetcp_reconnect4( int error_code, pCHAR error_msg );

static void ptetcp_sleep( LONG msecs );

//*********************************************************************************
pPTE_MSG ptetcp_receive( LONG time_out, pCHAR error_msg )
{
   INT      rc, err_code;
   WORD     incoming_msg_length = 0;
   LONG     bytes_received = 0, bytes_remaining;
   BYTE     recv_buffer[11];
   INT      ret;
   pPTE_MSG p_msg_in;
   fd_set   FD_SET;
   struct   timeval tv, *ptv;

	memset(incoming_msg, 0, sizeof(incoming_msg));
   if (time_out > 0)
   {
      tv.tv_sec  = time_out;
      tv.tv_usec = 0;
      ptv        = &tv;
   }
   else
   {
      /* they didn't pass a timeout: this will be a blocking/wait forever call */
      ptv = NULL;    
   }

   FD_ZERO( &FD_SET );
   FD_SET( (SOCKET)cli_sock, &FD_SET );

   ret = select( 1, &FD_SET, NULL, NULL, ptv );
   if( ret > 0 )
   {
      rc = recv( cli_sock, recv_buffer, TCP_HEADER_SIZE, 0 );
      if( (rc == SOCKET_ERROR) || (rc == 0))
      {
         err_code = WSAGetLastError();
         sprintf( error_msg, "ptetcp_receive: error %d", err_code );
		 LogEvent(error_msg, ERROR_MSG);
         //JMG - 1/7/02 - Added reconnection logic
         ptetcp_reconnect(err_code, error_msg);
         return NULL_PTR;
      }

      /* if the number of bytes we asked for is not the number of bytes we received */
      if (rc == TCP_HEADER_SIZE)
      {
         /* rc = tcp_header_size, so get the length */
         incoming_msg_length = recv_buffer[1] + (256 * (WORD)recv_buffer[0]);

         if ( (incoming_msg_length <= 0) || (incoming_msg_length > (64 * MAX_MSG_LENGTH - 1)))
         {
            LogEvent("ASCENDENT: ptetcp_receive, Invalid message length", ERROR_MSG);
            return NULL_PTR;
         }
      
         bytes_remaining = incoming_msg_length;
      
         while (bytes_remaining > 0)
         {
            rc = recv( cli_sock, (incoming_msg + bytes_received), bytes_remaining, 0 );

            if((rc == SOCKET_ERROR) || (rc == 0))
            {
               err_code = WSAGetLastError();
               sprintf( error_msg, "ptetcp_receive: error %d", err_code );
               return NULL_PTR;
            }
      
            bytes_received  += rc;
            bytes_remaining -= rc;
         }  /* while (bytes_received < bytes_remaining)  */
      
         bytes_received = bytes_remaining = 0;
      
         p_msg_in = (pPTE_MSG)( incoming_msg );
         return p_msg_in;
      }
      else
      {
         LogEvent("ASCENDENT: ptetcp_receive, Unexpected number of length bytes received", ERROR_MSG);
         return NULL_PTR;
      }   
   }
   else if( ret == 0 )
   {
      sprintf( error_msg, "ptetcp_receive: timed_out on receive" );
      //JMG - 2/1/02 - Added reconnection logic
      // No error code returned when the select function times out
	  err_code = 0;
      ptetcp_reconnect(err_code, error_msg);
      return NULL_PTR;
   }
   else
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_receive: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      ptetcp_reconnect(err_code, error_msg);
      return NULL_PTR;
   }
}

pPTE_MSG ptetcp_receive2( LONG time_out, pCHAR error_msg )
{
   INT      rc, err_code;
   WORD     incoming_msg_length = 0;
   LONG     bytes_received = 0, bytes_remaining;
   BYTE     recv_buffer[11];
   INT      ret;
   pPTE_MSG p_msg_in;
   fd_set   FD_SET;
   struct   timeval tv, *ptv;

	memset(incoming_msg2, 0, sizeof(incoming_msg2));
   if (time_out > 0)
   {
      tv.tv_sec  = time_out;
      tv.tv_usec = 0;
      ptv        = &tv;
   }
   else
   {
      /* they didn't pass a timeout: this will be a blocking/wait forever call */
      ptv = NULL;    
   }

   FD_ZERO( &FD_SET );
   FD_SET( (SOCKET)cli_sock2, &FD_SET );

   ret = select( 1, &FD_SET, NULL, NULL, ptv );
   if( ret > 0 )
   {
      rc = recv( cli_sock2, recv_buffer, TCP_HEADER_SIZE, 0 );
      if( (rc == SOCKET_ERROR) || (rc == 0))
      {
         err_code = WSAGetLastError();
         sprintf( error_msg, "ptetcp_receive: error %d", err_code );
		 LogEvent(error_msg, ERROR_MSG);
         //JMG - 1/7/02 - Added reconnection logic
         //ptetcp_reconnect2(err_code, error_msg);
         return NULL_PTR;
      }

      /* if the number of bytes we asked for is not the number of bytes we received */
      if (rc == TCP_HEADER_SIZE)
      {
         /* rc = tcp_header_size, so get the length */
         incoming_msg_length = recv_buffer[1] + (256 * (WORD)recv_buffer[0]);

         if ( (incoming_msg_length <= 0) || (incoming_msg_length > (64 * MAX_MSG_LENGTH - 1)))
         {
            LogEvent("ASCENDENT: ptetcp_receive, Invalid message length", ERROR_MSG);
            return NULL_PTR;
         }
      
         bytes_remaining = incoming_msg_length;
      
         while (bytes_remaining > 0)
         {
            rc = recv( cli_sock2, (incoming_msg2 + bytes_received), bytes_remaining, 0 );

            if((rc == SOCKET_ERROR) || (rc == 0))
            {
               err_code = WSAGetLastError();
               sprintf( error_msg, "ptetcp_receive: error %d", err_code );
               return NULL_PTR;
            }
      
            bytes_received  += rc;
            bytes_remaining -= rc;
         }  /* while (bytes_received < bytes_remaining)  */
      
         bytes_received = bytes_remaining = 0;
      
         p_msg_in = (pPTE_MSG)( incoming_msg2 );
         return p_msg_in;
      }
      else
      {
         LogEvent("ASCENDENT: ptetcp_receive, Unexpected number of length bytes received", ERROR_MSG);
         return NULL_PTR;
      }   
   }
   else if( ret == 0 )
   {
      sprintf( error_msg, "ptetcp_receive: timed_out on receive" );
      //JMG - 2/1/02 - Added reconnection logic
      // No error code returned when the select function times out
	  err_code = 0;
      //ptetcp_reconnect2(err_code, error_msg);
      return NULL_PTR;
   }
   else
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_receive: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      //ptetcp_reconnect2(err_code, error_msg);
      return NULL_PTR;
   }
}

pPTE_MSG ptetcp_receive3( LONG time_out, pCHAR error_msg )
{
   INT      rc, err_code;
   WORD     incoming_msg_length = 0;
   LONG     bytes_received = 0, bytes_remaining;
   BYTE     recv_buffer[11];
   INT      ret;
   pPTE_MSG p_msg_in;
   fd_set   FD_SET;
   struct   timeval tv, *ptv;

	memset(incoming_msg3, 0, sizeof(incoming_msg3));
   if (time_out > 0)
   {
      tv.tv_sec  = time_out;
      tv.tv_usec = 0;
      ptv        = &tv;
   }
   else
   {
      /* they didn't pass a timeout: this will be a blocking/wait forever call */
      ptv = NULL;    
   }

   FD_ZERO( &FD_SET );
   FD_SET( (SOCKET)cli_sock3, &FD_SET );

   ret = select( 1, &FD_SET, NULL, NULL, ptv );
   if( ret > 0 )
   {
      rc = recv( cli_sock3, recv_buffer, TCP_HEADER_SIZE, 0 );
      if( (rc == SOCKET_ERROR) || (rc == 0))
      {
         err_code = WSAGetLastError();
         sprintf( error_msg, "ptetcp_receive: error %d", err_code );
		 LogEvent(error_msg, ERROR_MSG);
         //JMG - 1/7/02 - Added reconnection logic
         //ptetcp_reconnect3(err_code, error_msg);
         return NULL_PTR;
      }

      /* if the number of bytes we asked for is not the number of bytes we received */
      if (rc == TCP_HEADER_SIZE)
      {
         /* rc = tcp_header_size, so get the length */
         incoming_msg_length = recv_buffer[1] + (256 * (WORD)recv_buffer[0]);

         if ( (incoming_msg_length <= 0) || (incoming_msg_length > (64 * MAX_MSG_LENGTH - 1)))
         {
            LogEvent("ASCENDENT: ptetcp_receive, Invalid message length", ERROR_MSG);
            return NULL_PTR;
         }
      
         bytes_remaining = incoming_msg_length;
      
         while (bytes_remaining > 0)
         {
            rc = recv( cli_sock3, (incoming_msg3 + bytes_received), bytes_remaining, 0 );

            if((rc == SOCKET_ERROR) || (rc == 0))
            {
               err_code = WSAGetLastError();
               sprintf( error_msg, "ptetcp_receive: error %d", err_code );
               return NULL_PTR;
            }
      
            bytes_received  += rc;
            bytes_remaining -= rc;
         }  /* while (bytes_received < bytes_remaining)  */
      
         bytes_received = bytes_remaining = 0;
      
         p_msg_in = (pPTE_MSG)( incoming_msg3 );
         return p_msg_in;
      }
      else
      {
         LogEvent("ASCENDENT: ptetcp_receive, Unexpected number of length bytes received", ERROR_MSG);
         return NULL_PTR;
      }   
   }
   else if( ret == 0 )
   {
      sprintf( error_msg, "ptetcp_receive: timed_out on receive" );
      //JMG - 2/1/02 - Added reconnection logic
      // No error code returned when the select function times out
	  err_code = 0;
      //ptetcp_reconnect3(err_code, error_msg);
      return NULL_PTR;
   }
   else
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_receive: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      //ptetcp_reconnect3(err_code, error_msg);
      return NULL_PTR;
   }
}

pPTE_MSG ptetcp_receive4( LONG time_out, pCHAR error_msg )
{
   INT      rc, err_code;
   WORD     incoming_msg_length = 0;
   LONG     bytes_received = 0, bytes_remaining;
   BYTE     recv_buffer[11];
   INT      ret;
   pPTE_MSG p_msg_in;
   fd_set   FD_SET;
   struct   timeval tv, *ptv;

	memset(incoming_msg4, 0, sizeof(incoming_msg4));
   if (time_out > 0)
   {
      tv.tv_sec  = time_out;
      tv.tv_usec = 0;
      ptv        = &tv;
   }
   else
   {
      /* they didn't pass a timeout: this will be a blocking/wait forever call */
      ptv = NULL;    
   }

   FD_ZERO( &FD_SET );
   FD_SET( (SOCKET)cli_sock4, &FD_SET );

   ret = select( 1, &FD_SET, NULL, NULL, ptv );
   if( ret > 0 )
   {
      rc = recv( cli_sock4, recv_buffer, TCP_HEADER_SIZE, 0 );
      if( (rc == SOCKET_ERROR) || (rc == 0))
      {
         err_code = WSAGetLastError();
         sprintf( error_msg, "ptetcp_receive: error %d", err_code );
		 LogEvent(error_msg, ERROR_MSG);
         //JMG - 1/7/02 - Added reconnection logic
         //ptetcp_reconnect4(err_code, error_msg);
         return NULL_PTR;
      }

      /* if the number of bytes we asked for is not the number of bytes we received */
      if (rc == TCP_HEADER_SIZE)
      {
         /* rc = tcp_header_size, so get the length */
         incoming_msg_length = recv_buffer[1] + (256 * (WORD)recv_buffer[0]);

         if ( (incoming_msg_length <= 0) || (incoming_msg_length > (64 * MAX_MSG_LENGTH - 1)))
         {
            LogEvent("ASCENDENT: ptetcp_receive, Invalid message length", ERROR_MSG);
            return NULL_PTR;
         }
      
         bytes_remaining = incoming_msg_length;
      
         while (bytes_remaining > 0)
         {
            rc = recv( cli_sock4, (incoming_msg4 + bytes_received), bytes_remaining, 0 );

            if((rc == SOCKET_ERROR) || (rc == 0))
            {
               err_code = WSAGetLastError();
               sprintf( error_msg, "ptetcp_receive: error %d", err_code );
               return NULL_PTR;
            }
      
            bytes_received  += rc;
            bytes_remaining -= rc;
         }  /* while (bytes_received < bytes_remaining)  */
      
         bytes_received = bytes_remaining = 0;
      
         p_msg_in = (pPTE_MSG)( incoming_msg4 );
         return p_msg_in;
      }
      else
      {
         LogEvent("ASCENDENT: ptetcp_receive, Unexpected number of length bytes received", ERROR_MSG);
         return NULL_PTR;
      }   
   }
   else if( ret == 0 )
   {
      sprintf( error_msg, "ptetcp_receive: timed_out on receive" );
      //JMG - 2/1/02 - Added reconnection logic
      // No error code returned when the select function times out
	  err_code = 0;
      //ptetcp_reconnect4(err_code, error_msg);
      return NULL_PTR;
   }
   else
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_receive: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      //ptetcp_reconnect4(err_code, error_msg);
      return NULL_PTR;
   }
}

//*********************************************************************************
//*********************************************************************************
INT ptetcp_send( pPTE_MSG p_msg_out, pCHAR error_msg )
{
   INT err_code, outgoing_length, rc;

   //ptemsg_set_orig_comms_info( p_msg_out, host_name );
   memset(outgoing_msg, 0, sizeof(outgoing_msg));
   outgoing_length = ptemsg_get_msg_length( p_msg_out );
   if( outgoing_length > (sizeof( outgoing_msg ) - 2) )
   {
      sprintf( error_msg, "ptetcp_send: message size exceeds maximum allowable size of %d bytes", sizeof( outgoing_msg ) - 2 );
      return 0;
   }

   memcpy( outgoing_msg + 2, (char *)p_msg_out, outgoing_length );

   outgoing_msg[0] = HIBYTE((WORD)outgoing_length);   /* put length in first two bytes */
   outgoing_msg[1] = LOBYTE((WORD)outgoing_length);
   
   outgoing_length += 2;

   rc = send( cli_sock, outgoing_msg, outgoing_length, 0 );
   if( rc == SOCKET_ERROR )
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_send: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      ptetcp_reconnect(err_code, error_msg);
      return 0;
   }
   else
      return 1;
}

INT ptetcp_send2( pPTE_MSG p_msg_out, pCHAR error_msg )
{
   INT err_code, outgoing_length, rc;

   //ptemsg_set_orig_comms_info( p_msg_out, host_name );
   memset(outgoing_msg2, 0, sizeof(outgoing_msg2));
   outgoing_length = ptemsg_get_msg_length( p_msg_out );
   if( outgoing_length > (sizeof( outgoing_msg2 ) - 2) )
   {
      sprintf( error_msg, "ptetcp_send: message size exceeds maximum allowable size of %d bytes", sizeof( outgoing_msg2 ) - 2 );
      return 0;
   }

   memcpy( outgoing_msg2 + 2, (char *)p_msg_out, outgoing_length );

   outgoing_msg2[0] = HIBYTE((WORD)outgoing_length);   /* put length in first two bytes */
   outgoing_msg2[1] = LOBYTE((WORD)outgoing_length);
   
   outgoing_length += 2;

   rc = send( cli_sock2, outgoing_msg2, outgoing_length, 0 );
   if( rc == SOCKET_ERROR )
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_send: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      //ptetcp_reconnect2(err_code, error_msg);
      return 0;
   }
   else
      return 1;
}

INT ptetcp_send3( pPTE_MSG p_msg_out, pCHAR error_msg )
{
   INT err_code, outgoing_length, rc;

   //ptemsg_set_orig_comms_info( p_msg_out, host_name );
   memset(outgoing_msg3, 0, sizeof(outgoing_msg3));
   outgoing_length = ptemsg_get_msg_length( p_msg_out );
   if( outgoing_length > (sizeof( outgoing_msg3 ) - 2) )
   {
      sprintf( error_msg, "ptetcp_send: message size exceeds maximum allowable size of %d bytes", sizeof( outgoing_msg3 ) - 2 );
      return 0;
   }

   memcpy( outgoing_msg3 + 2, (char *)p_msg_out, outgoing_length );

   outgoing_msg3[0] = HIBYTE((WORD)outgoing_length);   /* put length in first two bytes */
   outgoing_msg3[1] = LOBYTE((WORD)outgoing_length);
   
   outgoing_length += 2;

   rc = send( cli_sock3, outgoing_msg3, outgoing_length, 0 );
   if( rc == SOCKET_ERROR )
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_send: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      //ptetcp_reconnect3(err_code, error_msg);
      return 0;
   }
   else
      return 1;
}

INT ptetcp_send4( pPTE_MSG p_msg_out, pCHAR error_msg )
{
   INT err_code, outgoing_length, rc;

   //ptemsg_set_orig_comms_info( p_msg_out, host_name );
   memset(outgoing_msg4, 0, sizeof(outgoing_msg4));
   outgoing_length = ptemsg_get_msg_length( p_msg_out );
   if( outgoing_length > (sizeof( outgoing_msg4 ) - 2) )
   {
      sprintf( error_msg, "ptetcp_send: message size exceeds maximum allowable size of %d bytes", sizeof( outgoing_msg4 ) - 2 );
      return 0;
   }

   memcpy( outgoing_msg4 + 2, (char *)p_msg_out, outgoing_length );

   outgoing_msg4[0] = HIBYTE((WORD)outgoing_length);   /* put length in first two bytes */
   outgoing_msg4[1] = LOBYTE((WORD)outgoing_length);
   
   outgoing_length += 2;

   rc = send( cli_sock4, outgoing_msg4, outgoing_length, 0 );
   if( rc == SOCKET_ERROR )
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_send: error %d", err_code );
      //JMG - 1/7/02 - Added reconnection logic
      //ptetcp_reconnect4(err_code, error_msg);
      return 0;
   }
   else
      return 1;
}

//*********************************************************************************
//*********************************************************************************
pPTE_MSG ptetcp_send_receive( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg )
{
   pPTE_MSG p_msg_in;
unsigned char recv_data_type = -1;
unsigned char sent_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_out) );
   if( !ptetcp_send( p_msg_out, error_msg ) )
   {
      if( p_msg_out != NULL_PTR )
         free( p_msg_out );
      return 0;
   }

   if( p_msg_out != NULL_PTR )
      free( p_msg_out );

   p_msg_in = ptetcp_receive( time_out, error_msg );
   recv_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

if(sent_data_type > 0)
{
	if(recv_data_type != sent_data_type)
	{
		p_msg_in = ptetcp_receive( time_out, error_msg );
	}
}
   
   return p_msg_in;

}

pPTE_MSG ptetcp_send_receive2( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg )
{
    pPTE_MSG p_msg_in;
	unsigned char recv_data_type = -1;
	unsigned char sent_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_out) );

    if( !ptetcp_send2( p_msg_out, error_msg ) )
    {
       if( p_msg_out != NULL_PTR )
          free( p_msg_out );
       return 0;
    }

    if( p_msg_out != NULL_PTR )
       free( p_msg_out );

    p_msg_in = ptetcp_receive2( time_out, error_msg );
    recv_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

	if(sent_data_type > 0)
	{
		if(recv_data_type != sent_data_type)
		{
			p_msg_in = ptetcp_receive2( time_out, error_msg );
		}
	}
   
    return p_msg_in;
}

pPTE_MSG ptetcp_send_receive3( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg )
{
    pPTE_MSG p_msg_in;
	unsigned char recv_data_type = -1;
	unsigned char sent_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_out) );

    if( !ptetcp_send3( p_msg_out, error_msg ) )
    {
       if( p_msg_out != NULL_PTR )
          free( p_msg_out );
       return 0;
    }

    if( p_msg_out != NULL_PTR )
       free( p_msg_out );

    p_msg_in = ptetcp_receive3( time_out, error_msg );
    recv_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

	if(sent_data_type > 0)
	{
		if(recv_data_type != sent_data_type)
		{
			p_msg_in = ptetcp_receive3( time_out, error_msg );
		}
	}
   
    return p_msg_in;
}

pPTE_MSG ptetcp_send_receive4( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg )
{
    pPTE_MSG p_msg_in;
	unsigned char recv_data_type = -1;
	unsigned char sent_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_out) );

    if( !ptetcp_send4( p_msg_out, error_msg ) )
    {
       if( p_msg_out != NULL_PTR )
          free( p_msg_out );
       return 0;
    }

    if( p_msg_out != NULL_PTR )
       free( p_msg_out );

    p_msg_in = ptetcp_receive4( time_out, error_msg );
    recv_data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

	if(sent_data_type > 0)
	{
		if(recv_data_type != sent_data_type)
		{
			p_msg_in = ptetcp_receive4( time_out, error_msg );
		}
	}
   
    return p_msg_in;
}

//****************************************************************
INT ptetcp_init( pCHAR error_msg )
{
   WSADATA     wsa_data;
	INT         ret;

   // store the host name of the client
   gethostname( host_name, sizeof( host_name ) );

   // call the Windows Sockets initialization routine and check
   // if winsock.dll supports the requirements of this application.
	ret = WSAStartup( WS_VERSION_REQD, &wsa_data );
	if( ret != 0 ) 
	{
      sprintf( error_msg, "ptetcp_init: Windows Socket version required \
                        not supported by winsock.dll installed on this system" );
		printf( "%s\n", error_msg );
      return 0;
	}

	if( LOBYTE( wsa_data.wVersion ) <= WS_VERSION_MAJOR  &&
        HIBYTE( wsa_data.wVersion ) < WS_VERSION_MINOR )
	{
      sprintf( error_msg, "ptetcp_init: Windows Socket version required \
                        not supported by winsock.dll installed on this system" );
		printf( "%s\n", error_msg );
      return 0;
	}

	if( wsa_data.iMaxSockets < MIN_SOCKETS_REQD ) 
	{
      sprintf( error_msg, "ptetcp_init: Winsock.dll installed on this system does not \
               support the minimum number of sockets required by this application" );
		printf( "%s\n", error_msg );
      return 0;
	}

   return 1;
}


//****************************************************************
//****************************************************************
INT ptetcp_connect( pCHAR host_addr, pCHAR error_msg )
{
   IN_ADDR        remote_ip_address;
   SOCKADDR_IN    remoteAddr;
   PHOSTENT       host;
   CHAR           port_str[10];
   CHAR           server_selection_flag[10]={0};
   CHAR           ini_file[256];
   SHORT          port_num;
   INT            ret, rc;
   const CHAR     TRUE_VALUE[] = "true";
   INT			  msg_len;
   INT            server_selection_flag_int;
   CHAR			  tmpbuff[MAX_APPVERSION_SIZE+5];
   static int temp = 0;

   // initialise structures
   memset( &remote_ip_address, 0, sizeof( remote_ip_address ) );
   memset( &remoteAddr, 0, sizeof( remoteAddr ) );

   //To read the flag , server & IP
   GetAscendentConfigDirectory( ini_file );
   strcat( ini_file, "tf.ini" );

      rc = GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "1",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        ini_file ); 

	 server_selection_flag_int = atoi(server_selection_flag);

	// Create the client-side socket 
	cli_sock = socket( AF_INET, SOCK_STREAM, 0 );
	if( cli_sock == INVALID_SOCKET )
	{
      sprintf( error_msg, "ptetcp_connect: Windows Sockets error %d: Couldn't create socket.",
              WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

    //JMG - 1/7/02 - Added keep alive option to monitor socket connection
    ret = setsockopt( cli_sock, SOL_SOCKET, SO_KEEPALIVE, TRUE_VALUE, sizeof( TRUE_VALUE) );
	if( ret == INVALID_SOCKET )
	{
      sprintf( error_msg,
		       "ptetcp_connect: Windows Sockets error %d: Couldn't set socket option.",
               WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

   // get the remote ip-address for the server
	if( host_addr == NULL || strlen( host_addr ) == 0 )
		remote_ip_address.s_addr = htonl( INADDR_LOOPBACK );
	else
	{
		remote_ip_address.s_addr = inet_addr ( host_addr );
		if( remote_ip_address.s_addr == -1 )
		{
			host = gethostbyname( host_addr );   
			if( host == NULL )
			{
            sprintf ( error_msg, "ptetcp_connect: Unknown remote host: %s\n ", host_addr );
		      printf( "%s\n", error_msg );
				return 0;
			}
			else
				CopyMemory ((char *) &remote_ip_address, host->h_addr, host->h_length);
		}
	}

   // get the port number for the applink service and try to connect

  if(server_selection_flag_int == 1)
  {
   rc = GetPrivateProfileString(
        "APPLNK", 	               // points to section name 
        "APPLNK_TCP_PORT",	               // points to key name 
        "1450",	                  // points to default string 
        port_str,                   // points to destination buffer 
        sizeof(port_str) - 1,       // size of destination buffer 
        ini_file );                 // points to initialization filename 
  }
  else if(server_selection_flag_int == 2)
  {
      rc = GetPrivateProfileString(
        "APPLNK2", 	               // points to section name 
        "APPLNK_TCP_PORT",	       // points to key name 
        "1450",	                   // points to default string 
        port_str,                   // points to destination buffer 
        sizeof(port_str) - 1,       // size of destination buffer 
        ini_file );                 // points to initialization filename 
  }
    else if(server_selection_flag_int == 3)
	{
		GetPrivateProfileString(
		"APPLNK3", 	               // points to section name 
		"APPLNK_TCP_PORT",	       // points to key name 
		"1450",	                   // points to default string 
		port_str,                   // points to destination buffer 
		sizeof(port_str) - 1,       // size of destination buffer 
		ini_file );   
	}
   else if(server_selection_flag_int == 4)
	{
		GetPrivateProfileString(
		"APPLNK4", 	               // points to section name 
		"APPLNK_TCP_PORT",	       // points to key name 
		"1450",	                   // points to default string 
		port_str,                   // points to destination buffer 
		sizeof(port_str) - 1,       // size of destination buffer 
		ini_file );   
	}
    port_num = atol( port_str );
	strcpy(str_port,port_str);
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ( port_num );
	remoteAddr.sin_addr = remote_ip_address;

	ret = connect( cli_sock, (PSOCKADDR)&remoteAddr, sizeof( remoteAddr ) );
	if( ret == SOCKET_ERROR )
	{
      sprintf ( error_msg, "ptetcp_connect: connect failed to host \"%s\",  Port \"%d\". Error Code: %ld, str_port = %s",
                host_addr, port_num, WSAGetLastError() ,str_port);
      printf( "%s\n", error_msg );
		return 0;
	}

	//JMG - 1/7/02 - Connection was successful - so save host name
	//               in case we have to reconnect
	strncpy( current_host_address, host_addr, strlen(host_addr) );

	//Krishna@TF, 7.6.11 : Changes below are to block old exe's without PCI changes from connecting to Applnk, 
	//Use the Version global Variable
	sprintf(tmpbuff, "%s,%s", PCISTR_FOR_VERSION_VALIDATION, appProductVersion);
	msg_len = strlen(tmpbuff);
	ret = send( cli_sock, tmpbuff, msg_len, 0 );

   return 1;
}

INT ptetcp_connect2( pCHAR host_addr, pCHAR error_msg )
{
   IN_ADDR        remote_ip_address;
   SOCKADDR_IN    remoteAddr;
   PHOSTENT       host;
   CHAR           port_str[10];
   CHAR           server_selection_flag[10]={0};
   CHAR           ini_file[256];
   SHORT          port_num;
   INT            ret, rc;
   const CHAR     TRUE_VALUE[] = "true";
   INT			  msg_len;
   INT            server_selection_flag_int;
   CHAR			  tmpbuff[MAX_APPVERSION_SIZE+5];
   static int temp = 0;

   // initialise structures
   memset( &remote_ip_address, 0, sizeof( remote_ip_address ) );
   memset( &remoteAddr, 0, sizeof( remoteAddr ) );

	// Create the client-side socket 
	cli_sock2 = socket( AF_INET, SOCK_STREAM, 0 );
	if( cli_sock2 == INVALID_SOCKET )
	{
      sprintf( error_msg, "ptetcp_connect2: Windows Sockets error %d: Couldn't create socket.",
              WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

    //JMG - 1/7/02 - Added keep alive option to monitor socket connection
    ret = setsockopt( cli_sock2, SOL_SOCKET, SO_KEEPALIVE, TRUE_VALUE, sizeof( TRUE_VALUE) );
	if( ret == INVALID_SOCKET )
	{
      sprintf( error_msg,
		       "ptetcp_connect2: Windows Sockets error %d: Couldn't set socket option.",
               WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

   // get the remote ip-address for the server
	if( host_addr == NULL || strlen( host_addr ) == 0 )
		remote_ip_address.s_addr = htonl( INADDR_LOOPBACK );
	else
	{
		remote_ip_address.s_addr = inet_addr ( host_addr );
		if( remote_ip_address.s_addr == -1 )
		{
			host = gethostbyname( host_addr );   
			if( host == NULL )
			{
            sprintf ( error_msg, "ptetcp_connect2: Unknown remote host: %s\n ", host_addr );
		      printf( "%s\n", error_msg );
				return 0;
			}
			else
				CopyMemory ((char *) &remote_ip_address, host->h_addr, host->h_length);
		}
	}

    // get the port number for the applink service and try to connect
	GetAscendentConfigDirectory( ini_file );
    strcat( ini_file, "tf.ini" );

      rc = GetPrivateProfileString(
        "APPLNK2", 	               // points to section name 
        "APPLNK_TCP_PORT",	       // points to key name 
        "1450",	                   // points to default string 
        port_str,                   // points to destination buffer 
        sizeof(port_str) - 1,       // size of destination buffer 
        ini_file );                 // points to initialization filename 
   
    port_num = atol( port_str );
	strcpy(str_port2,port_str);
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ( port_num );
	remoteAddr.sin_addr = remote_ip_address;

	ret = connect( cli_sock2, (PSOCKADDR)&remoteAddr, sizeof( remoteAddr ) );
	if( ret == SOCKET_ERROR )
	{
      sprintf ( error_msg, "ptetcp_connect2: connect failed to host \"%s\",  Port \"%d\". Error Code: %ld, str_port = %s",
                host_addr, port_num, WSAGetLastError() ,str_port2);
      printf( "%s\n", error_msg );
		return 0;
	}

	//JMG - 1/7/02 - Connection was successful - so save host name
	//               in case we have to reconnect
	strncpy( current_host_address2, host_addr, strlen(host_addr) );

	//Krishna@TF, 7.6.11 : Changes below are to block old exe's without PCI changes from connecting to Applnk, 
	//Use the Version global Variable
	sprintf(tmpbuff, "%s,%s", PCISTR_FOR_VERSION_VALIDATION, appProductVersion);
	msg_len = strlen(tmpbuff);
	ret = send( cli_sock2, tmpbuff, msg_len, 0 );

   return 1;
}

INT ptetcp_connect3( pCHAR host_addr, pCHAR error_msg )
{
   IN_ADDR        remote_ip_address;
   SOCKADDR_IN    remoteAddr;
   PHOSTENT       host;
   CHAR           port_str[10];
   CHAR           server_selection_flag[10]={0};
   CHAR           ini_file[256];
   SHORT          port_num;
   INT            ret, rc;
   const CHAR     TRUE_VALUE[] = "true";
   INT			  msg_len;
   INT            server_selection_flag_int;
   CHAR			  tmpbuff[MAX_APPVERSION_SIZE+5];
   static int temp = 0;

    // initialise structures
    memset( &remote_ip_address, 0, sizeof( remote_ip_address ) );
    memset( &remoteAddr, 0, sizeof( remoteAddr ) );

	// Create the client-side socket 
	cli_sock3 = socket( AF_INET, SOCK_STREAM, 0 );
	if( cli_sock3 == INVALID_SOCKET )
	{
      sprintf( error_msg, "ptetcp_connect3: Windows Sockets error %d: Couldn't create socket.",
              WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

    //JMG - 1/7/02 - Added keep alive option to monitor socket connection
    ret = setsockopt( cli_sock3, SOL_SOCKET, SO_KEEPALIVE, TRUE_VALUE, sizeof( TRUE_VALUE) );
	if( ret == INVALID_SOCKET )
	{
      sprintf( error_msg,
		       "ptetcp_connect3: Windows Sockets error %d: Couldn't set socket option.",
               WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

   // get the remote ip-address for the server
	if( host_addr == NULL || strlen( host_addr ) == 0 )
		remote_ip_address.s_addr = htonl( INADDR_LOOPBACK );
	else
	{
		remote_ip_address.s_addr = inet_addr ( host_addr );
		if( remote_ip_address.s_addr == -1 )
		{
			host = gethostbyname( host_addr );   
			if( host == NULL )
			{
            sprintf ( error_msg, "ptetcp_connect3: Unknown remote host: %s\n ", host_addr );
		      printf( "%s\n", error_msg );
				return 0;
			}
			else
				CopyMemory ((char *) &remote_ip_address, host->h_addr, host->h_length);
		}
	}

    // get the port number for the applink service and try to connect
	GetAscendentConfigDirectory( ini_file );
    strcat( ini_file, "tf.ini" );

    rc = GetPrivateProfileString(
        "APPLNK3", 	               // points to section name 
        "APPLNK_TCP_PORT",	       // points to key name 
        "1450",	                   // points to default string 
        port_str,                   // points to destination buffer 
        sizeof(port_str) - 1,       // size of destination buffer 
        ini_file );                 // points to initialization filename 
   
    port_num = atol( port_str );
	strcpy(str_port3,port_str);
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ( port_num );
	remoteAddr.sin_addr = remote_ip_address;

	ret = connect( cli_sock3, (PSOCKADDR)&remoteAddr, sizeof( remoteAddr ) );
	if( ret == SOCKET_ERROR )
	{
      sprintf ( error_msg, "ptetcp_connect3: connect failed to host \"%s\",  Port \"%d\". Error Code: %ld, str_port = %s",
                host_addr, port_num, WSAGetLastError() ,str_port3);
      printf( "%s\n", error_msg );
		return 0;
	}

	//JMG - 1/7/02 - Connection was successful - so save host name
	//               in case we have to reconnect
	strncpy( current_host_address3, host_addr, strlen(host_addr) );

	//Krishna@TF, 7.6.11 : Changes below are to block old exe's without PCI changes from connecting to Applnk, 
	//Use the Version global Variable
	sprintf(tmpbuff, "%s,%s", PCISTR_FOR_VERSION_VALIDATION, appProductVersion);
	msg_len = strlen(tmpbuff);
	ret = send( cli_sock3, tmpbuff, msg_len, 0 );

   return 1;
}

INT ptetcp_connect4( pCHAR host_addr, pCHAR error_msg )
{
   IN_ADDR        remote_ip_address;
   SOCKADDR_IN    remoteAddr;
   PHOSTENT       host;
   CHAR           port_str[10];
   CHAR           server_selection_flag[10]={0};
   CHAR           ini_file[256];
   SHORT          port_num;
   INT            ret, rc;
   const CHAR     TRUE_VALUE[] = "true";
   INT			  msg_len;
   INT            server_selection_flag_int;
   CHAR			  tmpbuff[MAX_APPVERSION_SIZE+5];
   static int temp = 0;

   // initialise structures
   memset( &remote_ip_address, 0, sizeof( remote_ip_address ) );
   memset( &remoteAddr, 0, sizeof( remoteAddr ) );

   server_selection_flag_int = 4;

	// Create the client-side socket 
	cli_sock4 = socket( AF_INET, SOCK_STREAM, 0 );
	if( cli_sock4 == INVALID_SOCKET )
	{
      sprintf( error_msg, "ptetcp_connect4: Windows Sockets error %d: Couldn't create socket.",
              WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

    //JMG - 1/7/02 - Added keep alive option to monitor socket connection
    ret = setsockopt( cli_sock4, SOL_SOCKET, SO_KEEPALIVE, TRUE_VALUE, sizeof( TRUE_VALUE) );
	if( ret == INVALID_SOCKET )
	{
      sprintf( error_msg,
		       "ptetcp_connect4: Windows Sockets error %d: Couldn't set socket option.",
               WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}

   // get the remote ip-address for the server
	if( host_addr == NULL || strlen( host_addr ) == 0 )
		remote_ip_address.s_addr = htonl( INADDR_LOOPBACK );
	else
	{
		remote_ip_address.s_addr = inet_addr ( host_addr );
		if( remote_ip_address.s_addr == -1 )
		{
			host = gethostbyname( host_addr );   
			if( host == NULL )
			{
            sprintf ( error_msg, "ptetcp_connect4: Unknown remote host: %s\n ", host_addr );
		      printf( "%s\n", error_msg );
				return 0;
			}
			else
				CopyMemory ((char *) &remote_ip_address, host->h_addr, host->h_length);
		}
	}

    // get the port number for the applink service and try to connect
	GetAscendentConfigDirectory( ini_file );
    strcat( ini_file, "tf.ini" );

    rc = GetPrivateProfileString(
        "APPLNK4", 	               // points to section name 
        "APPLNK_TCP_PORT",	       // points to key name 
        "1450",	                   // points to default string 
        port_str,                   // points to destination buffer 
        sizeof(port_str) - 1,       // size of destination buffer 
        ini_file );                 // points to initialization filename 
   
    port_num = atol( port_str );
	strcpy(str_port4,port_str);
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ( port_num );
	remoteAddr.sin_addr = remote_ip_address;

	ret = connect( cli_sock4, (PSOCKADDR)&remoteAddr, sizeof( remoteAddr ) );
	if( ret == SOCKET_ERROR )
	{
      sprintf ( error_msg, "ptetcp_connect4: connect failed to host \"%s\",  Port \"%d\". Error Code: %ld, str_port = %s",
                host_addr, port_num, WSAGetLastError() ,str_port4);
      printf( "%s\n", error_msg );
		return 0;
	}

	//JMG - 1/7/02 - Connection was successful - so save host name
	//               in case we have to reconnect
	strncpy( current_host_address4, host_addr, strlen(host_addr) );

	//Krishna@TF, 7.6.11 : Changes below are to block old exe's without PCI changes from connecting to Applnk, 
	//Use the Version global Variable
	sprintf(tmpbuff, "%s,%s", PCISTR_FOR_VERSION_VALIDATION, appProductVersion);
	msg_len = strlen(tmpbuff);
	ret = send( cli_sock4, tmpbuff, msg_len, 0 );

   return 1;
}
//****************************************************************
//****************************************************************
INT ptetcp_disconnect()
{
   INT ret;
   ret = closesocket( cli_sock );
   if( ret == SOCKET_ERROR )
      return 0;
   else
      return 1;
}

INT ptetcp_disconnect2()
{
   INT ret;
   ret = closesocket( cli_sock2 );
   if( ret == SOCKET_ERROR )
      return 0;
   else
      return 1;
}

INT ptetcp_disconnect3()
{
   INT ret;
   ret = closesocket( cli_sock3 );
   if( ret == SOCKET_ERROR )
      return 0;
   else
      return 1;
}

INT ptetcp_disconnect4()
{
   INT ret;
   ret = closesocket( cli_sock4 );
   if( ret == SOCKET_ERROR )
      return 0;
   else
      return 1;
}

//****************************************************************
//  MODULE NAME:	ptetcp_sleep
//					Added 6/12/02 by JMG.
//	DESCRIPTION:	JMG Added sleep funtion copied from pteipc_sleep
//  This function sleeps for the number of milliseconds specified in the arg.
//
//  INPUTS:			msecs		Number of milliseconds to sleep
//  OUTPUTS:		NONE
//  RETURN VALUE:	NONE
//  MODIFICATIONS:	
//*********************************************************************************/
void ptetcp_sleep( LONG msecs )
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
}

//****************************************************************
//  MODULE NAME:	ptetcp_reconnect
//	DESCRIPTION:	JMG Added auto reconnection logic
//  This function is called in a send or receive error path.
//  If the error is due to loss of connection detected by keep alive option, then
//  we will attempt to reconnect to the same host. Note that when the connection
//  with applnk is severed, we may get one of several errors. The "recv" API may return
//  an error code of 0 if the system thinks the connection has been shutdown "gracefully".
//  Otherwise, the "revc" API may also return an error code of WSAESHUTDOWN (10058) that
//  indicates the connection was shutdown. Finally, the "send" API may return a SOCKET_ERROR
//  and an error code of any of the three checked for below.
//  Any errors in the reconnection attempt will be passed back to the calling routine
//  with "Connection lost!" appended.
//  If the reconnection goes smoothly, the connection error is passed back
//  up with a "Reconnection successful - try again!" message appended.
//  The original error message is overwritten if a connection error is detected.
//  Otherwise the original error message is untouched.
//  In either case, if the user tries the operation again, it will either succeed, if
//  the reconnection was successful, or there will be another reconnection attempt.
//
//  INPUTS:			error_code	Error code from send or recv operation
//					error_msg	Pointer to error message buffer to load
//  OUTPUTS:		*error_msg	Error message loaded with error code & result
//  RETURN VALUE:	NONE
//  MODIFICATIONS:	6/12/02 - JMG Added 3 sec delay between disconnect & connect.
//*********************************************************************************
void ptetcp_reconnect( int error_code, pCHAR error_msg )
{
    if ( (error_code == WSAENETRESET) || (error_code == WSAENOTCONN) || 
		 (error_code == WSAESHUTDOWN) || (error_code == WSAECONNRESET) ||
		 (error_code == 0) )
	{
	   // Disconnect (ignore any errors) and try to reconnect to same host
	   (void) ptetcp_disconnect();
	   // Sleep 3 seconds to allow disconnect to complete
	   ptetcp_sleep(3000);
       if ( ptetcp_connect( current_host_address, error_msg ) != 0 )
	   {
		 // Reconnection successful - report back to calling function
         sprintf( error_msg, "Connect error %d: Reconnection successful. Try again!",
			      error_code );
		 return;
	   }
	   // Reconnection attempt failed - report back to calling function
       sprintf( error_msg, "Connect error %d: Connection lost. Exit or try again!",
			    error_code );
	}
}

void ptetcp_reconnect2( int error_code, pCHAR error_msg )
{
    if ( (error_code == WSAENETRESET) || (error_code == WSAENOTCONN) || 
		 (error_code == WSAESHUTDOWN) || (error_code == WSAECONNRESET) ||
		 (error_code == 0) )
	{
	   // Disconnect (ignore any errors) and try to reconnect to same host
	   (void) ptetcp_disconnect2();
	   // Sleep 3 seconds to allow disconnect to complete
	   ptetcp_sleep(3000);
       if ( ptetcp_connect2( current_host_address2, error_msg ) != 0 )
	   {
		 // Reconnection successful - report back to calling function
         sprintf( error_msg, "ptetcp_reconnect2: Connect error %d: Reconnection successful. Try again!",
			      error_code );
		 return;
	   }
	   // Reconnection attempt failed - report back to calling function
       sprintf( error_msg, "ptetcp_reconnect2: Connect error %d: Connection lost. Exit or try again!",
			    error_code );
	}
}

void ptetcp_reconnect3( int error_code, pCHAR error_msg )
{
    if ( (error_code == WSAENETRESET) || (error_code == WSAENOTCONN) || 
		 (error_code == WSAESHUTDOWN) || (error_code == WSAECONNRESET) ||
		 (error_code == 0) )
	{
	   // Disconnect (ignore any errors) and try to reconnect to same host
	   (void) ptetcp_disconnect3();
	   // Sleep 3 seconds to allow disconnect to complete
	   ptetcp_sleep(3000);
       if ( ptetcp_connect3( current_host_address3, error_msg ) != 0 )
	   {
		 // Reconnection successful - report back to calling function
         sprintf( error_msg, "ptetcp_reconnect3: Connect error %d: Reconnection successful. Try again!",
			      error_code );
		 return;
	   }
	   // Reconnection attempt failed - report back to calling function
       sprintf( error_msg, "ptetcp_reconnect3: Connect error %d: Connection lost. Exit or try again!",
			    error_code );
	}
}

void ptetcp_reconnect4( int error_code, pCHAR error_msg )
{
    if ( (error_code == WSAENETRESET) || (error_code == WSAENOTCONN) || 
		 (error_code == WSAESHUTDOWN) || (error_code == WSAECONNRESET) ||
		 (error_code == 0) )
	{
	   // Disconnect (ignore any errors) and try to reconnect to same host
	   (void) ptetcp_disconnect4();
	   // Sleep 3 seconds to allow disconnect to complete
	   ptetcp_sleep(3000);
       if ( ptetcp_connect4( current_host_address4, error_msg ) != 0 )
	   {
		 // Reconnection successful - report back to calling function
         sprintf( error_msg, "ptetcp_reconnect4: Connect error %d: Reconnection successful. Try again!",
			      error_code );
		 return;
	   }
	   // Reconnection attempt failed - report back to calling function
       sprintf( error_msg, "ptetcp_reconnect4: Connect error %d: Connection lost. Exit or try again!",
			    error_code );
	}
}
//*********************************************************************************