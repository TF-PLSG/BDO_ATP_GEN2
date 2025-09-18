/**************************************************************************************************
*  
* MODULE:      txservice.h
*  
* TITLE:       ATP Standard - Process Incoming Request.
*  
* DESCRIPTION:  
*
* APPLICATION: TXCNTL
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\txcntl\txservice.h  $
      
	  Rev 1.27 Jan 02 2009, Girija Y ThoughtFocus 
   Updated version to 4.4.1.42
   Added function txservice_Eqit_POS_Field63() to pass Fld 63 for Tx coming from POS 
   and routed to "nceqit".
   Rel 08-40
   
      Rev 1.26   May 13 2006 14:14:46   dirby
   Modified to accept a test message from Monitor and respond to it.
   No action is taken; it just responds with an empty message.
   Monitor is just looking for a response, no data.
   SCR 19357
   
      Rev 1.25   Feb 10 2006 16:50:02   dirby
   Added a function to update date of the last transaction in CCF03P.
   This function is called when there is a pin mismatch. If this date
   does not get updated to 'today', the number of failed pin attempts
   keeps resetting to 0 then incremented by 1.  So it is always '1'.
   SCR 19383
   
      Rev 1.24   Oct 27 2005 16:25:38   dirby
   Fixed FraudGuard related issues:
   1. If txn is rejected by ATP, still need to send txn to FG.
   2. Do not send to FG if Host is unavailable (offline).
   3. Always send Acquirer Id (DE32), even on declines by ATP.
   4. Added capability to turn FG ON/OFF by card family.
   SCR 17661
   
      Rev 1.23   Jun 23 2005 09:17:12   chuang
   Version 4.4.1.6. Enhancements for FraudGuard.
   
      Rev 1.22   Apr 01 2005 10:10:28   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unlimited purchase card MCC ranges, and purchase card transaction limits(scr12785).
   
      Rev 1.21   Sep 13 2002 17:42:10   dirby
   Added function prototype for CashBonusTxn.
   SCR 804
   
      Rev 1.20   Aug 15 2002 16:48:22   dirby
   Added a function prototype for a function that determines if a
   txn is a deferred txn.
   SCR 551
   
      Rev 1.19   Oct 16 2001 13:20:24   dirby
   Removed parameter from function ValidateCardNumber.
   It was not being used.   SCR 403
   
      Rev 1.18   Sep 27 2001 15:33:34   SYARLAGA
   Added defintions for these functions
    validate_mcc_range,
    validate_acquirer_id,
    ValidateCardNumber,
    GetMod10CheckDigit.
   
      Rev 1.17   Jun 22 2001 11:40:46   SYARLAGA
   Added function prototype txservice_get_service_code_from_track1.
   
      Rev 1.16   Jan 09 2001 11:09:44   dirby
   Added function prototype "txservice_Switch_to_Cirrus".
   
      Rev 1.15   Jun 30 2000 14:26:50   dirby
   Added function prototypes to support Visa Electron.
   
      Rev 1.14   Feb 07 2000 17:19:44   iarustam
   SCR 373,375
   
      Rev 1.13   Jan 18 2000 13:05:04   iarustam
   bug fixes # 14,53,83,90,92,104,125,131,147,150,151,154
   
      Rev 1.12   Dec 30 1999 13:33:00   ddabberu
   aix_build
   
      Rev 1.11   Jul 23 1999 15:40:36   sdasgupt
   1. Added code to handle balance_inquiry
   2. Logic to not calculate authorization number if transaction is a Sales_adjustment
   3. Copied available balance to TLFO1's outstanding balance
   
      Rev 1.10   Jul 15 1999 13:20:54   sdasgupt
    
   
      Rev 1.9   Jul 12 1999 19:03:08   sdasgupt
    
   
      Rev 1.8   Jun 01 1999 17:14:16   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.7   May 24 1999 15:37:38   sdasgupt
    
   
      Rev 1.6   May 13 1999 14:53:18   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.5   Apr 09 1999 16:47:16   sdasgupt
   Bug Fixes
   Overrides of Voice Auth Transactions
   Reversals of Voice Auth Transactions
   
      Rev 1.4   Apr 01 1999 11:20:42   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.3   Feb 23 1999 11:11:38   sdasgupt
    
   
      Rev 1.2   Feb 17 1999 18:13:26   sdasgupt
   Program Changes
   
      Rev 1.1   Feb 16 1999 17:50:34   sdasgupt
   TxCntl Changes
   
      Rev 1.0   Feb 12 1999 18:54:38   sdasgupt
   Initial version
*******************************************************************************/
#ifndef _txservice
#define _txservice

#include "txcntl.h"
#include "equitdb.h"

#define  DE48_TAG43    "43"
#define AMEX03A

BOOLEAN txservice_Forward_Auth_Tx_To_Remote_Host (CHAR* que_name, BYTE message_type, 
																  BYTE message_subtype_1, BYTE AppDataType) ;
BOOLEAN txservice_Forward_Auth_Tx_To_Originator (CHAR* que_name, BYTE message, 
																BYTE message_subtype_1, BYTE AppDataType) ;
BOOLEAN txservice_Forward_Auth_Tx_To_Originator_for_RT_decline (CHAR* que_name, BYTE message,
																BYTE message_subtype_1, BYTE AppDataType) ;
BOOLEAN txservice_Forward_Auth_Tx_To_Updater   (CHAR* que_name, BYTE message, 
																BYTE message_subtype_1, BYTE AppDataType) ;
BOOLEAN txservice_Verify_Pin_From_Service (void) ;

BOOLEAN txservice_Send_Request_To_Service (BYTE* que_name, BYTE nType, 
										BYTE nSubType1, BYTE nSubType2, 
										BYTE nAppDataType, pBYTE p_data, 
										INT nDataSize) ;
BOOLEAN txservice_Send_Request_To_FraudGuard_Service();
BOOLEAN txservice_Send_Request_To_Falcon_Service();
BOOLEAN txservice_Send_Request_To_RT_Falcon_Service();
BOOLEAN txservice_Send_EXT10_Request_To_RT_Falcon_Service ();

void txcntl_read_falconrt_ext10_flag_for_transactions();
void txcntl_read_vertexon_codes_for_falcon_ext10();
BOOLEAN txservice_check_auth_rc_for_falcon_ext10();
BOOLEAN txservice_check_for_AMEX_partial_reversal_transaction();
BYTE txservice_Process_Auth_Request (pPTE_MSG) ;
BYTE txservice_Process_Auth_Response (pPTE_MSG p_msg_in) ;
INT txcntl_check_if_txn_falls_under_merchant_ingerity_validation();
void txservice_Prepare_AuthTx_For_Forwarding (BYTE, BYTE*) ;
void txservice_Replace_Tx_Key_In_Auth_Tx (void) ;
void txservice_Generate_Numeric_Auth_Number (BYTE* strAuthNumber) ;
void txservice_Get_Original_Transaction_Type (BYTE* message_type, BYTE* processing_code, 
												 BYTE* strTxType) ;
void txservice_Create_Date_Time_RRN_For_Auth_Tx (BYTE* pstrDate_yyyymmdd, 
												BYTE* pstrTime_hhmmss, 
                                                BYTE* pstrRRN) ;

void txservice_Get_Bin_Type_From_Processing_Code (BYTE* pnAccountType) ;
BYTE txservice_Process_When_Bin_Range_Not_Found (void) ; 
BYTE txservice_Process_When_Bin_Range_Found (void) ;
BYTE txservice_Process_Bin_Range_Switch_Out (BYTE context, 
											BYTE* destination_name, 
											BYTE* alt_destination_name) ;
BYTE txservice_Continue_Processing_When_Bin_Range_Found (void) ;
BYTE txservice_Is_Host_Up (BYTE* host_que_name) ;
BYTE txservice_Start_Local_Processing (void) ;
BYTE txservice_GetTransactionOrigin (void) ;
BYTE txservice_Get_Card_SubType (void) ;
BYTE txservice_Verify_Currency_Code (void) ;
BYTE txservice_Perform_Auth_Tx_Checks (void) ;

BYTE txservice_If_Tx_Requires_Pin_Validation(void) ;
BYTE txservice_If_Tx_Requires_CVV_Validation(void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Reversal (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Deferred (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Offline_Void (void) ;
BYTE txservice_If_Tx_Qualifies_For_Expiry_Date_Checking (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Offline_Sale (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Advice (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Adjustment (void) ;
BYTE txservice_If_Tx_Qualifies_For_Authorization_Checks (void) ;
BYTE txservice_If_Tx_Qualifies_For_Card_Velocity_Checking (void) ;
BYTE txservice_If_Tx_Qualifies_For_Fleet_Card_Checks (void) ;
BYTE txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing (void) ;
BYTE txservice_If_Tx_Qualifies_For_Balance_Account_Checks (void) ;
BYTE txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override (void) ;
BYTE txservice_If_Tx_Cannot_Be_Declined (void) ;
BYTE txservice_If_Reversed_Tx_Is_A_Sale_Type_Tx (void) ;
BYTE txservice_If_Tx_Is_A_Deferred_Purchase_Type (void) ;
INT txservice_Check_If_CUP_Transaction (void);
BOOLEAN txservice_JCB_get_CAVV_from_DESEncrypn(pBYTE block);
void TxDataServer_JCB_Input(char * Supplemental_output);


/*filter to check whether a transaction needs to go to FraudFuard*/
BYTE txservice_If_Txn_FraudGuard_Routing_Needed( INT ) ;

BYTE txservice_CashBonusTxn();
BYTE txservice_Tx_Qualifies_To_Be_Completed_Before_Authorization(void) ;
BYTE txservice_Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
															BYTE general_status, BYTE* response_code) ;

void Rj_with_lead_zeros( pCHAR str, INT FieldLen ) ;

BYTE txservice_Perform_PTE_Message_Checks (pPTE_MSG p_pte_msg) ;
BYTE txservice_Filter_Transactions (void) ;

INT txservice_is_txn_visa_electron( );
INT txservice_get_service_code_from_track2( pCHAR service_code, pCHAR error_msg );
INT txservice_get_service_code_from_track1( pCHAR service_code, pCHAR error_msg );
BYTE txservice_Switch_to_Cirrus (void);
INT txservice_is_selected_jcb_bin_for_cirrus(void) ;
BYTE txservice_perform_standin_processing(void);
BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Reversal_Response (void);
BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Offline_Void_Response(void);
BYTE txserviceCadience_Calculate_Deferred_Gross_Amount (double* pConvertedTxAmount) ;
BYTE txserviceCadience_Calculate_Deferred_Monthly_Amortized_Amount (double* pConvertedTxAmount);
BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Adjustment_Response (void);
BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Voice_Auth_Override_Response (void);
BYTE txserviceCadience_Perform_Balance_Update(void);
BYTE txserviceCadience_Continue_With_CCF02P_For_Balance_Update(void);
BYTE txserviceCadience_Continue_With_ACF01_For_Balance_Update(void);
BYTE txserviceCadience_Continue_With_ACF01_For_Not_Found(void);

INT txservice_If_Tx_Qualifies_For_RealTime();
INT txservice_check_if_transaction_qualifies_for_Realtime();
INT txservice_check_fraud_rules_for_card();

INT txservice_verify_fraud_rules_for_VISA();
INT txservice_verify_fraud_rules_for_MC();
INT txservice_verify_fraud_rules_for_AMEX();
INT txservice_verify_fraud_rules_for_DCI();
INT txservice_verify_fraud_rules_for_CUP();
INT txservice_verify_fraud_rules_for_JCB();

INT txservice_check_link_for_VISA();
INT txservice_check_link_for_MC();
INT txservice_check_link_for_AMEX();
INT txservice_check_link_for_DCI();
INT txservice_check_link_for_CUP();
INT txservice_check_link_for_JCB();
INT txservice_check_RTlink_for_card();

INT validate_mcc_range(void) ;
INT validate_acquirer_id(void);
BOOLEAN ValidateCardNumber(void);
INT GetMod10CheckDigit( pCHAR input_str, INT input_len );
INT txservice_Get_Network_Acquirer_ID(void);
void txservice_Set_Last_Txn_Date_To_Today(void);
BOOLEAN txservice_Send_To_Monitor( BYTE mtype, BYTE st1, BYTE st2,
                                  pBYTE msg,   INT  len );

void txservice_Eqit_POS_Field63(pAUTH_TX ,pBYTE); /* Girija Y, TF, Jan 02 2009 */
BYTE txservice_Perform_OMD_GlobalParamterBypass (BYTE);
BYTE txservice_Perform_OMD_MCCBypass(void);
void txservice_Prepare_Pos_Data(void);
void txservice_Prepare_posdata_for_Amex(void );
void txservice_Prepare_posdata_for_Amex_OfflineSale(void);
void txservice_Prepare_posdata_for_CUP(void);
void txservice_Prepare_posdata_for_CUP_OfflineSale(void);
void txservice_Prepare_posdata_for_VISA(void);
void txservice_Prepare_posdata_for_VISA_OfflineSale(void);
void txservice_Prepare_posdata_for_MC(void);
void txservice_Prepare_posdata_for_MC_OfflineSale(void);
void txservice_Prepare_posdata_for_JCB(void);
void txservice_Prepare_posdata_for_JCB_OfflineSale(void);
INT txservice_Parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg );
BYTE txservice_IsFieldNull (char str[]);


INT txservice_Check_If_CUP_Transaction (void);
INT txservice_Check_If_VISA_Transaction (void);
INT txservice_Check_If_MC_Transaction (void);
INT txservice_Check_If_JCB_Transaction (void);
INT txservice_Check_If_AMEX_Transaction (void);
INT txservice_Check_If_MC_DE_48_Tag_63_Trace_ID_Present(int len, char *src );

BYTE txserviceCadience_Generate_Usage_Error_Message( INT   SystemMonitor,
                                             pBYTE strTemp,
                                             pBYTE function,
                                             BYTE  general_status,
                                             pBYTE response_code );
void txcntl_get_error_warning_file_name_path(void);
INT  txcntl_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int details);
void txcntl_create_Error_Filename( void );
void txcntl_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
BOOLEAN txservice_Check_If_DCI_issuing_transaction(void);
BOOLEAN txservice_Check_If_VISA_issuing_transaction(void);
INT txservice_Check_If_DCI_pin_block_exists();
INT txservice_Check_If_blank_string(pCHAR astr, INT max_len);
INT txservice_Check_If_EMV_Trn();
BOOLEAN txservice_If_Tx_qualify_for_EMV_validation();
void get_MRACON_state_from_ini( void );
BOOLEAN txservice_Send_Request_To_Mracon_Service ( void );
void txservice_load_rps_flag_for_all_cardbrands();
void txservice_load_JCB_contactless_magstripe_Flag_RC();
BOOLEAN txservice_Check_If_Transaction_Qualifies_For_RPS_STIP();
BOOLEAN txservice_Check_RPS_Flag_For_ACQ_Txn();
void match_ISO_to_AMEX_response_code(pBYTE amex_response_code);
void match_ISO_to_Diners_response_code(pBYTE diners_response_code);
BOOLEAN txservice_Check_If_CUP_RPS_TRN(void);
BOOLEAN txservice_Check_If_CUP_MOTO_TRN(void);
BOOLEAN txservice_Check_If_CUP_BDO_PAY(void);
BOOLEAN txservice_Is_AsiaPay_Transaction();
BOOLEAN txservice_Is_Refund_Transaction();
BYTE txservice_Is_Incoming_stip_reversal_txn();
BOOLEAN txservice_validate_mcc();
BOOLEAN txservice_Check_if_POS_Acq_Transaction(void);
BOOLEAN txservice_Check_If_JCB_Contactless_Magstrip_Transaction(void);
BOOLEAN txservice_Check_If_POS_Online_PIN_transction(void);
BYTE txservice_is_txn_should_blocked_for_processingCode();
BYTE txservice_is_txn_should_blocked_for_MCC();
BYTE txservice_is_txn_AFT_for_partial_approval();
BYTE txservice_verify_MCC_blocked_for_send_fund_txn();
BYTE txservice_check_for_DCI_refund_txn();
INT txservice_Get_MCC_Details_For_Rps_Txn();
INT txservice_Check_If_MC_issuing_transaction(void);
INT txservice_Check_MCC_availablity();
BOOLEAN txservice_Check_CAVVResultCode_value_to_directly_decline_txn();
BOOLEAN txservice_Check_CAVVResultCode_value_to_skip_CAVV_validation();
BOOLEAN txservice_Is_Txn_Qualify_For_Self_CAVV_Validation();
BOOLEAN txservice_verify_VISA_self_validation();
BOOLEAN txservice_VISA_get_CAVV_from_DESEncrypn(pBYTE block);
BOOLEAN txservice_VISA_get_CAVV_from_DESEncrypn();
BOOLEAN txservice_AMEX_get_AV_from_3DES2keyEncrypn();
BOOLEAN txservice_Check_AVV_IVV_value_to_skip_CAVV_validation();
BOOLEAN txservice_check_control_bytes_validation();
BOOLEAN txservice_self_AAV_Validation_For_MC();
BOOLEAN txservice_check_for_ASI_txn();
BOOLEAN txservice_DCI_get_CAVV_from_DESEncrypn(pBYTE block);
void txservice_DCI_prepare_data_3DS2(char* Input_data);
BOOLEAN txservice_Check_DCI_3DS2_self_validation(void);
INT txservice_Check_MCC_availablity();
INT txservice_Check_If_IST_Message(void);
INT txservice_Check_If_Fleet_Transaction(void);
INT txservice_Check_If_Validate_CVN2_FOR_CUP_Non_ECOM_TRAN(void);
void txservice_prepare_data_for_MC_hmac_verification(char * data);
BOOLEAN txservice_self_AAV_Validation_For_MC();
INT parse_field_48( int src_len, char *src, pBYTE ucaf_data );
BOOLEAN txservice_get_3DS2_key_for_MC();
BOOLEAN txservice_get_3DS2_key_for_AMEX();
INT txcntl_read_tf_ini_TO_rc( pCHAR err_buf );
INT txcntl_read_tf_ini_FAI_rc( pCHAR err_buf );
void txservice_Check_and_update_rc_if_Voicetxn_is_TO();
DWORD txcntl_write_falcon_RT_falg_into_inifile(pCHAR fai_rt_value);
void txcntl_read_falconrt_flag_for_transactions();
void txcntl_read_flag_for_cup3ds2_transactions_in_stip();
BOOLEAN txservice_Check_if_PLFC_Transaction();
INT txcntl_map_amex_action_code_to_iso_repsonse_code(pCHAR amex_response_code );
void txcntl_match_DCI_to_ISO_response_code( pBYTE dci_response_code);
BOOLEAN txservice_Check_If_Transaction_Is_Incoming_RPS_STIP();
BOOLEAN txservice_Is_Txn_Qualify_for_DAF_with_CAVV_validation();
BOOLEAN txservice_Is_Txn_Qualify_for_DAF_without_CAVV_validation();
BOOLEAN txservice_Is_Txn_Qualify_for_nonDAF_with_CAVV_validation();
BOOLEAN txservice_Is_Txn_Qualify_for_nonDAF_without_CAVV_validation();
BOOLEAN txservice_Is_Txn_Qualify_For_Account_Verification();
BOOLEAN txservice_VISA_to_Support_DAF_for_MIT_transactions();
BOOLEAN txservice_verify_CAVV_KeyIndicator_from_Issuer();
BOOLEAN txservice_verify_CAVV_KeyIndicator_from_Visa();
BYTE txserviceCadience_Perform_Balance_Update_For_RT_Tx(void);
BYTE txservice_check_for_AMEX_refund_txn();

BOOLEAN txservice_Forward_Auth_Tx_To_Remote_Host_keyblock (CHAR* que_name, BYTE message_type,
																  BYTE message_subtype_1, BYTE AppDataType) ;
#endif


