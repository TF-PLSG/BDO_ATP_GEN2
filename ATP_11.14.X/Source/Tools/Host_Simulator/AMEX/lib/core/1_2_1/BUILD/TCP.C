/******************************************************************************

       Module: tcp.c

        Title: Pinnacle Dialog Manager

  Description: This is a modified file of original tcpip.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\TCP.C  $
   
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
#include <process.h>

#include "tcp.h"
#include "NTUtils.h"



/* Direct Connect, Client Socket, Server Socket */
DM_TCP_STRUCT       host;          
USER_DC_STRUCT      dc;
DM_TCP_STRUCT       conn;


PRIVATE SOCKET  volatile IncomingSock;
PRIVATE BOOLEAN volatile incomplete_tcp_msg;         
PRIVATE INT     volatile total_length;
PRIVATE INT     volatile remaining_length;
PRIVATE BOOLEAN volatile tcp_split_length;
PRIVATE BYTE    volatile tcp_length_byte;         

PRIVATE CHAR    data[MAX_INCOMING_BUFFER_SIZE+1];
PRIVATE CHAR    szBuff [DM_ERRBUF_SIZE];  // Temp buffer - used to pass strings

/*
 * The tcp header will consist of:
 *    a) a two byte length (hi-lo byte order) 
 *    b) zero or more bytes of additional stuff.
 */
PRIVATE BYTE    tcp_header_size = 2;



//------------------------------------------------------------------------------
void set_tcp_header_size ( BYTE hdr_size )
{
   if (hdr_size < 2)
      hdr_size = 2;

   tcp_header_size = hdr_size;
}

//------------------------------------------------------------------------------
void init_storage()
{
  total_length       = 0;
  remaining_length   = 0;
  incomplete_tcp_msg = FALSE;
  tcp_split_length   = FALSE;
  memset (data, 0x00, sizeof(data));
}

//------------------------------------------------------------------------------
void build_msg_and_foreward(pIPC_INFO ipc_info)
{
  if (total_length <= MAX_INCOMING_BUFFER_SIZE)
  {
    memcpy(&data [total_length - remaining_length], &lpBuf[0], remaining_length);
    pfNotify(data, (WORD)total_length, ipc_info);
  }
  else
    LogEvent("PINNACLE: build_msg_and_foreward, data exceeds buffer capacity", ERROR_MSG);

  init_storage();
}

//------------------------------------------------------------------------------
void process_new_msg(INT rc, INT i, pIPC_INFO ipc_info)
{
  WORD    msg_len;


  if (tcp_split_length == TRUE)
  {
    tcp_split_length = FALSE;
    msg_len = lpBuf[i] + (256 * (WORD)tcp_length_byte);

    i += tcp_header_size - 1; // bump past the remainder of the tcp header.
    rc-= tcp_header_size + 1; // remove remainder of the tcp header from the message length.
  }
  else
  {
    msg_len = lpBuf[1+i] + (256 * (WORD)lpBuf[i]);

    i += tcp_header_size;     // bump past the tcp header.
    rc-= tcp_header_size;     // remove header size from the message length.
  }

  if (msg_len == 0)
     return;

  if (rc >= msg_len)
  {        
    while (rc > 0)
    {
      if (rc >= msg_len)
      {   
        // case 1 & 2
        pfNotify(&lpBuf[i], msg_len, ipc_info);
      } 
      else
      {
        // case 5
        // it is less, so store the portion that we have on the stack and set the
        // dirty flag to TRUE to monitor tha following packets. 
        init_storage();

        // check the max length
        if ((rc <= MAX_INCOMING_BUFFER_SIZE) && (msg_len <= MAX_INCOMING_BUFFER_SIZE))
        {          
          incomplete_tcp_msg = TRUE;
          total_length       = msg_len;
          remaining_length   = msg_len - rc;
          memcpy(&data, &lpBuf[i], rc);
          break;
        }
        else
        {
          LogEvent("PINNACLE:process_new_message, data exceeds buffer capacity", ERROR_MSG);
          init_storage();
        }
      }
            
      rc-= msg_len;
      i += msg_len;

      if (rc <= 0)
        break;

      if (rc == 1)
      {
        tcp_length_byte  = lpBuf [i];
        tcp_split_length = TRUE;
        break;
      }

      msg_len = lpBuf[1+i] + (256 * (WORD)lpBuf[i]);

      i+=tcp_header_size;    // bump past the tcp header.
      rc-=tcp_header_size;   // remove header size from the message length.
    }  // while (rc > 0)
  }
  else
  {
    // rc < msg_len, so store the portion that we have on the stack and set the
    // dirty flag to TRUE to monitor the following packets.
    // check the max length
    if ((rc <= MAX_INCOMING_BUFFER_SIZE) && (msg_len <= MAX_INCOMING_BUFFER_SIZE))
    {          
      incomplete_tcp_msg = TRUE;
      total_length       = msg_len;
      remaining_length   = msg_len - rc;
      memcpy(&data, &lpBuf[i], rc);
    }
    else
    {
      LogEvent("PINNACLE: process_new_message, data exceeds buffer capacity", ERROR_MSG);
      init_storage();
    }
  }    // else rc < msg_len
}  

//------------------------------------------------------------------------------
void close_a_socket(SOCKET s)
{
  INT rc;

  rc = closesocket(s);

  if (rc == SOCKET_ERROR) 
  {
    sprintf(szBuff, "PINNACLE: close_a_socket, error: %d", WSAGetLastError());
    LogEvent(szBuff, ERROR_MSG);
  }
}

//------------------------------------------------------------------------------
void CloseSocket(int connection_type)
{

  switch (connection_type)
  {
  case DM_PORTTYPE_TCP:
  case DM_PORTTYPE_RAW_TCP:
  case DM_PORTTYPE_TCP_NO_TPDU:
    TCPCONNECTED( conn ) = FALSE;
    close_a_socket(LISTNER( conn ));
    LISTNER( conn ) = 0;
    SOCK_CLOSED( conn ) = TRUE;
    break;

  case DM_PORTTYPE_DIRECT:
  case DM_PORTTYPE_RAW_DIRECT:
    TCPCONNECTED( host ) = FALSE;
    close_a_socket(LISTNER( host ));
    LISTNER( host ) = 0;
    SOCK_CLOSED( host ) = TRUE;
    break;
  }

  Sleep(5000);
  WSACleanup();
}

//------------------------------------------------------------------------------
void tcp_close(int connection_type)
{ 
  switch (connection_type)
  {
  case DM_PORTTYPE_TCP:
  case DM_PORTTYPE_RAW_TCP:
  case DM_PORTTYPE_TCP_NO_TPDU:
    if (TCPCONNECTED( conn ) == TRUE) 
    {
      shutdown(IncomingSock, 2);
      close_a_socket(IncomingSock);
      shutdown(LISTNER( conn ), 2);
      CloseSocket(connection_type); 
    }
    else
      LogEvent("PINNACLE: tcp_close, TCPCONNECTED( conn ) = FALSE", WARN_MSG);
    break;

  case DM_PORTTYPE_DIRECT:
  case DM_PORTTYPE_RAW_DIRECT:
    if (TCPCONNECTED( host ) == TRUE) 
    {
      shutdown(LISTNER( host ), 2);
      CloseSocket(connection_type); 
    }
    else
      LogEvent("PINNACLE: tcp_close, TCPCONNECTED( conn ) = FALSE", WARN_MSG);
    break;
  }

  Sleep(3000);
}

//------------------------------------------------------------------------------
void tcp_read(pIPC_INFO ipc_info)
{

  INT    rc, size;
  INT    i;

  // Idiot check the receive buffer
  if(!lpBuf)
  {
    connection_up = FALSE;
    LogEvent("PINNACLE: tcp_read, Bad data pointer", ERROR_MSG);
    _endthread();
  }

  if (ipc_info != NULL_PTR)
    login_xipc(ipc_info->connectiontype, ipc_info->instancename, ipc_info->loginname);
  
  size = sizeof(SIN( conn ));
  
  // Accept the incoming connection
  IncomingSock = accept(LISTNER( conn ), (LPSOCKADDR)&(SIN( conn )), &size);

  if(IncomingSock == SOCKET_ERROR)
  {
    connection_up = FALSE;
    LogEvent("PINNACLE: tcp_read, Accept Failed", ERROR_MSG);
    logoff_xipc(ipc_info->connectiontype); 
    _endthread();
  }

  while(TCPCONNECTED( conn ))
  {
    // Read data from the socket
    rc = recv(IncomingSock, lpBuf, wBufLen, NO_FLAGS_SET);

    if(!TCPCONNECTED( conn ))
    {
      LogEvent("PINNACLE: tcp_read, Disconnected", INFO_MSG);
      logoff_xipc(ipc_info->connectiontype); 
      _endthread();
    }

    if((rc == SOCKET_ERROR) || (rc == 0) || !connection_up)
    {
      connection_up = FALSE;
      logoff_xipc(DM_PORTTYPE_TCP); 

      if (rc == SOCKET_ERROR)
        LogEvent("PINNACLE: tcp_read, recv, socket_error", ERROR_MSG);
      else if (rc == 0)
        LogEvent("PINNACLE: tcp_read, recv, connection remotely closed", ERROR_MSG);
      else
        LogEvent("PINNACLE: tcp_read, Disconnected", ERROR_MSG);

      _endthread();
    }
  
    // Code to handle message segmentation, tcp layer max message size is about 1560
    // including the header, which will leave about 1460 for actual data. So this piece 
    // of logic will handle incomplete message using a structure to store incomplete messages
    //
    // Cases to take care of:
    // 1) complete msg in one frame
    // 2) several complete messages in one frame (concatenation)
    // 3) incomplete msg in one frame
    // 4) incomplete msg and a complete msg concatenated in one frame
    // 5) complete msgs or msgs and an incomplete message concatenated in one frame
    // 6) 2 incomplete msgs in one frame
    // 7) completion message portion in one frame.

    if (incomplete_tcp_msg == FALSE)
    {
      // Cases 1, 2, 5
      // check if what was received is less than what should have been received
      process_new_msg(rc, 0, ipc_info);
    }      // if direct_incomplete_tcp_msg = false   
    else
    {
      // cases 3,4,6,7
      // check if rc == msg length, if it is equal, then send the msg
      // else go through the array and compare the rc against the remaining length field
      // if a match was found, then concat and send the complete msg, else, if there is space
      // push it.
      
      // We are going to assume that TCP will guarantee delivery of fragments in 
      // sequence, unduplicated and non interleaved with other messages.
      // therefore, the logic will verify if the number of bytes we got is
      // equal to the number of bytes calculated from the first two bytes of the message
      // compare that number to what we need to complete the message, extract that
      // many bytes then deal with the rest if there is any.

      // case 3
      // test if we found the completion of the message, then concatenate and send
      // else concatenate and wait for the completion
      
      // what we received is also a partial message, concatenate it
      if (remaining_length > rc)
      {
        if (rc <= MAX_INCOMING_BUFFER_SIZE)
        {
          memcpy(&data[total_length - remaining_length], &lpBuf[0], rc);
          remaining_length -= rc;
        }
        else
        {
          LogEvent("PINNACLE: tcp_read, data exceeds buffer capacity", ERROR_MSG);
          init_storage();
        }
      }
      else
      {
        // if rc > remaining length, in this case we have three possibilities
        // either we have a completion portion and a complete message or two
        // incomplete messages (cases 4,6,7), or a completion portion
        // however, the first portion is always the completion portion.

        // case 7, remianing_length == rc, case 4,6 direct_remaining_length < rc
        // set rc = direct_remaining_length because direct_remaining_length gets initialized to zero
        // in direct_build_msg_and_foreward
        i   = remaining_length;
        rc -= remaining_length;
        build_msg_and_foreward(ipc_info);
        
        if (rc > 0)
        {                   
          process_new_msg(rc, i, ipc_info);
        } // else direct_remaining_length < rc

      }   // else direct_remaining_length <= rc
    }       // else direct_incomplete_tcp_msg = true       
  }  // while(TCPCONNECTED())

  LogEvent("PINNACLE: tcp_read, exit loop, receive disconnect", INFO_MSG);
  logoff_xipc(ipc_info->connectiontype); 
  _endthread();
}

//------------------------------------------------------------------------------
BOOLEAN tcp_write(const WORD len, const void * data)
{
  WORD   tmp;
  INT    rc;
  pBYTE buf;

  // Idiot check the send buffer
  if(!data)
  { 
    LogEvent("PINNACLE: tcp_write, bad buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  // Create a temporary storage area
  tmp = len + tcp_header_size;
  buf = malloc(tmp);
   
  if(!buf)
  {
    LogEvent("PINNACLE: tcp_write, Cannot allocate Memory", ERROR_MSG);
    return(FALSE);
  }

  // Prepend the data with a 2-byte length attribute
  // IMPORTANT: The length does not include the two length
  // bytes, so when calculating the length, use the len variable
  // and not the tmp variable.
  // NOTE: the tcp header may be larger than 2 bytes.

  memset(buf, 0, tcp_header_size);           /* clear the tcp header portion  */
  buf[0] = HIBYTE(len);                      /* put length in first two bytes */
  buf[1] = LOBYTE(len);
  memcpy(buf + tcp_header_size, data, len);  /* move data after tcp header    */

  // Write data to the socket
  rc = send(IncomingSock, buf, tmp, NO_FLAGS_SET);
 
  free(buf);

  if(rc == SOCKET_ERROR)
  {
    sprintf( szBuff, "PINNACLE: tcp_write, send/write failed, Error:%d", WSAGetLastError() );
    LogEvent(szBuff, ERROR_MSG);
    return(FALSE);
  }

  return(TRUE);
}

//------------------------------------------------------------------------------
void direct_tcp_read(pIPC_INFO ipc_info)
{
  int    rc;
  int    i, j;

  // Idiot check the receive buffer
  if(!lpBuf)
  {
    connection_up = FALSE;
    LogEvent("PINNACLE: direct_tcp_read, Bad data pointer", ERROR_MSG);
    _endthread();
  }

  if (ipc_info != NULL_PTR)
    login_xipc(ipc_info->connectiontype, ipc_info->instancename, ipc_info->loginname);

  while(TCPCONNECTED( host ))
  {
    // Read data from the socket
    rc = recv(LISTNER( host ), lpBuf, wBufLen, NO_FLAGS_SET);

    if(!TCPCONNECTED( host ))
    {
      logoff_xipc(ipc_info->connectiontype); 
      LogEvent("PINNACLE: direct_tcp_read, Disconnected", INFO_MSG);
      _endthread();
    }
    
    if((rc == SOCKET_ERROR) || (rc == 0) || !connection_up)
    {
      logoff_xipc(ipc_info->connectiontype); 
      connection_up = FALSE;

      if (rc == SOCKET_ERROR)
        LogEvent("PINNACLE: direct_tcp_read, recv, socket_error", ERROR_MSG);
      else if (rc == 0)
        LogEvent("PINNACLE: direct_tcp_read, recv, connection remotely closed", ERROR_MSG);
      else
        LogEvent("PINNACLE: direct_tcp_read, Disconnected", ERROR_MSG);

     _endthread();
    }

    // Code to handle message segmentation, tcp layer max message size is about 1560
    // including the header, which will leave about 1460 for actual data. So this piece 
    // of logic will handle incomplete message using a structure to store incomplete messages
    //
    // Cases to take care of:
    // 1) complete msg in one frame
    // 2) several complete messages in one frame (concatenation)
    // 3) incomplete msg in one frame
    // 4) incomplete msg and a complete msg concatenated in one frame
    // 5) complete msgs or msgs and an incomplete message concatenated in one frame
    // 6) 2 incomplete msgs in one frame
    // 7) completion message portion in one frame.

    if (incomplete_tcp_msg == FALSE)
    {
      // Cases 1, 2, 5
      // check if what was received is less than what should have been received
      process_new_msg(rc, 0, ipc_info);
    }      // if direct_incomplete_tcp_msg = false   
    else
    {
      // cases 3,4,6,7
      // check if rc == msg length, if it is equal, then send the msg
      // else go through the array and compare the rc against the remaining length field
      // if a match was found, then concat and send the complete msg, else, if there is space
      // push it.
      
      // We are going to assume that TCP will guarantee delivery of fragments in 
      // sequence, unduplicated and non interleaved with other messages.
      // therefore, the logic will verify if the number of bytes we got is
      // equal to the number of bytes calculated from the first two bytes of the message
      // compare that number to what we need to complete the message, extract that
      // many bytes then deal with the rest if there is any.

      // case 3
      // test if we found the completion of the message, then concatenate and send
      // else concatenate and wait for the completion
      
      // what we received is also a partial message, concatenate it
      if (remaining_length > rc)
      {
        if (rc <= MAX_INCOMING_BUFFER_SIZE)
        {
          j = total_length - remaining_length;
          memcpy(&data[j], &lpBuf[0], rc);
          remaining_length -= rc;
        }
        else
        {
          LogEvent("PINNACLE: direct_tcp_read, data exceeds buffer capacity", ERROR_MSG);
          init_storage();
        }
      }
      else
      {
        // if rc > remaining length, in this case we have three possibilities
        // either we have a completion portion and a complete message or two
        // incomplete messages (cases 4,6,7), or a completion portion
        // however, the first portion is always the completion portion.

        // case 7, remianing_length == rc, case 4,6 direct_remaining_length < rc
        // set rc = direct_remaining_length because direct_remaining_length gets initialized to zero
        // in direct_build_msg_and_foreward
        i   = remaining_length;
        rc -= remaining_length;
        build_msg_and_foreward(ipc_info);
        
        if (rc > 0)
        {                   
          process_new_msg(rc, i, ipc_info);
        } // else direct_remaining_length < rc
      }     // else rc != msglen+2
    }       // else direct_incomplete_tcp_msg = true      
  }         // while(TCPCONNECTED( host )) 

  LogEvent("PINNACLE: direct_tcp_read, exit loop, receive disconnect", INFO_MSG);
  logoff_xipc(ipc_info->connectiontype); 
  _endthread();
}

//------------------------------------------------------------------------------
BOOLEAN direct_tcp_write(const WORD len, const void * data)
{
  WORD   tmp;
  int    rc;
  pBYTE buf;

  // Idiot check the send buffer
  if(!data)
  { 
    LogEvent("PINNACLE: direct_tcp_write, bad buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  // Create a temporary storage area
  tmp = len + tcp_header_size;
  buf = malloc(tmp);
   
  if(!buf)
  {
    LogEvent("PINNACLE: direct_tcp_write, Cannot allocate Memory", ERROR_MSG);
    return(FALSE);
  }

  // Prepend the data with a 2-byte length attribute
  // IMPORTANT: The length does not include the two length
  // bytes, so when calculating the length, use the len variable
  // and not the tmp variable.
  // NOTE: the tcp header may be larger than 2 bytes.

  memset(buf, 0, tcp_header_size);           /* clear the tcp header portion  */
  buf[0] = HIBYTE(len);                      /* put length in first two bytes */
  buf[1] = LOBYTE(len);
  memcpy(buf + tcp_header_size, data, len);  /* move data after tcp header    */

  // Write data to the socket
  rc = send(LISTNER( host ), buf, tmp, NO_FLAGS_SET);

  free(buf);

  if(rc == SOCKET_ERROR)
  {
    sprintf( szBuff, "PINNACLE: direct_tcp_write, send/write failed, Error:%d", WSAGetLastError() );
    LogEvent(szBuff, ERROR_MSG);
    return(FALSE);
  }

  return(TRUE);
}

//------------------------------------------------------------------------------
void raw_tcp_read(pIPC_INFO ipc_info)
{

  INT    rc, size;

  // Idiot check the receive buffer
  if(!lpBuf)
  {
    connection_up = FALSE;
    LogEvent("PINNACLE: raw_tcp_read, Bad data pointer", ERROR_MSG);
    _endthread();
  }

  if (ipc_info != NULL_PTR)
    login_xipc(ipc_info->connectiontype, ipc_info->instancename, ipc_info->loginname);
  
  size = sizeof(SIN( conn ));
  
  // Accept the incoming connection
  IncomingSock = accept(LISTNER( conn ), (LPSOCKADDR)&(SIN( conn )), &size);

  if(IncomingSock == SOCKET_ERROR)
  {
    connection_up = FALSE;
    LogEvent("PINNACLE: raw_tcp_read, Accept Failed", ERROR_MSG);
    logoff_xipc(ipc_info->connectiontype); 
    _endthread();
  }

  while(TCPCONNECTED( conn ))
  {
    // Read data from the socket
    rc = recv(IncomingSock, lpBuf, wBufLen, NO_FLAGS_SET);

    if(!TCPCONNECTED( conn ))
    {
      LogEvent("PINNACLE: raw_tcp_read, Disconnected", INFO_MSG);
      logoff_xipc(ipc_info->connectiontype); 
      _endthread();
    }

    if((rc == SOCKET_ERROR) || (rc == 0) || !connection_up)
    {
      connection_up = FALSE;
      logoff_xipc(ipc_info->connectiontype); 

      if (rc == SOCKET_ERROR)
        LogEvent("PINNACLE: raw_tcp_read, recv, socket_error", ERROR_MSG);
      else if (rc == 0)
        LogEvent("PINNACLE: raw_tcp_read, recv, connection remotely closed", ERROR_MSG);
      else
        LogEvent("PINNACLE: raw_tcp_read, Disconnected", ERROR_MSG);

      _endthread();
    }
  
    /* 
      We have no way of handling concatenation and segmentation, 
      the application logic will have to take care of that
    */
    pfNotify(lpBuf, (WORD)rc, ipc_info);
  }  // while(TCPCONNECTED())

  LogEvent("PINNACLE: raw_tcp_read, exit loop, receive disconnect", INFO_MSG);
  logoff_xipc(ipc_info->connectiontype); 
  _endthread();
}

//------------------------------------------------------------------------------
BOOLEAN raw_tcp_write(const WORD len, const void * data)
{
  INT    rc;

  // Idiot check the send buffer
  if(!data)
  { 
    LogEvent("PINNACLE: raw_tcp_write, bad buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  // Write data to the socket
  rc = send(IncomingSock, data, len, NO_FLAGS_SET);
 
  if(rc == SOCKET_ERROR)
  {
    sprintf( szBuff, "PINNACLE: raw_tcp_write, send/write failed, Error:%d", WSAGetLastError() );
    LogEvent(szBuff, ERROR_MSG);
    return(FALSE);
  }

  return(TRUE);
}

//------------------------------------------------------------------------------
void raw_direct_tcp_read(pIPC_INFO ipc_info)
{
  int    rc;

  // Idiot check the receive buffer
  if(!lpBuf)
  {
    connection_up = FALSE;
    LogEvent("PINNACLE: raw_direct_tcp_read, Bad data pointer", ERROR_MSG);
    _endthread();
  }

  if (ipc_info != NULL_PTR)
    login_xipc(ipc_info->connectiontype, ipc_info->instancename, ipc_info->loginname);

  while(TCPCONNECTED( host ))
  {
    // Read data from the socket
    rc = recv(LISTNER( host ), lpBuf, wBufLen, NO_FLAGS_SET);

    if(!TCPCONNECTED( host ))
    {
      logoff_xipc(ipc_info->connectiontype); 
      LogEvent("PINNACLE: raw_direct_tcp_read, Disconnected", INFO_MSG);
      _endthread();
    }
    
    if((rc == SOCKET_ERROR) || (rc == 0) || !connection_up)
    {
      logoff_xipc(ipc_info->connectiontype); 
      connection_up = FALSE;

      if (rc == SOCKET_ERROR)
        LogEvent("PINNACLE: raw_direct_tcp_read, recv, socket_error", ERROR_MSG);
      else if (rc == 0)
        LogEvent("PINNACLE: raw_direct_tcp_read, recv, connection remotely closed", ERROR_MSG);
      else
        LogEvent("PINNACLE: raw_direct_tcp_read, Disconnected", ERROR_MSG);

     _endthread();
    }

    pfNotify(lpBuf, (WORD)rc, ipc_info);
  }         // while(TCPCONNECTED( host )) 

  LogEvent("PINNACLE: raw_direct_tcp_read, exit loop, receive disconnect", INFO_MSG);
  logoff_xipc(ipc_info->connectiontype); 
  _endthread();
}

//------------------------------------------------------------------------------
BOOLEAN raw_direct_tcp_write(const WORD len, const void * data)
{
  int    rc;

  // Idiot check the send buffer
  if(!data)
  { 
    LogEvent("PINNACLE: raw_direct_tcp_write, bad buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  // Write data to the socket

  rc = send(LISTNER( host ), data, len, NO_FLAGS_SET);

  if(rc == SOCKET_ERROR)
  {
    sprintf( szBuff, "PINNACLE: raw_direct_tcp_write, send/write failed, Error:%d", WSAGetLastError() );
    LogEvent(szBuff, ERROR_MSG);
    return(FALSE);
  }

  return(TRUE);
}

//------------------------------------------------------------------------------
BOOLEAN tcp_init(const USER_TCP_STRUCT *ud, pIPC_INFO ipc_info, BOOLEAN raw_data)
{

  WORD     wVersionRequested;  
  WSADATA  wsaData; 

  INT err, size;
   
  // Idiot check all incoming pointers
  if(!ud)
  {
    LogEvent("PINNACLE: tcp_init, bad tcp structure pointer passed", ERROR_MSG);
    return(FALSE);
  }
  else if(!ud->lpBuf || !ud->pfNotify)
  {
    LogEvent("PINNACLE: tcp_init, bad Function/Buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  // Assign tcp/ip functions to the library primitives
  if ( raw_data )
    port_write = raw_tcp_write;
  else
    port_write = tcp_write;

  port_close = tcp_close;

  // Initialize winsock
  wVersionRequested = MAKEWORD(1, 1); 
  err = WSAStartup(wVersionRequested, &wsaData); 

  // See if winsock started okay
  if(err != 0)
  {
    LogEvent("PINNACLE: tcp_init, winsock.dll bad or missing", ERROR_MSG);
    return(FALSE);
  }

  TCPCONNECTED( conn ) = TRUE;

  // Open the socket
  LISTNER( conn ) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (LISTNER( conn ) == INVALID_SOCKET)
  {
    LogEvent("PINNACLE: tcp_init, socket failed", ERROR_MSG);
    return(FALSE);
  }

  // Bind the socket to a local port number
  size = sizeof(SIN( conn ));
  memset (&SIN( conn ), 0, size); 
  SIN( conn ).sin_family      = AF_INET;
  SIN( conn ).sin_port        = htons(ud->usPort);
  SIN( conn ).sin_addr.s_addr = 0;

  if(bind(LISTNER( conn ), (LPSOCKADDR)&(SIN( conn )), size) == SOCKET_ERROR)
  {
    LogEvent("PINNACLE: tcp_init, bind socket failed", ERROR_MSG);
    CloseSocket(ipc_info->connectiontype);
    return(FALSE);
  }

  // Listen the socket
  if(listen(LISTNER( conn ), MAX_PENDING_CONNECTS) < 0)
  {
    sprintf(szBuff, "PINNACLE: tcp_init, listen failed error:%d", WSAGetLastError());
    LogEvent(szBuff, ERROR_MSG);
    CloseSocket(ipc_info->connectiontype);
    return(FALSE);
  }

  // Populate callback fields
  pfNotify  = ud->pfNotify;
  wBufLen   = ud->wBufferLen;
  lpBuf     = ud->lpBuf;

  init_storage();

  // Start the read thread
  if ( ipc_info->connectiontype == DM_PORTTYPE_RAW_TCP )
    _beginthread(raw_tcp_read, 0, ipc_info);
  else
    _beginthread(tcp_read, 0, ipc_info);

  return(TRUE);
}

//------------------------------------------------------------------------------
// When a network client wants to connect to a server,
// it must have:
// 1.) a TCP port number (via getservbyname())
//    and
// 2.) an IP address of the remote host (gethostbyname()).
//
// The following summarizes the steps used to connect.
// Get the name of the remote host computer in which
// to connect from the setup
// * Check to see if the hosts file knows the computer (gethostbyname)
// * Get the host information (hostent structure filled)
// * Fill in the address of the remote host into the servent structure (memcpy)
// * Get the port number (getservbyname)
// * Fill in the port number of the servent structure
//   Establish a connection (connect)
//------------------------------------------------------------------------------
BOOLEAN direct_tcp_init(const USER_DC_STRUCT *dc, pIPC_INFO ipc_info, BOOLEAN raw_data)
{

  PHOSTENT  phe;      // Host Name
  IN_ADDR   RemoteIpAddress;
  WORD      wVersionRequested;  
  WSADATA   wsaData; 

  int   size, err, i = 1;
  
  // Idiot check all incoming pointers
  if(!dc)
  {
    LogEvent("PINNACLE: direct_tcp_init, bad tcp structure pointer passed", ERROR_MSG);
    return(FALSE);
  }
  else if(!dc->lpBuf|| !dc->pfNotify)
  {
    LogEvent("PINNACLE: direct_tcp_init, bad Function/Buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }
  
  if( ipc_info->connectiontype == DM_PORTTYPE_RAW_DIRECT )
    port_write = raw_direct_tcp_write;
  else
    port_write = direct_tcp_write;

  port_close = tcp_close;

  // Initialize winsock
  wVersionRequested = MAKEWORD(1, 1); 
  err = WSAStartup(wVersionRequested, &wsaData); 

    // See if winsock started okay
  if(err != 0)
  {
    LogEvent("PINNACLE: direct_tcp_init, winsock.dll bad or missing", ERROR_MSG);
    return(FALSE);
  }

  TCPCONNECTED( host ) = TRUE;

  // Open the socket
  LISTNER( host ) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (LISTNER( host ) == INVALID_SOCKET)
  {
    LogEvent("PINNACLE: direct_tcp_init, socket failed", ERROR_MSG);
    return(FALSE);
  }

  size = sizeof(SIN( conn ));
  memset (&SIN( host ), 0, size); 

  // Now do the direct tcp connection to host setup
  // Bind the socket to a local port number
  SIN( host ).sin_family = AF_INET;
  SIN( host ).sin_port = htons(dc->DirectPort);
  
  phe = gethostbyname(dc->HostName);
  if (phe == NULL) 
  { 
    RemoteIpAddress.s_addr = inet_addr (dc->HostName);
    if (RemoteIpAddress.s_addr == -1) 
    {
      sprintf(szBuff, "PINNACLE: direct_tcp_init, RemoteIpAddress = -1, Error: %d", WSAGetLastError());
      LogEvent(szBuff, ERROR_MSG);
      return (FALSE);
    }
    else
    {
      SIN( host ).sin_addr = RemoteIpAddress;
    }
  }
  else
  {
    memcpy(&(SIN(host).sin_addr), phe->h_addr, phe->h_length);
  }
  
  size = sizeof(SIN( host ));

  while(i <= MAX_DC_RETRIES)
  {
    if (connect( LISTNER( host ), (LPSOCKADDR) &(SIN( host )), size) == SOCKET_ERROR) 
    {
      sprintf(szBuff, "PINNACLE: direct_tcp_init, connect failed, Error: %d", WSAGetLastError());
      if (i%5 ) LogEvent(szBuff, ERROR_MSG);
      if (i == MAX_DC_RETRIES) return(FALSE);
      i++;
    }
    else
      break;
  }

  // Start the host thread
  lpBuf     = dc->lpBuf;
  pfNotify  = dc->pfNotify;
  wBufLen   = dc->wBufferLen;

  init_storage();

  if( ipc_info->connectiontype == DM_PORTTYPE_RAW_DIRECT )
    _beginthread(raw_direct_tcp_read, 0, ipc_info);
  else
    _beginthread(direct_tcp_read, 0, ipc_info);

  return(TRUE);
}

