/*******************************************************************************
* Copyright (c) 2008, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       visautil.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module contains all the function prototypes for visautil.c
*
* APPLICATION: 
*
*******************************************************************/

#ifndef VISAUTIL_H
#define VISAUTIL_H

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "visamain.h"

#define BDOR_68_REQ_1_VISA 1
#define BDOR_68_REQ_3_VISA 1
#define BDOR_68_REQ_5_VISA 1

#define POS_ENTRY_MODE_LEN 							    					4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 							"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 						"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM						"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM					"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM				"0012"

#define VISA_DE22_PEM_LEN													4
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 							"0710"
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM						"0720"
#define VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM 						"0310"
#define VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM					"0320"
#define VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM							"0120"

#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN									3
#define BDO_POS_ACQ_TXN_HANDLER_QUEUE										"dcp"

#define ISO_MESSAGE_TYPE_LEN												4
#define POS_ACQ_SALE_MESSAGE_TYPE 											"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 											"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 										"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE								"0420"

#define VISA_POS_DATA_LEN													12
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN				 		"980000100010"
#define VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN				 	"980000100010"
#define VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA							"030000000010"
#define VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA						"510000000740"

#define POS_CONDITION_CODE_LEN												2
#define POS_ECOM_CONDITION_CODE												"59"

void    Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
INT     hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT     hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx);
void    convert_from_julian_day( pCHAR julian_str, pCHAR date_time );
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx);
INT     hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx);
INT     move_iso_response_to_auth_struct(CHAR nmi_code[], pAUTH_TX p_auth_tx);
void    build_host_structure( pAUTH_TX p_auth_tx );
void    copy_to_req_resp_buffer (pCHAR indata, INT  msglen   );
BYTE    get_tran_type_2(BYTE);
BYTE    get_tran_type( pAUTH_TX p_auth_tx ) ;
void    TestResponses(BYTE ReqRsp_Buffer[COMM_BUF_SZE]);
void    Build_Host_Header (pAUTH_TX p_auth_tx);
BOOLEAN blank_string(pCHAR astr, INT max_len);
BOOLEAN empty_string( pCHAR astr, INT max_len );
void    hhutil_get_strtime ( pCHAR deststr );
BYTE    hhutil_process_host_request(pAUTH_TX p_auth_tx);
INT     incoming_other_host_response(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx);
void    hhutil_send_retry_response_to_host(pCHAR code, pAUTH_TX p_auth_tx);
void    hhutil_parse_db_ok(pPTE_MSG p_msg_in);
void    hhutil_parse_db_error(pPTE_MSG p_msg_in);
BYTE    hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg );
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
XINT    ReadVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src);
LONG    WriteVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src);
VOID    hhutil_get_response_text( pAUTH_TX p_auth_tx);
void    trim_spaces( pBYTE pString );
void    ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
INT     verify_auth_tx_data(pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx);
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT     insert_tlf01(pAUTH_TX p_auth_tx);
void    Field126Parser( pAUTH_TX p_auth_tx, pBYTE cvv2_data );
void    get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx );
INT     isalphanum_space(pCHAR string);
INT     ParseField63( pCHAR, pAUTH_TX );
void    LogStipReasonCode( pAUTH_TX p_auth_tx );
INT     Field116_Handler( pCHAR f116, pAUTH_TX p_auth_tx );
void 	genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void 	generate_VISA_EMV_data( INT emv_len_in, pBYTE emv_data_in );
void 	Replace_9C_Tag_EMV_Data(pBYTE emv_data_in,INT emv_len_in,pBYTE processing_code);
INT 	Move41_IfThere(enum bitfields fieldid, BYTE p_trantype);
INT 	If_Tx_is_visa_RPS_Asiapay_Transaction( pAUTH_TX p_auth_tx  );

void 	ncvisa_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num);
void 	ncvisa_create_Error_Filename();
INT  	ncvisa_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail);
void 	ncvisa_get_error_warning_file_name_path(void );
void 	Mask_card_number(char * card_num);
BOOLEAN ncvisa_Is_AsiaPay_Transaction();
BOOLEAN ncvisa_check_for_acq_refund_transaction(pAUTH_TX p_auth_tx);
BOOLEAN ncvisa_transfer_refund_response_msg_to_VISA2( pAUTH_TX p_auth_tx );
BOOLEAN ncvisa_check_for_cadencie_acq_transaction(pAUTH_TX p_auth_tx);
void 	ncvisa_build_field_104_for_debit_txns(pAUTH_TX p_auth_tx,pCHAR de104_data ,pCHAR de104_len);
void 	ncvisa_build_field_104_for_rps_acq_txns(pAUTH_TX p_auth_tx,pCHAR de104_data ,pCHAR de104_len);
void 	parse_field34_for_MIT( pAUTH_TX p_auth_tx, pCHAR f34, INT len_f34 );
void 	parse_field104_for_Ext_Authzn( pAUTH_TX p_auth_tx, pCHAR f104, INT len_f104 );
void 	parse_field104_for_Offus_Authzn( pAUTH_TX p_auth_tx, pCHAR f104, INT len_f104 );
INT 	ncvisa_check_if_trx_is_bdo_pos_acq_trx( pAUTH_TX p_auth_tx  );
void	parse_field111_from_Offus_Authzn_response( pAUTH_TX p_auth_tx, pCHAR f111, INT len_f111 );

/*MobilePOS-NFC Terminal for Tap2Phone Transactions */
INT 	ncvisa_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  );

/*Swiftpass BDO Pay Credit Transactions*/
INT 	ncvisa_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  );

/*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
INT 	ncvisa_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  );
INT 	ncvisa_check_if_trx_is_visa_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  );

/* Currency Codes */
#define  PESO     "608"
#define  USD      "840"

#define   VISA_TO                        "91"
#define	  VISA_APPROVED					 "00"
#define	  VISA_ACCOUNT_VERIFIED			 "85"

#define VISA_DEFAULT_TID 		"99999999"
#define VISA_DEFAULT_TID_LEN 	8
#define VISA_INCREMENTAL_DE_63_3 "3900"

typedef struct
{
   BYTE net_consec_tmouts_ctr    [4];
   BYTE active_txns_ctr          [4];
   BYTE current_state            [2];
   BYTE prev_src_station         [4];
}VISA_MEM, *pVISA_MEM;

#define BDOR_62_PREAUTH_VOID 1

#define PRIMARY_AUTH 										'P'
#define INCREMENTAL_AUTH 									'I'
#define ESTIMATED_AUTH 										'E'
#define PARTIAL_REVERSAL 									'R'
#endif
