/******************************************************************************
  
        Module: ptetcp.c
  
         Title: Ascendent TCP/IP library routines
  
   Description: This module contains the routines for communicating with the
                Ascendent Applink Server.

   $Log:   N:\PVCS\PTE\CORE\ptetcp_lib\PTETCP.C  $
   
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

#include "ptemsg.h"
#include "ntutils.h"

#define MAX_MSG_LENGTH 1024


#define      WS_VERSION_REQD    0x0101
#define      WS_VERSION_MAJOR   HIBYTE(WS_VERSION_REQD)
#define      WS_VERSION_MINOR   LOBYTE(WS_VERSION_REQD)
#define      MIN_SOCKETS_REQD   16


//****************************************************************
//****************** GLOBAL VARIABLES ****************************
SOCKET   cli_sock;
CHAR     outgoing_msg[64 * MAX_MSG_LENGTH];
CHAR     incoming_msg[64 * MAX_MSG_LENGTH];
CHAR     host_name[50];

CHAR     interactive_que_name[12];


//*********************************************************************************
//*********************************************************************************
pPTE_MSG ptetcp_receive( LONG time_out, pCHAR error_msg )
{
   INT   temp, rc, err_code;
   INT   incoming_msg_length = 0;
   INT   bytes_received = 0;
   BYTE  recv_buffer[MAX_MSG_LENGTH];
   INT   ret;
   pPTE_MSG p_msg_in;
   fd_set FD_SET;
   struct timeval tv, *ptv;


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
      rc = recv( cli_sock, recv_buffer, MAX_MSG_LENGTH, 0 );
      if( (rc == SOCKET_ERROR) || (rc == 0))
      {
         err_code = WSAGetLastError();
         sprintf( error_msg, "ptetcp_receive: error %d", err_code );
         return NULL_PTR;
      }
      else
      {
         temp = (unsigned char)recv_buffer[0];
         temp <<= 8;
         incoming_msg_length |= temp;
         temp = (unsigned char)recv_buffer[1];
         incoming_msg_length |= temp;
         bytes_received += rc;
         memcpy( incoming_msg, recv_buffer, rc);
      }
   }
   else if( ret == 0 )
   {
      sprintf( error_msg, "ptetcp_receive: timed_out on receive" );
      return NULL_PTR;
   }
   else
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_receive: error %d", err_code );
      return NULL_PTR;
   }

   while( bytes_received < incoming_msg_length )
   {
      rc = recv( cli_sock, recv_buffer, MAX_MSG_LENGTH, 0 );
      if( (rc == SOCKET_ERROR) || (rc == 0))
      {
         err_code = WSAGetLastError();
         sprintf( error_msg, "ptetcp_receive: error %d", err_code );
         return NULL_PTR;
      }
      else
         memcpy( incoming_msg + bytes_received, recv_buffer, rc );

      bytes_received += rc;
   }

   p_msg_in = (pPTE_MSG)( incoming_msg + 2 );
   return p_msg_in;
}



//*********************************************************************************
//*********************************************************************************
INT ptetcp_send( pPTE_MSG p_msg_out, pCHAR error_msg )
{
   INT err_code, outgoing_length, rc;

   //ptemsg_set_orig_comms_info( p_msg_out, host_name );

   outgoing_length = ptemsg_get_msg_length( p_msg_out );
   if( outgoing_length > (sizeof( outgoing_msg ) - 2) )
   {
      sprintf( error_msg, "ptetcp_send: message size exceeds maximum allowable size of %d bytes", sizeof( outgoing_msg ) - 2 );
      return 0;
   }

   memcpy( outgoing_msg + 2, (char *)p_msg_out, outgoing_length );
   outgoing_length += 2;

   outgoing_msg[0] = outgoing_length >> 8;
   outgoing_msg[1] = outgoing_length & 0x00ff;

   rc = send( cli_sock, outgoing_msg, outgoing_length, 0 );
   if( rc == SOCKET_ERROR )
   {
      err_code = WSAGetLastError();
      sprintf( error_msg, "ptetcp_send: error %d", err_code );
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

   if( !ptetcp_send( p_msg_out, error_msg ) )
   {
      if( p_msg_out != NULL_PTR )
         free( p_msg_out );
      return 0;
   }

   if( p_msg_out != NULL_PTR )
      free( p_msg_out );

   p_msg_in = ptetcp_receive( time_out, error_msg );
   return p_msg_in;

}

//****************************************************************
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
   CHAR           ini_file[256];
   SHORT          port_num;
   INT            ret, rc;

   // initialise structures
   memset( &remote_ip_address, 0, sizeof( remote_ip_address ) );
   memset( &remoteAddr, 0, sizeof( remoteAddr ) );

	// Create the client-side socket 
	cli_sock = socket( AF_INET, SOCK_STREAM, 0 );
	if( cli_sock == INVALID_SOCKET )
	{
      sprintf( error_msg, "ptetcp_connect: Windows Sockets error %d: Couldn't create socket.",
              WSAGetLastError() );
		printf( "%s\n", error_msg );
      return 0;
	}


/*******************************************
   // set the TCP_NODELAY option for the socket
	optval = 1;
	ret = setsockopt ( cli_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof( INT ));
	if( ret == SOCKET_ERROR )
	{
      sprintf( error_msg, "ptetcp_connect: Unable to set TCP_NODELAY option for the socket. Error %d",
              WSAGetLastError() );
		printf( "%s\n", error_msg );
		closesocket( cli_sock );
      return 0;
	}
**********************************************/

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
   GetPinnacleConfigDirectory( ini_file );
   strcat( ini_file, "applink.ini" );

   rc = GetPrivateProfileString(
        "AppLink", 	               // points to section name 
        "PortNumber",	            // points to key name 
        "1450",	                  // points to default string 
        port_str,                   // points to destination buffer 
        sizeof(port_str) - 1,       // size of destination buffer 
        ini_file );                 // points to initialization filename 

   port_num = atoi( port_str );

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ( port_num );
	remoteAddr.sin_addr = remote_ip_address;

	ret = connect( cli_sock, (PSOCKADDR)&remoteAddr, sizeof( remoteAddr ) );
	if( ret == SOCKET_ERROR )
	{
      sprintf ( error_msg, "ptetcp_connect: connect failed to host \"%s\",  Port \"%d\". Error Code: %ld",
                host_addr, port_num, WSAGetLastError() );
      printf( "%s\n", error_msg );
		return 0;
	}


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
