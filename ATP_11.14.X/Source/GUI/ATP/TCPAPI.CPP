/*****************************************************
tcpapi.cpp
*****************************************************/
//#include "stdafx.h"
/*****************************************************
tcpapi.cpp
*****************************************************/
#include "stdafx.h"
extern "C" {
#include "pteipc.h"
pPTE_MSG ptetcp_send_receive ( pPTE_MSG, pCHAR) ;
}
#include "ptetotcp.h"

static char logger_que_name[] = "L_AP_S1";

PteToTcp* pPteToTcpObject;
void InitPteToTcpObject(char*) ;
void DeletePteToTcpObject() ;
/********************************************************************************************/
pPTE_MSG ptetcp_send_receive ( pPTE_MSG p_msg_out, pCHAR que_name )
{
   pPTE_MSG p_msg_in = NULL_PTR;
   BOOLEAN bError = FALSE ;
   char modified_que_name[MAX_QUE_NAME_SIZE] ;
   memset (modified_que_name,0,sizeof(modified_que_name)) ;
   
   if (!strcmp (que_name, logger_que_name)) {
	   strcpy (modified_que_name, "@") ;
	   strcat (modified_que_name, ptemsg_get_msg_dest_queue (p_msg_out)) ;
	   ptemsg_set_dest_queue (p_msg_out, modified_que_name) ;
   }

   if (pPteToTcpObject->SocketActive == 1) {
	   if (pPteToTcpObject->Send(p_msg_out)) { 
		   if (pPteToTcpObject->Receive()) {  // guaranteed to receive the object in whole
			   p_msg_in = pPteToTcpObject->GetResponse() ;
		   }
		   else bError = TRUE ;
	   }
	   else bError = TRUE ;
   }
   else bError = TRUE ;

   if (bError) {
	  //LogEvent( "failed in communicating with Server", WARN_MSG );
      p_msg_in = NULL_PTR;
   }

   //free (p_msg_out);
   
   return (p_msg_in);
}  /* ptetcp_send_recieve */

void InitPteToTcpObject(char* strIPAddress)
{
	pPteToTcpObject = new PteToTcp (strIPAddress) ;
	
}

void DeletePteToTcpObject()
{
	delete pPteToTcpObject ;
}

