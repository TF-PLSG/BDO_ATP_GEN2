/*******************************************************************************
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       jcbutil.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module contains all the function prototypes for jcbutil.c
*
* APPLICATION: 

*******************************************************************/

#ifndef JCBUTIL_H
#define JCBUTIL_H

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "jcbmain.h"

#define BDOR_62_PREAUTH_VOID 1

#define BDOR_68_REQ_1_JCB 1
#define BDOR_68_REQ_3_JCB 1
#define BDOR_68_REQ_5_JCB 1

#define POS_ENTRY_MODE_LEN 							    			4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 					"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 				"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM				"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM			"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM		"0012"

#define JCB_DE22_PEM_LEN											4
#define JCB_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN 						"0710"
#define JCB_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN					"0720"
#define JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM 				"0110"
#define JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM			"0120"
#define JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM					"8120"

#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN							3
#define BDO_POS_ACQ_TXN_HANDLER_QUEUE								"dcp"
#define BDO_SBATCH_TXN_HANDLER_QUEUE								"sbatch"

#define ISO_MESSAGE_TYPE_LEN										4
#define POS_ACQ_SALE_MESSAGE_TYPE 									"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 									"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 								"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE						"0420"

#define JCB_POS_DATA_LEN											6
#define JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA						"222608"
#define JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA					"222608"
#define JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA					"122608"

#define POS_CONDITION_CODE_LEN										2
#define POS_ECOM_CONDITION_CODE										"59"
#define JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_CONDITION_CODE		"00"
#define JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_ECI_NOT_AUTHENITICATED	"07"

void    Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
INT     hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT     hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx);
void    convert_from_julian_day( pCHAR julian_str, pCHAR date_time );
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx);
INT     hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx);
INT     move_iso_response_to_auth_struct(CHAR nmi_code[5], pAUTH_TX p_auth_tx);
void    build_host_structure( pAUTH_TX p_auth_tx );
void    copy_to_req_resp_buffer (pCHAR indata, INT  msglen   );
BYTE    get_tran_type_2(BYTE);
BYTE    get_tran_type( pAUTH_TX p_auth_tx ) ;
BOOLEAN blank_string(pCHAR astr, INT max_len);
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
void    hhutil_get_strtime ( pCHAR deststr );
BYTE 	hhutil_process_host_request(pAUTH_TX p_auth_tx);
INT 	incoming_other_host_response(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx);
void    hhutil_send_retry_response_to_host(pCHAR code, pAUTH_TX p_auth_tx);
void    hhutil_parse_db_ok(pPTE_MSG p_msg_in);
void    hhutil_parse_db_error(pPTE_MSG p_msg_in);
BYTE    hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg );
VOID    hhutil_get_response_text( pAUTH_TX p_auth_tx);
void    trim_spaces( pBYTE pString );
void    ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
INT     verify_auth_tx_data(pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx);
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT     insert_tlf01(pAUTH_TX p_auth_tx);
void    init_txn( pAUTH_TX p_auth_tx );
void    get_gmt_time( pBYTE gmt_time );
void    get_stan( pBYTE stan );
INT     isalphanum_space(pCHAR string);
LONG    get_conv_rate_from_database( pCRF01 p_crf01, pAUTH_TX p_auth_tx );
BYTE    ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );
void    get_JCB_EMV_data( INT emv_len_in, pBYTE emv_data_in );
INT 	send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
BOOLEAN send_response_to_sbatch(pAUTH_TX p_auth_tx,BYTE msg_type, pCHAR source_que, pCHAR error_msg);

void 	ncjcb2_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num);
void 	ncjcb2_create_Error_Filename();
INT 	ncjcb2_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail);
void 	ncjcb2_get_error_warning_file_name_path(void );
void 	Mask_card_number(char * card_num);
void 	get_cavv_result_error( BYTE cavv_result, pCHAR error_msg );
void 	get_cav2_result_msg( BYTE cavv_result, pCHAR error_msg );
BOOLEAN ncjcb2_Is_AsiaPay_Transaction(pAUTH_TX p_auth_tx);
BOOLEAN ncjcb_check_RPS_Transaction(pAUTH_TX p_auth_tx);

INT 	ncjcb2_check_if_trx_is_bdo_pos_acq_trx( pAUTH_TX p_auth_tx  );

INT 	ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  );

INT 	ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  );

INT 	ncjcb2_check_if_trx_qualify_for_ecommerce_Swiftpass_BDOPay_trx ( pAUTH_TX p_auth_tx);
INT 	ncjcb2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx);
INT 	ncjcb2_check_if_trx_is_jcb_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx);

#endif
