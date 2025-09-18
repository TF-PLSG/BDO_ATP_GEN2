/******************************************************************************
  
        Module: notify.h
  
         Title: Common Comms API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\notify.h  $
   
      Rev 1.4   Oct 15 1999 10:17:46   MSALEH
   Modify Project settings for SSL
   
      Rev 1.3   Oct 14 1999 08:12:46   MSALEH
   integrate SSL modifications
   
      Rev 1.2   Oct 08 1998 16:42:50   MSALEH
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
   
   
      Rev 1.1   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.0   27 Mar 1998 16:07:50   MSALEH
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

#ifndef NOTIFY_H
#define NOTIFY_H

#include "basictyp.h"
#include "ptemsg.h"

#ifdef SSL_SUPPORT

#include "e_os.h"
#include "sslc.h"

void cm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info );

#endif

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info );

#endif

