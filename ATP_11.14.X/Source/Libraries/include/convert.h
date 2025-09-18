/******************************************************************************
  
        Module: convert.h
  
         Title: Common Comms API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\convert.h  $
   
      Rev 1.6   Oct 15 1999 10:17:46   MSALEH
   Modify Project settings for SSL
   
      Rev 1.5   Oct 14 1999 08:12:46   MSALEH
   integrate SSL modifications
   
      Rev 1.4   Apr 01 1999 12:22:00   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.3   Oct 08 1998 16:42:50   MSALEH
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
   
   
      Rev 1.2   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.1   27 Mar 1998 16:07:42   MSALEH
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
   
      Rev 1.0   Mar 26 1998 08:45:06   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types wherever possible.
   

******************************************************************************/

#ifndef CONVERT_H
#define CONVERT_H

#include "basictyp.h"
#include "common.h"
#include "ptemsg.h"

int  log_message (pCHAR str);

void pteipc_send_with_check (pPTE_MSG p_msg, pCHAR que_name, pCHAR loginname);

void send_trace_to_gui (pBYTE tempbuf, BYTE msg_type, pCHAR ascii_tpdu, pCHAR loginname);

void write_to_dialog_trace_file (pBYTE bufptr, INT len,INT outgoing);

void PrintBuffer (pBYTE tmpbuf, INT len, INT outgoing, pCHAR ascii_tpdu, pCHAR loginname);

#ifdef SSL_SUPPORT

BOOLEAN ssl_convert( BYTE connection_type          , WORD tcp_port, 
                     LOCAL_CM_CONFIG cm_config     , pCHAR host_name );
#endif

BOOLEAN serial_convert(BYTE connection_type  , pCHAR pComPort, 
                       pCHAR pBaud           , pCHAR pByte,  
                       pCHAR pParity         , pCHAR pStop, 
                       INT   headerlength);

BOOLEAN tcp_convert(BYTE connection_type  , WORD tcp_port, 
                    INT header_length     , pCHAR host_name );
#endif

