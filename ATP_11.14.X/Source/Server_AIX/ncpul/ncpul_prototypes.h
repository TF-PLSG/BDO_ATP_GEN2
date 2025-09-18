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


/*-------*/
/* MAIN  */
/*-------*/
void ascendent_msg_handler( pPTE_MSG p_msg_in );
INT  ncpul_ncpul1_startup( void );



/*-------*/
/* TRANS */
/*-------*/
 BOOLEAN ncpul_process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx);
void  ncpul_process_request_for_host( pPTE_MSG p_msg_in );
void  ncpul_process_response_for_host( pPTE_MSG p_msg_in );
void  ncpul_process_host_msg( pPTE_MSG p_msg_in );
INT   ncpul_process_request_from_host( void );
void  ncpul_process_response_from_host( void );
void  ncpul_process_clear_timer_reply( pPTE_MSG p_msg_in );
void  ncpul_process_db_response_atm(   pBYTE p_data_stream, INT record_found );
void  ncpul_process_db_response_ncf30( pBYTE p_data_stream );
void  ncpul_process_request_for_host_after_security( void );
void  ncpul_process_request_for_host_after_currency( void );
void  ncpul_update_auth_tx_with_ncf30_data( void );
void  ncpul_copy_resp_into_request( pAUTH_TX host_auth_tx );
INT   ncpul_verify_host_status( pCHAR err_buf );
BOOLEAN  ncpul_hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
BYTE  ncpul_match_request_to_response(BYTE request);
INT  ncpul_incoming_other_host_response(pAUTH_TX p_auth_tx);
INT  ncpul_txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
INT  ncpul_get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
BYTE   ncpul_get_tran_type( pAUTH_TX p_auth_tx ) ;
BYTE   ncpul_get_tran_type_2(BYTE tx_key) ;
BOOLEAN  ncpul_hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
INT  ncpul_incoming_other_host_response_db_reply( );
BOOLEAN  ncpul_send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx );
INT  ncpul_perform_host_logon_request(char request_type[], int source,	pAUTH_TX p_auth_tx);
INT  ncpul_perform_op_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx );
INT  ncpul_hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
int  ncpul_perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx);
INT  ncpul_hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT  ncpul_set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_out );
BOOLEAN  ncpul_process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx);
void  ncpul_process_logon_Key_Reset_Transaction (void);
void  ncpul_update_ncf01( pAUTH_TX p_auth_tx );
BOOLEAN  ncpul_hhutil_send_host_response(pAUTH_TX p_auth_tx);
BOOLEAN  ncpul_blank_or_null_string(pCHAR astr, INT max_len);





/*-------*/
/* UTILS */
/*-------*/

void ncpul_calc_julian_day( pCHAR date_str, pCHAR julian_str );
void ncpul_get_date( pCHAR time_date, pCHAR date_str );
void ncpul_get_time( pCHAR time_date, pCHAR time_str );
void ncpul_create_rrn( pCHAR rrn );
void ncpul_copy_to_req_resp_buffer ( pPTE_MSG p_msg_in );
INT ncpul_determine_txn_flow( pCHAR msgtype, INT source );
void ncpul_send_response_to_host( INT send_flag );
void ncpul_send_request_to_host( pCHAR timer_resp_flag );
INT ncpul_send_msg_to_authorizer( BYTE msgtype );
INT ncpul_build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );
INT ncpul_send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
INT ncpul_init_Auth_Tx(void);
INT ncpul_determine_tx_key( );
void ncpul_set_tx_key_to_response();
void ncpul_RspMovevBCD (enum bitfields fieldid);
INT ncpul_GenericMove(enum bitfields fieldid);
void ncpul_vDataMove(enum bitfields fieldid);
void ncpul_RspMovevData (enum bitfields fieldid);
INT ncpul_RspMoveGeneric(enum bitfields fieldid);
INT ncpul_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT ncpul_Move_Always( enum bitfields bitfield, BYTE p_trantype );
int ncpul_get_variable_length( unsigned char *DataField, int FieldLen );
struct trandef *ncpul_Find_Tran_Table( pAUTH_TX p_auth_tx );
struct bitfield_data *ncpul_Find_Bitfield_Table( enum bitfields bitfield );
INT ncpul_build_host_msg( pCHAR err_buf );
INT ncpul_build_host_structure( pCHAR err_buf );
INT ncpul_parse_host_msg( pCHAR err_buf );
INT ncpul_populate_auth_tx( pCHAR err_buf );
INT ncpul_parse_track2();
void ncpul_remove_track2_end_sentinel( pBYTE track2 );
INT ncpul_build_response_msg();
INT ncpul_build_request_msg( pCHAR err_buf );
void ncpul_create_stan( pBYTE stan );
void ncpul_log_message( INT dest, INT sev, pCHAR msg, pCHAR func,int details );
INT ncpul_send_network_status_to_monitor();
INT ncpul_send_message_to_host( pCHAR err_buf,INT mgt );
INT ncpul_pin_block_exists();
INT  ncpul_verify_host_status( pCHAR err_buf );
void ncpul_perform_error_processing( pCHAR resp_code );
INT ncpul_perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err_buf );
INT ncpul_is_reversal();
void ncpul_trim_trailing_spaces( pBYTE p_string );
void ncpul_write_to_txn_file();
void ncpul_Rj_with_lead_zeros( pCHAR str, INT FieldLen );
BYTE ncpul_hhutil_process_host_request(pAUTH_TX p_auth_tx);
BOOLEAN ncpul_hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT ncpul_init_txn( pAUTH_TX p_auth_tx );
BOOLEAN ncpul_process_logon_request(CHAR nmi_code[4], pAUTH_TX p_auth_tx);
BYTE ncpul_match_request_to_response(BYTE request);
INT ncpul_incoming_other_host_response(pAUTH_TX p_auth_tx);
INT ncpul_txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
INT ncpul_get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
INT ncpul_incoming_other_host_response_db_reply( );
BYTE  ncpul_get_tran_type_2(BYTE tx_key);
INT ncpul_insert_tlf01(pAUTH_TX p_auth_tx);
INT ncpul_hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR ncpul_response_code[4]);
INT ncpul_hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
void ncpul_update_ncf01( pAUTH_TX p_auth_tx );
BOOLEAN ncpul_hhutil_send_host_response(pAUTH_TX p_auth_tx);
INT ncpul_get_service_code( pCHAR service_code, pCHAR error_msg );
BOOLEAN ncpul_blank_or_null_string(pCHAR astr, INT max_len);
BOOLEAN ncpul_is_issuing_transaction(void);
BOOLEAN ncpul_is_approved_by_Cadencie(void);
BOOLEAN ncpul_check_if_valid_zero_amount_transaction(void);
void ncpul_build_pos_data_from_iso_pos_entry_mode(void);
void ncpul_populate_iso_pos_entry_mode();
BOOLEAN ncpul_is_pin_transaction(void);
BOOLEAN ncpul_is_Card_present_pos_transaction(void);
BOOLEAN ncpul_is_Magstrip_pos_transaction(void);
BOOLEAN ncpul_is_Chip_pos_transaction(void);
BOOLEAN ncpul_is_Fallback_pos_transaction(void);
BOOLEAN ncpul_is_Contactless_Mastripe_pos_transaction(void);
BOOLEAN ncpul_is_Contactless_Chip_pos_transaction(void);
BOOLEAN ncpul_is_Manual_pos_transaction(void);
BOOLEAN ncpul_is_voice_transaction(void);
void ncpul_build_Magstrip_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Chip_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Fallback_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Contactless_Mastripe_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Contactless_Chip_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Manual_pos_data_from_iso_pos_entry_mode();
void ncpul_build_unknow_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Magstrip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void ncpul_build_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void ncpul_build_Fallback_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void ncpul_build_Contactless_Mastripe_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void ncpul_build_Contactless_Chip_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void ncpul_build_voice_transaction_pos_data_from_iso_pos_entry_mode();
void ncpul_build_Manual_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
void ncpul_build_unknow_pos_data_in_iso_pos_entry_mode_format_from_pos_data();
VOID match_DCI_to_ISO_response_code(AUTH_TX Auth_Tx, pBYTE dci_response_code);
void ncpul_create_Error_Filename( void);
INT ncpul_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func , int detail);
void ncpul_get_error_warning_file_name_path(void );

/*----------*/
/* DATABASE */
/*----------*/
INT  ncpul_update_database();
void ncpul_process_db_reply( pPTE_MSG p_msg );
INT  ncpul_insert_tlf01(pAUTH_TX p_auth_tx);
void ncpul_process_db_ok( pPTE_MSG p_msg );
void ncpul_process_db_error( pPTE_MSG p_msg );
INT  ncpul_get_ncf01_with_wait( pNCF01 Ncf01_I, pCHAR Buffer );
INT  ncpul_get_db_record( BYTE app_data_type, BYTE subtype1, pBYTE network_type);
INT  ncpul_insert_db_record( BYTE app_data_type );
BYTE ncpul_send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg );


/*-------*/
/* 0800  */
/*-------*/
void ncpul_process_monitor_request( pPTE_MSG p_msg_in );
void ncpul_issue_req_08X4( INT nm_type );

INT  ncpul_issue_rsp_08X4( INT request_type );
void ncpul_process_08X4_request(  void );
void ncpul_process_08X4_response( void );
void ncpul_process_monitor_query( pPTE_MSG p_msg );
void ncpul_process_net_management_from_host( void );
INT  ncpul_build_08X0_msg_request(    pCHAR err_buf, int nm_type );
INT  ncpul_build_08X0_msg_response(    pCHAR err_buf, int nm_type );
INT  ncpul_send_18nn_to_host( pCHAR err_buf );

/*-------*/
/* TIMER */
/*-------*/
void ncpul_process_timeout_msg( pPTE_MSG p_msg_in );
INT  ncpul_send_timeout_response( pCHAR err_buf );
INT  ncpul_set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer );
INT  ncpul_clear_timer( pCHAR timer_key );
INT  ncpul_clear_request_timer( pCHAR lookup_key );
INT  ncpul_set_echo_timer(    pCHAR err_buf );
INT  ncpul_clear_echo_timer(  pCHAR err_buf );
INT  ncpul_reset_echo_timer( pCHAR err_buf );
void ncpul_create_echo_timer_key(    pCHAR echo_timer_key     );
void ncpul_create_request_timer_key( pCHAR timer_key          );
void ncpul_create_echo_req_timer_key(pCHAR echo_req_timer_key );
void ncpul_transform_sale_into_0420( void );


/*----------*/
/* SECURITY */
/*----------*/
void ncpul_process_translate_pin_response( pPTE_MSG p_msg_in );
void ncpul_translate_pin( void );
void ncpul_key_translate(void);
void ncpul_process_key_translate_response( pPTE_MSG p_msg_in );


/*---------------*/
/* SHARED MEMORY */
/*---------------*/
INT  ncpul_create_shared_mem_table( pCHAR errbuf );
INT  ncpul_set_host_state( pBYTE host_state, pCHAR err_buf );
INT  ncpul_decrement_active_count(  pCHAR errbuf );
INT  ncpul_increment_active_count(  pCHAR errbuf );
INT  ncpul_increment_timeout_count( pCHAR errbuf );
INT  ncpul_reset_timeout_count(     pCHAR errbuf );

INT  ncpul_write_to_shared_mem_table( pBYTE consec_time_out_count,
                                pBYTE active_tran_count,
                                pBYTE host_state,
                                pCHAR errbuf );

INT  ncpul_read_shared_mem_table( pBYTE consec_time_out_count,
                            pBYTE active_tran_count,
                            pBYTE host_state,
                            pCHAR errbuf );

XINT ncpul_ReadMemTable(  pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );

LONG ncpul_WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );



/*------------*/
/* STATISTICS */
/*------------*/
INT  ncpul_send_transaction_statistics( pPTE_MSG p_msg_in );
INT  ncpul_reset_transaction_statistics( pPTE_MSG p_msg_in );


/*----------*/
/* CHARCODE */
/*----------*/
void ncpul_convert_to_ascii(  pBYTE source, pBYTE dest, INT len );
void ncpul_convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
void ncpul_init_character_code_maps( void );
void ncpul_hex_to_ebcdic(pBYTE source, pBYTE dest, INT len );
void ncpul_ebcdic_to_hex(pBYTE source, pBYTE dest, INT len );



/*------------*/
/* VALIDATION */
/*------------*/
INT  ncpul_isnum( pCHAR in_str );
INT  ncpul_isalphanum(pCHAR string);
INT  ncpul_isalphanum_space(pCHAR string);
INT  ncpul_validate_date_yymm( pBYTE, pCHAR );
INT  ncpul_validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf );
INT  ncpul_validate_time(          pBYTE time_str  );
INT  ncpul_validate_date_mmdd(     pBYTE date_str  );
INT  ncpul_validate_date_time(     pBYTE date_time );
INT  ncpul_validate_host_msg( void );
INT  ncpul_validate_msgtype( pCHAR msgtype, pCHAR err_buf );
INT  ncpul_blank_string(pCHAR astr, INT max_len);
void ncpul_check_end_of_year_rollover( pBYTE txn_mmdd, pBYTE ATP_date_yyyymmdd );


/*----------*/
/* CURRENCY */
/*----------*/
INT ncpul_perform_currency_conversion( DWORD  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 pDWORD amt_out, pCHAR err_buf );

/*-------*/
/* 1304  */
/*-------*/
INT  ncpul_process_file_update_request( pPTE_MSG p_msg );
void ncpul_process_file_update_response( void );
void ncpul_map_1304_to_tlf01( void );
INT  ncpul_send_response_to_GUI( void );
INT  ncpul_validate_gui_inputs( pCHAR err_msg );

INT  ncpul_populate_1314_resp_struct( pBYTE resp_code,
                                pCHAR err_msg );

void ncpul_issue_1304_response( pBYTE resp_code,
                          pCHAR err_msg );

/*------------*/
/* BIT FIELDS */
/*------------*/
void ncpul_build_request_field_2( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_3( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_4( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_5( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_6( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_7( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_9( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_10( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_11( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_12( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_13( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_14( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_15( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_16( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_18( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_19( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_22( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_23( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_26( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_28( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_30( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_32( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_33( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_35( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_37( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_38( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_39( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_40( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_41( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_42( pINT result_flag, pCHAR err_buf ); /* Girija Y - ThoughtFocus */
void ncpul_build_request_field_43( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_44( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_45( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_46( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_47( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_48( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_49( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_51 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_52( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_54 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_55 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_56 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_57 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_58( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_59( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_60 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_62 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_64 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_70 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_72 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_90( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_91( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_95( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_96 ( pINT result_flag, pCHAR err_buf );  /* Girija Y - ThoughtFocus */
void ncpul_build_request_field_98 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_101 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_102 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_103 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_104 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_105 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_109 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_110 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_111 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_113 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_114 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_120 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_121 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_122 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_123 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_124 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_125 ( pINT result_flag, pCHAR err_buf );
void ncpul_build_request_field_126( pINT result_flag, pCHAR err_buf );

void ncpul_move_field_2( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_3( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_4( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_5( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_6( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_7( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_9( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_10( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_11( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_12( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_13( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_14( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_15( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_16( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_17( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_18( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_19( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_22( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_23( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_26( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_28( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_29( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_32( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_33( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_35( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_37( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_38( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_39( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_40( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_41( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_42( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_43( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_44( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_46( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_47( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_48( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_49( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_50( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_51( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_52( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_53( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_54( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_55( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_56( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_58( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_59( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_60( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_61( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_62( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_63( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_70( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_90( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_91( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_92( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_93( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_94( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_95( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_96( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_98( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_101( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_102( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_103( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_104( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_105( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_109( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_110( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_120( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_121( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_122( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_123( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_124( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_125( pINT result_flag, pCHAR err_buf );
void ncpul_move_field_126( pINT result_flag, pCHAR err_buf );


#endif
