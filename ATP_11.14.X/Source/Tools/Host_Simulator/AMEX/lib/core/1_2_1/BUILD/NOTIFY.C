/******************************************************************************
  
        Module: notify.c
  
         Title: dialog manager API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptecom_lib\notify.c  $
   
      Rev 1.4   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.3   Apr 27 1998 09:48:08   drodden
   Moved all of the common system wide defines and typedefs out of the application
   headers (app_info) into the core headers (pte, coreinfo).
   This is part of the creation of the Core Dataserver.
   
   
      Rev 1.2   08 Apr 1998 14:40:52   MSALEH
   dtcp_notify(), switch on the application_que_name
   variable to find where to route the incoming host 
   response. so the router table will have to have an 
   entry with the lo/hi parameters set to the host connection
   queue name
   
      Rev 1.1   Apr 07 1998 08:27:28   drodden
   Removed the dialogg header.  The defines were move to pte_h and it is no longer used.
   
   
      Rev 1.0   27 Mar 1998 16:07:48   MSALEH
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

#include <stdlib.h>
#include <stdio.h>

#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
#include "NTUtils.h"

extern INT get_queue_name  (pCHAR str, pCHAR dest_que);
extern INT get_tpdu_length ();

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info )
{    
  WORD      x;
  INT       found_destination;
  char      ascii_tpdu[60] = {"^"};
  char      dest_que[17];
  pPTE_MSG  p_msg_out;
  
  /*  
    Put in indicator for a terminal transaction, on the way back, 
    if the indicator is found, then no TPDU search is done, 
    the length specified by tpdu_size() entry in the TPDU table

    IMPORTANT: 
    if the connection type is DM_PORTTYPE_TCP_NO_TPDU, then there
    is no TPDU in front of the incoming message, if that
    is the case, the org_info field will be nulls, and that is what is 
    going to be returned to us on an outgoing message.
  */

  if (ipc_info->connectiontype != DM_PORTTYPE_TCP_NO_TPDU)
  {  
    x = get_tpdu_length();
    bin_to_ascii((pBYTE)tmpbuf, (pBYTE)(ascii_tpdu + 1), x);
    found_destination = get_queue_name(ascii_tpdu, dest_que);
  }
  else
  {
    memset (ascii_tpdu, 0x00, sizeof(ascii_tpdu));
    found_destination = get_queue_name(application_que_name, dest_que);
    x = 0;
  }


  if (found_destination)
  {
    p_msg_out = ptemsg_build_msg( MT_INCOMING,
                                  0,
                                  trace_number,
                                  dest_que,
                                  application_que_name,
                                  tmpbuf + x,
                                  (int)len - x,
                                  0);
    if (p_msg_out != NULL_PTR)
    {
      ptemsg_set_orig_comms_info(p_msg_out, ascii_tpdu);
      pteipc_send_with_check(p_msg_out, dest_que, "dialog");
      free(p_msg_out);

      if (trace_on)
      {
        while (trace_lock_on);
        trace_lock_on = TRUE;

        if (ipc_info->connectiontype != DM_PORTTYPE_TCP_NO_TPDU)
         PrintBuffer(tmpbuf, (int)len, false, ascii_tpdu + 1, "dialog");
        else
         PrintBuffer(tmpbuf, (int)len, false, "DIRECT", "dialog");

        trace_lock_on = FALSE;
      }
    }
    else 
    {
      LogEvent("PINNACLE: dm_notify, out of memory when creating new message", ERROR_MSG);
    }
  } 
  else
  {
    LogEvent("PINNACLE: dm_notify, invalid destination queue", ERROR_MSG);
  }
}

//***********************************************************
//***********************************************************
void dtcp_notify ( unsigned char *tmpbuf, WORD len, pIPC_INFO ipc_info )
{    
  char      dest_queue[20];
  pPTE_MSG  p_msg_out;
  
  if (get_queue_name(application_que_name, dest_queue))
  {
    p_msg_out = ptemsg_build_msg(MT_INCOMING,
                                 0,
                                 0,
                                 dest_queue,
                                 application_que_name,
                                 tmpbuf,
                                 len,
                                 0);
    if (p_msg_out != NULL_PTR)
    {
      ptemsg_set_orig_comms_info(p_msg_out, DIRECT_INDICATOR);
      pteipc_send(p_msg_out, dest_queue);
      free(p_msg_out);

      if (trace_on) 
      {
        while (trace_lock_on);
        trace_lock_on = TRUE;
        PrintBuffer(tmpbuf, len, false, DIRECT_INDICATOR, "hstconG");
        trace_lock_on = FALSE;
      }
    }
    else 
    {
      LogEvent("PINNACLE: dtcp_notify, out of memory when creating new message", ERROR_MSG);
    }
  }
  else
  {
    LogEvent("PINNACLE: dtcp_notify, invalid destination queue", ERROR_MSG);
  }
}

