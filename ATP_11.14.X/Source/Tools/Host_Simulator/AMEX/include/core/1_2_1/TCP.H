/******************************************************************************

       Module: tcp.h

        Title: Pinnacle Dialog Manager

  Description: This is a modified file of original tcpip.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\tcp.h  $
   
      Rev 1.3   Sep 11 1998 11:29:06   drodden
   Changed to use variable that defines the size of the header on the front 
   of our TCP messages.  The header may need to be larger than just the 
   two length bytes.  This variable is a default 2, but can be set with a new
   function set_tcp_header_size.
   
   
      Rev 1.2   Aug 26 1998 09:42:02   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.1   27 Mar 1998 16:07:44   MSALEH
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
   
      Rev 1.0   Mar 25 1998 15:31:14   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types whever possible.
   
   
******************************************************************************/

#ifndef TCP_H
#define TCP_H

#include "common.h"


/* Direct Connect, Client Socket, Server Socket */
extern DM_TCP_STRUCT  host;          
extern USER_DC_STRUCT dc;
extern DM_TCP_STRUCT  conn;


void    set_tcp_header_size ( BYTE hdr_size );

void    init_storage();

void    build_msg_and_foreward(pIPC_INFO ipc_info);

void    process_new_msg     (INT rc, INT i, pIPC_INFO ipc_info);

void    close_a_socket      (SOCKET s);

void    CloseSocket         (int connection_type);

void    tcp_close           (int connection_type);

void    tcp_read            (pIPC_INFO ipc_info);

BOOLEAN tcp_write           (const WORD len, const void * data);

void    direct_tcp_read     (pIPC_INFO ipc_info);

BOOLEAN direct_tcp_write    (const WORD len, const void * data);

void    raw_tcp_read        (pIPC_INFO ipc_info);

BOOLEAN raw_tcp_write       (const WORD len, const void * data);

void    raw_direct_tcp_read (pIPC_INFO ipc_info);

BOOLEAN raw_direct_tcp_write(const WORD len, const void * data);

BOOLEAN tcp_init            (const USER_TCP_STRUCT *ud, pIPC_INFO ipc_info, BOOLEAN raw_data);

BOOLEAN direct_tcp_init     (const USER_DC_STRUCT *dc, pIPC_INFO ipc_info, BOOLEAN raw_data);


#endif

