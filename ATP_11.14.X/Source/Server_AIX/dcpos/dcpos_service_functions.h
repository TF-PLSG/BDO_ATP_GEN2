#ifndef _SERVICE_FUNCTIONS
#define _SERVICE_FUNCTIONS

#include "basictyp.h"
#include "ptemsg.h" 

#define ORIG_QUEUE  "ORIGQ"
#define ORIG_INFO   "ORIGINFO"

BYTE 	Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   	   BYTE general_status, BYTE* response_code) ;
BYTE 	Perform_Auth_Tx_Checks (void) ;
void 	Prepare_AuthTx_For_Forwarding (BYTE general_status, BYTE* strResponseCode) ;
BYTE 	Verify_Field_Length (char field[], char *field_desc) ;
BYTE 	IsFieldLengthGreaterThanDatabaseLength (unsigned char field[], unsigned int field_datastructure_length) ;
BYTE 	PopulateAuthTxDateField (void) ;
BYTE 	PopulateAuthTxTimeField (void) ;
INT 	dcpos_Check_If_CUP_Transaction (void);
BOOLEAN Is_EMV_Transaction(void);
void 	Prepare_TPOS01_details(void);
void 	create_stan_dcpos( pBYTE stan );
void 	dcpos_get_error_warning_file_name_path(void);
INT  	dcpos_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int details);
void 	dcpos_create_Error_Filename( void );
void 	dcpos_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT 	dcpos_check_If_preauth_void_transaction (void);

#endif
