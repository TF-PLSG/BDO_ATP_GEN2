/******************************************************************************

       Module: tcp.c

        Title: ASCENDENT Dialog Manager

  Description: This is a modified file of original tcpip.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\TCP.C  $
   
      Rev 1.35   Jan 17 2000 17:05:58   MSALEH
   correct getpeername to work on Unix
   
      Rev 1.34   Jan 14 2000 15:07:44   MSALEH
   * Use new buffer field of IPC_INFO to 
   pass IP address to PTE_MSG in
   current_tran_id field
   * Correct keepalive for Client sockets
   * implement getpeername() for server/client
   sockets
   
      Rev 1.33   Jan 05 2000 13:08:58   MSALEH
   added ret variable
   
      Rev 1.32   Dec 03 1999 17:06:42   MSALEH
   fix accept() problem, timing issue
   
      Rev 1.31   Nov 19 1999 09:42:36   MSALEH
   * correct keep alive parameters
   * Perform keepalive enable/disable 
   at the socket level
   
      Rev 1.30   Nov 09 1999 16:01:52   MSALEH
   make DefaultSocket volatile
   
      Rev 1.29   Nov 05 1999 16:40:16   MSALEH
   * Correct bug with sleep between accepts
   * Add Keep alive support to tcp.c
   
      Rev 1.28   Oct 08 1999 11:34:12   MSALEH
   call thread_cancel before terminating
   the tcp/ip connection
   
      Rev 1.26   Oct 07 1999 11:12:04   MSALEH
   Correct bug related to accepting multiple sockets
   where the socket id was being over written
   when a new connection was made
   
      Rev 1.25   Oct 07 1999 07:11:20   MSALEH
   log the socket id in error messages
   
      Rev 1.24   Oct 06 1999 09:38:24   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.23   Oct 05 1999 11:28:38   MSALEH
   if socket id is 0, log an error
   
      Rev 1.22   Sep 29 1999 14:20:26   MSALEH
   AIX Modifications
   
      Rev 1.20   Sep 28 1999 14:47:38   MSALEH
   AIX Modifications
   
      Rev 1.19   Aug 27 1999 16:33:54   MSALEH
   correct variable type to long to handle
   large messages
   
      Rev 1.18   Aug 25 1999 15:54:28   MSALEH
   reworked the socket handling to use linked lists
   so that there's no maximum on the number of 
   connections
   
      Rev 1.17   Jun 16 1999 12:06:48   MSALEH
   For a server socket, if the request is generated
   from within ascendent without a socket id, then
   use a default socket id.
   
      Rev 1.16   Apr 14 1999 15:23:08   MSALEH
   Mods to handle new IGNORE option
   
      Rev 1.15   Apr 06 1999 16:23:00   MSALEH
   TCP:in tcp_write, if the connection is client
   socket then use the main socket id and don't
   depend on the value passed by other apps, this
   has the following limitation:Cannot have a server 
   listen for a connection from the network.
   Serial: Use CreateThread instead of _beginthread
   
      Rev 1.14   Apr 01 1999 12:22:58   MSALEH
   * handle multiple socket connections in server mode
   * streamline the code to be more efficient
   
      Rev 1.12   Mar 01 1999 17:23:38   MSALEH
   TCP: added functionality to handle inclusive
   and exclusive length bytes, if the tcp_header_length >= 100
   then inclusive length is assumed.
   Serial: For length 0, don't send an LRC, this
   is a racal thing only
   
      Rev 1.11   Oct 08 1998 16:42:48   MSALEH
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
   
   
      Rev 1.10   Sep 11 1998 11:29:04   drodden
   Changed to use variable that defines the size of the header on the front 
   of our TCP messages.  The header may need to be larger than just the 
   two length bytes.  This variable is a default 2, but can be set with a new
   function set_tcp_header_size.
   
   
      Rev 1.9   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.8   27 Mar 1998 16:07:40   MSALEH
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
   
      Rev 1.7   Mar 26 1998 13:58:34   drodden
   Changed to handle the case where the two length bytes are split
   across TCP packets.
   Changed to allow a NULL_PTR to be passed as the ipc-info.  Some
   applications may not want to be logged in to XIPC and can pass null
   to indicate this.
   
   
      Rev 1.6   Mar 25 1998 15:30:58   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types whever possible.
   
   
      Rev 1.5   Mar 25 1998 14:54:46   drodden
   Created a common communication library (ptecom).
   
   
      Rev 1.4  Mar 09 1998 09:43:36    MSALEH
   Correct bugs related to TCP/IP
   segmentation and concatenation


      Rev 1.3  Mar 03 1998 09:20:06    JXIAO
   Fix closing socket error


      Rev 1.2  Feb 18 1998 17:49:30    JXIAO
   Fix the TCP reconnecting problem


      Rev 1.1  Jan 29 1998 09:44:10    JXIAO
   Make necessary changes to make dialog manager
   working with AppLnk


      Rev 1.0  Dec 23 1997 15:05:00    JXIAO
   Initial version of Dialog Manager NT Service version.
   It is a converting of Passport Dialog Manager. All functionalities
   are kept, all GUI are removed.

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
// nirbhay - BDO solaris porting
#ifdef BDO_SOLARIS_PORT
#include <signal.h>
#endif
#endif

#include "tcp.h"
#include "ntutils.h"
#include "ptellist.h"
#include "pteipc.h"

#ifdef WIN32

PRIVATE HANDLE            lhandle;
#define OS_SOCKET_ERROR   SOCKET_ERROR
#define OS_INVALID_SOCKET INVALID_SOCKET
#else

#define HIBYTE(w)   ((BYTE) (((WORD) (w) >> 8) & 0xFF))
#define LOBYTE(w)   ((BYTE) (w))
#define OS_SOCKET_ERROR   -1
#define OS_INVALID_SOCKET -1

#endif

/* Count of responses received from Server. */
long volatile responsesReceived = 0;
long volatile transactionsSent = 0;

/* Client Socket, Server Socket */
DM_TCP_STRUCT        Client;          
DM_TCP_STRUCT        Server;

PRIVATE SOCKET volatile DefaultSocket;

PRIVATE LINKED_LIST  OpenSockets;
PRIVATE int volatile LinkedListLocked = FALSE;
PRIVATE int volatile ipc_info_copy_in_progress = FALSE;

extern int EndProcessSignalled;

#ifndef WIN32
extern int errno;
extern char *sys_errlist[];
extern int  ReqTpsRate;
extern char flowListFileName[256];
extern char CardNumberListFileName[256];
extern char deviceMerchantListFileName[256];
extern int  autoMode;

PRIVATE pthread_t tcpServerThread;
PRIVATE pthread_t tcpClientThread;

extern void set_connection_status(int);

/*************************************************************************
 * POSIX thread cleanup function.
 * Called when thread is cancelled.
 ************************************************************************/
void thread_cleanup(void *connectiontype)
{
  thread_logoff(); 
}
#endif

/*
 * The tcp header will consist of:
 *    a) a two byte length (hi-lo byte order) 
 *    b) zero or more bytes of additional stuff.
 */

/*------------------------------------------------------------------------------*/
void close_a_socket(SOCKET s)
{
  int    rc;
  char   szBuff[DM_ERRBUF_SIZE];

#ifdef WIN32
  shutdown(s, 2);
  rc = closesocket(s);
#else
  rc = close(s);
#endif

  if (rc == OS_SOCKET_ERROR) 
  {
#ifdef WIN32
    sprintf(szBuff, "ASCENDENT: close_a_socket, socket %0ld, error: %d", s, WSAGetLastError());
#else
    sprintf(szBuff, "ASCENDENT: close_a_socket, socket %0ld, error: %d: %s", s, errno, sys_errlist[errno]);
#endif

    printf(szBuff);
  }
}

/*------------------------------------------------------------------------------*/
void close_and_clear_socket( SOCKET s)
{
  BOOLEAN       found = FALSE;
  pLIST_ELEMENT plist_element, pcur_element;
  char          tmpbuf[11];

  close_a_socket(s);

  /* Now delete the socket from the linked list */
  sprintf(tmpbuf, "%0ld", s);

  while (LinkedListLocked)
            {
		struct timespec intv;
		intv.tv_sec = 0;
		intv.tv_nsec = 10000000;
               //pteipc_sleep(100);
               nanosleep (&intv, NULL);
            }

  LinkedListLocked = TRUE;

  pcur_element = ptellist_first_element ( &OpenSockets );
  if (pcur_element != NULL_PTR)
  {
     while ( !found && (pcur_element != NULL_PTR) )
     {
        if (strncmp(tmpbuf, ptellist_element_data (pcur_element), strlen(tmpbuf)) == 0)
        {
           found = TRUE;
           break;
        }

        pcur_element = ptellist_next_element (pcur_element);
     }

     if (found && (pcur_element != NULL_PTR))
        plist_element = ptellist_delete_element( &OpenSockets, pcur_element);
  }
  LinkedListLocked = FALSE;
}

/*------------------------------------------------------------------------------*/
void close_and_clear_all_sockets()
{
   SOCKET sock;
   pLIST_ELEMENT pcur_element;

   while (LinkedListLocked)
            {
		struct timespec intv;
		intv.tv_sec = 0;
		intv.tv_nsec = 10000000;
               //pteipc_sleep(100);
               nanosleep (&intv, NULL);
            }

   LinkedListLocked = TRUE;
   
   pcur_element = ptellist_first_element ( &OpenSockets );
   if (pcur_element != NULL_PTR)
   {
      while ( pcur_element != NULL_PTR )
      {
         sock = atol(ptellist_element_data(pcur_element));
         close_a_socket(sock);
         pcur_element = ptellist_next_element (pcur_element);
      }
   }

   ptellist_free_list_memory( &OpenSockets );

   LinkedListLocked = FALSE;
}

/*------------------------------------------------------------------------------*/
void tcp_close(int connection_type)
{ 
  switch (connection_type)
  {
  case DM_PORTTYPE_TCP_SERVER_WITH_TPDU:
  case DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU:
  case DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU:
#ifndef WIN32
   pthread_cancel(tcpServerThread);
#endif

   close_and_clear_all_sockets();
   LISTNER( Server )       = 0;
   break;

  case DM_PORTTYPE_TCP_CLIENT_WITH_TPDU:
  case DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU:
  case DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU:
#ifndef WIN32
   pthread_cancel(tcpClientThread);
#endif
   close_a_socket(LISTNER( Client ));
   LISTNER( Client )       = 0;
   break;
  }

  //pteipc_sleep(5000);
            {
		struct timespec intv;
		intv.tv_sec = 5;
		intv.tv_nsec = 0;
               //pteipc_sleep(100);
               nanosleep (&intv, NULL);
            }

#ifdef WIN32
  WSACleanup();
#endif
}

/*------------------------------------------------------------------------------*/
void tcp_read(pIPC_INFO ipc_info)
{

  LONG      rc, bytes_remaining;
  LONG      bytes_received = 0;
  LONG      msg_length     = 0;
  BOOLEAN   length_bytes_including_themselves   = FALSE;
  BOOLEAN   volatile error_condition            = FALSE;
  BYTE      tcp_header_size, conn_type;
  char *     p_incoming_buffer;
  SOCKET    sock;
  BYTE      tmpbuf[32];
  IPC_INFO  local_ipc_info;
  int       ret;
  struct    sockaddr_in  name;
  int       namelength = sizeof(name);
  char      szBuff[DM_ERRBUF_SIZE];

#ifndef WIN32
  sigset_t  set;
#endif

  /* 
     We need to do this because the ipc_info is a pointer and the value changes with
     every new connection 
  */
  memcpy(&local_ipc_info, ipc_info, sizeof (IPC_INFO));
  ipc_info_copy_in_progress = FALSE;

#ifndef WIN32

  /**
   * Block all signals to this thread.
   */
  sigfillset(&set);
  pthread_sigmask(SIG_SETMASK, &set, NULL);

  /**
   * The following (unused) code blocks everything
   * except SIGUSR1, so that we can use it as a back door
   * to kill the main thread.  See dialog.c.
   */

  /*sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &set, NULL);*/

  /* Get PID for use in error messages. */
#endif

  conn_type       = local_ipc_info.connectiontype;
  tcp_header_size = local_ipc_info.messageheaderlength;
  sock            = (SOCKET)(local_ipc_info.socket_id);
 
  ret             = getpeername ( sock, (struct sockaddr*)&name, (pDWORD)&namelength );
  
  if (ret == OS_SOCKET_ERROR)
  {
#ifdef WIN32
      sprintf(szBuff, "ASCENDENT: tcp_read, getpeername failed, socket:%0ld, error: %d ", sock, WSAGetLastError());
#else
      sprintf(szBuff, "ASCENDENT: tcp_read, getpeername failed, socket:%0ld, error: %d: %s", sock, errno, sys_errlist[errno]);
#endif
      printf(szBuff);

      sprintf(szBuff, "ASCENDENT: tcp_read, client connected with socket:%0ld", sock );
  }
  else
  {
  /* allocate memory to receive incoming messages from this client */
      strcpy( (char*)local_ipc_info.buffer, (char*)inet_ntoa(name.sin_addr));
      if ((conn_type == DM_PORTTYPE_TCP_CLIENT_WITH_TPDU) ||
          (conn_type == DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU) ||
          (conn_type == DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU))
         sprintf(szBuff, "ASCENDENT: tcp_read, client connected with socket:%0ld to IP address:%s", sock, local_ipc_info.buffer);
      else
         sprintf(szBuff, "ASCENDENT: tcp_read, client at IP address:%s connected with socket:%0ld", local_ipc_info.buffer, sock);
  }

  printf(szBuff);

  /* allocate memory to receive incoming messages from this client */
  p_incoming_buffer = NULL_PTR;
  p_incoming_buffer = (char *)malloc( MAX_INCOMING_BUFFER_SIZE );
  
  if( p_incoming_buffer == NULL_PTR )
  {
    printf( "ASCENDENT: tcp_read: Unable to allocate memory for incoming message");
    close_and_clear_socket(sock);
#ifdef WIN32
    return;
#else
    pthread_exit((void *)0);
#endif
  }

  if (conn_type)
    thread_logon(local_ipc_info.instancename, local_ipc_info.loginname);

#ifndef WIN32
  pthread_cleanup_push(thread_cleanup, (void*)&(local_ipc_info.connectiontype));
#endif
 
  /* 
    Read data from the socket
    Check of the length bytes flag, if there is one, then read the length bytes
    first, then get the remainder of the packet
    otherwise, read up to the maximum
  */

  if (tcp_header_size > 100)
  {
     tcp_header_size -= 100;
     length_bytes_including_themselves = TRUE;
  }

  while(!error_condition)
  {
   if (tcp_header_size)
   {
      rc = recv(sock, tmpbuf, tcp_header_size, NO_FLAGS_SET);

      if((rc == OS_SOCKET_ERROR) || (rc == 0) || !connection_up)
      {
         error_condition = TRUE;

         /* This is done for cleint only becuase the server main socket is in 
            server_tcp_accept, and even if the worker socket is terminated, we do
            not want the listner socket terminated.
         */
         if ((conn_type == DM_PORTTYPE_TCP_CLIENT_WITH_TPDU) ||
             (conn_type == DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU) ||
             (conn_type == DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU))
             connection_up = FALSE;

         if (rc == 0)
         {
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld disconnected", sock );
            printf(szBuff);
         }
         else
         {
#ifdef WIN32
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, rc = %d,%d error: %d ", sock, rc, connection_up, WSAGetLastError());
#else
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, rc = %d,%d error: %d: %s", sock, rc, connection_up, errno, sys_errlist[errno]);
#endif
            perror(szBuff);
         }
         break;
      }

      /* if the number of bytes we asked for is not the number of bytes we received */
      if (rc == tcp_header_size)
      {
         /* rc = tcp_header_size, so get the length */
         msg_length = tmpbuf[1] + (256 * (WORD)tmpbuf[0]);
         if (length_bytes_including_themselves)
            msg_length -= tcp_header_size;

         if (msg_length == 0)
            continue;

         if ( (msg_length < 0) || (msg_length > (MAX_INCOMING_BUFFER_SIZE - 1)))
         {
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, msglen:%0ld, invalid message length ", 
                                                               sock, 
                                                               msg_length );

            printf(szBuff);
            error_condition = TRUE;
            break;
         }
      
         bytes_remaining = msg_length;

         while (bytes_remaining > 0)
         {
            rc = recv(sock, (p_incoming_buffer + bytes_received), bytes_remaining, NO_FLAGS_SET );

            if((rc == OS_SOCKET_ERROR) || (rc == 0) || !connection_up)
            {
               /* This is done for client only because the server main socket is in 
                  server_tcp_accept, and even if the worker socket is terminated, we do
                  not want the listner socket terminated.
               */
               if ((conn_type == DM_PORTTYPE_TCP_CLIENT_WITH_TPDU) ||
                   (conn_type == DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU) ||
                   (conn_type == DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU))
                  connection_up = FALSE;

               error_condition = TRUE;

               if (rc == 0)
               {
                  sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld disconnected", sock );
                  printf(szBuff);
               }
               else
               {
#ifdef WIN32
                  sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, error: %d ", sock, WSAGetLastError());
#else          
                  sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, error: %d: %s", sock, errno, sys_errlist[errno]);
#endif         
                  printf(szBuff);
               }
               break;
            }

            bytes_received  += rc;
            bytes_remaining -= rc;
         }  /* while (bytes_received < bytes_remaining)  */

         if (error_condition)
            break;

         bytes_received = bytes_remaining = 0;

         pfNotify(p_incoming_buffer, (WORD)msg_length, &local_ipc_info);
      }
      else
      {
         printf("ASCENDENT: tcp_read, Unexpected number of length bytes received");
         error_condition = TRUE;
         break;
      }     /* if (rc == tcp_header_length)              */
   }        /* if (tcp_header_size)                      */
   else
   {
      rc = recv(sock, p_incoming_buffer, (MAX_INCOMING_BUFFER_SIZE - 1), NO_FLAGS_SET);

      if((rc == OS_SOCKET_ERROR) || (rc == 0) || !connection_up)
      {
         /* This is done for cleint only becuase the server main socket is in 
            server_tcp_accept, and even if the worker socket is terminated, we do
            not want the listner socket terminated.
         */
         if ((conn_type == DM_PORTTYPE_TCP_CLIENT_WITH_TPDU) ||
             (conn_type == DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU) ||
             (conn_type == DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU))
             connection_up = FALSE;

         error_condition = TRUE;

         if (rc == 0)
         {
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld disconnected", sock );
            printf(szBuff);
         }
         else
         {
#ifdef WIN32
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, error: %d ", sock, WSAGetLastError());
#else
            sprintf(szBuff, "ASCENDENT: tcp_read, recv, socket:%0ld, error: %d: %s", sock, errno, sys_errlist[errno]);
#endif
            printf(szBuff);
         }
      }

      pfNotify(p_incoming_buffer, (WORD)rc, &local_ipc_info);
   }
  }

  free( p_incoming_buffer );
#ifdef BDO_SOLARIS_PORT
#ifndef WIN32
  pthread_cleanup_pop(0);
#endif

    /* Wait until the main thread signals END.*/
    while ( !EndProcessSignalled )
    {
	struct timespec intv;
	intv.tv_sec = 1;
	intv.tv_nsec = 0;
	//sleep(1);
	nanosleep (&intv, NULL);
    }

#endif
  thread_logoff(); 
  close_and_clear_socket(sock);
}

/*------------------------------------------------------------------------------*/
BOOLEAN tcp_write(const WORD len, const void *data, pIPC_INFO ipc_info)
{
  WORD    tmp;
  int     rc, tcp_header_size;
  pBYTE   buf;
  BOOLEAN length_bytes_include_themselves = FALSE;
  SOCKET  sock;
  char    szBuff[DM_ERRBUF_SIZE];

  /* Idiot check the send buffer */
  if(!data)
  { 
    printf("ASCENDENT: tcp_write, bad buffer pointer passed");
    return(FALSE);
  }

  tcp_header_size = ipc_info->messageheaderlength;

  /* When working with server sockets, we do not know which socket id to
     use when sending out, so we get it in the ipc_info, however, when
     working with client socket, there is only one socket to work with, 
     so we use it, this is essential when dealing with messages to the host
     that originate from within ascendent, example, dialog manager forwards
     a message to hstcon to be sent to the host, hstcon HAS TO BE a client 
     socket.
  */
  if ( (ipc_info->connectiontype == DM_PORTTYPE_TCP_CLIENT_WITH_TPDU) ||
       (ipc_info->connectiontype == DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU) ||
       (ipc_info->connectiontype == DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU))
     sock = LISTNER( Client );
  else
  {
      /* Assign a values to DefaultSocket, this is to resolve the case when we are 
         generating a message from within ascendent to go to a host that is hanging off
         the NAC, in that case, the socket id in the request will be blank, we'll use 
         the DefaultSocket value as a default
      */
      sock = (ipc_info->socket_id ? ipc_info->socket_id : DefaultSocket);

      if (!sock)
      {
         sprintf(szBuff, "ASCENDENT: tcp_write, socket:%0ld, invalid socket id", sock);
         printf(szBuff);
         return(FALSE);
      }
  }

  if (tcp_header_size >= 100)
  {
     tcp_header_size -= 100;
     length_bytes_include_themselves = TRUE;
  }

  /* Create a temporary storage area */
  tmp = len + tcp_header_size;
  buf = malloc(tmp);
   
  if(!buf)
  {
    printf("ASCENDENT: tcp_write, Cannot allocate Memory");
    return(FALSE);
  }

  /*  Prepend the data with a 2-byte length attribute
      IMPORTANT: The length does not include the two length
      bytes, so when calculating the length, use the len variable
      and not the tmp variable.
      NOTE: the tcp header may be larger than 2 bytes.
      IMPORTANT: if the tcp_header_size is greate then 100, then this
      means that the length bytes include themselves
  */

  /* add the length bytes only if the setup requires it */
  if (tcp_header_size)
  {
      memset(buf, 0, tcp_header_size);           /* clear the tcp header portion  */

      if (!length_bytes_include_themselves)
      {
         buf[0] = HIBYTE(len);                   /* put length in first two bytes */
         buf[1] = LOBYTE(len);
      }
      else
      {
         buf[0] = HIBYTE(tmp);                   /* put length in first two bytes */
         buf[1] = LOBYTE(tmp);
      }
  }

  memcpy(buf + tcp_header_size, data, len);  /* move data after tcp header    */

  /* Write data to the socket */
  rc = send(sock, buf, tmp, NO_FLAGS_SET);
 
  free(buf);

  if(rc == OS_SOCKET_ERROR)
  {
#ifdef WIN32
    sprintf( szBuff, "ASCENDENT: tcp_write, send failed, socket:%0ld, Error:%d", sock, WSAGetLastError() );
#else
    sprintf( szBuff, "ASCENDENT: tcp_write, send failed, socket:%0ld, Error:%d: %s", sock, errno, sys_errlist[errno]);
#endif

    printf(szBuff);
    return(FALSE);
  }

  return(TRUE);
}

/*------------------------------------------------------------------------------*/
void server_tcp_accept(pIPC_INFO ipc_info)
{
  int             size;
  int             rc;
  DWORD           cli_tid;
  SOCKET          incoming_socket;
  pLIST_ELEMENT   list_element;
  char            tmpstr[11];
  IPC_INFO        local_ipc_info;
  char            szBuff[DM_ERRBUF_SIZE];


#ifndef WIN32
  sigset_t        set;
  pthread_t       thread_id;
  /**
   * Block all signals to this thread.
   */

  sigfillset(&set);
  pthread_sigmask(SIG_SETMASK, &set, NULL);

  /**
   * The following (unused) code blocks everything
   * except SIGUSR1, so that we can use it as a back door
   * to kill the main thread.  See dialog.c.
   */

  /*sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &set, NULL);*/

  /* Get PID for use in error messages. */

  pthread_cleanup_push(thread_cleanup, (void*)&(ipc_info->connectiontype));
#endif

  /*
     We need to do this because the ipc_info is a pointer and the value changes with
     every new accept
  */
  memcpy(&local_ipc_info, ipc_info, sizeof (IPC_INFO));

  size = sizeof(SIN( Server ));

  sprintf(tmpstr, "%0ld", LISTNER( Server ));
  while (LinkedListLocked)
            {
		struct timespec intv;
		intv.tv_sec = 0;
		intv.tv_nsec = 10000000;
               //pteipc_sleep(100);
               nanosleep (&intv, NULL);
            }

  LinkedListLocked = TRUE;
  ptellist_init_list( &OpenSockets );
  list_element = ptellist_append_element( &OpenSockets,
                                          tmpstr,
                                          strlen(tmpstr) );
  LinkedListLocked = FALSE;

  while(1)
  {
    /* Accept the incoming connection */
#ifdef WIN32
    incoming_socket = accept(LISTNER( Server ), (LPSOCKADDR)&(SIN( Server )), &size);
#else
    incoming_socket = accept(LISTNER( Server ), (struct sockaddr*)&(SIN( Server )), (size_t *)&size);
#endif

    if(incoming_socket == OS_INVALID_SOCKET)
    {
      connection_up = FALSE;
      printf("ASCENDENT: server_tcp_accept, Accept Failed");
      return;
    }

    sprintf(tmpstr, "%0ld", incoming_socket);
    
    while (LinkedListLocked)
            {
		struct timespec intv;
		intv.tv_sec = 0;
		intv.tv_nsec = 10000000;
               //pteipc_sleep(100);
               nanosleep (&intv, NULL);
            }

    LinkedListLocked = TRUE;
    list_element = ptellist_append_element( &OpenSockets,
                                            tmpstr, 
                                            strlen(tmpstr) );
    LinkedListLocked = FALSE;

    /*   Assign a values to DefaultSocket, this is to resolve the case when we are 
         generating a message from within ascendent to go to a host that is hanging off
         the NAC, in that case, the socket id in the request will be blank, we'll use 
         the DefaultSocket value as a default
    */

    /* Create the worker thread to serve the new client */
    local_ipc_info.socket_id = DefaultSocket = incoming_socket;

    rc = setsockopt (incoming_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&send_keepalive, sizeof(int));
    if (rc == OS_SOCKET_ERROR )
    {
#ifdef WIN32
      sprintf( szBuff, "ASCENDENT: server_tcp_accept, keep alive init failed, socket %0ld, error %d", 
                              incoming_socket, WSAGetLastError() );
#else
      sprintf( szBuff, "ASCENDENT: server_tcp_accept, keep alive init failed: socket %0ld, error %d %s\n", 
                              incoming_socket, errno, sys_errlist[errno] );
#endif
      printf(szBuff);
    }

    ipc_info_copy_in_progress = TRUE;

#ifdef WIN32
    if( CreateThread( NULL,
                      0,
                      (LPTHREAD_START_ROUTINE)tcp_read,
                      &local_ipc_info,
	      	          0,
                      &cli_tid ) == NULL )
    {
      printf( "ASCENDENT: server_tcp_accept, Unable to create tcp_read thread");
    }
#else
    rc = pthread_create(  &thread_id, NULL, (void*(*)(void*))tcp_read, (void*)&local_ipc_info);

    if( rc != 0 )
    {
       sprintf(szBuff,
       "ASCENDENT: server_tcp_accept, thread create failed: %d %s\n", errno, sys_errlist[errno] );
       printf(szBuff);
    }
#endif

    /* this is required to allow time for the thread to get initialized with the proper parameters */
    while (ipc_info_copy_in_progress)
            {
		struct timespec intv;
		intv.tv_sec = 1;
		intv.tv_nsec = 0;
               //pteipc_sleep(1000);
               nanosleep (&intv, NULL);
            }

  }
#ifdef BDO_SOLARIS_PORT
#ifndef WIN32
  pthread_cleanup_pop(1);
#endif
#endif
}

/*------------------------------------------------------------------------------*/
BOOLEAN server_tcp_init(const USER_TCP_STRUCT *ud, pIPC_INFO ipc_info)
{

#ifdef WIN32
  WORD      wVersionRequested;  
  WSADATA   wsaData; 
  DWORD	   lthread_id;
#else
  int       ret;
#endif

  int       err, size;
  char      szBuff[DM_ERRBUF_SIZE];
   
  /* Idiot check all incoming pointers */
  if(!ud)
  {
    printf("ASCENDENT: server_tcp_init, bad tcp structure pointer passed");
    return(FALSE);
  }
  else if(!ud->pfNotify)
  {
    printf("ASCENDENT: server_tcp_init, bad Function/Buffer pointer passed");
    return(FALSE);
  }

  LinkedListLocked = FALSE;

  /* Assign tcp/ip functions to the library primitives */
  port_write = tcp_write;
  port_close = tcp_close;

#ifdef WIN32
  /* Initialize winsock */
  wVersionRequested = MAKEWORD(1, 1); 
  err = WSAStartup(wVersionRequested, &wsaData); 

  /* See if winsock started okay */
  if(err != 0)
  {
    printf("ASCENDENT: server_tcp_init, winsock.dll bad or missing");
    return(FALSE);
  }

  if( LOBYTE( wsaData.wVersion ) <= WS_VERSION_MAJOR  &&
      HIBYTE( wsaData.wVersion ) < WS_VERSION_MINOR )
  {
     printf( "ASCENDENT: server_tcp_init: Windows Socket version required not supported by winsock.dll");
     return (FALSE);
  }

  if( wsaData.iMaxSockets < MIN_SOCKETS_REQD ) 
  {
     printf( "ASCENDENT: server_tcp_init: winsock.dll installed on this system does not support the minimum number of sockets");
     return (FALSE);
  }
#endif

  /* Open the socket */
  LISTNER( Server ) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (LISTNER( Server ) == OS_INVALID_SOCKET)
  {
#ifdef WIN32
    sprintf( szBuff, "ASCENDENT: server_tcp_init, socket failed, %d", WSAGetLastError() );
#else
    printf("ASCENDENT: server_tcp_init, socket failed");
#endif
    printf(szBuff);
    return(FALSE);
  }

  /* Bind the socket to a local port number */
  size = sizeof(SIN( Server ));
  memset (&SIN( Server ), 0, size); 
  SIN( Server ).sin_family      = AF_INET;
  SIN( Server ).sin_port        = htons(ud->usPort);
  SIN( Server ).sin_addr.s_addr = INADDR_ANY;

#ifdef WIN32
  if(bind(LISTNER( Server ), (LPSOCKADDR)&(SIN( Server )), size) == OS_SOCKET_ERROR)
  {
    sprintf( szBuff, "ASCENDENT: server_tcp_init, bind socket failed, %d", WSAGetLastError() );
    printf(szBuff);
    tcp_close(ipc_info->connectiontype);
    return(FALSE);
  }
#else
  if(bind(LISTNER( Server ), (const struct sockaddr*)&(SIN( Server )), size) == OS_SOCKET_ERROR)
  {
// Next two lines added for debug by JMG on 1/30/02
    sprintf(szBuff, "ASCENDENT: server_tcp_init, bind socket error %d: %s",
                     errno, sys_errlist[errno]);
    printf(szBuff);
//    printf("ASCENDENT: server_tcp_init);
    tcp_close(ipc_info->connectiontype);
    return(FALSE);
  }
#endif

  /* Listen the socket */
  if(listen(LISTNER( Server ), MAX_PENDING_CONNECTS) < 0)
  {
#ifdef WIN32
    sprintf(szBuff, "ASCENDENT: server_tcp_init, listen failed error:%d", WSAGetLastError());
#else
    sprintf(szBuff, "ASCENDENT: server_tcp_init, listen failed error:%d: %s", errno, sys_errlist[errno]);
#endif

    printf(szBuff);
    tcp_close(ipc_info->connectiontype);
    return(FALSE);
  }

  /* Populate callback fields */
  pfNotify  = ud->pfNotify;

#ifdef WIN32
  /* create thread to listen on the socket */
  lhandle = CreateThread(  NULL,
                           0,
                           (LPTHREAD_START_ROUTINE)server_tcp_accept, 
		                     ipc_info,
                           0,
                           &lthread_id );
  if( lhandle == NULL )
  {
    printf( "ASCENDENT: server_tcp_init, Unable to create listener thread");
    tcp_close(ipc_info->connectiontype);
    return (FALSE);
  }
#else
  ret = pthread_create( &tcpServerThread, NULL, (void*(*)(void*))server_tcp_accept, (void*)ipc_info );
  if( ret != 0 )
  {
     sprintf(szBuff,
       "ASCENDENT: server_tcp_init, Unable to create listener thread, %d %s\n", errno, sys_errlist[errno] );
     printf(szBuff);
  }
#endif

  return(TRUE);
}


#ifdef WIN32

#define OS_SOCKET_ERROR   SOCKET_ERROR
#define OS_INVALID_SOCKET INVALID_SOCKET

#else

#define HIBYTE(w)   ((BYTE) (((WORD) (w) >> 8) & 0xFF))
#define LOBYTE(w)   ((BYTE) (w))
#define OS_SOCKET_ERROR   -1
#define OS_INVALID_SOCKET -1

#endif

volatile int tpsrtcount = 0;
#define SIMULTANEOUS_POS_CONNECTIONS 30
extern INT MainProcessDone;

void tpsrSendHandler(int *tlevel)
{
	char inst_name[256];
	pPTE_MSG  p_msg_out;
	CHAR      Buffer[256], ErrorMsg[100];
	LONG      ret_code;
	CHAR      dest_que[17] = "dcpisoA";
	CHAR      szBuff[256];
	int newlevel;
	INT ret;
	LONG stanInt = 0;

#ifdef WIN32
	PRIVATE HANDLE            thandle;
	DWORD	   lthread_id;
#else
	PRIVATE pthread_t tThread;
#endif
	// Sale transaction.
	//char txnToSend[] = "\x02""\x00""\x70""\x24""\x05""\x80""\x00""\xC0""\x00""\x00""\x16""\x41""\x77""\x77""\x77""\x77""\x77""\x77""\x74""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x01""\x00""\x11""\x11""\x14""\x07""\x10""\x00""\x10""\x00""\x03""\x00""\x33""\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31";

	// log into XCOM/XIPC, send a transaction to ATP, increment
	// thread-count, create another thread until MainProcessDone
	// is not 1

	tpsrtcount++;	// number of threads started.
	printf ("Thread started - level = %d, count = %d.\n", *tlevel, tpsrtcount);
	newlevel = (*tlevel) + 1 ;

	// Now launch next thread.
	if ( newlevel < SIMULTANEOUS_POS_CONNECTIONS )
	{
		// Start another thread. Total 2^10 threads will try to
		// simultaneously log into XCOM/XIPC.
#ifdef WIN32
		/* create thread to listen on the socket */
		thandle = CreateThread(  NULL,
							   0,
							   (LPTHREAD_START_ROUTINE)tpsrSendHandler,
								 &newlevel,
							   0,
							   &lthread_id );
		if( thandle == NULL )
		{
			LogEvent( "XCOM: dtest: send thread, Unable to create thread", ERROR_MSG );
			//pteipc_shutdown_single_instance_app(); 
			MainProcessDone = 1;
			return;
		}
#else
		ret = pthread_create( &tThread, NULL, (void*(*)(void*))tpsrSendHandler, (void*)&newlevel );
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

	}

	tpsr_establish_connection(*tlevel);
}

// memory allocated for 'line' must be atleast 513 bytes.
int getTxnPartLine(char *line, int *lengthRead, FILE *flfPtr)
{
    char tempStrStorage[1024];
    int data_len;
    memset ( tempStrStorage, 0x00, sizeof( tempStrStorage ) );

    // Read a part of the transaction.
    if ( fgets( line, 512, flfPtr) == NULL )
    {
	printf ("ERROR: Cannot read flow list file in auto mode.\n");
	return (FALSE);
    }

    // Convert part data from ascii to bin.
    data_len = strlen ( line );	// Hex data in ascii format - Eg:- "FF302A"
    if ( ! ( line [ data_len - 1 ] == '\n' ||
	line [ data_len - 1 ] == '\r' ) )
    {	// problem with file format
	printf ("ERROR: Problem with file format in auto mode.\n");
	return (FALSE);
    }
    else if ( ( data_len % 2 ) == 0 )
    {
	printf ("ERROR: Invalid flow file\n");
	return (FALSE);
    }
    else
    {	// remove newline character
	line [ data_len - 1 ] = '\0';
	data_len = ( data_len - 1 ) / 2 ;
    }
    ascii_to_bin ((unsigned char *)&line[0], tempStrStorage, data_len * 2);
    memcpy (line, tempStrStorage, data_len);
    *lengthRead = data_len;

    return (TRUE);
}

double ptetime_get_time();

#define LEN_OF_CARD_NUMBER_FIELD		20
#define LEN_OF_DEV_MRCHNT_FIELD		23

/*------------------------------------------------------------------------------
 When a network client wants to connect to a server,
 it must have:
 1.) a TCP port number (via getservbyname())
    and
 2.) an IP address of the remote Client (gethostbyname()).

 The following summarizes the steps used to connect.
 Get the name of the remote host computer in which
 to connect from the setup
 * Check to see if the hosts file knows the computer (gethostbyname)
 * Get the host information (hostent structure filled)
 * Fill in the address of the remote host into the servent structure (memcpy)
 * Get the port number (getservbyname)
 * Fill in the port number of the servent structure
   Establish a connection (connect)
------------------------------------------------------------------------------*/
BOOLEAN tpsr_client_tcp_init(const USER_TCP_STRUCT *dc, pIPC_INFO ipc_info, int tlevel)
{
#ifdef WIN32
	PHOSTENT        phe;       /* Host Name */
	IN_ADDR         RemoteIpAddress;
	WORD            wVersionRequested;  
	WSADATA         wsaData; 
	DWORD	         lthread_id;
#else
	struct hostent  *phe; /* host name */
	struct in_addr  RemoteIpAddress;
	int             ret;
#endif

	int             size, err, i = 1, rc, not, data_len, write_status;
	char            szBuff[DM_ERRBUF_SIZE];
	FILE		  *flowListFilePtr = NULL, *cardNumberListFilePtr = NULL,
					*deviceMerchantCombinationListFilePtr =NULL;
	double	  initial_time, ending_time;
	unsigned char	  autoModeTxnPart1[1024], autoModeTxnPart2[1024],
		    autoModeTxnPart3[1024], autoModeTxnPart4[1024], tmpAutoModeReadStr[1024];
	int lengthOfPart1, lengthOfPart2, lengthOfPart3, lengthOfPart4,
		stanInt = ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * tlevel);
	int numberOfCardNumbersAvailable = 0, numberOfDevMrchntAvailable = 0, ij = 0;

	char	*cardNumberList = NULL, *devMerchntList = NULL;
	int		*cardNumberListLengths = NULL, *devMerchntListLengths = NULL;

	/* Idiot check all incoming pointers */
	if(!dc)
	{
		printf("ASCENDENT: tpsr_client_tcp_init, bad tcp structure pointer passed");
		return(FALSE);
	}
	else if(!dc->pfNotify)
	{
		printf("ASCENDENT: tpsr_client_tcp_init, bad Function/Buffer pointer passed");
		return(FALSE);
	}

	LinkedListLocked = FALSE;

	port_write = tcp_write;
	port_close = tcp_close;

#ifdef WIN32
	/* Initialize winsock */
	wVersionRequested = MAKEWORD(1, 1); 
	err = WSAStartup(wVersionRequested, &wsaData); 

	/* See if winsock started okay */
	if(err != 0)
	{
		printf("ASCENDENT: tpsr_client_tcp_init, winsock.dll bad or missing");
		return(FALSE);
	}

	/* Open the socket */
	LISTNER( Client ) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (LISTNER( Client ) == INVALID_SOCKET)
	{
		printf("ASCENDENT: tpsr_client_tcp_init, socket failed");
		return(FALSE);
	}
#else
	/* Open the socket */
	LISTNER( Client ) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (LISTNER( Client ) == -1)
	{
		printf("ASCENDENT: tpsr_client_tcp_init, socket failed");
		return(FALSE);
	}
#endif

	rc = setsockopt (LISTNER( Client ), SOL_SOCKET, SO_KEEPALIVE, (char *)&send_keepalive, sizeof(int));
	if (rc == OS_SOCKET_ERROR )
	{
#ifdef WIN32
		sprintf( szBuff, "ASCENDENT: tpsr_client_tcp_init, keep alive init failed, socket %0ld, error %d", 
		                      LISTNER( Client ), WSAGetLastError() );
#else
		sprintf( szBuff, "ASCENDENT: tpsr_client_tcp_init, keep alive init failed: socket %0ld, error %d %s\n", 
		                      LISTNER( Client ), errno, sys_errlist[errno] );
#endif
		printf(szBuff);
	}

	size = sizeof(SIN( Server ));
	memset (&SIN( Client ), 0, size); 

	/* Now do the direct tcp connection to host setup
	 Bind the socket to a local port number
	*/
	SIN( Client ).sin_family = AF_INET;
	SIN( Client ).sin_port = htons(dc->usPort);

	phe = gethostbyname(dc->HostName);
	if (phe == NULL) 
	{ 
		RemoteIpAddress.s_addr = inet_addr (dc->HostName);
		if (RemoteIpAddress.s_addr == -1) 
		{
#ifdef WIN32
			sprintf(szBuff, "ASCENDENT: tpsr_client_tcp_init, RemoteIpAddress = -1, Error: %d", WSAGetLastError());
#else
			sprintf(szBuff, "ASCENDENT: tpsr_client_tcp_init, RemoteIpAddress = -1, Error: %d, %s", errno, sys_errlist[errno]);
#endif
			printf(szBuff);
			return (FALSE);
		}
		else
		{
			SIN( Client ).sin_addr = RemoteIpAddress;
		}
	}
	else
	{
		memcpy(&(SIN(Client).sin_addr), phe->h_addr, phe->h_length);
	}

	size = sizeof(SIN( Client ));

#ifdef WIN32
	if (connect( LISTNER( Client ), (LPSOCKADDR) &(SIN( Client )), size) == OS_SOCKET_ERROR)
	{
		sprintf(szBuff, "ASCENDENT: tpsr_client_tcp_init, connect failed, Error: %d", WSAGetLastError());
		printf(szBuff);
		return(FALSE);
	}
#else
	if (connect( LISTNER( Client ), (const struct sockaddr*)&(SIN( Client )), size) == OS_SOCKET_ERROR) 
	{
		sprintf(szBuff, "ASCENDENT: tpsr_client_tcp_init, connect failed, Error: %d, %s", errno, sys_errlist[errno]);
		printf(szBuff);
		return(FALSE);
	}
#endif

	/* Start the listening thread to receive responses, before sending
	 transactions. So that dialog manager can respond immediately */
	pfNotify  = dc->pfNotify;

	ipc_info->socket_id = LISTNER( Client );

	flowListFilePtr = fopen (flowListFileName, "r");
	if ( flowListFilePtr == NULL )
	{
		printf ("ERROR: Cannot open flow list file.\n");
		return (FALSE);
	}

    if ( autoMode == 1 || autoMode == 2 )
    {
		cardNumberListFilePtr = fopen (CardNumberListFileName, "r");
		if ( cardNumberListFilePtr == NULL )
		{
			printf ("ERROR: Cannot open card number list file.\n");
			return (FALSE);
		}
		deviceMerchantCombinationListFilePtr = fopen (deviceMerchantListFileName, "r");
		if ( deviceMerchantCombinationListFilePtr == NULL )
		{
			printf ("ERROR: Cannot open device merchant list file.\n");
			return (FALSE);
		}
	}
#ifdef WIN32
	/* create thread to listen on the socket */
	lhandle = CreateThread(  NULL,
	                       0,
	                       (LPTHREAD_START_ROUTINE)tcp_read, 
				           ipc_info,
	                       0,
	                       &lthread_id );
	if( lhandle == NULL )
	{
		printf( "ASCENDENT: tpsr_client_tcp_init, Unable to create read thread");
		tcp_close(ipc_info->connectiontype);
		return (FALSE);
	}
#else
	ret = pthread_create( &tcpClientThread, NULL, (void*(*)(void*))tcp_read, (void*)ipc_info );
	if( ret != 0 )
	{
		 sprintf(szBuff, "ASCENDENT: tpsr_client_tcp_init, Unable to create read thread, %d, %s",
		                  errno, sys_errlist[errno] );
		 printf(szBuff);
		 return(FALSE);
	}
#endif

	// set it here itself as we are not going to return.
	set_connection_status(TRUE);

	/* Now post all transactions from the flow list file. Responses are
	   being read in the parallel thread simultaneously. Rate of
	   sending transactions per second (TPS) is read from tf.ini into the
	   global variable ReqTpsRate. */


	// Automode 0 is similar to traffic generator - reads a flow list
	// file, line by line, and posts to ATP without any modifications,
	// at the set TPS rate provided in tf.ini - TPSRTOOL section.
	// Tool stops after all transactions in the file are sent and all
	// responses are received.
	//
	// Automode 1 generates random STAN, card number, device_id and
	// merchant id's. Requires a flow file having the other parts of
	// the transaction in four parts (one part in each line). Generated
	// transaction will have the folowing form:
	// <Part1>Stan<Part2>CardNum<Part3>Device and Merchant
	// Tool stops only after closing the tool with CTRL-C.
	// May not generate valid transactions, can be used for negative
	// testing. TXNS are sent at the set TPS rate in tf.ini
	// Tool runs indefinitely until CTRL-C is not pressed.
	//
	// Automode 2 takes THREE FILES as INPUT. First file is same as
	// the file required in Automode 1 with four parts of the transactions.
	// Second file should contain a list of valid card numbers preceded
	// by the length of card_number - Eg: 164111222233334444 where 16 is
	// the length of card number and card number is 4111222233334444.
	// In case card_number is of 15 digits (odd number lengths), last digit
	// should be set to 0. Eg: 154111222233334440 - here length is 15
	// and card number is 411122223333444.
	// Third file shall contain combination of valid device_id and
	// merchant_id, one in each line, in ASCII format as requred in ISO
	// protocol.
	// -
	// In this mode, transactions are generated sequentially with STAN
	// starting at 000001, 000002 and so on. Card number and device/merchant
	// id combination is picked randomly from the second/third input file.
	// Transactions are sent at the set TPS rate in tf.ini
	// Tool runs indefinitely until CTRL-C is not pressed.

    if ( autoMode == 1 || autoMode == 2 )
	{	// auto generate STAN mode.
		// Read parts of the transaction before and after the STAN.
		if ( getTxnPartLine ( autoModeTxnPart1, &lengthOfPart1,
						flowListFilePtr) == FALSE )
		    return (FALSE);

		if ( getTxnPartLine ( autoModeTxnPart2, &lengthOfPart2,
						flowListFilePtr) == FALSE )
		    return (FALSE);

		if ( getTxnPartLine ( autoModeTxnPart3, &lengthOfPart3,
						flowListFilePtr) == FALSE )
		    return (FALSE);

		if ( getTxnPartLine ( autoModeTxnPart4, &lengthOfPart4,
						flowListFilePtr) == FALSE )
		    return (FALSE);

		if ( autoMode == 2 )
		{
			char tempStr[1024];
		    // Read up all card_numbers and device_merchant combinations.
		    // Store in a huge array allocated dynamically.

		    // Calculate number of lines in Card number list file
		    while (fgets(tempStr, 100, cardNumberListFilePtr) != NULL )
				numberOfCardNumbersAvailable++;

			fseek(cardNumberListFilePtr, SEEK_SET, 0);// reset to first character

		    // Calculate number of lines in Card number list file
		    while (fgets(tempStr, 200, deviceMerchantCombinationListFilePtr) != NULL )
				numberOfDevMrchntAvailable++;

			fseek(deviceMerchantCombinationListFilePtr, SEEK_SET, 0);// reset to first character

			// allocate memory to store card numbers and device/merchant combinations.
			cardNumberList	=	malloc ( numberOfCardNumbersAvailable * LEN_OF_CARD_NUMBER_FIELD );
			cardNumberListLengths = malloc (numberOfCardNumbersAvailable * sizeof(int));
			devMerchntList	=	malloc ( numberOfDevMrchntAvailable * LEN_OF_DEV_MRCHNT_FIELD );
			devMerchntListLengths = malloc (numberOfDevMrchntAvailable * sizeof(int));

			ij = 0;
			// Read all card numbers
		    while (getTxnPartLine(cardNumberList + ij*LEN_OF_CARD_NUMBER_FIELD, cardNumberListLengths + ij, cardNumberListFilePtr) != FALSE )
			{
				if ( ij > numberOfCardNumbersAvailable )
					break;

				ij++;
			}

			ij = 0;
			// Read all device/merchant combinations
		    while (getTxnPartLine( tmpAutoModeReadStr, devMerchntListLengths + ij, deviceMerchantCombinationListFilePtr) != FALSE )
			{
				if ( *(devMerchntListLengths + ij) != 23 )	// Do not accept anything other than 23
					return (FALSE);
				if ( ij > numberOfDevMrchntAvailable )
					break;
				memcpy ( devMerchntList +ij*LEN_OF_DEV_MRCHNT_FIELD, tmpAutoModeReadStr, *(devMerchntListLengths + ij));
				ij++;
			}
		}
	}

	sleep(3);
	printf ("\n");

	while( 1 )
	{
		initial_time = (double)ptetime_get_time();
		for ( not = 0 ; not < ReqTpsRate ; not++ )
		{
			unsigned char	readBuf[513] = "", outgoing_buf1[1024], stanStr[7],
					cardNbrStr[20], cardNbrBCD[11], DevMerchIdStr[24];
			if ( autoMode == 1 )
			{	// auto generate Card-Number, STAN, device-Id and Merch-Id.
			    stanInt++;	// Increment STAN.
			    if ( stanInt >= 1000000 || stanInt >= ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * (tlevel + 1)) )
			    {
					stanInt = ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * tlevel) + 1;
			    }
			    sprintf( stanStr, "%06d", stanInt );

			    // Generate Card Number based on count
			    sprintf ( cardNbrStr, "4177777777%06d", stanInt );
			    cardNbrBCD[0] = 0x16;	// Length of card number
			    ascii_to_bin ( (unsigned char *)&cardNbrStr[0],
					    (unsigned char *)&cardNbrBCD[1], 16);

			    // Generate DeviceId and Merchant-ID based on count.
			    sprintf (DevMerchIdStr, "00%06d000000000%06d", stanInt, stanInt );

			    //		Transaction is generated as follows
			    //		-----------------------------------
			    // Copy Part1, followed by card-num, followed by Part2,
			    // followed by stan, followed by Part3, followed by
			    // device-id / merchant-ID, followed by Part4
			    memcpy ( outgoing_buf1, autoModeTxnPart1, lengthOfPart1);	// Part1
			    memcpy ( outgoing_buf1 + lengthOfPart1, cardNbrBCD, 9);	// CardNbr
			    memcpy ( outgoing_buf1 + lengthOfPart1 + 9, autoModeTxnPart2,
							    lengthOfPart2);
			    ascii_to_bin ( (unsigned char *)&stanStr[0], outgoing_buf1 +
					lengthOfPart1 + 9 + lengthOfPart2, 6);// STAN
			    memcpy ( outgoing_buf1 + lengthOfPart1 + 9 +
				    lengthOfPart2 + 3, autoModeTxnPart3,
							    lengthOfPart3);
			    memcpy ( outgoing_buf1 + lengthOfPart1 + 9 +
				    lengthOfPart2 + 3 + lengthOfPart3, DevMerchIdStr, 23);
			    memcpy ( outgoing_buf1 + lengthOfPart1 + 9 +
				    lengthOfPart2 + 3 + lengthOfPart3 + 23,
				    autoModeTxnPart4, lengthOfPart4);

			    data_len = lengthOfPart1 + 9 + lengthOfPart2 + 3 +
					    lengthOfPart3 + 23 + lengthOfPart4 ;
			}
			else if ( autoMode == 2 )
			{	// auto generate STAN and pick up card-number, device-Id and Merch-Id from list files.
				stanInt++;	// Increment STAN.
			    if ( stanInt >= 1000000 || stanInt >= ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * (tlevel + 1)) )
			    {
					stanInt = ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * tlevel) + 1;
			    }
				sprintf( stanStr, "%06d", stanInt );

				// Generate Card Number based on count
				//sprintf ( cardNbrStr, "4177777777%06d", stanInt );
				//cardNbrBCD[0] = 0x16;	// Length of card number
				//ascii_to_bin ( (unsigned char *)&cardNbrStr[0],
				//		    (unsigned char *)&cardNbrBCD[1], 16);
				memcpy(&cardNbrBCD[0], cardNumberList + (stanInt%numberOfCardNumbersAvailable)*LEN_OF_CARD_NUMBER_FIELD,
							cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)]);

				// Generate DeviceId and Merchant-ID based on count.
				//sprintf (DevMerchIdStr, "00%06d000000000%06d", stanInt, stanInt );

				// first iteration, 1-1 2-2 3-3... and so on will combine. Next iteration, 1-2 2-3 3-4,... will combine.
				memcpy (DevMerchIdStr, devMerchntList + (((stanInt%numberOfDevMrchntAvailable) + (stanInt/numberOfDevMrchntAvailable))%numberOfDevMrchntAvailable)*LEN_OF_DEV_MRCHNT_FIELD,
						devMerchntListLengths[(((stanInt%numberOfDevMrchntAvailable) + (stanInt/numberOfDevMrchntAvailable))%numberOfDevMrchntAvailable)]);

				//		Transaction is generated as follows
				//		-----------------------------------
				// Copy Part1, followed by card-num, followed by Part2,
				// followed by stan, followed by Part3, followed by
				// device-id / merchant-ID, followed by Part4
				memcpy ( outgoing_buf1, autoModeTxnPart1, lengthOfPart1);	// Part1
				memcpy ( outgoing_buf1 + lengthOfPart1, cardNbrBCD, cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)]);	// CardNbr
				memcpy ( outgoing_buf1 + lengthOfPart1 + cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)], autoModeTxnPart2,
							    lengthOfPart2);
				ascii_to_bin ( (unsigned char *)&stanStr[0], outgoing_buf1 +
					lengthOfPart1 + cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)] + lengthOfPart2, 6);// STAN
				memcpy ( outgoing_buf1 + lengthOfPart1 + cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)] +
				    lengthOfPart2 + 3, autoModeTxnPart3,
							    lengthOfPart3);
				memcpy ( outgoing_buf1 + lengthOfPart1 + cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)] +
				    lengthOfPart2 + 3 + lengthOfPart3, DevMerchIdStr, 23);
				memcpy ( outgoing_buf1 + lengthOfPart1 + cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)] +
				    lengthOfPart2 + 3 + lengthOfPart3 + 23,
				    autoModeTxnPart4, lengthOfPart4);

				data_len = lengthOfPart1 + cardNumberListLengths[(stanInt%numberOfCardNumbersAvailable)] + lengthOfPart2 + 3 +
					    lengthOfPart3 + 23 + lengthOfPart4 ;
			}
			else
			{	// stanInt used in automode only but still incrementing here
				// as a counter for display purposes.
				stanInt++;
			    if ( stanInt >= 1000000 || stanInt >= ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * (tlevel + 1)) )
			    {
					stanInt = ((1000000 / SIMULTANEOUS_POS_CONNECTIONS) * tlevel) + 1;
			    }

				if ( fgets( readBuf, 512, flowListFilePtr) == NULL )
					break;
				data_len = strlen ( readBuf );
				if ( ! ( readBuf [ data_len - 1 ] == '\n' ||
				readBuf [ data_len - 1 ] == '\r' ) )
				{	// problem with file format
					break;
				}
				else if ( ( data_len % 2 ) == 0 )
				{
					printf ("ERROR: Invalid flow file\n");
					break;
				}
				else
				{	// remove newline character
					readBuf [ data_len - 1 ] = '\0';
					data_len = ( data_len - 1 ) / 2 ;
				}
				ascii_to_bin ((unsigned char *)&readBuf[0], outgoing_buf1, data_len * 2);
			}

			write_status = port_write((WORD)data_len, outgoing_buf1, ipc_info);

	        if (write_status)
			{
				transactionsSent++;
				//	    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
				//	    "Sent = %06d, Received = %06ld", stanInt, responsesReceived);
				//printf("\rSent = %06d, Received = %06ld  ", stanInt, responsesReceived);
				//fflush(stdout);

				if (trace_on) 
				{
					while (trace_lock_on)
					{
						struct timespec intv;
						intv.tv_sec = 0;
						intv.tv_nsec = 10000000;
						//pteipc_sleep(100);
						nanosleep (&intv, NULL);
					}
					trace_lock_on = TRUE;
					PrintBuffer(outgoing_buf1, data_len, TRUE, "NO TPDU", "dialog");
					trace_lock_on = FALSE;
				}
			}
	        else
			{
				printf("Ascendent: PinnacleMSgHandler, dialog port_write failed\n");
				/* We don't want to do that because there might be other connections established
				set_connection_status(FALSE);
				*/
			}
		}

		if ( not < ReqTpsRate )	// exit due to end of file or error.
			break;

		// Wait until 1 second is over.
		ending_time = 0.0;
		initial_time += (double)1.0;
		while ( ending_time < initial_time )
		{
			ending_time = ptetime_get_time();
			usleep(1000000/ReqTpsRate);
		}
    }

    fclose ( flowListFilePtr );

    // Make sure application doesn't exit without waiting for all responses.
    // tcp_read thread may still be processing incoming responses from
    // the server.
    if ( autoMode == 1 )
    {
    }
    else
    {
#if 0	// waiting is being done in the main thread.
		// While all responses have been received.
		while ( ( stanInt - 1 ) > responsesReceived )
		{
		    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
		    "Sent = %06d, Received = %06ld", stanInt, responsesReceived);
		    fflush(stdout);
		    //pteipc_sleep(400);
		    //{
			//struct timespec intv;
			//intv.tv_sec = 0;
			//intv.tv_nsec = 40000000;
			//nanosleep (&intv, NULL);
		    //}
		    sleep(1);
		}
#endif
    }

	EndProcessSignalled = 1; // Send signal to exit application.
	return(TRUE);
}

