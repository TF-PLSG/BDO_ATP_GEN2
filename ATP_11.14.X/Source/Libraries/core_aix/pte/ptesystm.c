/******************************************************************************
  
        Module: ptesystm.c
  
         Title: Pinnacle System Message Handler
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTESYSTM.C  $
   
      Rev 1.4   28 Jul 1998 09:49:58   rmalathk
   Modified all system response handlers to 
   send back the orig_comms_info field in the
   response message.
   
      Rev 1.3   Feb 13 1998 08:23:40   drodden
   Getting compile errors for undefined message subtype.
   Changed the IPC trace response to use the subtypes 
   from the request.
   
   
      Rev 1.2   12 Feb 1998 12:56:44   muysal
   - Modified "pte system msg handler" to recognize
   "trace on" and "trace off" message types which
   are sent from the "Services Monitor".
   - Added a new function which replies back to the
   "Services Monitor" as confirmation that 
   the message is received.
   
   
   
      Rev 1.1   Dec 01 1997 09:35:06   drodden
   Changed the terminate message processing to call EndProcess
   rather than exit_application.
   Added the IPC trace on/off system message types.
   
   
      Rev 1.0   Oct 23 1997 11:42:30   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#include <stdlib.h>

#include "ptemsg.h"
#include "pteipc.h"
#include "ptesystm.h"



extern void EndProcess ();



/*****************************************************************************
*
*
*
*
*
*
*
******************************************************************************/
void send_ping_response ( pPTE_MSG p_msg_in )
{
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   pPTE_MSG      p_msg_out;
   pCHAR         p_orig_info;


   p_orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );
   p_msg_data = ptemsg_get_pte_msg_data      (p_msg_in);
   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);

   p_msg_out = ptemsg_build_msg (MT_SYSTEM_REPLY,
                                 ST1_SYS_PING,
                                 ptemsg_get_msg_subtype2   (p_msg_in),
                                 ptemsg_get_msg_orig_queue (p_msg_in),
                                 ptemsg_get_msg_dest_queue (p_msg_in),
                                 ptemsg_get_pte_msg_data_data          (p_msg_data),
                                 ptemsg_get_pte_msg_data_data_length   (p_msg_data),
                                 ptemsg_get_pte_msg_data_app_data_type (p_msg_data) );

   if (p_msg_out != NULL_PTR)
   {
      ptemsg_set_result_code (p_msg_out, PTEMSG_OK);
      ptemsg_set_orig_comms_info( p_msg_out, p_orig_info );
      pteipc_send (p_msg_out, ptemsg_get_msg_orig_queue (p_msg_in));
      free (p_msg_out);
   }
}

/*****************************************************************************
*
*
*
*
*
*
*
******************************************************************************/
void send_trace_response ( pPTE_MSG p_msg_in )
{
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   pPTE_MSG      p_msg_out;
   pCHAR         p_orig_info;

   p_orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );
   p_msg_data = ptemsg_get_pte_msg_data      (p_msg_in);
   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);

   p_msg_out = ptemsg_build_msg (MT_SYSTEM_REPLY,
                                 ptemsg_get_msg_subtype1   (p_msg_in),
                                 ptemsg_get_msg_subtype2   (p_msg_in),
                                 ptemsg_get_msg_orig_queue (p_msg_in),
                                 ptemsg_get_msg_dest_queue (p_msg_in),
                                 ptemsg_get_pte_msg_data_data          (p_msg_data),
                                 ptemsg_get_pte_msg_data_data_length   (p_msg_data),
                                 ptemsg_get_pte_msg_data_app_data_type (p_msg_data) );

   if (p_msg_out != NULL_PTR)
   {
      ptemsg_set_result_code (p_msg_out, PTEMSG_OK);
      ptemsg_set_orig_comms_info( p_msg_out, p_orig_info );

      pteipc_send (p_msg_out, ptemsg_get_msg_orig_queue (p_msg_in));
      free (p_msg_out);
   }
}


/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
void terminate_the_app ( pPTE_MSG p_msg_in )
{
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   pPTE_MSG      p_msg_out;
   pCHAR         p_orig_info;


   p_orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );
   p_msg_data = ptemsg_get_pte_msg_data      (p_msg_in);
   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);

   p_msg_out = ptemsg_build_msg (MT_SYSTEM_REPLY,
                                 ST1_SYS_TERMINATE,
                                 ptemsg_get_msg_subtype2   (p_msg_in),
                                 ptemsg_get_msg_orig_queue (p_msg_in),
                                 ptemsg_get_msg_dest_queue (p_msg_in),
                                 ptemsg_get_pte_msg_data_data          (p_msg_data),
                                 ptemsg_get_pte_msg_data_data_length   (p_msg_data),
                                 ptemsg_get_pte_msg_data_app_data_type (p_msg_data) );

   if (p_msg_out != NULL_PTR)
   {
      ptemsg_set_result_code (p_msg_out, PTEMSG_OK);
      ptemsg_set_orig_comms_info( p_msg_out, p_orig_info );

      pteipc_send (p_msg_out, ptemsg_get_msg_orig_queue (p_msg_in));
      free (p_msg_out);
   }

   free (p_msg_in);     /* free before we exit; no other chance after this */

   EndProcess ();
}



/*****************************************************************************
*
*
*
*
*
*
*
******************************************************************************/
void ptesystm_system_msg_handler ( pPTE_MSG p_msg_in )
{
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_SYS_PING         : send_ping_response (p_msg_in);
                                  break;

      case ST1_SYS_TERMINATE    : terminate_the_app  (p_msg_in);
                                  break;

      case ST1_SYS_IPC_TRACE_ON : send_trace_response (p_msg_in);
                                  pteipc_trace_on     ();
                                  break;

      case ST1_SYS_IPC_TRACE_OFF: pteipc_trace_off    ();
                                  send_trace_response (p_msg_in);
                                  break;
      default:
         break;
   }

}  /* procedure system_msg_handler */


void send_nullcheck_dbstatics_response ( pPTE_MSG p_msg_in )
{
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   pPTE_MSG      p_msg_out;
	   pCHAR         p_orig_info;

	   p_orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );
	   p_msg_data = ptemsg_get_pte_msg_data      (p_msg_in);
	   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);

	   p_msg_out = ptemsg_build_msg (MT_SYSTEM_REPLY,
			   	   	   	   	   	   	 ptemsg_get_msg_subtype1   (p_msg_in),
	                                 ptemsg_get_msg_subtype2   (p_msg_in),
	                                 ptemsg_get_msg_orig_queue (p_msg_in),
	                                 ptemsg_get_msg_dest_queue (p_msg_in),
	                                 ptemsg_get_pte_msg_data_data          (p_msg_data),
	                                 ptemsg_get_pte_msg_data_data_length   (p_msg_data),
	                                 ptemsg_get_pte_msg_data_app_data_type (p_msg_data) );

	   if (p_msg_out != NULL_PTR)
	   {
	      ptemsg_set_result_code (p_msg_out, PTEMSG_OK);
	      ptemsg_set_orig_comms_info( p_msg_out, p_orig_info );

	      pteipc_send (p_msg_out, ptemsg_get_msg_orig_queue (p_msg_in));
	      free (p_msg_out);
	   }
}
