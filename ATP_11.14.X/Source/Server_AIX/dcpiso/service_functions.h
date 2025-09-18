#ifndef _SERVICE_FUNCTIONS
#define _SERVICE_FUNCTIONS

#include "basictyp.h"
#include "ptemsg.h" 

CHAR acq_amex_variable_auth_flag;
char last_date[9];

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
INT dcpiso_Check_If_CUP_Transaction (void);
INT dcpiso_check_If_AMEX_Transaction (void);
INT dcpiso_check_If_JCB_Transaction (void);
INT dcpiso_check_If_MC_Transaction (void);
INT dcpiso_check_If_VISA_Transaction (void);
void dcpiso_get_error_warning_file_name_path(void);
INT  dcpiso_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int details);
void dcpiso_create_Error_Filename( void );
void dcpiso_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
/* SCR 22830 - ThoughtFocus */
BOOLEAN IsOriginatorATerminal(pBYTE pos_condition_code);
BYTE Send_partial_reversal_to_host( char *host_que_name);
BYTE Check_Is_Host_Up(BYTE* destination_name);
void dcpiso_generate_tranasction_id( void);
void dcpiso_update_TO_rc_basedon_card_brand(CHAR *resp_code);
INT dcpiso_check_If_Tra_qualify_for_incremental_Preauth(void);
INT dcpiso_check_if_mid_is_smcc ();
#define ORIG_QUEUE  "ORIGQ"
#define ORIG_INFO   "ORIGINFO"
#define MEMACCESSERROR 1


#endif
