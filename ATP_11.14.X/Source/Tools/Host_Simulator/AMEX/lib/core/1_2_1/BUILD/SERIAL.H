/******************************************************************************
  
       Module: serial.h

        Title: Pinnacle Dialog Manager

  Description: This is a modified file of original serial.c.
               Error logging to DB is replaced by logging to NT's
               event log
  
  $Log:   N:\PVCS\PTE\Comm Lib\serial.h  $
   
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

BOOLEAN serial_write(const WORD len, const void * lpData);

BOOLEAN serial_init(const USER_SERIAL_STRUCT * ud, pIPC_INFO ipc_info);


#endif

