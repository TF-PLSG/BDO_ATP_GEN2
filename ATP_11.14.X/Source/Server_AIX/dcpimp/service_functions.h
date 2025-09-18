#ifndef _SERVICE_FUNCTIONS
#define _SERVICE_FUNCTIONS

#include "basictyp.h"
#include "ptemsg.h" 

#define NO_AUTH_ERROR 0
#define MAX_DATASERVER_BUFFER_LENGTH 25000

#define PESO 1
#define DOLLAR 2


typedef struct 
{
	long peso_julian_date ;
	long dollar_julian_date ;

	double daily_amt_usage ;

	int failed_pin_attempts ;

	struct
	{
		int nbr_usage ;
		double amt_usage ;	// 8 bytes // 15 digits
	} peso_usage[14] ;

	struct
	{
		int nbr_usage ;
		double amt_usage ;	// 8 bytes // 15 digits
	} dollar_usage[14] ;

	struct
	{
		BYTE category_code[5] ;
		int  usage ;
	} mcc[20] ;

	struct
	{
		BYTE merchant_id[16] ;
		int nbr_uses ;
	} repeat [20] ;


} PRIVATE_CCF03P_STRUCT ;


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
void returnNumMP(pCHAR input);

BYTE Update_Private_Ccf03p_Structure ();
BYTE Populate_Usage_Bucket (int context);
BYTE Populate_Merchant_Usage_Bucket (void);
BYTE If_Transaction_Occurred_On_Same_Day (void);
BYTE Validate_Expiry_Date (BYTE* strExpiryDate);
BYTE Update_CCF03_Structure( INT update_date );
void  Init_CCF03_Structure (void );


void dcpimp_Prepare_Pos_Data(void);
void dcpimp_prepare_posdata_for_Amex(void );
void dcpimp_prepare_posdata_for_CUP(void);
void dcpimp_prepare_posdata_for_VISA(void);
void dcpimp_prepare_posdata_for_MC(void);
void dcpimp_prepare_posdata_for_JBC(void);
BYTE Process_incoming_dcpiso_message(pPTE_MSG p_msg_in);
INT dcpimp_check_If_CUP_Retail_Transaction (void);
void dcpimp_Prepare_posdata_for_DINERS (void);
void dcpimp_get_error_warning_file_name_path(void);
INT  dcpimp_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int details);
void dcpimp_create_Error_Filename( void );
void dcpimp_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );

BYTE dcpimp_Send_Interactive_DB_Request( pPTE_MSG  p_msg_out,  pPTE_MSG *p_msg_in,
                                         pCHAR     que_name,  pCHAR     error_msg );

void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_CUP();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_DCI();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_JCB();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_MC();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_VISA();
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_CUP();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_DCI();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_JCB();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_MC();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_VISA();
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_PL();
void create_system_date( pBYTE system_date );
INT Dcpimp_verify_fraud_rules_for_NCIFTR();
INT Dcpimp_check_link_for_NCIFTR(void);
INT Dcpimp_If_Tx_Qualifies_For_RealTime();
INT Dcpimp_check_link_for_NCIFTR(void);
INT Dcpimp_verify_fraud_rules_for_NCIFTR(void);
BOOLEAN Dcpimp_Send_Request_To_RT_Falcon_Service ( void );

/* SCR 22830 - ThoughtFocus */
BOOLEAN IsOriginatorATerminal(pBYTE pos_condition_code);

#define ORIG_QUEUE  "ORIGQ"
#define ORIG_INFO   "ORIGINFO"

#endif
