/***************************************************************************
*
* Copyright (c) 2003, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      nccup_prptotypes.h
*  
* TITLE:       CUP Network Control Function Prototypes
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

#include "nccup_bitfields.h"
#include "nccup_constants.h"


struct trandef *Find_Tran_Table( pAUTH_TX p_auth_tx );
struct bitfield_data *Find_Bitfield_Table( enum bitfields bitfield );

/*-------*/
/* MAIN  */
/*-------*/
void ascendent_msg_handler( pPTE_MSG p_msg_in );
INT  startup( void );



/*-------*/
/* TRANS */
/*-------*/
void 	process_request_for_host( pPTE_MSG p_msg_in );
void 	process_response_for_host( pPTE_MSG p_msg_in );
void	process_host_msg( pPTE_MSG p_msg_in );
void 	process_request_from_host( void );
void 	process_response_from_host( void );
void 	process_clear_timer_reply( pPTE_MSG p_msg_in );
void 	process_db_response_atm(   pBYTE p_data_stream, INT record_found );
void 	process_db_response_ncf30( pBYTE p_data_stream );
void 	process_request_for_host_after_security( void );
void 	process_request_for_host_after_currency( void );
void 	update_auth_tx_with_ncf30_data( void );
void 	copy_resp_into_request( pAUTH_TX host_auth_tx );
INT  	verify_host_status( pCHAR err_buf );
BYTE 	hhutil_process_host_request(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
BYTE 	match_request_to_response(BYTE request);
INT 	incoming_other_host_response(pAUTH_TX p_auth_tx);
INT 	txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
INT 	get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
void	process_1304_from_host();
BYTE  	get_tran_type( pAUTH_TX p_auth_tx ) ;
BYTE  	get_tran_type_2(BYTE tx_key) ;
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
INT 	incoming_other_host_response_db_reply( );
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
INT 	process_incoming_message_continued( pAUTH_TX p_auth_tx );
INT 	incoming_terminal_request(pAUTH_TX p_auth_tx);
BOOLEAN send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx );
INT 	perform_host_logon_request(char request_type[], int source,	pAUTH_TX p_auth_tx);
INT 	perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx );
INT 	hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
int 	perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx);
INT 	hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT 	set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out );
BOOLEAN process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx);
void 	process_logon_Key_Reset_Transaction (void);
void 	update_ncf01( pAUTH_TX p_auth_tx );
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx);
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
BOOLEAN nccup_check_txn_for_CUP_authentication_attempt();
void 	nccup_Compute_AVN_Value(char * AVN_value);
void 	Cup_string2hexString(char* input, char* output);
void 	Mask_Keys(char * key_value);


/*-------*/
/* UTILS */
/*-------*/

void 	nccup_log_message( INT dest, INT severity, pCHAR mesg, pCHAR func_name, int details );
BOOLEAN process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx);
INT  	send_network_status_to_monitor( void );
void 	calc_julian_day( pCHAR date_str, pCHAR julian_str );
void 	get_date( pCHAR time_date, pCHAR date_str );
void 	get_time( pCHAR time_date, pCHAR time_str );
void 	create_rrn(  pCHAR rrn  );
void 	create_stan( pBYTE stan );
INT  	determine_txn_flow( pCHAR msgtype, INT source );
void 	send_response_to_host( INT );
void 	send_request_to_host( pCHAR timer_resp_flag );
INT  	init_Auth_Tx( void );
INT 	init_txn( pAUTH_TX p_auth_tx );
INT 	send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
INT  	send_msg_to_authorizer( BYTE msgtype );
void 	copy_to_req_resp_buffer ( pPTE_MSG p_msg_in );
INT  	parse_host_msg(       pCHAR err_buf );
INT  	build_host_msg(       pCHAR err_buf );
INT  	build_host_structure( pCHAR err_buf );
INT  	populate_auth_tx(     pCHAR err_buf );
INT 	 build_request_msg(    pCHAR err_buf );
INT  	send_message_to_host( pCHAR err_buf ,INT mgt);
INT  	build_response_msg(     void );
void   set_tx_key_to_response( void );
INT  	determine_tx_key(       void );
INT  	parse_track2(           void );
INT  	pin_block_exists(       void );
void 	perform_error_processing( pCHAR resp_code );
INT  	perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err );
void 	populate_tlf01_for_end_of_txn( void );
INT  	is_reversal( void );
void 	ascii_to_bin_with_length( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
void 	trim_trailing_spaces( pBYTE string );
void 	write_to_txn_file( void );
void 	remove_track2_end_sentinel( pBYTE track2 );
void 	send_update_Key_message_to_nccup2(void);
void 	nccup_send_logoff_message_to_nccup_after_key_translation_read_fail( void );
INT 	nccup_get_service_code( pCHAR service_code, pCHAR error_msg );
void 	nccup_send_logoff_request_to_nccup3_after_key_transalation_failed(void);
INT 	nccup_read_incoming_cup_key_from_nccup3_ncf01_issuing_table(void);
INT 	nccup_read_cup_key_for_nccup3_active_active_trigger(void);
void 	nccup_activ_active_trigger_handler_for_nccup();
INT 	nccup_check_emv_capability_device(void);

INT  	vEBCDICMove(          enum bitfields );
INT  	EBCDICMove(           enum bitfields );
INT  	RspEBCDICvMove(       enum bitfields );
INT  	RspEBCDICMove(        enum bitfields );
INT 	Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT 	Move_Always(  enum bitfields fieldid, BYTE p_trantype );
INT  	get_variable_length(  pBYTE, INT );
INT  	GenericMove(enum bitfields fieldid);
INT  	RspMoveGeneric(enum bitfields fieldid);
INT  	build_and_send_request_message( BYTE     msgtype,	 BYTE     subtype1,
										 BYTE     subtype2,	 pBYTE    que_name,
										 pBYTE    buffer,	 INT      length,
										 BYTE     app_type,  pCHAR    comm_info,
										 pCHAR    err_msg );

void 	nccup_get_error_warning_file_name_path(void);
INT  	nccup_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int details);
void 	nccup_create_Error_Filename( void );
BOOLEAN nccup_is_issuing_transaction(void);
BOOLEAN nccup_is_POS_Acq_transaction(void);
BOOLEAN nccup_is_voice_transaction(void);
BOOLEAN nccup_is_sbatch_transaction(void);
BOOLEAN nccup_is_approved_by_Cadencie(void);
BOOLEAN nccup_Check_If_CUP_ECOM(void);
BOOLEAN nccup_Check_If_CUP_MOTO_TRN(void);
BOOLEAN nccup_Check_If_CUP_RPS_TRN(void);
BOOLEAN nccup_Check_If_CUP_BDO_PAY(void);

INT 	nccup_check_if_trx_is_bdo_pos_acq_trx( );

INT 	nccup_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( );
INT 	nccup_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( );
INT 	nccup_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( );
INT 	nccup_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_with_pin(  );
INT 	nccup_check_if_trx_is_upi_MobilePOS_NFC_Tap2Phone_trx_without_pin( );

INT 	nccup_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( );
INT 	nccup_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( );
INT 	nccup_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx( );
INT 	nccup_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_with_pin( );
INT 	nccup_check_if_trx_is_upi_Swiftpass_BDO_Pay_credit_trx_without_pin( );

INT 	nccup_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( );
INT 	nccup_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( );
INT 	nccup_check_if_trx_is_upi_ecommerce_swiftpass_BDOPay_trx( );


/*----------*/
/* DATABASE */
/*----------*/
INT  	update_database();
void 	process_db_reply( pPTE_MSG p_msg );
INT 	insert_tlf01(pAUTH_TX p_auth_tx);
void 	process_db_ok( pPTE_MSG p_msg );
void 	process_db_error( pPTE_MSG p_msg );
INT  	get_ncf01_with_wait( pNCF01 Ncf01_I, pCHAR Buffer );
INT  	get_db_record( BYTE app_data_type, BYTE subtype1, pBYTE network_type);
INT  	insert_db_record( BYTE app_data_type );
BYTE 	send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg );
INT 	nccup_Get_Pin_Transalation_Info_from_nccup_table(void);
INT 	nccup_Key_exchange_enable_Flag_value_from_nccup3_table(void);

/*-------*/
/* 0800  */
/*-------*/
void 	process_monitor_request( pPTE_MSG p_msg_in );
void 	issue_req_08X0( INT nm_type );

INT  	issue_rsp_08X0( INT request_type );
void 	process_08X0_request(  void );
void 	process_08X0_response( void );
void 	process_monitor_query( pPTE_MSG p_msg );
void 	process_net_management_from_host( void );
INT  	build_08X0_msg_request(    pCHAR err_buf, int nm_type );
INT  	build_08X0_msg_response(    pCHAR err_buf, int nm_type );
INT  	send_18nn_to_host( pCHAR err_buf );
void 	process_logon_Key_Reset_Reject_Transaction(void);
void 	process_logon_Reset_Key_exchange_disable_flag(void);
void 	process_logon_Reset_Key_exchange_enable_flag(void);
void 	process_logon_Reset_Key_exchange_disable_flag_KEY_TRANSLATION_FAILED();
void 	process_logon_Reset_Key_exchange_disable_flag_KEY_TRANSLATION_SUCCESS();
/*-------*/
/* TIMER */
/*-------*/
void 	process_timeout_msg( pPTE_MSG p_msg_in );
INT  	send_timeout_response( pCHAR err_buf );
INT  	set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer );
INT  	clear_timer( pCHAR timer_key );
INT  	clear_request_timer( pCHAR lookup_key );
INT  	set_echo_timer(    pCHAR err_buf );
INT  	clear_echo_timer(  pCHAR err_buf );
INT  	reset_echo_timer( pCHAR err_buf );
void 	create_echo_timer_key(    pCHAR echo_timer_key     );
void 	create_request_timer_key( pCHAR timer_key          );
void 	create_echo_req_timer_key(pCHAR echo_req_timer_key );
void 	transform_0200_into_0420( void );

/*----------*/
/* SECURITY */
/*----------*/
void 	process_translate_pin_response( pPTE_MSG p_msg_in );
void 	translate_pin( void );
void 	key_translate(void);
void 	process_key_translate_response( pPTE_MSG p_msg_in );


/*---------------*/
/* SHARED MEMORY */
/*---------------*/
INT  	create_shared_mem_table( pCHAR errbuf );
INT  	set_host_state( pBYTE host_state, pCHAR err_buf );
INT  	decrement_active_count(  pCHAR errbuf );
INT  	increment_active_count(  pCHAR errbuf );
INT  	increment_timeout_count( pCHAR errbuf );
INT  	reset_timeout_count(     pCHAR errbuf );

INT  	write_to_shared_mem_table( pBYTE consec_time_out_count,  	pBYTE active_tran_count,
									pBYTE host_state,	pCHAR errbuf );

INT  	read_shared_mem_table( pBYTE consec_time_out_count, 	pBYTE active_tran_count,
								pBYTE host_state,	pCHAR errbuf );

XINT 	ReadMemTable(  pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );
LONG 	WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );


/*------------*/
/* STATISTICS */
/*------------*/
INT  	send_transaction_statistics( pPTE_MSG p_msg_in );
INT  	reset_transaction_statistics( pPTE_MSG p_msg_in );


/*----------*/
/* CHARCODE */
/*----------*/
void 	convert_to_ascii(  pBYTE source, pBYTE dest, INT len );
void 	convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
void 	init_character_code_maps( void );
void 	hex_to_ebcdic(pBYTE source, pBYTE dest, INT len );
void 	ebcdic_to_hex(pBYTE source, pBYTE dest, INT len );

/*------------*/
/* VALIDATION */
/*------------*/
INT  	isnum( pCHAR in_str );
INT  	isalphanum(pCHAR string);
INT  	isalphanum_space(pCHAR string);
INT  	validate_date_yymm( pBYTE, pCHAR );
INT  	validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf );
INT  	validate_time(          pBYTE time_str  );
INT  	validate_date_mmdd(     pBYTE date_str  );
INT  	validate_date_time(     pBYTE date_time );
INT  	validate_host_msg( void );
INT  	validate_msgtype( pCHAR msgtype, pCHAR err_buf );
INT  	blank_string(pCHAR astr, INT max_len);
void 	check_end_of_year_rollover( pBYTE txn_mmdd, pBYTE ATP_date_yyyymmdd );


/*----------*/
/* CURRENCY */
/*----------*/
INT 	perform_currency_conversion( DWORD  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 pDWORD amt_out, pCHAR err_buf );

/*-------*/
/* 1304  */
/*-------*/
INT  	process_file_update_request( pPTE_MSG p_msg );
void 	process_file_update_response( void );
void 	map_1304_to_tlf01( void );
INT  	send_response_to_GUI( void );
INT  	validate_gui_inputs( pCHAR err_msg );

INT  	populate_1314_resp_struct( pBYTE resp_code, pCHAR err_msg );
void 	issue_1304_response( pBYTE resp_code, pCHAR err_msg );

/*------------*/
/* BIT FIELDS */
/*------------*/
INT 	build_request_field_2( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_3( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_4( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_5( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_7( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_11( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_12( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_13( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_14( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_15( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_19( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_22( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_25( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_26( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_32( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_33( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_37( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_35( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_38( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_39( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_41( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_42( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_43( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_44( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_45( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_48( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_49( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_50( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_52( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_53( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_56( pINT result_flag, pCHAR err_buf );
INT 	build_request_field_63( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_2( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_3( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_4( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_5( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_7( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_9( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_11( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_12( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_14( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_15( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_16( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_19( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_20( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_21( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_24( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_25( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_32( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_33( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_37( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_38( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_39( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_41( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_42( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_44( pINT result_flag, pCHAR err_buf );
INT 	build_response_field_48( pINT result_flag, pCHAR err_buf );
INT 	build_SMS_response_field_48( pINT result_flag, pCHAR err_buf );
INT 	build_SMS_Admin_response_field_48( pINT result_flag, pCHAR err_buf );

INT 	move_field_2( pINT result_flag, pCHAR err_buf );
INT 	move_field_3( pINT result_flag, pCHAR err_buf );
INT 	move_field_4( pINT result_flag, pCHAR err_buf );
INT 	move_field_5( pINT result_flag, pCHAR err_buf );
INT 	move_field_7( pINT result_flag, pCHAR err_buf );
INT 	move_field_11( pINT result_flag, pCHAR err_buf );
INT 	move_field_12( pINT result_flag, pCHAR err_buf );
INT 	move_field_13( pINT result_flag, pCHAR err_buf );
INT 	move_field_14( pINT result_flag, pCHAR err_buf );
INT 	move_field_15( pINT result_flag, pCHAR err_buf );
INT 	move_field_18( pINT result_flag, pCHAR err_buf );
INT 	move_field_19( pINT result_flag, pCHAR err_buf );
INT 	move_field_21( pINT result_flag, pCHAR err_buf );
INT 	move_field_22( pINT result_flag, pCHAR err_buf );
INT 	move_field_25( pINT result_flag, pCHAR err_buf );
INT 	move_field_26( pINT result_flag, pCHAR err_buf );
INT 	move_field_32( pINT result_flag, pCHAR err_buf );
INT 	move_field_33( pINT result_flag, pCHAR err_buf );
INT 	move_field_35( pINT result_flag, pCHAR err_buf );
INT 	move_field_37( pINT result_flag, pCHAR err_buf );
INT 	move_field_38( pINT result_flag, pCHAR err_buf );
INT 	move_field_39( pINT result_flag, pCHAR err_buf );
INT 	move_field_44( pINT result_flag, pCHAR err_buf );
INT 	move_field_41( pINT result_flag, pCHAR err_buf );
INT 	move_field_42( pINT result_flag, pCHAR err_buf );
INT 	move_field_43( pINT result_flag, pCHAR err_buf );
INT 	move_admin_request_field_48( pINT result_flag, pCHAR err_buf );
INT 	move_request_field_48( pINT result_flag, pCHAR err_buf );
INT 	move_response_field_48( pINT result_flag, pCHAR err_buf );
INT 	move_field_49( pINT result_flag, pCHAR err_buf );
INT 	move_field_50( pINT result_flag, pCHAR err_buf );
INT 	move_field_52( pINT result_flag, pCHAR err_buf );
INT 	move_field_55( pINT result_flag, pCHAR err_buf );
INT 	move_field_56( pINT result_flag, pCHAR err_buf );
INT 	move_request_field_63( pINT result_flag, pCHAR err_buf );
INT 	move_field_117( pINT result_flag, pCHAR err_buf );

#endif
