/******************************************************************************

       Module: tcp.h

        Title: ASCENDENT Dialog Manager

  Description: This is a modified file of original tcpip.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\tcp.h  $
   
      Rev 1.5   Apr 01 1999 12:22:04   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.4   Oct 08 1998 16:42:52   MSALEH
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

#define WS_VERSION_REQD    0x0101
#define WS_VERSION_MAJOR   HIBYTE(WS_VERSION_REQD)
#define WS_VERSION_MINOR   LOBYTE(WS_VERSION_REQD)
#define MIN_SOCKETS_REQD   16
#define MAX_NUMBER_OF_SOCKETS 512

/* Client Socket, Server Socket */
extern DM_TCP_STRUCT  Client;          
extern DM_TCP_STRUCT  Server;

void    close_a_socket      (SOCKET s);

void    tcp_close           (int connection_type);

void    tcp_read            (pIPC_INFO ipc_info);

BOOLEAN tcp_write           (const WORD len, const void * data, pIPC_INFO ipc_info);

BOOLEAN server_tcp_init     (const USER_TCP_STRUCT *ud, pIPC_INFO ipc_info);

BOOLEAN client_tcp_init     (const USER_TCP_STRUCT *dc, pIPC_INFO ipc_info);
#endif

