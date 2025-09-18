#ifndef _SERVICE_FUNCTIONS
#define _SERVICE_FUNCTIONS

#include "basictyp.h"
#include "ptemsg.h" 

BYTE Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   BYTE general_status, BYTE* response_code) ;
BYTE Check_Transaction_Tx_Key (void) ;
BYTE Check_If_Transaction_Has_Valid_Response_Code (void) ;
BYTE Perform_Auth_Tx_Checks (void) ;
void Prepare_AuthTx_For_Forwarding (BYTE general_status, BYTE* strResponseCode) ;
void Get_Bin_Type_From_Processing_Code (BYTE* pnAccountType) ;
BYTE If_Transaction_Cannot_Be_Declined (void) ;
BYTE Send_Transaction_To_Authorizer (void) ;
BYTE If_Transaction_Is_A_Advice (void) ;
BYTE Verify_Field_Length (char field[], char *field_desc) ;
BYTE IsFieldLengthGreaterThanDatabaseLength (unsigned char field[], unsigned int field_datastructure_length) ;
BYTE PopulateAuthTxDateField (void) ;
BYTE PopulateAuthTxTimeField (void) ;
BYTE Get_Bin_Type( pBYTE proc_code );
BYTE txn_is_financial( void );
void Map_Response_Code( pBYTE resp_code );
BYTE send_0302_to_network_controller( pPTE_MSG p_msg );
void process_0312( pPTE_MSG p_msg_in );

/* SCR 22830 - ThoughtFocus */
BOOLEAN IsOriginatorATerminal(pBYTE pos_condition_code);

#define ORIG_QUEUE  "ORIGQ"
#define ORIG_INFO   "ORIGINFO"

#endif