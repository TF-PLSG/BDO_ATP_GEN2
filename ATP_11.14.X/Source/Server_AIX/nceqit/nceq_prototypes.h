/***************************************************************************
*
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      NCEQ_PROTOTYPES.H
*  
* TITLE:       Equitable Network Control Function Prototypes
*  
* DESCRIPTION: This header file contains a list function prototypes that are
*              used by the Telecheck Network Control application.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      D. Irby
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq_prototypes.h  $
   
      Rev 1.17   Nov 12 2008 16:47:48   TF-Ajay
   Added prototype display message on monitor.
   Rel 09-76
   
	 Rev 1.16   Oct 12 2005 14:47:48   dirby
   Added prototype is_void.
   SCR 17660
   
      Rev 1.15   Sep 02 2004 17:10:42   dirby
   Modified to support extremely large amounts.
   SCR 1471
   
      Rev 1.14   Mar 24 2004 15:08:56   dirby
   Added function prototypes for build_request_field_36, 102, and 103.
   SCR 1352
   
      Rev 1.13   Sep 29 2003 13:51:40   dirby
   Added support for Fee Collection and Funds Disbursement for
   Acquirers.
   SCR 1083
   
      Rev 1.12   Sep 11 2003 15:50:26   dirby
   Added function prototype for map_response_code.
   SCR 1186
   
      Rev 1.11   Aug 28 2003 14:30:22   svelaga
   Fixed bugs/issues with 0600 message.
   
      Rev 1.10   Aug 25 2003 13:47:56   svelaga
   Fixed
   1. Added support for Field 62
   2. No check for valid NMI code for Administrative text message.
   3. Backoffice adjustment and Representment are distinguised based on Field 25 ( pos condition code ) instead of processing code.
   
      Rev 1.9   Aug 05 2003 10:29:02   svelaga
   VISASMS Specification Changes: New Private field 62 and some changes to Private field 63.
   
      Rev 1.8   Jul 30 2003 14:45:56   svelaga
   Administrative message ( 0620 ) message added.
   
      Rev 1.7   Jul 25 2003 17:32:58   svelaga
   SMS projects changes -
   1. Support for BACKOFFICE_ADJUSTMENT, REPRESENTMENT transactions.
   2. Added to parse and build some extra fields like DE48, DE63,
   DE18,DE19 etc...
   
      Rev 1.6   Jul 26 2002 13:25:38   dirby
   Modified to allow 0800 echo requests to go out when host
   state is Offline due to Maximum Time Outs.  Will go back Online
   when a response to the echo request is received.
   SCR 594
   
      Rev 1.5   Jun 24 2002 11:05:42   dirby
   Added function prototype 'remove_track2_end_sentinel'.
   SCR 790
   
      Rev 1.4   Jan 16 2002 12:30:56   dirby
   Added transaction tracing capability.     SCR 546
   
      Rev 1.3   Jan 14 2002 18:14:18   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.2   Aug 14 2001 12:08:54   dirby
   Removed prototypes for 2 functions that are not used anymore.
   
      Rev 1.1   Aug 13 2001 17:21:30   SYARLAGA
   Added function definitions.
   
      Rev 1.0   Aug 06 2001 09:48:22   dirby
   Initial revision.
*
****************************************************************************/

#ifndef NCEQ_PROTOTYPES_H
#define NCEQ_PROTOTYPES_H

#include "nceq_bitfields.h"
/*-------*/
/* MAIN  */
/*-------*/
void ascendent_msg_handler( pPTE_MSG p_msg_in );
INT  startup( void );
void shut_down(void);


/*-------*/
/* TRANS */
/*-------*/
void process_request_for_host( pPTE_MSG p_msg_in );
void process_response_for_host( pPTE_MSG p_msg_in );
void process_host_msg( pPTE_MSG p_msg_in );
void process_request_from_host( void );
void process_response_from_host( void );
void process_clear_timer_reply( pPTE_MSG p_msg_in );
void process_db_response_atm(   pBYTE p_data_stream, INT record_found );
void process_db_response_ncf30( pBYTE p_data_stream );
void process_request_for_host_after_security( void );
void process_request_for_host_after_currency( void );
void update_auth_tx_with_ncf30_data( void );
void copy_resp_into_request( pAUTH_TX host_auth_tx );
INT  verify_host_status( pCHAR err_buf );


void get_0120_enablingFlag_FromTF_ini();
void Masked_card_number(char * card_num);


/*-------*/
/* UTILS */
/*-------*/

/* Log Errors, Warnings, Info:
 *    Dest     => 1=Monitor, 2=System Log, 3=both
 *    Severity => 1=Info, 2=Warning, 3=Error
 */
void nceqit_log_message( INT dest, INT severity, pCHAR mesg, pCHAR func_name, int details );
INT  send_network_status_to_monitor( void );
void calc_julian_day( pCHAR date_str, pCHAR julian_str );
void get_date( pCHAR time_date, pCHAR date_str );
void get_time( pCHAR time_date, pCHAR time_str );
void create_rrn(  pCHAR rrn  );
void create_stan( pBYTE stan );
INT  determine_txn_flow( pCHAR msgtype, INT source );
void send_response_to_host( INT );
void send_request_to_host( pCHAR timer_resp_flag );
INT  init_Auth_Tx( void );
INT  send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
INT  send_msg_to_authorizer( BYTE msgtype );
void copy_to_req_resp_buffer ( pPTE_MSG p_msg_in );
INT  parse_host_msg(       pCHAR err_buf );
INT  build_host_msg(       pCHAR err_buf );
INT  build_host_structure( pCHAR err_buf );
INT  populate_auth_tx(     pCHAR err_buf );
INT  build_request_msg(    pCHAR err_buf );
INT  send_message_to_host( pCHAR err_buf ,INT mgt);//praneeth added parameter bool mgt 
INT  build_response_msg(     void );
void set_tx_key_to_response( void );
INT  determine_tx_key(       void );
INT  parse_track2(           void );
INT  pin_block_exists(       void );
void perform_error_processing( pCHAR resp_code );
INT  perform_standin_processing( pCHAR rsp_code, BYTE gen_status, pCHAR err );
void populate_tlf01_for_end_of_txn( void );
INT  is_reversal( void );
INT  is_void( void );
void ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
void trim_trailing_spaces( pBYTE string );
void write_to_txn_file( void );
void remove_track2_end_sentinel( pBYTE track2 );
void map_response_code( pBYTE host_rc, pBYTE atp_rc );

struct trandef *Find_Tran_Table( pAUTH_TX p_auth_tx );
struct bitfield_data *Find_Bitfield_Table( enum bitfields bitfield );
INT RspMovevData (enum bitfields fieldid);
INT  vDataMove(          enum bitfields );
INT  vEBCDICMove(          enum bitfields );
INT  EBCDICMove(           enum bitfields );
INT  RspEBCDICvMove(       enum bitfields );
INT  RspEBCDICMove(        enum bitfields );
INT  Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT  Move_Always(  enum bitfields fieldid, BYTE p_trantype );
INT  get_variable_length(  pBYTE, INT );
INT  GenericMove(enum bitfields fieldid);
INT  RspMoveGeneric(enum bitfields fieldid);

INT  build_and_send_request_message( BYTE     msgtype,
                                     BYTE     subtype1,
                                     BYTE     subtype2, 
                                     pBYTE    que_name, 
                                     pBYTE    buffer, 
                                     INT      length,
                                     BYTE     app_type,
                                     pCHAR    comm_info,
                                     pCHAR    err_msg );
extern BOOLEAN  TxUtils_Send_Msg_To_Operator(INT,pBYTE,INT,INT,pBYTE,
                                            BYTE,pBYTE,pBYTE,pBYTE,pBYTE);


/*----------*/
/* DATABASE */
/*----------*/
INT  update_database();
void process_db_reply( pPTE_MSG p_msg );
void process_db_ok( pPTE_MSG p_msg );
void process_db_error( pPTE_MSG p_msg );
INT  get_ncf01_with_wait( pNCF01 Ncf01_I, pCHAR Buffer );
INT  get_db_record( BYTE app_data_type, BYTE subtype1, pBYTE network_type);
INT  insert_db_record( BYTE app_data_type );
BYTE send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg );


/*-------*/
/* 0800  */
/*-------*/
void process_monitor_request( pPTE_MSG p_msg_in );
void issue_0800( INT request_type );
INT  issue_0810( INT request_type );
void process_0800_request(  void );
void process_0810_response( void );
void process_monitor_query( pPTE_MSG p_msg );
void process_net_management_from_host( void );
INT  build_0800_msg(    pCHAR err_buf );
INT  build_0810_msg(    pCHAR err_buf );
INT  send_08nn_to_host( pCHAR err_buf );

/*-------*/
/* TIMER */
/*-------*/
void process_timeout_msg( pPTE_MSG p_msg_in );
INT  send_timeout_response( pCHAR err_buf );
INT  set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer );
INT  clear_timer( pCHAR timer_key );
INT  clear_request_timer( pCHAR lookup_key );
INT  set_echo_timer(    pCHAR err_buf );
INT  clear_echo_timer(  pCHAR err_buf );
INT  reset_echo_timer( pCHAR err_buf );
void create_echo_timer_key(    pCHAR echo_timer_key     );
void create_request_timer_key( pCHAR timer_key          );
void create_request_timer_key_for_Visa( pCHAR timer_key          );
void create_echo_req_timer_key(pCHAR echo_req_timer_key );
void transform_0200_into_0420( void );


/*----------*/
/* SECURITY */
/*----------*/
void process_translate_pin_response( pPTE_MSG p_msg_in );
void translate_pin( void );



/*---------------*/
/* SHARED MEMORY */
/*---------------*/
INT  create_shared_mem_table( pCHAR errbuf );
INT  set_host_state( pBYTE host_state, pCHAR err_buf );
INT  decrement_active_count(  pCHAR errbuf );
INT  increment_active_count(  pCHAR errbuf );
INT  increment_timeout_count( pCHAR errbuf );
INT  reset_timeout_count(     pCHAR errbuf );

INT  write_to_shared_mem_table( pBYTE consec_time_out_count,
                                pBYTE active_tran_count,
                                pBYTE host_state,
                                pCHAR errbuf );

INT  read_shared_mem_table( pBYTE consec_time_out_count,
                            pBYTE active_tran_count,
                            pBYTE host_state,
                            pCHAR errbuf );

XINT ReadMemTable(  pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );

LONG WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );



/*------------*/
/* STATISTICS */
/*------------*/
INT  send_transaction_statistics( pPTE_MSG p_msg_in );
INT  reset_transaction_statistics( pPTE_MSG p_msg_in );


/*----------*/
/* CHARCODE */
/*----------*/
void convert_to_ascii(  pBYTE source, pBYTE dest, INT len );
void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
void init_character_code_maps( void );
void hex_to_ebcdic(pBYTE source, pBYTE dest, INT len );
void ebcdic_to_hex(pBYTE source, pBYTE dest, INT len );
void hex_to_ascii( pBYTE instr, pBYTE outstr, INT len ); /* Girija Y */



/*------------*/
/* VALIDATION */
/*------------*/
INT  isnum( pCHAR in_str );
INT  isalphanum(pCHAR string);
INT  isalphanum_space(pCHAR string);
INT  validate_date_yymm( pBYTE, pCHAR );
INT  validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf );
INT  validate_time(          pBYTE time_str  );
INT  validate_date_mmdd(     pBYTE date_str  );
INT  validate_date_time(     pBYTE date_time );
INT  validate_host_msg( void );
INT  validate_msgtype( pCHAR msgtype, pCHAR err_buf );
INT  blank_string(pCHAR astr, INT max_len);
void check_end_of_year_rollover( pBYTE txn_mmdd, pBYTE ATP_date_yyyymmdd );


/*----------*/
/* CURRENCY */
/*----------*/
INT perform_currency_conversion( double  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 double *amt_out, pCHAR err_buf );



/*------------*/
/* BIT FIELDS */
/*------------*/
INT build_request_field_2( pINT result_flag, pCHAR err_buf );
INT build_request_field_3( pINT result_flag, pCHAR err_buf );
INT build_request_field_4( pINT result_flag, pCHAR err_buf );
INT build_request_field_5( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_7( pINT result_flag, pCHAR err_buf );
INT build_request_field_9( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_11( pINT result_flag, pCHAR err_buf );
INT build_request_field_12( pINT result_flag, pCHAR err_buf );
INT build_request_field_13( pINT result_flag, pCHAR err_buf );
INT build_request_field_15( pINT result_flag, pCHAR err_buf );
INT build_request_field_16( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_18( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_19( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_20( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_21( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_22( pINT result_flag, pCHAR err_buf );
INT build_request_field_25( pINT result_flag, pCHAR err_buf );
INT build_request_field_28( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_32( pINT result_flag, pCHAR err_buf );
INT build_request_field_33( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_35( pINT result_flag, pCHAR err_buf );
INT build_request_field_36( pINT result_flag, pCHAR err_buf );
INT build_request_field_37( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_38( pINT result_flag, pCHAR err_buf );
INT build_request_field_39( pINT result_flag, pCHAR err_buf );
INT build_request_field_41( pINT result_flag, pCHAR err_buf );
INT build_request_field_42( pINT result_flag, pCHAR err_buf );
INT build_request_field_43( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_48( pINT result_flag, pCHAR err_buf );
INT build_request_field_49( pINT result_flag, pCHAR err_buf );
INT build_request_field_52( pINT result_flag, pCHAR err_buf );
INT build_request_field_60( pINT result_flag, pCHAR err_buf ); /* Phani TF */
INT build_request_field_62( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_63( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT build_request_field_90( pINT result_flag, pCHAR err_buf );
INT build_request_field_95( pINT result_flag, pCHAR err_buf );
INT build_request_field_102( pINT result_flag, pCHAR err_buf );
INT build_request_field_103( pINT result_flag, pCHAR err_buf );
INT build_request_field_104( pINT result_flag, pCHAR err_buf );
INT build_request_field_115( pINT result_flag, pCHAR err_buf );
INT build_request_field_128( pINT result_flag, pCHAR err_buf );
INT build_response_field_2( pINT result_flag, pCHAR err_buf );
INT build_response_field_3( pINT result_flag, pCHAR err_buf );
INT build_response_field_4( pINT result_flag, pCHAR err_buf );
INT build_response_field_5( pINT result_flag, pCHAR err_buf );
INT build_response_field_7( pINT result_flag, pCHAR err_buf );
INT build_response_field_9( pINT result_flag, pCHAR err_buf );
INT build_response_field_11( pINT result_flag, pCHAR err_buf );
INT build_response_field_15( pINT result_flag, pCHAR err_buf );
INT build_response_field_16( pINT result_flag, pCHAR err_buf );
INT build_response_field_19( pINT result_flag, pCHAR err_buf );
INT build_response_field_20( pINT result_flag, pCHAR err_buf );
INT build_response_field_21( pINT result_flag, pCHAR err_buf );
INT build_response_field_25( pINT result_flag, pCHAR err_buf );
INT build_response_field_32( pINT result_flag, pCHAR err_buf );
INT build_response_field_37( pINT result_flag, pCHAR err_buf );
INT build_response_field_38( pINT result_flag, pCHAR err_buf );
INT build_response_field_39( pINT result_flag, pCHAR err_buf );
INT build_response_field_41( pINT result_flag, pCHAR err_buf );
INT build_response_field_42( pINT result_flag, pCHAR err_buf );
INT build_response_field_44( pINT result_flag, pCHAR err_buf );
INT build_response_field_48( pINT result_flag, pCHAR err_buf );
INT build_response_field_49( pINT result_flag, pCHAR err_buf );
INT build_SMS_response_field_48( pINT result_flag, pCHAR err_buf );
INT build_SMS_Admin_response_field_48( pINT result_flag, pCHAR err_buf );

INT build_response_field_62( pINT result_flag, pCHAR err_buf );
INT build_response_field_63( pINT result_flag, pCHAR err_buf );
INT build_admin_response_field_63( pINT result_flag, pCHAR err_buf );
INT build_response_field_70( pINT result_flag, pCHAR err_buf );
INT build_admin_response_field_70( pINT result_flag, pCHAR err_buf );
INT build_response_field_90( pINT result_flag, pCHAR err_buf );
INT build_response_field_95( pINT result_flag, pCHAR err_buf );
INT build_response_field_100( pINT result_flag, pCHAR err_buf );
INT build_response_field_102( pINT result_flag, pCHAR err_buf );
INT build_response_field_103( pINT result_flag, pCHAR err_buf );
INT build_response_field_128( pINT result_flag, pCHAR err_buf );
INT move_field_2( pINT result_flag, pCHAR err_buf );
INT move_field_3( pINT result_flag, pCHAR err_buf );
INT move_field_4( pINT result_flag, pCHAR err_buf );
INT move_field_7( pINT result_flag, pCHAR err_buf );
INT gmt_get_time(pBYTE); /* Girija Y */
INT move_field_11( pINT result_flag, pCHAR err_buf );
INT move_field_12( pINT result_flag, pCHAR err_buf );
INT move_field_13( pINT result_flag, pCHAR err_buf );
INT move_field_15( pINT result_flag, pCHAR err_buf );
INT move_field_18( pINT result_flag, pCHAR err_buf );
INT move_field_19( pINT result_flag, pCHAR err_buf );
INT move_field_21( pINT result_flag, pCHAR err_buf );
INT move_field_22( pINT result_flag, pCHAR err_buf );
INT move_field_25( pINT result_flag, pCHAR err_buf );
INT move_field_28( pINT result_flag, pCHAR err_buf ); /* Ajay S.T. - TF */
INT move_field_32( pINT result_flag, pCHAR err_buf );
INT move_field_35( pINT result_flag, pCHAR err_buf );
INT move_field_37( pINT result_flag, pCHAR err_buf );
INT move_field_38( pINT result_flag, pCHAR err_buf );
INT move_field_39( pINT result_flag, pCHAR err_buf );
INT move_field_41( pINT result_flag, pCHAR err_buf );
INT move_field_42( pINT result_flag, pCHAR err_buf );
INT move_field_43( pINT result_flag, pCHAR err_buf );
INT move_field_44( pINT result_flag, pCHAR err_buf ); /* Girija Y - TF */
INT move_admin_request_field_48( pINT result_flag, pCHAR err_buf );
INT move_request_field_48( pINT result_flag, pCHAR err_buf, INT usage );
INT move_response_field_48( pINT result_flag, pCHAR err_buf );
INT move_field_49( pINT result_flag, pCHAR err_buf );
INT move_field_52( pINT result_flag, pCHAR err_buf );
INT move_request_field_62( pINT result_flag, pCHAR err_buf );
INT move_request_field_63( pINT result_flag, pCHAR err_buf );
INT move_field_70( pINT result_flag, pCHAR err_buf );
INT move_admin_field_70( pINT result_flag, pCHAR err_buf );
INT move_field_90( pINT result_flag, pCHAR err_buf );
INT move_field_95( pINT result_flag, pCHAR err_buf );
INT move_field_100( pINT result_flag, pCHAR err_buf );
INT move_field_102( pINT result_flag, pCHAR err_buf );
INT move_field_103( pINT result_flag, pCHAR err_buf );
INT move_field_104( pINT result_flag, pCHAR err_buf );

INT move_field_128( pINT result_flag, pCHAR err_buf );

void nceqit_create_Error_Filename(  );

INT nceqit_check_if_Visa_transaction( );

INT nceqit_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail);
/* DE 104 DATA SETID*/
#define DATA_SET_104_56 56
#define DATA_SET_104_57 57
#define DATA_SET_104_58 58
#define DATA_SET_104_59 59
#define DATA_SET_104_5C 5C
#define DATA_SET_104_5D 5D
#define DATA_SET_104_5F 5F
#define DATA_SET_104_60 60
#define DATA_SET_104_61 61
#define DATA_SET_104_62 62
#define DATA_SET_104_63 63
#define DATA_SET_104_65 65
#define DATA_SET_104_71 71

// TLV
#define DATA_SET_104_57_TLV01 01
#define DATA_SET_104_57_TLV02 02



#endif
