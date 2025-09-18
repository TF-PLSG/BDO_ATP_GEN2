/******************************************************************************
  
        Module: notify.c
  
         Title: dialog manager API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ARCHLNK\NOTIFY.C  $
   
      Rev 1.2   Oct 22 1999 11:53:58   MSALEH
   Priority based message retrieval
   
      Rev 1.1   Aug 26 1999 09:36:04   MSALEH
   modify to be more efficient
   
      Rev 1.0   Apr 13 1999 14:03:56   MSALEH
   Modify to use the ptecom.lib functions
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptetime.h"
#include "ntutils.h"

#define ROUTE_VIA_LOGGER '@'

extern  INT system_message_priority; 

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info )
{    
  LONG      ret_code;
  pPTE_MSG  p_msg_out;
  CHAR      dest_que[MAX_QUE_NAME_SIZE];
  CHAR      org_info[60];
  CHAR      Buffer[256], ErrorMsg[100];
  
  sprintf(org_info, "%s,%0ld", application_que_name, ipc_info->socket_id);
  p_msg_out = (pPTE_MSG)tmpbuf;

  strcpy( dest_que, ptemsg_get_msg_dest_queue( p_msg_out ) );

  ptemsg_set_orig_comms_info  (p_msg_out, org_info);
  ptemsg_set_orig_queue       (p_msg_out, application_que_name);

  ret_code = pteipc_send_hp( p_msg_out, dest_que, system_message_priority );

  if( ret_code < 0 )
  {
     pteipc_get_errormsg( ret_code, ErrorMsg );
     sprintf( Buffer, "ASCENDENT: pteipc_send, error at sending to que %s: %s", dest_que, ErrorMsg );
     LogEvent( Buffer, ERROR_MSG );
  }
}
