/******************************************************************************
  
       Module: serial.h

        Title: ASCENDENT Dialog Manager

  Description: This is a modified file of original serial.c.
               Error logging to DB is replaced by logging to NT's
               event log
  
  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\serial.h  $
   
      Rev 1.2   Apr 01 1999 12:22:04   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.1   Oct 08 1998 16:42:52   MSALEH
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
   
   
      Rev 1.0   Mar 25 1998 15:31:10   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types whever possible.
   

******************************************************************************/

#ifndef SERIAL_H
#define SERIAL_H

#include "common.h"


extern DM_SERIAL_STRUCT  com;


BOOLEAN serial_get_byte(pBYTE ch, pDWORD len);

BOOLEAN serial_put_bytes(pBYTE lpBuf, const DWORD dwLen);

void serial_read(pIPC_INFO ipc_info);

void CloseAllHandles();

void serial_close(int dummy);

BOOLEAN serial_write(const WORD len, const void * lpData, pIPC_INFO ipc_info);

BOOLEAN serial_init(const USER_SERIAL_STRUCT * ud, pIPC_INFO ipc_info);


#endif

