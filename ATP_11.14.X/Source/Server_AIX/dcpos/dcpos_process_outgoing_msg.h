#ifndef _PROCESS_OUTGOING_MSG
#define _PROCESS_OUTGOING_MSG

BYTE Send_Transaction_To_Originator (void) ;
BYTE Populate_Response_Fields_In_Auth_Tx (void) ;
BYTE Populate_ISO_Structure_From_Auth_Tx (void) ;
BYTE Send_Response_To_Terminal (void) ;

#endif
