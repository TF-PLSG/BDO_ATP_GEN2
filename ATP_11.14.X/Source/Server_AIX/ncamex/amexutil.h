/*******************************************************************************
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       amexutil.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module contains all the function prototypes for amexutil.c
*
* APPLICATION: 
*
*******************************************************************/

#ifndef AMEXUTIL_H
#define AMEXUTIL_H

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "amexmain.h"

/*EVM data Ascii Length */
#define APP_CRYT_DATA_ASCII_LEN 16
#define UNPRE_NUM_DATA_ASCII_LEN 8
#define ATC_DATA_ASCII_LEN  4
#define TVR_DATA_ASCII_LEN 10
#define TAR_DATE_DATA_ASCII_LEN 6
#define TAR_TYPE_DATA_ASCII_LEN 2
#define AMT_AUTH_DATA_ASCII_LEN 12
#define TRA_CUR_CODE_DATA_ASCII_LEN 4
#define TER_CUR_CODE_DATA_ASCII_LEN 4
#define AIP_DATA_ASCII_LEN 4
#define AMT_OTH_DATA_ASCII_LEN 12
#define APP_PAN_SEQ_NUM_DATA_ASCII_LEN 2
#define CRYPTO_INFO_DATA_ASCII_LEN 2
#define AMEX_EMV_MAX_LEN 255

/* AMEX 3DS 2.0 declaration */
#define AEVV_VALUE_POSITION 			12
#define AEVV_VALUE_LEN					20
#define SAFEKEY_POSITION				35
#define DE61_SF8_LEN				    40
#define DE61_SF8_DATALEN_1				20
#define DE61_SF8_DATALEN_2				20
#define VERSION_LEN						12

#define  TIMER_FINANCIAL  "Financial"

void    Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
INT     hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
INT     hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
void    convert_from_julian_day( pCHAR julian_str, pCHAR date_time );
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx);
INT     hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx);
INT     move_iso_response_to_auth_struct(CHAR nmi_code[4], pAUTH_TX p_auth_tx, INT);
void    build_host_structure( pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
void    copy_to_req_resp_buffer (pCHAR indata, INT  msglen   );
BYTE    get_tran_type_2(BYTE);
BYTE    get_tran_type( pAUTH_TX p_auth_tx ) ;
void    TestResponses(BYTE ReqRsp_Buffer[COMM_BUF_SZE]);
BOOLEAN blank_string(pCHAR astr, INT max_len);
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
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx);
void Tlf_Logging_ncamex_cadience( pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_on_error( pAUTH_TX p_auth_tx,CHAR amex_response_code[4]);
INT     insert_tlf01(pAUTH_TX p_auth_tx);
void    get_current_date_time( pCHAR system_date, pCHAR system_time );
LONG    get_crf01( pCRF01 pcrf01 );

void ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
void ncamex_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num);
void ncamex_create_Error_Filename();
INT  ncamex_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail);
void ncamex_get_error_warning_file_name_path(void );
void Mask_card_number(char * card_num);
BOOLEAN ncamex_check_for_3DS2_txn_inc(pAUTH_TX p_auth_tx);
//moved from amexutil.c to amexutil.h
BYTE ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );
INT isnum(pCHAR string);
INT isalphanum(pCHAR string);
INT isalphanumSpace(pCHAR string);
INT sendBkToHost(pAUTH_TX p_auth_tx,CHAR amex_response_code[4]);
INT verify_auth_tx_data(pAUTH_TX p_auth_tx);
//
void trim_trailing_spaces( pBYTE p_string );
INT send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
/* Currency Codes */
#define  PESO     "608"
#define  USD      "840"
// TIME OUT CODES

#define   TO                             "19"
#define   AMEX_TIMEOUT                   "68"

#define AMEX03A
#define BIT60_ADITIONAL_DATA_ID "AXAAD"
#endif
