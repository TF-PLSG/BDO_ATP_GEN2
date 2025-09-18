#ifndef _DATABASE_RESPONSE
#define _DATABASE_RESPONSE

#include "basictyp.h"
#include "ptemsg.h" 

BYTE Process_Dataserver_OK_Message (pPTE_MSG p_msg_in) ;
BYTE Process_Dataserver_ERROR_Message (pPTE_MSG p_msg_in) ;
BYTE Process_Dataserver_NotFound_Message (pPTE_MSG p_msg_in) ;
BYTE Process_select_bin01_db_OK (pBYTE pCurrent) ;
BYTE Process_bin01_db_not_found	(pBYTE pCurrent) ;

#endif
