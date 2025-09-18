/******************************************************************************
  
        Module: notify.h
  
         Title: Common Comms API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\notify.h  $
   
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


void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info );

void dtcp_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info );

#endif

