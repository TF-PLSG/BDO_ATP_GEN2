/***************************************************************************
*
* Copyright (c) 2003, ThoughtFocus, Inc.
* All Rights Reserved.
*
* MODULE:      ncdci2_prptotypes.h
*  
* TITLE:       Diners Network Control Function Prototypes
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

/*-------*/
/* MAIN  */
/*-------*/

INT  	ncdci2_startup ( void );

/*-------*/
/* TRANS */
/*-------*/
void 	ncdci2_process_request_for_host( pPTE_MSG p_msg_in );
void 	ncdci2_process_host_msg( pPTE_MSG p_msg_in );
void 	ncdci2_process_request_from_host( void );
void 	ncdci2_process_response_from_host( void );
void 	ncdci2_process_clear_timer_reply( pPTE_MSG p_msg_in );
void 	ncdci2_process_db_response_ncf30( pBYTE p_data_stream );
void 	ncdci2_process_request_for_host_after_security( void );
void 	ncdci2_process_request_for_host_after_currency( void );
void 	ncdci2_ncdci2_update_auth_tx_with_ncf30_data( void );
void 	ncdci2_copy_resp_into_request( pAUTH_TX host_auth_tx );
INT  	ncdci2_verify_host_status( pCHAR err_buf );
BYTE 	ncdci2_hhutil_process_host_request(pAUTH_TX p_auth_tx);
BOOLEAN ncdci2_hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
BYTE 	ncdci2_match_request_to_response(BYTE request);
INT 	ncdci2_incoming_other_host_response(pAUTH_TX p_auth_tx);
INT 	ncdci2_txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
INT 	ncdci2_get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
void 	ncdci2_process_1304_from_host();
BYTE  	ncdci2_get_tran_type( pAUTH_TX p_auth_tx ) ;
BYTE  	ncdci2_get_tran_type_2(BYTE tx_key) ;
BOOLEAN ncdci2_hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
INT 	ncdci2_incoming_other_host_response_db_reply( );
BOOLEAN ncdci2_hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
INT 	ncdci2_ncdci2_process_incoming_message_continued( pAUTH_TX p_auth_tx );
INT 	ncdci2_incoming_terminal_request(pAUTH_TX p_auth_tx);
BOOLEAN ncdci2_send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx );
INT 	ncdci2_perform_host_logon_request(char request_type[], int source,	pAUTH_TX p_auth_tx);
INT 	ncdci2_perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx );
INT 	ncdci2_hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
int 	ncdci2_perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx);
INT 	ncdci2_hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT 	ncdci2_set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out );
BOOLEAN ncdci2_process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx);
void 	ncdci2_process_logon_Key_Reset_Transaction (void);
void 	ncdci2_update_ncf01( pAUTH_TX p_auth_tx );
void 	ncdci2_perform_host_key_send(void);
void 	ncdci2_Get_Key_from_ncf01_table(void);
BOOLEAN ncdci2_hhutil_send_host_response(pAUTH_TX p_auth_tx);
BOOLEAN ncdci2_check_for_cadencieacq_transaction();
BOOLEAN ncdci2_transfer_response_msg_to_dci();

/*-------*/
/* UTILS */
/*-------*/
void 	ncdci2_vBCDMove(enum bitfields fieldid);
INT 	ncdci2_vEBCDICMove( enum bitfields fieldid );
INT 	ncdci2_EBCDICMove( enum bitfields fieldid );
INT 	ncdci2_RspEBCDICvMove (enum bitfields fieldid);
INT 	ncdci2_RspEBCDICMove (enum bitfields fieldid);
void 	ncdci2_RspMovevBCD (enum bitfields fieldid);
INT 	ncdci2_GenericMove(enum bitfields fieldid);
void 	ncdci2_vDataMove(enum bitfields fieldid);
void 	ncdci2_RspMovevData (enum bitfields fieldid);
INT 	ncdci2_RspMoveGeneric(enum bitfields fieldid);
INT 	ncdci2_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT 	ncdci2_Move_IfThereWithSpaces( enum bitfields fieldid, BYTE p_trantype );
INT 	ncdci2_Move_Always( enum bitfields bitfield, BYTE p_trantype );
void    ncdci2_log_message( INT dest, INT severity, pCHAR mesg, pCHAR func_name, int details );
BOOLEAN ncdci2_process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx);
INT     ncdci2_send_network_status_to_monitor( void );
void 	ncdci2_calc_julian_day( pCHAR date_str, pCHAR julian_str );
void 	ncdci2_get_date( pCHAR time_date, pCHAR date_str );
void 	ncdci2_get_time( pCHAR time_date, pCHAR time_str );
void 	ncdci2_create_rrn(  pCHAR rrn  );
void 	ncdci2_create_stan( pBYTE stan );
INT  	ncdci2_determine_txn_flow( pCHAR msgtype, INT source );
void 	ncdci2_send_response_to_host( INT );
void 	ncdci2_send_request_to_host( pCHAR timer_resp_flag );
INT  	ncdci2_init_Auth_Tx( void );
INT  	ncdci2_init_txn( pAUTH_TX p_auth_tx );
INT  	ncdci2_send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
INT  	ncdci2_send_msg_to_authorizer( BYTE msgtype );
void	ncdci2_copy_to_req_resp_buffer ( pPTE_MSG p_msg_in );
INT 	ncdci2_parse_host_msg(       pCHAR err_buf );
INT  	ncdci2_build_host_msg(       pCHAR err_buf );
INT  	ncdci2_build_host_structure( pCHAR err_buf );
INT  	ncdci2_populate_auth_tx(     pCHAR err_buf );
INT  	ncdci2_build_request_msg(    pCHAR err_buf );
INT  	ncdci2_send_message_to_host( pCHAR err_buf ,INT mgt);
INT  	ncdci2_build_response_msg(     void );
void 	ncdci2_set_tx_key_to_response( void );
INT  	ncdci2_determine_tx_key(       void );
INT  	ncdci2_parse_track2(           void );
INT  	ncdci2_pin_block_exists(       void );
void 	ncdci2_perform_error_processing( pCHAR resp_code );
INT  	ncdci2_perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err );
void 	ncdci2_populate_tlf01_for_end_of_txn( void );
INT  	ncdci2_is_reversal( void );
void 	ncdci2_ascii_to_bin_with_length( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
void 	ncdci2_trim_trailing_spaces( pBYTE string );
void 	ncdci2_write_to_txn_file( void );
void 	ncdci2_remove_track2_end_sentinel( pBYTE track2 );

struct trandef *ncdci2_Find_Tran_Table( pAUTH_TX p_auth_tx );
struct bitfield_data *ncdci2_Find_Bitfield_Table( enum bitfields bitfield );

INT  	ncdci2_vEBCDICMove(          enum bitfields );
INT  	ncdci2_EBCDICMove(           enum bitfields );
INT  	ncdci2_RspEBCDICvMove(       enum bitfields );
INT  	ncdci2_RspEBCDICMove(        enum bitfields );
INT  	ncdci2_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT  	ncdci2_Move_Always(  enum bitfields fieldid, BYTE p_trantype );
INT 	ncdci2_get_variable_length( unsigned char *DataField, int FieldLen );
INT  	ncdci2_GenericMove(enum bitfields fieldid);
INT  	ncdci2_RspMoveGeneric(enum bitfields fieldid);
INT  	ncdci2_build_and_send_request_message( BYTE     msgtype, BYTE     subtype1,
                                     BYTE     subtype2, pBYTE    que_name,
                                     pBYTE    buffer, INT      length,
                                     BYTE     app_type, pCHAR    comm_info, pCHAR    err_msg );


BOOLEAN ncdci2_check_if_valid_zero_amount_transaction(void);
void 	ncdci2_build_pos_data_from_iso_pos_entry_mode(void);
void 	ncdci2_populate_iso_pos_entry_mode();
BOOLEAN ncdci2_is_pin_transaction(void);
BOOLEAN ncdci2_is_Card_present_pos_transaction(void);
BOOLEAN ncdci2_is_Magstrip_pos_transaction(void);
BOOLEAN ncdci2_is_Chip_pos_transaction(void);
BOOLEAN ncdci2_is_Fallback_pos_transaction(void);
BOOLEAN ncdci2_is_Contactless_Mastripe_pos_transaction(void);
BOOLEAN ncdci2_is_Contactless_Chip_pos_transaction(void);
BOOLEAN ncdci2_is_Manual_pos_transaction(void);
void 	ncdci2_build_Magstrip_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Chip_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Fallback_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Contactless_Mastripe_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Contactless_Chip_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Manual_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_unknow_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Magstrip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci2_build_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci2_build_Fallback_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci2_build_Contactless_Mastripe_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci2_build_Contactless_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci2_build_Manual_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	ncdci2_build_voice_transaction_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_unknow_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void 	match_DCI_to_ISO_response_code( pBYTE dci_response_code);
VOID 	match_ISO_to_Diners_response_code(  pBYTE diners_response_code);
void 	ncdci2_populate_DE43_name_street_city(char * temp_cardaccname);
BOOLEAN ncdci2_is_approved_by_Cadencie(void);
INT 	ncdci2_get_service_code( pCHAR service_code, pCHAR error_msg );
INT 	ncdci2_Sale_transactio (void);
INT 	ncdci2_Auth_transactio (void);
INT 	ncdci2_Auth_Advice_transaction (void);
void 	ncdci2_get_error_warning_file_name_path(void);
INT 	ncdci2_Log_error_warning_to_File(pCHAR Error_Buf,pCHAR func,int sev,int details);
void 	ncdci2_create_Error_Filename( void );
void 	ncdci2_genutil_EMV_To_Auth_Tx_ncdci_specific( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );
INT 	send_reversal_to_host_for_late_response( pAUTH_TX p_auth_tx );

INT 	ncdci2_check_if_trx_is_bdo_pos_acq_trx( );

INT 	ncdci2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( );
INT 	ncdci2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( );
INT 	ncdci2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( );
INT 	ncdci2_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_with_pin(  );
INT 	ncdci2_check_if_trx_is_dci_MobilePOS_NFC_Tap2Phone_trx_without_pin( );
void 	ncdci2_build_MobilePOS_NFC_Tap2Phone_trx_with_pin_pos_data_from_iso_pos_entry_mode ();
void 	ncdci2_build_MobilePOS_NFC_Tap2Phone_trx_without_pin_pos_data_from_iso_pos_entry_mode ();

INT 	ncdci2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( );
INT 	ncdci2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( );
INT 	ncdci2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx( );
INT 	ncdci2_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_with_pin( );
INT 	ncdci2_check_if_trx_is_dci_Swiftpass_BDO_Pay_credit_trx_without_pin( );
void 	ncdci2_build_Swiftpass_BDO_Pay_credit_trx_with_pin_pos_data_from_iso_pos_entry_mode();
void 	ncdci2_build_Swiftpass_BDO_Pay_credit_trx_without_pin_pos_data_from_iso_pos_entry_mode ();

INT 	ncdci2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( );
INT 	ncdci2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( );
INT 	ncdci2_check_if_trx_is_dci_ecommerce_swiftpass_BDOPay_trx( );
void    ncdci2_build_ecommerce_swiftpass_BDOPay_trx_pos_data_from_iso_pos_entry_mode();

/*----------*/
/* DATABASE */
/*----------*/
INT  	ncdci2_update_database();
void 	ncdci2_process_db_reply( pPTE_MSG p_msg );
INT  	ncdci2_insert_tlf01(pAUTH_TX p_auth_tx);
void 	ncdci2_process_db_ok( pPTE_MSG p_msg );
void 	ncdci2_process_db_error( pPTE_MSG p_msg );
INT  	ncdci2_get_ncf01_with_wait( pNCF01 Ncf01_I, pCHAR Buffer );
/*INT 	ncdci2_get_ncf01_keyblock_with_wait( pNCF01_KEYBLOCK01 pncf01_keyblock, pCHAR error_msg );*/
INT  	ncdci2_get_db_record( BYTE app_data_type, BYTE subtype1, pBYTE network_type);
INT  	ncdci2_insert_db_record( BYTE app_data_type );
BYTE 	ncdci2_send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg );


BOOLEAN ncdci2_is_issuing_transaction(void);
BOOLEAN ncdci2_is_sbatch_transaction(void);
BOOLEAN ncdci2_is_POS_Acq_transaction(void);
BOOLEAN ncdci2_is_voice_transaction(void);


/*-------*/
/* 0800  */
/*-------*/
void 	ncdci2_process_monitor_request( pPTE_MSG p_msg_in );
INT 	ncdci2_issue_req_08X4( INT nm_type );

INT  	ncdci2_issue_rsp_08X04( INT request_type );
void 	ncdci2_process_08X4_request(  void );
void 	ncdci2_process_08X4_response( void );
void 	ncdci2_process_monitor_query( pPTE_MSG p_msg );
void 	ncdci2_process_net_management_from_host( void );
INT  	ncdci2_build_08X0_msg_request(    pCHAR err_buf, int nm_type );
INT  	ncdci2_build_08X0_msg_response(    pCHAR err_buf, int nm_type );
INT  	ncdci2_send_18nn_to_host( pCHAR err_buf );
void ncdci2_perform_host_key_send_keyblock(void);

/*-------*/
/* TIMER */
/*-------*/
void 	ncdci2_process_timeout_msg( pPTE_MSG p_msg_in );
INT  	ncdci2_send_timeout_response( pCHAR err_buf );
INT  	ncdci2_set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer );
INT  	ncdci2_clear_timer( pCHAR timer_key );
INT  	ncdci2_clear_request_timer( pCHAR lookup_key );
INT  	ncdci2_set_echo_timer(    pCHAR err_buf );
INT  	ncdci2_clear_echo_timer(  pCHAR err_buf );
INT  	ncdci2_reset_echo_timer( pCHAR err_buf );
void 	ncdci2_create_echo_timer_key(    pCHAR echo_timer_key     );
void 	ncdci2_create_request_timer_key( pCHAR timer_key          );
void 	ncdci2_create_echo_req_timer_key(pCHAR echo_req_timer_key );
void 	ncdci2_transform_0200_into_0420( void );

/*----------*/
/* SECURITY */
/*----------*/
void 	ncdci2_process_translate_pin_response( pPTE_MSG p_msg_in );
void 	ncdci2_translate_pin( void );
void 	ncdci2_key_translate(void);
void 	ncdci2_process_key_translate_response( pPTE_MSG p_msg_in );

/*---------------*/
/* SHARED MEMORY */
/*---------------*/
INT  	ncdci2_create_shared_mem_table( pCHAR errbuf );
INT  	ncdci2_set_host_state( pBYTE host_state, pCHAR err_buf );
INT  	ncdci2_decrement_active_count(  pCHAR errbuf );
INT  	ncdci2_increment_active_count(  pCHAR errbuf );
INT 	ncdci2_increment_timeout_count( pCHAR errbuf );
INT  	ncdci2_reset_timeout_count(     pCHAR errbuf );
INT  	ncdci2_write_to_shared_mem_table( pBYTE consec_time_out_count,  pBYTE active_tran_count,
                                		  pBYTE host_state,  pCHAR errbuf );
INT  	ncdci2_read_shared_mem_table( pBYTE consec_time_out_count, pBYTE active_tran_count,
                            		  pBYTE host_state, pCHAR errbuf );

XINT 	ncdci2_ReadMemTable(  pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );
LONG 	ncdci2_WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );

void Mask_Keys(char * key_value);

/*------------*/
/* STATISTICS */
/*------------*/
INT  	ncdci2_send_transaction_statistics( pPTE_MSG p_msg_in );
INT  	ncdci2_reset_transaction_statistics( pPTE_MSG p_msg_in );

/*----------*/
/* CHARCODE */
/*----------*/
void 	ncdci2_convert_to_ascii(  pBYTE source, pBYTE dest, INT len );
void 	convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
void 	ncdci2_init_character_code_maps( void );
void 	ncdci2_hex_to_ebcdic(pBYTE source, pBYTE dest, INT len );
void 	ncdci2_ebcdic_to_hex(pBYTE source, pBYTE dest, INT len );

/*------------*/
/* VALIDATION */
/*------------*/
INT  	ncdci2_isnum( pCHAR in_str );
INT  	ncdci2_isalphanum(pCHAR string);
INT  	ncdci2_isalphanum_space(pCHAR string);
INT  	ncdci2_validate_date_yymm( pBYTE, pCHAR );
INT  	ncdci2_validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf );
INT  	ncdci2_validate_time(          pBYTE time_str  );
INT  	ncdci2_validate_date_mmdd(     pBYTE date_str  );
INT  	ncdci2_validate_date_time(     pBYTE date_time );
INT  	ncdci2_validate_host_msg( void );
INT  	ncdci2_validate_msgtype( pCHAR msgtype, pCHAR err_buf );
INT  	ncdci2_blank_string(pCHAR astr, INT max_len);
void 	ncdci2_check_end_of_year_rollover( pBYTE txn_mmdd, pBYTE ATP_date_yyyymmdd );

/*----------*/
/* CURRENCY */
/*----------*/
INT 	ncdci2_perform_currency_conversion( DWORD  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 pDWORD amt_out, pCHAR err_buf );

/*-------*/
/* 1304  */
/*-------*/
INT  	ncdci2_process_file_update_request( );
void 	ncdci2_process_file_update_response( pPTE_MSG p_msg );
void 	ncdci2_map_1304_to_tlf01( void );
INT  	ncdci2_send_response_to_GUI( void );
INT  	ncdci2_validate_gui_inputs( pCHAR err_msg );
INT  	ncdci2_populate_1314_resp_struct( pBYTE resp_code,pCHAR err_msg );
void 	ncdci2_issue_1304_response( pBYTE resp_code,pCHAR err_msg );
void 	ncdci2_process_key_request_response(pPTE_MSG p_msg_in);

/*------------*/
/* BIT FIELDS */
/*------------*/
void 	ncdci2_build_request_field_2( pINT result_flag, pCHAR err_buf );
void 	ncdci2_build_request_field_3( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_4( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_5( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_7( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_11( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_12( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_13( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_14( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_15( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_19( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_22( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_25( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_26( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_32( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_33( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_37( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_35( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_38( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_39( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_41( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_42( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_43( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_44( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_45( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_48( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_49( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_50( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_52( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_53( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_56( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_62( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_63( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_106( pINT result_flag, pCHAR err_buf );
void	ncdci2_build_request_field_122( pINT result_flag, pCHAR err_buf );

void	ncdci2_move_field_2( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_3( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_4( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_5( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_7( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_11( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_12( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_13( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_14( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_15( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_18( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_19( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_21( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_22( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_25( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_26( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_32( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_33( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_35( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_37( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_38( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_39( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_44( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_41( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_42( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_43( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_49( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_50( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_52( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_55( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_56( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_62( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_106( pINT result_flag, pCHAR err_buf );
void	ncdci2_move_field_122( pINT result_flag, pCHAR err_buf );

#endif
