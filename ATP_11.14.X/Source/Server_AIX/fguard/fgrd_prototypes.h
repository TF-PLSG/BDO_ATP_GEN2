/***************************************************************************
*
* Copyright (c) 2006, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      FGRD_PROTOTYPES.H
*  
* TITLE:       Equitable Fraudguard Function Prototypes
*  
* DESCRIPTION: This header file contains a list function prototypes that are
*              used by the Fraudguard Interface application.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      D. Irby
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\fguard\fgrd_prototypes.h  $
   
      Rev 1.5   Nov 06 2006 13:15:32   DIrby
   Modified to get number of messages in the FSoap dialog connection
   queue.  If the count is too high, do not send request to FG.  Put out
   a warning message every 25 txns.
   SCR 23657
   
      Rev 1.4   Oct 27 2006 13:42:40   DIrby
   Made the following bug fixes for Issuer txns:
   1.  Issuer labels to be lower case
   2.  Card Currency field needs to be always present
   3.  For local txns, never convert f4, f6, f49, and f51.
   4.  F43 should always be present; f43c defaults to PHL for local txns.
   5.  If f19 not present, populate it from field 49.
   6.  Field 25 should always be present. Use default values is missing.
   7.  If fields 5/6 and 50/51 are missing, populate using field 4 and 49.
   SCR 23522
   
      Rev 1.3   Oct 10 2006 13:48:20   DIrby
   Added capability to send Issuer (ATP approves the txns) to Fraudguard.
   SCR 21851
   
      Rev 1.2   Dec 21 2005 16:01:18   dirby
   Added function prototypes for functions that convert and unconvert
   strings containing characters that have special meaning to XML.
   SCR 19054
   
      Rev 1.1   Aug 23 2005 16:54:14   dirby
   Modified to escape the angle brackets so they do not get converted
   by Microsoft at Fraudguard's end.
   SCR 16789
   
      Rev 1.0   Jul 08 2005 09:15:46   dirby
   Initial revision.
*
****************************************************************************/

#ifndef FGRD_PROTOTYPES_H
#define FGRD_PROTOTYPES_H

/*-------*/
/* MAIN  */
/*-------*/
void ascendent_msg_handler( pPTE_MSG p_msg_in );
INT  startup( void );


/*-------*/
/* TRAN  */
/*-------*/
void process_host_msg( pPTE_MSG p_msg_in );
void process_request_from_host( void );
void process_request_for_host( pPTE_MSG p_msg_in );
void process_single_txn( void );
void begin_batch_send( pPTE_MSG p_msg_in );
void continue_resend_process( pBYTE p_data, BYTE subtype2 );
void resend_multiple_txns( pBYTE p_data, BYTE subtype2 );
void resend_single_txn( pBYTE p_data );
void process_response_from_host( void );



/*----------*/
/* DATABASE */
/*----------*/
INT  update_database( BYTE data_type );
void process_db_reply( pPTE_MSG p_msg );
void process_db_ok( pPTE_MSG p_msg );
void process_db_error( pPTE_MSG p_msg );
INT  get_fg01_with_wait( pFG01 Fg01, pCHAR Buffer );
INT  get_db_record( BYTE app_data_type, BYTE subtype2, pBYTE db_keys );
INT  insert_db_record( BYTE app_data_type );
BYTE send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg );


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



/*-------*/
/* UTILS */
/*-------*/

/* Log Errors, Warnings, Info:
 *    Dest     => 1=Monitor, 2=System Log, 3=both
 *    Severity => 1=Info, 2=Warning, 3=Error
 */
void  log_message( INT dest, INT severity, pCHAR mesg, pCHAR func_name );

void  copy_to_req_resp_buffer ( pPTE_MSG p_msg_in );
void  copy_xml_to_fguard( pCHAR tag, pCHAR tag_value );
void  create_xml_request( void );
void  create_timestamp( pBYTE timestamp );
INT   currency_conversion( pCHAR in_amount, pCHAR curr_code,
                           INT   direction, pCHAR out_amount );
void  convert_usd_to_peso( pBYTE usd_amt, pBYTE peso_amt );
INT   determine_txn_flow( pCHAR transaction, INT source );
void  generate_tran_id( pBYTE tran_id );
pCHAR get_next_tag( pCHAR p_txn, pCHAR tag, pCHAR tag_value );
void  get_date( pCHAR time_date, pCHAR date_str );
void  get_time( pCHAR time_date, pCHAR time_str );
INT   get_amounts( pCHAR peso, pCHAR usd );
INT   parse_request_msg(  pCHAR txn, pINT plen );
INT   parse_response_msg( pCHAR txn, pINT plen );
void  populate_fgraw( pCHAR p_txn, INT txn_type );
INT   populate_fguard_request( void );
void  populate_tag( pCHAR tag, pCHAR tag_value, INT indicator );
void  populate_tag( pCHAR tag, pCHAR tag_value, INT indicator );
void  remove_leading_zeroes( pCHAR string );
INT   send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );
INT   send_to_fraudguard( pCHAR err_buf );
void  write_to_txn_file();
INT   escape_angle_brackets( pCHAR strin, pCHAR strout );
void  convert_xml_chars( pCHAR instr, pCHAR outstr );
void  unconvert_xml_chars( pCHAR instr, pCHAR outstr );
void  get_field4_amount( void );
void  get_field6_amount( void );
void  get_amt_center   ( void );
void  get_amt_card     ( void );
void  get_otb_amt_card ( void );
void  get_card_family  ( void );
void  get_country_code ( void );
void  get_field25      ( void );
void  get_fields_49_and_51( void );
INT   get_queue_msg_count( pCHAR qname );

INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );

#endif
