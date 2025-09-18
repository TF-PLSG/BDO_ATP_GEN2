#ifndef _PROCESS_OUTGOING_MSG
#define _PROCESS_OUTGOING_MSG

BYTE Process_Response_From_Authorizer (pPTE_MSG p_msg_in) ;
BYTE Send_Transaction_To_Originator (void) ;
BYTE Populate_Response_Fields_In_Auth_Tx (void) ;
BYTE Populate_ISO_Structure_From_Auth_Tx (void) ;
BYTE Populate_ISO_Private63_Field (void) ;
BYTE Populate_ISO_Private62_Field (void) ;
BYTE Send_Response_To_Terminal (void) ;
BYTE Match_Request_To_Response_Transaction_Key (void) ;
BYTE Add_Response_Fields_To_Auth_Tx (BYTE tx_type, pCHAR resp_code) ;
BYTE Match_Request_To_Response_Transaction_Key (void) ;

#endif
