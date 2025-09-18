#ifndef _PROCESS_INCOMING_MSG
#define  _PROCESS_INCOMING_MSG

#include "basictyp.h"

BYTE Process_Incoming_Message_From_Terminal (pPTE_MSG p_msg_in) ;
BYTE Copy_Message_Data_To_Global_Buffer (pCHAR indata, LONG msglen) ;
BYTE Copy_Message_Fields_From_Global_Structure_To_Auth_Tx (void) ;
BYTE Get_Transaction_Type_From_Global_Structure (void) ;
BYTE Populate_Sections_Of_Auth_Tx (BYTE tx_type, pPTE_MSG p_msg_in) ;
BYTE Process_Terminal_Statistics_Transaction (BYTE) ;
BYTE Generate_Transaction_Date_In_YYYYMMDD_format (BYTE* strTranDate, 
												   BYTE* datestr1) ;
#endif