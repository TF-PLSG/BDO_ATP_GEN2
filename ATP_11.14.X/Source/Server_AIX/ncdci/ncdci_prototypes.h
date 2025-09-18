/***************************************************************************
*
* Copyright (c) 2003, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      ncdci_prptotypes.h
*  
* TITLE:       ncdci Network Control Function Prototypes
*  
* DESCRIPTION: This header file contains a list function prototypes that are
*              used by the Telecheck Network Control application.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      Abhishek Verma
  
*
****************************************************************************/

#ifndef NCEQ_PROTOTYPES_H
#define NCEQ_PROTOTYPES_H

struct trandef *ncdci_Find_Tran_Table( pAUTH_TX p_auth_tx );
struct bitfield_data *ncdci_Find_Bitfield_Table( enum bitfields bitfield );
/*-------*/
/* MAIN  */
/*-------*/
void ascendent_msg_handler( pPTE_MSG p_msg_in );
INT  ncdci_ncdci1_startup( void );
void ncdci_get_keyblock_flag_details( void );
void ncdci_set_keyblock_flag_details( pCHAR flag );



/*-------*/
/* TRANS */
/*-------*/
BOOLEAN ncdci_process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx);
void  	ncdci_process_request_for_host( pPTE_MSG p_msg_in );
void  	ncdci_process_response_for_host( pPTE_MSG p_msg_in );
void  	ncdci_process_host_msg( pPTE_MSG p_msg_in );
INT   	ncdci_process_request_from_host( void );
void 	ncdci_process_response_from_host( void );
void  	ncdci_process_clear_timer_reply( pPTE_MSG p_msg_in );
void  	ncdci_process_db_response_atm(   pBYTE p_data_stream, INT record_found );
void  	ncdci_process_db_response_ncf30( pBYTE p_data_stream );
void  	ncdci_process_request_for_host_after_security( void );
void  	ncdci_process_request_for_host_after_currency( void );
void  	ncdci_update_auth_tx_with_ncf30_data( void );
void  	ncdci_copy_resp_into_request( pAUTH_TX host_auth_tx );
void 	Mask_Keys(char * key_value);
INT   	ncdci_verify_host_status( pCHAR err_buf );
BYTE  	ncdci_hhutil_process_host_request(pAUTH_TX p_auth_tx);
BOOLEAN ncdci_hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
BYTE  	ncdci_match_request_to_response(BYTE request);
INT  	ncdci_incoming_other_host_response(pAUTH_TX p_auth_tx);
INT  	ncdci_txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
INT  	ncdci_get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
void  	ncdci_process_1304_from_host();
BYTE   	ncdci_get_tran_type( pAUTH_TX p_auth_tx ) ;
BYTE   	ncdci_get_tran_type_2(BYTE tx_key) ;
BOOLEAN ncdci_hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
INT  	ncdci_incoming_other_host_response_db_reply( );
BOOLEAN ncdci_hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
INT  	ncdci_process_incoming_message_continued( pAUTH_TX p_auth_tx );
INT  	ncdci_incoming_terminal_request(pAUTH_TX p_auth_tx);
BOOLEAN ncdci_send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx );
INT  	ncdci_perform_host_logon_request(char request_type[], int source,	pAUTH_TX p_auth_tx);
INT  	ncdci_perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx );
INT 	ncdci_hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
int  	ncdci_perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx);
INT  	ncdci_hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT  	ncdci_set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out );
BOOLEAN ncdci_process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx);
void  	ncdci_process_logon_Key_Reset_Transaction (void);
void  	ncdci_update_ncf01( pAUTH_TX p_auth_tx );
BOOLEAN ncdci_hhutil_send_host_response(pAUTH_TX p_auth_tx);
BOOLEAN ncdci_blank_or_null_string(pCHAR astr, INT max_len);
BOOLEAN ncdci_transfer_response_msg_to_dci2();
BOOLEAN ncdci_check_for_cadencieacq_transaction();

/*-------*/
/* UTILS */
/*-------*/

void 	ncdci_calc_julian_day( pCHAR date_str, pCHAR julian_str );
void 	ncdci_get_date( pCHAR time_date, pCHAR date_str );
void 	ncdci_get_time( pCHAR time_date, pCHAR time_str );
void 	ncdci_create_rrn( pCHAR rrn );
void 	ncdci_copy_to_req_resp_buffer ( pPTE_MSG p_msg_in );
INT 	ncdci_determine_txn_flow( pCHAR msgtype, INT source );
void 	ncdci_send_response_to_host( INT send_flag );
void 	ncdci_send_request_to_host( pCHAR timer_resp_flag );
INT 	ncdci_send_msg_to_authorizer( BYTE msgtype );
INT 	ncdci_build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );
INT 	ncdci_send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
INT 	ncdci_init_Auth_Tx(void);
INT 	ncdci_determine_tx_key( );
void 	ncdci_set_tx_key_to_response();
void 	ncdci_vBCDMove(enum bitfields fieldid);
INT 	ncdci_vEBCDICMove( enum bitfields fieldid );
INT 	ncdci_EBCDICMove( enum bitfields fieldid );
INT 	ncdci_RspEBCDICvMove (enum bitfields fieldid);
INT 	ncdci_RspEBCDICMove (enum bitfields fieldid);
void 	ncdci_RspMovevBCD (enum bitfields fieldid);
INT 	ncdci_GenericMove(enum bitfields fieldid);
void 	ncdci_vDataMove(enum bitfields fieldid);
void 	ncdci_RspMovevData (enum bitfields fieldid);
INT 	ncdci_RspMoveGeneric(enum bitfields fieldid);
INT 	ncdci_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT 	ncdci_Move_Always( enum bitfields bitfield, BYTE p_trantype );
int 	ncdci_get_variable_length( unsigned char *DataField, int FieldLen );
INT 	ncdci_build_host_msg( pCHAR err_buf );
INT 	ncdci_build_host_structure( pCHAR err_buf );
INT 	ncdci_parse_host_msg( pCHAR err_buf );
INT 	ncdci_populate_auth_tx( pCHAR err_buf );
INT 	ncdci_parse_track2();
void 	ncdci_remove_track2_end_sentinel( pBYTE track2 );
INT 	ncdci_build_response_msg();
INT 	ncdci_build_request_msg( pCHAR err_buf );
void 	ncdci_create_stan( pBYTE stan );
void 	ncdci_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT 	ncdci_send_network_status_to_monitor();
INT 	ncdci_send_message_to_host( pCHAR err_buf,INT mgt );
INT 	ncdci_pin_block_exists();
INT  	ncdci_verify_host_status( pCHAR err_buf );
void 	ncdci_perform_error_processing( pCHAR resp_code );
INT 	ncdci_perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err_buf );
INT 	ncdci_is_reversal();
void 	ncdci_ascii_to_bin_with_length( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
void 	ncdci_trim_trailing_spaces( pBYTE p_string );
void 	ncdci_write_to_txn_file();
void 	ncdci_Rj_with_lead_zeros( pCHAR str, INT FieldLen );
BYTE 	ncdci_hhutil_process_host_request(pAUTH_TX p_auth_tx);
BOOLEAN ncdci_hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT 	ncdci_init_txn( pAUTH_TX p_auth_tx );
BOOLEAN ncdci_process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx);
BYTE 	ncdci_match_request_to_response(BYTE request);
INT 	ncdci_incoming_other_host_response(pAUTH_TX p_auth_tx);
INT 	ncdci_txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
INT 	ncdci_get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
INT 	ncdci_incoming_other_host_response_db_reply( );
BYTE  	ncdci_get_tran_type_2(BYTE tx_key);
INT 	ncdci_insert_tlf01(pAUTH_TX p_auth_tx);
INT 	ncdci_hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR ncdci_response_code[4]);
INT 	ncdci_hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
void 	ncdci_update_ncf01( pAUTH_TX p_auth_tx );
BOOLEAN ncdci_hhutil_send_host_response(pAUTH_TX p_auth_tx);

void 	ncdci_send_update_Key_message_to_ncdci_ACQ( );
INT 	ncdci_get_service_code( pCHAR service_code, pCHAR error_msg );
BOOLEAN ncdci_blank_or_null_string(pCHAR astr, INT max_len);
BOOLEAN ncdci_is_issuing_transaction(void);
BOOLEAN ncdci_is_issuing_3DS2_txn(void);
BOOLEAN ncdci_is_POS_Acq_transaction(void);
BOOLEAN ncdci_is_voice_transaction(void);
BOOLEAN ncdci_is_sbatch_transaction(void);
BOOLEAN ncdci_is_approved_by_Cadencie(void);
BOOLEAN ncdci_check_if_valid_zero_amount_transaction(void);
void 	ncdci_build_pos_data_from_iso_pos_entry_mode(void);
void 	ncdci_populate_iso_pos_entry_mode();
BOOLEAN ncdci_is_pin_transaction(void);
BOOLEAN ncdci_is_Card_present_pos_transaction(void);
BOOLEAN ncdci_is_Magstrip_pos_transaction(void);
BOOLEAN ncdci_is_Chip_pos_transaction(void);
BOOLEAN ncdci_is_Fallback_pos_transaction(void);
BOOLEAN ncdci_is_Contactless_Mastripe_pos_transaction(void);
BOOLEAN ncdci_is_Contactless_Chip_pos_transaction(void);
BOOLEAN ncdci_is_Manual_pos_transaction(void);
BOOLEAN ncdci_is_ECOM_transaction(void);
INT 	ncdci_is_CAVV_Key_Indicator_set_for_BDO_CAVV(void);
BOOLEAN ncdci_is_Advice_tranasction(void);
BOOLEAN ncdci_is_Advice_tranasction_req(void);
BOOLEAN ncdci_is_Advice_tranasction_resp(void);
BOOLEAN ncdci_is_EMV_Trn(void);
void 	ncdci_build_Magstrip_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Chip_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Fallback_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Contactless_Mastripe_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Contactless_Chip_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Manual_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_unknow_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Magstrip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_Fallback_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_Contactless_Mastripe_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_Contactless_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_voice_transaction_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Manual_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_unknow_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci_build_Ecom_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
VOID 	match_DCI_to_ISO_response_code( pBYTE dci_response_code);
VOID 	match_ISO_to_Diners_response_code(  pBYTE diners_response_code);
void 	ncdci_populate_DE43_name_street_city(char * temp_cardaccname);
void 	ncdci_send_logoff_message_to_ncdci( void );
void 	ncdci_send_logoff_message_to_ncdci3( void );
INT 	ncdci_Sale_transactio (void);
INT 	ncdci_Auth_transactio (void);
INT 	ncdci_Auth_Advice_transaction (void);
void 	ncdci_get_error_warning_file_name_path(void);
INT 	ncdci_Log_error_warning_to_File(pCHAR Error_Buf,pCHAR func,int sev,int details);
void 	ncdci_create_Error_Filename( void );
void 	ncdci_str_asc_to_bcd (pCHAR, INT, BYTE []);
void 	ncdci_genutil_EMV_To_Auth_Tx_ncdci_specific( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void 	ncdci_get_local_time(pCHAR localtime_str);
void 	ncdci_get_GMT_Time_MMDDhhmmss(pCHAR GMTtime_str);
void 	ncdci_read_TO_rc_from_tf_ini();

INT 	nccup_check_if_trx_is_bdo_pos_acq_trx( );

INT 	ncdci_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( );
INT 	ncdci_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( );
INT 	ncdci_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( );
INT 	ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_with_pin(  );
INT 	ncdci_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_without_pin( );
void 	ncdci_build_MobilePOS_NFC_Tap2Phone_trx_with_pin_pos_data_from_iso_pos_entry_mode ();
void 	ncdci_build_MobilePOS_NFC_Tap2Phone_trx_without_pin_pos_data_from_iso_pos_entry_mode ();

INT 	ncdci_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( );
INT 	ncdci_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( );
INT 	ncdci_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx( );
INT 	ncdci_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_with_pin( );
INT 	ncdci_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_without_pin( );
void 	ncdci_build_Swiftpass_BDO_Pay_credit_trx_with_pin_pos_data_from_iso_pos_entry_mode();
void 	ncdci_build_Swiftpass_BDO_Pay_credit_trx_without_pin_pos_data_from_iso_pos_entry_mode ();

INT 	ncdci_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( );
INT 	ncdci_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( );
INT 	ncdci_check_if_trx_is_dci_ecommerce_swiftpass_BDOPay_trx( );
void    ncdci_build_ecommerce_swiftpass_BDOPay_trx_pos_data_from_iso_pos_entry_mode();

/*----------*/
/* DATABASE */
/*----------*/
INT  	ncdci_update_database();
void 	ncdci_process_db_reply( pPTE_MSG p_msg );
INT  	ncdci_insert_tlf01(pAUTH_TX p_auth_tx);
void 	ncdci_process_db_ok( pPTE_MSG p_msg );
void 	ncdci_process_db_error( pPTE_MSG p_msg );
INT  	ncdci_get_ncf01_with_wait( pNCF01 Ncf01_I, pCHAR Buffer );
INT  	ncdci_get_db_record( BYTE app_data_type, BYTE subtype1, pBYTE network_type);
INT  	ncdci_insert_db_record( BYTE app_data_type );
BYTE	 ncdci_send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg );


/*-------*/
/* 0800  */
/*-------*/
void 	ncdci_process_monitor_request( pPTE_MSG p_msg_in );
void 	ncdci_issue_req_08X4( INT nm_type );
INT  	ncdci_issue_rsp_08X4( INT request_type );
void 	ncdci_process_08X4_request(  void );
void 	ncdci_process_08X4_response( void );
void 	ncdci_process_monitor_query( pPTE_MSG p_msg );
void 	ncdci_process_net_management_from_host( void );
INT  	ncdci_build_08X0_msg_request(    pCHAR err_buf, int nm_type );
INT  	ncdci_build_08X0_msg_response(    pCHAR err_buf, int nm_type );
INT  	ncdci_send_18nn_to_host( pCHAR err_buf );

/*-------*/
/* TIMER */
/*-------*/
void 	ncdci_process_timeout_msg( pPTE_MSG p_msg_in );
INT  	ncdci_send_timeout_response( pCHAR err_buf );
INT  	ncdci_set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer );
INT  	ncdci_clear_timer( pCHAR timer_key );
INT  	ncdci_clear_request_timer( pCHAR lookup_key );
INT  	ncdci_set_echo_timer(    pCHAR err_buf );
INT  	ncdci_clear_echo_timer(  pCHAR err_buf );
INT  	ncdci_reset_echo_timer( pCHAR err_buf );
void 	ncdci_create_echo_timer_key(    pCHAR echo_timer_key     );
void 	ncdci_create_request_timer_key( pCHAR timer_key          );
void 	ncdci_create_echo_req_timer_key(pCHAR echo_req_timer_key );
void 	ncdci_transform_sale_into_0420( void );


/*----------*/
/* SECURITY */
/*----------*/
void 	ncdci_process_translate_pin_response( pPTE_MSG p_msg_in );
void 	ncdci_translate_pin( void );
void 	ncdci_key_generate_req();


/*---------------*/
/* SHARED MEMORY */
/*---------------*/
INT  	ncdci_create_shared_mem_table( pCHAR errbuf );
INT  	ncdci_set_host_state( pBYTE host_state, pCHAR err_buf );
INT  	ncdci_decrement_active_count(  pCHAR errbuf );
INT  	ncdci_increment_active_count(  pCHAR errbuf );
INT  	ncdci_increment_timeout_count( pCHAR errbuf );
INT  	ncdci_reset_timeout_count(     pCHAR errbuf );
INT  	ncdci_write_to_shared_mem_table( pBYTE consec_time_out_count,  pBYTE active_tran_count,
										pBYTE host_state, pCHAR errbuf );
INT  	ncdci_read_shared_mem_table( pBYTE consec_time_out_count, pBYTE active_tran_count,
										pBYTE host_state,    pCHAR errbuf );
XINT 	ncdci_ReadMemTable(  pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );
LONG 	ncdci_WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );



/*------------*/
/* STATISTICS */
/*------------*/
INT  	ncdci_send_transaction_statistics( pPTE_MSG p_msg_in );
INT  	ncdci_reset_transaction_statistics( pPTE_MSG p_msg_in );


/*----------*/
/* CHARCODE */
/*----------*/
void 	ncdci_convert_to_ascii(  pBYTE source, pBYTE dest, INT len );
void 	ncdci_convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
void 	ncdci_init_character_code_maps( void );
void 	ncdci_hex_to_ebcdic(pBYTE source, pBYTE dest, INT len );
void 	ncdci_ebcdic_to_hex(pBYTE source, pBYTE dest, INT len );

/*------------*/
/* VALIDATION */
/*------------*/
INT  	ncdci_isnum( pCHAR in_str );
INT  	ncdci_isalphanum(pCHAR string);
INT  	ncdci_isalphanum_space(pCHAR string);
INT  	ncdci_validate_date_yymm( pBYTE, pCHAR );
INT  	ncdci_validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf );
INT  	ncdci_validate_time(          pBYTE time_str  );
INT  	ncdci_validate_date_mmdd(     pBYTE date_str  );
INT  	ncdci_validate_date_time(     pBYTE date_time );
INT  	ncdci_validate_host_msg( void );
INT  	ncdci_validate_msgtype( pCHAR msgtype, pCHAR err_buf );
INT  	ncdci_blank_string(pCHAR astr, INT max_len);
void 	ncdci_check_end_of_year_rollover( pBYTE txn_mmdd, pBYTE ATP_date_yyyymmdd );


/*----------*/
/* CURRENCY */
/*----------*/
INT 	ncdci_perform_currency_conversion( DWORD  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 pDWORD amt_out, pCHAR err_buf );

/*-------*/
/* 1304  */
/*-------*/
INT  	ncdci_process_file_update_request( pPTE_MSG p_msg );
void 	ncdci_process_file_update_response( void );
void 	ncdci_map_1304_to_tlf01( void );
INT  	ncdci_send_response_to_GUI( void );
INT  	ncdci_validate_gui_inputs( pCHAR err_msg );

INT  	ncdci_populate_1314_resp_struct( pBYTE resp_code, pCHAR err_msg );
void 	ncdci_issue_1304_response( pBYTE resp_code, pCHAR err_msg );

/*------------*/
/* BIT FIELDS */
/*------------*/
void 	ncdci_build_request_field_2( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_3( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_4( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_6( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_7( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_11( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_12( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_13( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_14( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_22( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_23( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_24( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_25( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_26( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_30( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_32( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_33( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_35( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_37( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_38( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_39( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_40( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_41( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_42( pINT result_flag, pCHAR err_buf ); /* Girija Y - ThoughtFocus */
void 	ncdci_build_request_field_43( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_44( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_45( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_46( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_48( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_49( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_51 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_52( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_54 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_55 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_56 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_58( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_59( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_62 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_64 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_72 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_92( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_93 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_94 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_96 ( pINT result_flag, pCHAR err_buf );  /* Girija Y - ThoughtFocus */
void 	ncdci_build_request_field_100 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_101 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_106 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_122 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_123 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_124 ( pINT result_flag, pCHAR err_buf );
void 	ncdci_build_request_field_125 ( pINT result_flag, pCHAR err_buf );

void 	ncdci_move_field_2( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_3( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_4( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_6( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_7( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_11( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_12( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_13( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_14( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_22( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_23( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_24( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_25( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_26( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_30( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_32( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_33( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_35( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_37( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_38( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_39( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_40( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_41( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_42( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_43( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_44( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_45( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_46( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_48( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_49( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_51( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_52( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_54( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_55( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_56( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_58( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_59( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_62( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_64( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_72( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_92( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_93( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_94( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_96( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_100( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_101( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_106( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_122( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_123( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_124( pINT result_flag, pCHAR err_buf );
void 	ncdci_move_field_125( pINT result_flag, pCHAR err_buf );


#endif
