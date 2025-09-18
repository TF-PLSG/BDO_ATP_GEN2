/******************************************************************************
  
        Module: convert.h
  
         Title: Common Comms API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\convert.h  $
   
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

#include "common.h"
#include "basictyp.h"
#include "ptemsg.h"


int  log_message (pCHAR str);

void pteipc_send_with_check (pPTE_MSG p_msg, pCHAR que_name, pCHAR loginname);

void send_trace_to_gui (pBYTE tempbuf, BYTE msg_type, pCHAR ascii_tpdu, pCHAR loginname);

void write_to_dialog_trace_file (pBYTE bufptr, INT len,INT outgoing);

void PrintBuffer (pBYTE tmpbuf, INT len, INT outgoing, pCHAR ascii_tpdu, pCHAR loginname);

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info );

void dtcp_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info );

BOOLEAN serial_convert(INT connection_type, pCHAR pComPort, 
                       pCHAR pBaud        , pCHAR pByte,  
                       pCHAR pParity      , pCHAR pStop, 
                       pCHAR pBuffer);

BOOLEAN tcp_convert(BYTE connection_type, WORD tcp_port, pCHAR pBuffer);

BOOLEAN direct_tcp_convert(WORD  host_tcp_port,
                           pCHAR host_name, 
                           pCHAR pBuffer);

BOOLEAN raw_tcp_convert(WORD tcp_port, pCHAR pBuffer);

BOOLEAN raw_direct_tcp_convert(WORD  host_tcp_port,
                               pCHAR host_name, 
                               pCHAR pBuffer);

#endif

