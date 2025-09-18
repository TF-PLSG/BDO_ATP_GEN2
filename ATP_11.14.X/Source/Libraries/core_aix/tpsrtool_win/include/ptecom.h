/******************************************************************************
  
       Module: ptecom.h

        Title: Ascendent Dialog Manager

  Description: Header file includes all data structure definitions
  
  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\ptecom.h  $
   
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
   
   
      Rev 1.1   Mar 25 1998 15:31:00   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types whever possible.
   
   
      Rev 1.0   17 Mar 1998 13:45:08   MSALEH
   Communication Library files - Initial Release

******************************************************************************/

#ifndef PTECOM_H
#define PTECOM_H


#include "common.h"
#include "serial.h"
#include "tcp.h"


#endif
