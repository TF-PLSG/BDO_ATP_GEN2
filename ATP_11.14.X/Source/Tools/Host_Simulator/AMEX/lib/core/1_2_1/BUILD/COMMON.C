/******************************************************************************

       Module: common.c

        Title: Pinnacle Dialog Manager

  Description: This is a modified file of original common.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\COMMON.C  $
   
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
#include "NTUtils.h"


INT      volatile connection_up = false;

INT      volatile trace_on      = false;
BYTE     volatile trace_number  = 0;
INT      volatile trace_lock_on = false;
CHAR     trace_file  [64];
CHAR     trace_queue [MAX_QUE_NAME_SIZE];

CHAR     gui_orig_info[20];

pBYTE    lpBuf;                    // global pointer to main read thread
WORD     wBufLen;

void     (*pfNotify)   (pBYTE, WORD, pIPC_INFO);
BOOLEAN  (*port_write) (const WORD, const void *);
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
   case  DM_PORTTYPE_SERIAL:
    return(serial_init(userdata, ipc_info));
    break;

   case  DM_PORTTYPE_TCP:
   case  DM_PORTTYPE_TCP_NO_TPDU:
    return(tcp_init(userdata, ipc_info, FALSE));
    break;

   case  DM_PORTTYPE_DIRECT:
    return(direct_tcp_init(userdata, ipc_info, FALSE));
    break;

   case  DM_PORTTYPE_RAW_TCP:
    return(tcp_init(userdata, ipc_info, TRUE));
    break;

   case  DM_PORTTYPE_RAW_DIRECT:
    return(direct_tcp_init(userdata, ipc_info, TRUE));
    break;

   default:
     LogEvent("PINNACLE: port_init, unknown port type passed to port init function", ERROR_MSG);
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

/****************************************************
 ****************************************************/
void logoff_xipc(INT connection_type)
{
  switch (connection_type)
  {
  case DM_PORTTYPE_TCP:
  case DM_PORTTYPE_RAW_TCP:
  case DM_PORTTYPE_TCP_NO_TPDU:
    if (LOGGED_IN( conn ))
    { 
      thread_logoff();
      LOGGED_IN( conn ) = 0;
    }
    break;

  case DM_PORTTYPE_DIRECT:
  case DM_PORTTYPE_RAW_DIRECT:
    if (LOGGED_IN( host ))
    { 
      thread_logoff();
      LOGGED_IN( host ) = 0;
    }
    break;

  case DM_PORTTYPE_SERIAL:
    if (LOGGED_IN( com ))
    { 
      thread_logoff();
      LOGGED_IN( com ) = 0;
    }
    break;
  }
}

/****************************************************
 ****************************************************/
void login_xipc(INT connection_type, pCHAR instancename, pCHAR loginname)
{

  switch (connection_type)
  {
  case DM_PORTTYPE_TCP:
  case DM_PORTTYPE_RAW_TCP:
  case DM_PORTTYPE_TCP_NO_TPDU:
    if (!LOGGED_IN( conn ))
    {
        if (thread_logon(instancename, loginname))
        LOGGED_IN( conn ) = 1;
    }
    break;

  case DM_PORTTYPE_DIRECT:
  case DM_PORTTYPE_RAW_DIRECT:
    if (!LOGGED_IN( host ))
    {
        if (thread_logon(instancename, loginname))
        LOGGED_IN( host ) = 1;
    }
    break;

  case DM_PORTTYPE_SERIAL:
    if (!LOGGED_IN( com ))
    {
        if (thread_logon(instancename, loginname))
        LOGGED_IN( com ) = 1;
    }
    break;
  }
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

