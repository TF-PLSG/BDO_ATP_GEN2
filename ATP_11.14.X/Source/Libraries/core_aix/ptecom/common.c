/******************************************************************************

       Module: common.c

        Title: ASCENDENT Dialog Manager

  Description: This is a modified file of original common.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\COMMON.C  $
   
      Rev 1.13   Nov 05 1999 16:40:14   MSALEH
   * Correct bug with sleep between accepts
   * Add Keep alive support to tcp.c
   
      Rev 1.12   Sep 29 1999 14:20:24   MSALEH
   AIX Modifications
   
      Rev 1.11   Apr 14 1999 15:23:06   MSALEH
   Mods to handle new IGNORE option
   
      Rev 1.10   Apr 01 1999 12:21:50   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.9   Mar 01 1999 17:23:32   MSALEH
   TCP: added functionality to handle inclusive
   and exclusive length bytes, if the tcp_header_length >= 100
   then inclusive length is assumed.
   Serial: For length 0, don't send an LRC, this
   is a racal thing only
   
      Rev 1.8   Oct 08 1998 16:42:48   MSALEH
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
   
   
      Rev 1.7   Aug 26 1998 09:41:56   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.6   19 Aug 1998 14:58:42   rmalathk
   1. modified send_trace_to_gui function to
       send trace messages from a service to
       GUI via applink.
   2. The tpdu field is no longer sent in the 
       orig_comms_info field to GUI. It is sent
       as a second piece of data chained to the
       trace.
   
   
      Rev 1.5   05 Jun 1998 10:36:02   MSALEH
   corrected bug in ascii_to_bin
   was not handling A-F properly
   
      Rev 1.4   27 Mar 1998 16:07:36   MSALEH
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
   
      Rev 1.3   Mar 25 1998 15:30:58   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types whever possible.
   
   
      Rev 1.2   Mar 25 1998 14:45:14   drodden
   Created a common communication library (ptecom).
   
   
      Rev 1.1   Jan 29 1998 09:43:56   JXIAO
   Make necessary changes to make dialog manager
   working with AppLnk


      Rev 1.0   Dec 23 1997 15:04:52   JXIAO
   Initial version of Dialog Manager NT Service version.
   It is a converting of Passport Dialog Manager. All functionalities
   are kept, all GUI are removed.

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "serial.h"
#include "tcp.h"
#include "pteipc.h"
#include "ntutils.h"


INT      volatile connection_up  = FALSE;

INT      volatile trace_on       = FALSE;
BYTE     volatile trace_number   = 0;
INT      volatile trace_lock_on  = FALSE;
BYTE     volatile send_keepalive = FALSE;

CHAR     trace_file  [64];
CHAR     trace_queue [MAX_QUE_NAME_SIZE];

CHAR     gui_orig_info[20];

void     (*pfNotify)   (pBYTE, WORD, pIPC_INFO);
BOOLEAN  (*port_write) (const WORD, const void *, pIPC_INFO);
void     (*port_close) (INT);

/* Convert a two byte BCD value to an integer value.
   bp - pointer to first byte (MSB) of 2 byte area. */
WORD bcd2bin(const pBYTE lpBcd)
{      
   WORD wTot;

   wTot = (lpBcd[0] / 0x10) * 1000  +
         (lpBcd[0] % 0x10) *  100  +
         (lpBcd[1] / 0x10) *   10  +
         (lpBcd[1] % 0x10);

   return(wTot);
}


/* Convert an unsigned number to a two byte bcd value
   bp  - pointer to first byte (MSB) of 2 byte area
   INT - unsigned integer value to be translated */
void bin2bcd(pBYTE lpBcd, const WORD wInt)
{
  WORD wJ;

  wJ = wInt / 100;
  lpBcd[0] = (BYTE)((wJ/10)*0x10 + (wJ%10));
  wJ = wInt % 100;
  lpBcd[1] = (BYTE)((wJ/10)*0x10 + (wJ%10));
}

/****************************************************
 ****************************************************/
BOOLEAN port_init(BYTE type, const void * userdata, pIPC_INFO ipc_info)
{
  switch(type)
  {
   case  DM_PORTTYPE_SERIAL_WITH_TPDU:
   case  DM_PORTTYPE_SERIAL_WITHOUT_TPDU:
   case  DM_PORTTYPE_SERIAL_IGNORE_TPDU:
#ifdef WIN32
    return(serial_init(userdata, ipc_info));
#endif
    break;

   case  DM_PORTTYPE_TCP_SERVER_WITH_TPDU:
   case  DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU:
   case  DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU:
    return(server_tcp_init(userdata, ipc_info));
    break;

   case  DM_PORTTYPE_TCP_CLIENT_WITH_TPDU:
   case  DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU:
   case  DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU:
    return(client_tcp_init(userdata, ipc_info));
    break;

   default:
    LogEvent("ASCENDENT: port_init, unknown port type passed to port init function", ERROR_MSG);
    return(FALSE);
    break;
  }

  return(TRUE);
}

/****************************************************
 ****************************************************/
INT thread_logon(pCHAR instancename, pCHAR loginname)
{
  return pteipc_xipclogin(instancename, loginname);
}

/****************************************************
 ****************************************************/
void thread_logoff()
{
  pteipc_xipclogout();
}

/***********************************************************
 Save the binary TPDU as ASCII data
 ***********************************************************/
void bin_to_ascii (pCHAR src, pCHAR dst, INT len)
{
   INT  i, j;

   for (i = 0; i < (len*2); i+=2)
   {
       j = (src[i/2] < 0) ? (src[i/2] + 256) : src[i/2];

      if ((j / 16) <= 9) 
        dst[i] = (j / 16) + '0';
      else
        dst[i] = (j/16 - 0x0a)+'A';

      if ((j & 0x0f) <= 9)
        dst[i+1] = (j & 0x0F) + '0'; 
      else
        dst[i+1] = (j & 0x0f) - 0x0a + 'A';    
   }
} 

/************************************************************
 Retrieve the ASCII hex TPDU and convert it to binary data
 ************************************************************/
void ascii_to_bin (pBYTE src, pBYTE dst, INT len)
{
   INT  i, j, k;

   for (i = 0; i < (len*2); i+=2)
   {
      /* if I hit a null terminator, quit.  */
      if (src[i] == '\0')
         break;

      /* if these are leading blanks, leave the bcd zero.  */
      if (src[i] != ' ')
      {
         if (src[i] <= '9')
           dst[i/2] = ((src[i]-'0')*16); 
         else
           dst[i/2] = ((src[i]-'A')+0x0A)*16;   /* handle 0a-0f */
      }

      if (((i+1) < (len*2)) && (src[i+1] != ' '))
      {
         if (src[i+1] <= '9')
           dst[i/2] += (src[i+1]-'0'); 
         else
         {
           j         = (toupper(src[i+1]) - 'A');
           k         = j + 0x0A;
           dst[i/2] += k;

           /* dst[i/2] += ((src[i+1]-'A')+0x0A);   */
         }
      }
   }
} /* ascii_to_bin */

